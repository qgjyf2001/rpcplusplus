#ifndef LRUSETCACHE_H
#define LRUSETCACHE_H
#include <set>

#include "LRUCache.h"

template <typename K,typename V>
class LRUSetCache:public LRUCache<K,std::set<V>>
{
private:
    /* data */
public:
    LRUSetCache(int cacheSize=100):LRUCache<K,std::set<V>>(cacheSize){

    };
    void insert(K key,V value) {
        std::lock_guard<std::mutex> lck(this->mutex);
        this->data[key].insert(value);
    }
    bool erase(K key,V value) {
        std::lock_guard<std::mutex> lck(this->mutex);
        bool result=(this->data.find(key)!=this->data.end());
        if (result)
            this->data[key].erase(value);
        return result;
    }
    bool exist(K key) {
        std::lock_guard<std::mutex> lck(this->mutex);
        return (this->data.find(key)!=this->data.end());
    }
};

#endif