#ifndef RPCCLIENT_H
#define RPCCLIENT_H
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

#include "rpcHandler.h"
#include "config.h"
#include "cache.h"
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
    using fdCacheType=LRUSetCache<std::string,int>;
private:
    std::string ipPort;
    std::string service="",service_;
    fdCacheType fdCache,serviceFdCache;
    std::map<int,std::string> connectedIpportsMap;
    std::set<std::string> connectedIpports;
    std::mutex mutex;
    JsonParser remoteCall(JsonParser json,fdCacheType &mCache);
    void connectOne(std::string &ip,int port,fdCacheType &mCache);
    void refreshFdCache();
    std::thread *refreshThread=nullptr;
    bool shutdownRefersh=false;
public:
    enum connectionType{
        SERVER,CLIENT
    };
    rpcClient(std::vector<std::pair<std::string,int>> ipports);
    rpcClient(std::string ip,int port);
    rpcClient(std::string service,connectionType type=SERVER,std::vector<std::pair<std::string,int>> ipports=config::hosts);
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