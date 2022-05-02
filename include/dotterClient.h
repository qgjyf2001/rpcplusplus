#ifndef DOTTERCLIENT_H
#define DOTTERCLIENT_H
#include "config.h"

#include <map>
#include <mutex>
#include <future>
#include <thread>
class rpcClient;
class dotterClientImpl
{
private:
    std::shared_ptr<rpcClient> client;
    std::map<std::string,std::map<std::string,int>> dotMap;
    std::mutex mutex;
    std::thread thread;
    std::promise<bool> shutdownThread;
    void report();
public:
    dotterClientImpl(std::string dotterService);
    int throughput_(std::map<std::string,std::map<std::string,int>> &reportMap);
    std::map<std::string,int> getThroughput(std::string key,int fromTimeStamp,int toTimeStamp);
    int throughput(std::string key,int value);
    ~dotterClientImpl();
};
class dotterClient
{
private:
    static std::shared_ptr<dotterClientImpl> client;
public:
    static void init(std::string dotterService=config::dotterService)
    {
        client.reset(new dotterClientImpl(dotterService));
    }
    static int throughput(std::string key,int value)
    {
        return client->throughput(key,value);
    }
    static std::map<std::string,int> getThroughput(std::string key,int fromTimeStamp,int toTimeStamp)
    {
        return client->getThroughput(key,fromTimeStamp,toTimeStamp);
    }
};
#endif