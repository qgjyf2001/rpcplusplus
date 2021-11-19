#ifndef RPCCLIENT_H
#define RPCCLIENT_H
#include <iostream>

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
public:
    
    rpcClient(std::string ip,std::string port);
    template <typename returnType,typename ...args>
    auto makeRpcCall(std::string name,returnType(args...))
    {
        return rpcCall<returnType,args...>(this,name);
    }
    JsonParser remoteCall(JsonParser json);
    ~rpcClient();
};



#endif