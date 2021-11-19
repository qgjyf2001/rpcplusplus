#include "http.h"
#include "httpBasicResponse.h"
#include "httpParser.h"

http::http(httpHandler *handler,int maxThreads)
{
    pool=new threadPool(maxThreads);
    this->handler=handler;
    sendThread=std::thread([&](){
        signal(SIGPIPE , SIG_IGN);
        while (true)
        {
            int index=0;
            std::unique_lock<std::mutex> lck(mutex);
            consumer.wait(lck);
            while (vec.size()!=0)
            {
                auto &&[fd,sockfd,fileSize,offset]=*vec[index];
                if (offset+sendLength<fileSize)
                {
                    if (sendfile(sockfd,fd,&offset,sendLength)<sendLength)
                    {
                        delete vec[index];
                        vec.erase(std::begin(vec)+index);
                        close(fd);
                        continue;
                    }
                    index++;
                }
                else
                {
                    sendfile(sockfd,fd,&offset,fileSize-sendLength);
                    delete vec[index];
                    vec.erase(std::begin(vec)+index);
                    close(fd);
                }
                if (vec.size()==0)
                    break;
                if (index==vec.size())
                {
                    index%=vec.size();
                }
            }
            
        }
    });
}
void http::free(int sockfd)
{
    if (uncompleted.find(sockfd)!=uncompleted.end())
        uncompleted.erase(sockfd);
}
void http::doHttp(int* sockfd,std::string httpRequest,std::function<void(int*)> handleClose)
{
    pool->addThread([=](void *args){     
        signal(SIGPIPE , SIG_IGN);
        httpRequestType request;
        if (uncompleted.find(*sockfd)!=uncompleted.end())
        {
        if (auto &now=uncompleted[*sockfd];now.second!=0)
        {
               now.second-=httpRequest.length();
               if (now.second>0)
               {
                   now.first+=httpRequest;
                   return;
               }
                else
                {
                    now.second=0;
                    try
                    {
                        request=httpParser::parse(now.first+httpRequest);
                    }
                    catch(const std::exception& e)
                    {
                        httpResponse resp=http400BasicResponse();
                        std::string badRequest=resp.toString();
                        write(*sockfd,badRequest.c_str(),badRequest.length());
                        if (!resp.getConnection())
                            handleClose(sockfd);
                        uncompleted.erase(*sockfd);
                        return;
                    }
                    uncompleted.erase(*sockfd);
                }
        }
    }
        else 
        {
            try
            {
                request=httpParser::parse(httpRequest);    
                if (auto len1=std::atoi(request["Content-Length"].c_str()),len2=(int)request["text"].length();len1>len2)
                {
                    uncompleted[*sockfd]=std::make_pair(httpRequest,len1-len2);
                    return;
                }
            }
            catch(const std::exception& e)
            {
                httpResponse resp=http400BasicResponse();
                std::string badRequest=resp.toString();
                write(*sockfd,badRequest.c_str(),badRequest.length());
                if (!resp.getConnection())
                    handleClose(sockfd);
                return;
            }
        }
        auto result=handler->handleRequest(request);
        auto responseText=result.toString();
        int wrote=write(*sockfd,responseText.c_str(),responseText.length());
        if (result.fd!=-1)
        {
            if (result.fileSize<sendLength)
            {
                sendfile(*sockfd,result.fd,NULL,result.fileSize);
                close(result.fd);
            }
            else
            {
                vec.push_back(new fileStruct(result.fd,*sockfd,result.fileSize,0));
                if (vec.size()==1)
                {
                    std::unique_lock<std::mutex> lck(mutex);
                    consumer.notify_one();
                }
            }
        }
        if (!result.getConnection())
            handleClose(sockfd);
    },&sockfd);
}
void http::waitAll()
{
    pool->waitAll();
}
http::~http()
{
    delete pool;
}