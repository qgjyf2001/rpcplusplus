#ifndef LRUCACHE_H
#define LRUCACHE_H
#include <map>
#include <list>
#include <mutex>

template <typename K,typename V>
class LRUCache{
public:
    LRUCache(int cacheSize=100):cacheSize(cacheSize) {
    }
    V get(K key) {
        bool result;
        return get(key,&result);
    }
    V get(K key,bool* result) {
        std::lock_guard<std::mutex> lck(mutex);
        *result=data.find(key)!=data.end();
        if (*result) {
            if (iterMap.find(key)!=iterMap.end()) {
                auto iter=iterMap[key];
                recentGets.erase(iter);
            }
            recentGets.push_back(key);
            iterMap[key]=--recentGets.end();
            return data[key]; 
        } else {
            return V();
        }
    }
    void set(K key,V value) {
        std::lock_guard<std::mutex> lck(mutex);
        if (data.find(key)==data.end()) {
            if (data.size()==cacheSize) {
                auto last=recentGets.begin();
                iterMap.erase(*last);
                recentGets.erase(last);
                data.erase(*last);
            }
            data[key]=value;
            recentGets.push_back(key);
            iterMap[key]=--recentGets.end();
        } else {
            data[key]=value;
        }
    }
protected:
    std::mutex mutex;
    std::map<K,V> data;
    std::list<K> recentGets;
    std::map<K,typename std::list<K>::iterator> iterMap;
    int cacheSize;
};
#endif