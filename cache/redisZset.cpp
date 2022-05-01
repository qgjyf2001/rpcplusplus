#include "redisZset.h"
#include "serialization.h"

#include <sstream>
#include <cstring>

#include <boost/algorithm/string.hpp>

redisZset::redisZset(std::string ip,int port) : ip(ip),port(port) {
    ctx=redisConnect(ip.c_str(),port);
    if (ctx==NULL||ctx->err==1) {
        throw std::runtime_error("redis connect error");
    }
}
bool redisZset::add(std::string key,std::string timestamp,int count) {
    if (count==0) {
        return true;
    }
    auto *reply=(redisReply*)redisCommand(ctx,"ZRANGEBYSCORE %s %s %s",key.c_str(),timestamp.c_str(),timestamp.c_str());
    if (reply==NULL) {
        return false;
    }
    if (!(reply->type == REDIS_REPLY_ARRAY && reply->elements<=1)) {
        return false;
    }
    int originCount=0;
    if (reply->elements == 1) {
        std::string replyKey=reply->element[0]->str;
        std::vector<std::string> replys;
        boost::split(replys,replyKey,boost::is_any_of(":"));
        std::string &origin=replys[0];
        originCount=std::atoi(origin.c_str());
        reply=(redisReply*)redisCommand(ctx,"ZREM %s %s",key.c_str(),replyKey.c_str());
        if (reply==NULL) {
            return false;
        }
    }
    char buffer[256]={0};
    sprintf(buffer,"%d:%s",originCount+count,timestamp.c_str());
    reply=(redisReply*)redisCommand(ctx,"ZADD %s %s %s",key.c_str(),timestamp.c_str(),buffer);
    if (reply==NULL) {
        return false;
    }
    return true;
}
std::map<std::string,int> redisZset::get(std::string key,int fromTimeStamp,int toTimeStamp)
{
    if (toTimeStamp<fromTimeStamp) {
        return {{}};
    }
    auto *reply=(redisReply*)redisCommand(ctx,"ZRANGEBYSCORE %s %d %d withscores",key.c_str(),fromTimeStamp,toTimeStamp);
    if (reply==NULL) {
        return {{}};
    }
    if (!reply->type == REDIS_REPLY_ARRAY) {
        return {{}};
    }
    int elements=reply->elements/2;
    std::map<std::string,int> result;
    for (int i=0;i<elements;i++)
    {
        std::string replyKey=reply->element[i*2]->str;
        std::vector<std::string> replys;
        boost::split(replys,replyKey,boost::is_any_of(":"));
        std::string &origin=replys[0];
        std::string timestamp=reply->element[i*2+1]->str;
        result[timestamp]=std::atoi(origin.c_str());
    }
    return result;
}