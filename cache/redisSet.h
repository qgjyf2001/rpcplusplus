#ifndef REDISSET_H
#define REDISSET_H
#include <iostream>
#include <set>
#include <vector>

#include <hiredis/hiredis.h>

class redisSet
{
private:
    std::string ip;
    int port;
    redisContext* ctx;
public:
    redisSet(std::string ip,int port);
    std::set<std::string> get(std::string key);
    bool set(std::string key,std::set<std::string> &value);
    bool insert(std::string key,std::set<std::string> &value);
    void toStringLiteral(std::set<std::string> &value,std::vector<const char*> &result);
    bool erase(std::string key,std::set<std::string>& value);
    ~redisSet() {
        redisFree(ctx);
    };
};

#endif