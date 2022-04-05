#ifndef CONNECTIONBALANCER_H
#define CONNECTIONBALANCER_H
#include "baseBalancer.h"
class connectionBalancer:public baseBalancer
{
protected:
    class LRUAtomicMapCache:public LRUCache<std::string,std::atomic<int>>
    {
    public:
        LRUAtomicMapCache(int cacheSize=100):LRUCache<std::string,std::atomic<int>>(cacheSize) {

        }
        int get(std::string key) {
            std::lock_guard<std::mutex> lck(mutex);
            bool result=data.find(key)!=data.end();
            if (result) {
                if (iterMap.find(key)!=iterMap.end()) {
                    auto iter=iterMap[key];
                    recentGets.erase(iter);
                }
                recentGets.push_back(key);
                iterMap[key]=--recentGets.end();
                return data[key]; 
            } else {
                return 0;
            }
        }
        void increase(std::string key) {
            std::lock_guard<std::mutex> lck(mutex);
            if (data.find(key)==data.end()) {
                if (data.size()==cacheSize) {
                    auto last=recentGets.begin();
                    iterMap.erase(*last);
                    recentGets.erase(last);
                    data.erase(*last);
                }
                data[key]=1;
                recentGets.push_back(key);
                iterMap[key]=--recentGets.end();
            } else {
                data[key]+=1;
            }
        }
        void decrease(std::string key) {
            std::lock_guard<std::mutex> lck(mutex);
            if (data.find(key)!=data.end()) {
                data[key]-=1;
            }
        }
    };
    virtual void recordTimeCost(std::string& ip,time_t timecost) override;
    virtual std::string getIP(std::set<std::string>& ipports,std::string fromIpPort) override;
    LRUAtomicMapCache ipConnectionCache;
};
#endif