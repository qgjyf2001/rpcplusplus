#ifndef REDISZSET_H
#define REDISZSET_H
#include <iostream>
#include <map>
#include <vector>

#include <hiredis/hiredis.h>
//key:value:timestamp value:timestamp
class redisZset
{
private:
    std::string ip;
    int port;
    redisContext* ctx;
public:
    redisZset(std::string ip,int port);
    std::map<std::string,int>  get(std::string key,int fromTimeStamp,int toTimeStamp);
    bool add(std::string key,std::string timestamp,int value);
    ~redisZset() {
        redisFree(ctx);
    };
};

#endif