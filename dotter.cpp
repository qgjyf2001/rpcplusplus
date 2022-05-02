#include "jsonParser.h"
#include "rpcHandler.h"
#include "cache.h"
#include "tcpServer.h"
#include "serialization.h"
#include "config.h"

#include <gflags/gflags.h>

DEFINE_int32(port,8000,"port");
redisZset zsetClient("127.0.0.1",6379);
std::map<std::string,int> throughput(std::map<std::string,std::map<std::string,int>> reportMap)
{
    bool success=true;
    for (auto &[key,value]:reportMap) {
        for (auto &[timestamp,count]:value) {
            success&=zsetClient.add(key,timestamp,count);
        }
    }
    return {{"status",success}};
}
std::map<std::string,int> getThroughput(std::string key,int fromTimeStamp,int toTimeStamp)
{
    return zsetClient.get(key,fromTimeStamp,toTimeStamp);
}
int main(int argc,char** argv)
{    
    google::ParseCommandLineFlags(&argc,&argv,true);
    rpcHandler* handler=new rpcHandler(config::dotterService,true);
    handler->AddRpcHandler(throughput);
    handler->AddRpcHandler(getThroughput);
    poolServer *server=new poolServer(handler,FLAGS_port);
    server->startForever();
    return 0;
}