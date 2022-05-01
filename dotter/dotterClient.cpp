#include "dotterClient.h"
std::shared_ptr<dotterClientImpl> dotterClient::client=nullptr;
dotterClientImpl::dotterClientImpl(std::string dotterService)
{
    client=std::make_shared<rpcClient>(dotterService);
    thread=std::thread([&](){
        auto isShutdown=shutdownThread.get_future();
        while (isShutdown.wait_for(std::chrono::seconds(5))==std::future_status::timeout)
        {
            report();
        }
    });
}
void dotterClientImpl::report()
{
    std::lock_guard<std::mutex> lck(mutex);
    if (dotMap.size()==0) {
        return;
    }
    throughput_(dotMap);
    dotMap.clear();
}
int dotterClientImpl::throughput_(std::map<std::string,std::map<std::string,int>> &reportMap)
{
    auto remoteThroughput=client->makeRpcCall<std::function<std::map<std::string,int>(std::map<std::string,std::map<std::string,int>>)>>("throughput");
    return remoteThroughput(reportMap)["status"];
}
int dotterClientImpl::throughput(std::string key,int value)
{
    std::chrono::time_point<std::chrono::system_clock,std::chrono::seconds> timestamp_ = 
        std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
    auto timestamp=timestamp_.time_since_epoch().count();
    {
        std::lock_guard<std::mutex> lck(mutex);
        dotMap[key][std::to_string(timestamp)]+=value;
    }
    return 0;
}
std::map<std::string,int> dotterClientImpl::getThroughput(std::string key,int fromTimeStamp,int toTimeStamp)
{
    decltype(dotterClient::getThroughput) a;
    auto remoteGetThroughput=client->makeRpcCall<decltype(&dotterClient::getThroughput)>("getThroughput");
    return remoteGetThroughput(key,fromTimeStamp,toTimeStamp);
}
dotterClientImpl::~dotterClientImpl()
{
    shutdownThread.set_value(true);
    thread.join();
    report();
}