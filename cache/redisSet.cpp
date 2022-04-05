#include "redisSet.h"
#include "serialization.h"

#include <sstream>
#include <cstring>

redisSet::redisSet(std::string ip,int port) : ip(ip),port(port) {
    ctx=redisConnect(ip.c_str(),port);
    if (ctx==NULL||ctx->err==1) {
        throw std::runtime_error("redis connect error");
    }
}
std::set<std::string> redisSet::get(std::string key) {
    auto *reply=(redisReply*)redisCommand(ctx,"SMEMBERS %s",key.c_str());
    if (reply==NULL) {
        return std::set<std::string>();
    }
    if (!(reply->type == REDIS_REPLY_ARRAY && reply->elements>0)) {
        return std::set<std::string>();
    }
    std::set<std::string> result;
    for (int i=0;i<reply->elements;i++) {
        result.insert(reply->element[i]->str);
    }
    return result;
}
void redisSet::toStringLiteral(std::set<std::string> &value,std::vector<const char*>& result) {
    for (auto &v:value) 
        result.push_back(v.data());
}
bool redisSet::insert(std::string key,std::set<std::string> &value) {
    if (value.size()==0) 
        return true;
    std::vector<const char*> values;
    values.push_back("SADD");
    values.push_back(key.c_str());
    toStringLiteral(value,values);
    std::vector<size_t> size;
    for (auto &val:values)
        size.push_back(strlen(val));
    redisAppendCommandArgv(ctx,values.size(),values.data(),size.data());
    redisReply *reply;
    redisGetReply(ctx,(void**)&reply);
    if (reply==NULL) {
        return false;
    }
    if (!(reply->type == REDIS_REPLY_INTEGER && reply->elements==value.size())) {
        return false;
    }
    return true;
}
bool redisSet::erase(std::string key,std::set<std::string> &value) {
    if (value.size()==0) 
        return true;
    std::vector<const char*> values;
    values.push_back("SREM");
    values.push_back(key.c_str());
    toStringLiteral(value,values);
    std::vector<size_t> size;
    for (auto &val:values)
        size.push_back(strlen(val));
    redisAppendCommandArgv(ctx,values.size(),values.data(),size.data());
    redisReply *reply;
    redisGetReply(ctx,(void**)&reply);
    if (reply==NULL) {
        return false;
    }
    if (!(reply->type == REDIS_REPLY_INTEGER && reply->elements==value.size())) {
        return false;
    }
    return true;
}
bool redisSet::set(std::string key,std::set<std::string>&value) {
    auto *reply=(redisReply*)redisCommand(ctx,"DEL %s",key.c_str());
    if (reply==NULL)
        return false;
    return insert(key,value);
}