#ifndef RWSETCACHE_H
#define RWSETCACHE_H
#include <map>
#include <set>
#include <shared_mutex>

template <typename K,typename V>
class rwSetCache
{
private:
    std::map<K,std::set<V>> data;
    std::map<K,std::set<V>> removed;
    std::shared_mutex mutex1,mutex2;
public:
    void erase(K key,V value) {
        bool inSet;
        {
            std::shared_lock<std::shared_mutex> lck(mutex1);
            inSet=data.find(key)!=data.end()&&data[key].find(value)!=data[key].end();
        }
        if (inSet) {
            std::unique_lock<std::shared_mutex> lck(mutex1);
            data[key].erase(value);
            if (data[key].size()==0)
                data.erase(key);
        }
        else {
            std::unique_lock<std::shared_mutex> lck(mutex2);
            removed[key].insert(value);
        }
    } 
    void insert(K key,V value) {
        bool inSet;
        {
            std::shared_lock<std::shared_mutex> lck(mutex2);
            inSet=removed.find(key)!=removed.end()&&removed[key].find(value)!=removed[key].end();
        }
        if (inSet) {
            std::unique_lock<std::shared_mutex> lck(mutex2);
            removed[key].erase(key);
            if (removed[key].size()==0)
                removed.erase(key);
        }
        std::unique_lock<std::shared_mutex> lck(mutex1);
        data[key].insert(value);
    }
    void set(K key,std::set<V> value) {
        {
            std::unique_lock<std::shared_mutex> lck(mutex2);
            removed.erase(key);
        }
        {
            std::unique_lock<std::shared_mutex> lck(mutex1);
            data.erase(key);
        }
    }
    auto reset() {
        std::unique_lock<std::shared_mutex> lck1(mutex1);
        std::unique_lock<std::shared_mutex> lck2(mutex2);
        auto result=std::make_pair(std::move(data),std::move(removed));
        data=std::map<K,std::set<V>>();
        removed=std::map<K,std::set<V>>();
        return result;
    }
};

#endif