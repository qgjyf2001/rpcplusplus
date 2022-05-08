#include "rpcServer.h"
#include "rpcClient.h"
#include "kafka.h"

#include <boost/algorithm/string.hpp>

rpcServer::rpcServer(rpcHandler *handler,int maxThreads)
{
    this->handler=handler;
    this->pool=new threadPool(maxThreads);
}
void rpcServer::free(int sockfd)
{
    if (uncompleted.find(sockfd)!=uncompleted.end())
        uncompleted.erase(sockfd);
    if (futureMap.find(sockfd)!=futureMap.end())
        futureMap.erase(sockfd);
}
void rpcServer::registService(std::string service,std::string ip,int port) {
    rpcClient client(config::hosts);
    using registFunc=std::function<std::map<std::string,std::string>(std::string,std::string)>;
    auto regist=client.makeRpcCall<registFunc>("insertService");
    regist(service,ip+":"+std::to_string(port));
}
void rpcServer::doRpc(int sockfd,std::string httpRequest)
{
    auto res=pool->addThread([=]() -> std::string {     
        signal(SIGPIPE , SIG_IGN);
        rpcMessage request(0,"");
        if (uncompleted.find(sockfd)!=uncompleted.end())
        {
        if (auto &now=uncompleted[sockfd];now.second!=0)
        {
               now.second-=httpRequest.length();
               if (now.second>0)
               {
                   now.first+=httpRequest;
                   return "";
               }
                else
                {
                    now.second=0;
                    request=rpcParser::parse(now.first+httpRequest);
                    uncompleted.erase(sockfd);
                }
        }
    }
        else 
        {
            try
            {
                request=rpcParser::parse(httpRequest);
                if (request.length>request.message.length())
                {
                    uncompleted[sockfd]=std::make_pair(httpRequest,request.length-request.message.length());
                    return "";
                }
            }
            catch(const std::exception& e)
            {
                JsonParser errorMessage;
                errorMessage["status"]="failed";
                rpcMessage resp(errorMessage);
                std::string badRequest=resp.toString();
                return badRequest;
            }
        }
        JsonParser rpc(&request.message);
        std::string ip;
        sockaddr_in sa;
        socklen_t lenAddr=sizeof(sa);
        if (!getpeername(sockfd,(sockaddr*)&sa,&lenAddr)) {
            ip=std::string(inet_ntoa(sa.sin_addr))+":"+std::to_string(ntohs(sa.sin_port));
        }
        auto result=handler->handleRPC(rpc);
        if (handler->service!="") {
            std::string kafkaTopic=handler->service;
            boost::replace_all(kafkaTopic,".","_");
            auto result=kafkaProducer::instance()->produce(kafkaTopic,rpc).get();
        }
        auto responseText=rpcMessage(result).toString();
        return responseText;
    });
    
    futureMap[sockfd]=std::move(res);
}
std::string rpcServer::getResult(int sockfd,bool& result) {
    if (futureMap.find(sockfd)==futureMap.end()) {
        result=false;  
        return "";
    }
    result=true;
    auto httpResult=futureMap[sockfd].get();
    futureMap.erase(sockfd);
    return httpResult;
}
rpcServer::~rpcServer()
{
    delete pool;
}