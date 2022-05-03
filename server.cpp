#include "jsonParser.h"
#include "rpcHandler.h"
#include "rpcParser.h"
#include "tcpServer.h"
#include "serialization.h"
#include "testClass.h"

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
testClass add2(testClass a,testClass b)
{
    return testClass(a.a+b.a,a.b+b.b,a.c+b.c,a.d+b.d);
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