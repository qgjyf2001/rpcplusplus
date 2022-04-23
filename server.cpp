#include "jsonParser.h"
#include "rpcHandler.h"
#include "rpcParser.h"
#include "tcpServer.h"
#include "serialization.h"

#include <gflags/gflags.h>

DEFINE_int32(port,10011,"port");

std::vector<std::string>  add(std::vector<std::string> a,std::vector<long long> b)
{
    std::cout<<"[add]"<<std::endl;
    if (a.size()!=b.size())
        return std::vector<std::string>();
    std::vector<std::string> result;
    int len=a.size();
    for (int i=0;i<len;i++)
        result.emplace_back(a[i]+std::to_string(b[i]));
    return result;
}
std::vector<std::string> add2(std::vector<std::pair<int,std::string>> a,int b)
{
    std::vector<std::string> result;
    for (auto &&each:a)
    {
        auto &&[u,v]=each;
        result.push_back(std::to_string(u)+v+std::to_string(b));
    }
    return result;
}
int main(int argc,char** argv)
{    
    google::ParseCommandLineFlags(&argc,&argv,true);
    rpcHandler* handler=new rpcHandler("test.service");
    handler->AddRpcHandler(add);
    handler->AddRpcHandler(add2);
    poolServer *server=new poolServer(handler,FLAGS_port);
    server->startForever();
    return 0;
}