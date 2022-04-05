#ifndef BASEBALANCER_H
#define BASEBALANCER_H
#include "jsonParser.h"
#include "rpcClient.h"
#include "cache.h"
class baseBalancer
{
public:
    JsonParser balanceCall(cache& serviceCache,std::string service,JsonParser params);
protected:
    virtual void recordTimeCost(std::string& ip,time_t timecost);
    virtual std::string getIP(std::set<std::string>& ipports,std::string fromIpPort);
    LRUCache<std::string,clock_t> ipTimeCostCache;
};

#include "../balancers/minimumBalancer.h"
#include "../balancers/hashBalancer.h"
#include "../balancers/connectionBalancer.h"
#endif