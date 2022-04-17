#include <iostream>
#include <boost/algorithm/string.hpp>

#include "cache.h"
#include "rpcHandler.h"
#include "rpcClient.h"
#include "tcpServer.h"
#include "rpcParser.h"
#include "baseBalancer.h"
cache serviceCache("127.0.0.1",6379);
std::map<std::string,std::string> newService(std::string service,std::set<std::string> ipport) {
    try
    {
        serviceCache.set(service,ipport);
        return {{"status","success"}};
    }
    catch(...)
    {
        return {{"status","failed"}};
    }
}
std::set<std::string> getService(std::string service) {
    try
    {
        return serviceCache.get(service);
    }
    catch(...)
    {
        return {};
    }
    
}
std::map<std::string,std::string> insertService(std::string service,std::string ipport) {
    try
    {
        serviceCache.insert(service,ipport);
        return {{"status","success"}};
    }
    catch(...)
    {
        return {{"status","failed"}};
    }
}
std::map<std::string,std::string> eraseService(std::string service,std::string ipport) {
    try
    {
        return serviceCache.erase(service,ipport)?std::map<std::string,std::string>({{"status","success"}})
                                        :std::map<std::string,std::string>({{"status","failed"}});
    }
    catch(...)
    {
        return {{"status","failed"}};
    }
}
int main() {
    bool result;
    rpcHandler* handler=new rpcHandler();
    handler->AddRpcHandler(newService);
    handler->AddRpcHandler(getService);
    handler->AddRpcHandler(insertService);
    handler->AddRpcHandler(eraseService);
    baseBalancer balancer;
    std::function<JsonParser(std::string,JsonParser)> functor(std::bind(&baseBalancer::balanceCall,&balancer,std::ref(serviceCache),std::placeholders::_1,std::placeholders::_2)); 
    handler->addRpcHandler("requestService",functor);
    poolServer *server=new poolServer(handler,8080);
    server->startForever();
    return 0;
}