#include "cache.h"
void cache::insert(std::string key,const std::string value) {
    if (!setCache::exist(key))
        get(key);
    setCache::insert(key,value);
    if (!useRWCache) {
        std::set<std::string> val={value};
        redis->insert(key,val);
        return;
    }
    rwCache::insert(key,value);
}
bool cache::erase(std::string key,std::string value) {
    if (!setCache::exist(key))
        get(key);
    auto result=setCache::erase(key,value);
    if (!useRWCache) {
        std::set<std::string> val={value};
        redis->erase(key,val);
        return result;
    }
    if (result)
        rwCache::erase(key,value);
    return result;
}
void cache::set(std::string key,std::set<std::string> value) {
    setCache::set(key,value);
    rwCache::set(key,value);
    redis->set(key,value);
}
std::set<std::string>  cache::get(std::string key) {
    bool result;
    auto res=setCache::get(key,&result);
    if (!result) {
        auto value=redis->get(key);
        setCache::set(key,value);
        return value;
    } else {
        return res;
    }
}
void cache::syncThread() {
    while (stop) {
        std::this_thread::sleep_for(std::chrono::microseconds(syncTime));
        auto [data,removed]=rwSetCache::reset();
        for (auto &[key,value]:data) {
            redis->insert(key,value);
        }
        for (auto &[key,value]:removed) {
            redis->erase(key,value);
        }
    }
    promise.set_value(true);
    return;
}