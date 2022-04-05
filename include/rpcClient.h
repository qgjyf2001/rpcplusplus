#ifndef RPCCLIENT_H
#define RPCCLIENT_H
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

#include "rpcHandler.h"
class rpcClient;
template <typename returnType,typename ...args>
class rpcCall
{
private:
    rpcClient *connection;
    std::string name;
public:
    rpcCall(rpcClient *connection,std::string name)
    {
          this->connection=connection;  
          this->name=name;
    }
    returnType operator()(args... params)
    {
        rpcSender sender;
        auto rpc=sender.sendRPC(params...);
        rpc["name"]=name;
        JsonParser response=connection->remoteCall(rpc);
        return sender.getRPC<returnType>(response);
    }
};
class rpcClient
{
private:
    int sockfd;
    std::string ipPort;
    std::string service="";
public:
    rpcClient(std::string ip,int port);
    rpcClient(std::string service,std::string hostIp="127.0.0.1",int hostPort=8080);
    template <typename returnType,typename ...args>
    auto makeRpcCall(std::string name,std::function<returnType(args...)>)
    {
        return rpcCall<returnType,args...>(this,name);
    }
    template <typename returnType,typename ...args>
    auto makeRpcCall(std::string name,returnType(args...))
    {
        return rpcCall<returnType,args...>(this,name);
    }
    template <typename F>
    auto makeRpcCall(std::string name)
    {
        return makeRpcCall(name,(F)nullptr);
    }
    JsonParser remoteCall(JsonParser json);
    ~rpcClient();
};



#endif