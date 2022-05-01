#ifndef CACHE_H
#define CACHE_H
#include "../cache/LRUSetCache.h"
#include "../cache/rwSetCache.h"
#include "../cache/redisSet.h"
#include "../cache/redisZset.h"

#include <future>

using setCache=LRUSetCache<std::string,std::string>;
using rwCache=rwSetCache<std::string,std::string>;
class cache:public setCache,rwCache
{
public:
    cache(std::string ip,int port,bool useRWCache=true,int cacheSize=100,long syncTime=1000):
        setCache(cacheSize),rwCache(),useRWCache(useRWCache),syncTime(syncTime){
        thread=new std::thread(&cache::syncThread,this);
        redis=new redisSet(ip,port);
    }
    void syncThread();
    void insert(std::string key,std::string value);
    bool erase(std::string key,std::string value);
    void set(std::string key,std::set<std::string> value);
    std::set<std::string> get(std::string key);
    ~cache() {
        if (useRWCache) {
            stop=false;
            promise.get_future().get();
        }
        delete redis;
    }
private:
    redisSet* redis;
    long syncTime;
    bool useRWCache;
    bool stop=true;
    std::promise<bool> promise;
    std::thread* thread;
};


#endif