#include "rpcClient.h"
#include "tcpServer.h"
rpcClient::rpcClient(std::string ip,int port)
{
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if (sockfd==-1)
        throw std::runtime_error("socket error");
    sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    inet_pton(AF_INET,ip.c_str(),&addr.sin_addr);
   
    if (connect(sockfd,(sockaddr*)&addr,sizeof(addr))<0)
    { 
        throw std::runtime_error("connect error");
    }
}
JsonParser rpcClient::remoteCall(JsonParser json)
{
    std::string msg=rpcMessage(json).toString();
    write(sockfd,msg.c_str(),msg.length());
    char buf[MAXLINE];
    auto n=read(sockfd,buf,MAXLINE);
    std::string message(buf,n);
    auto rpcRequest=rpcParser::parse(message);
    if (rpcRequest.length>rpcRequest.message.length())
    {
        std::string now=message;
        int length=rpcRequest.length;
        while (now.length()<length)
        {
            auto n=read(sockfd,buf,MAXLINE);
            now+=std::string(buf,n);
        }
        rpcRequest=rpcParser::parse(now);
    }    
       return JsonParser(&rpcRequest.message);
}
rpcClient::~rpcClient()
{
    close(sockfd);
}