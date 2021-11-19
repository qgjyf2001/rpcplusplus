#include "jsonParser.h"
#include "rpcHandler.h"
#include "rpcParser.h"
#include "tcpServer.h"
#include "serialization.h"
std::vector<std::string>  add(std::vector<std::string> a,std::vector<long long> b)
{
    if (a.size()!=b.size())
        return std::vector<std::string>();
    std::vector<std::string> result;
    int len=a.size();
    for (int i=0;i<len;i++)
        result.emplace_back(a[i]+std::to_string(b[i]));
    return result;
}
int main()
{    
    
    rpcHandler* handler=new rpcHandler();
    /*rpcSender sender;
    handler.addRpcHandler("add",add);  
    std::vector<std::string> a={"aaaa","bbbb","cccc"};
    std::vector<long long> b={12345,23456,34567};
    JsonParser rpc=sender.sendRPC(a,b);
    rpc["name"]="add";

    JsonParser response=handler.handleRPC(rpc);
    auto result=sender.getRPC<std::vector<std::string>>(response);
    for (auto each:result)
        std::cout<<each<<std::endl;*/
    handler->addRpcHandler("add",add);

    poolServer *server=new poolServer(handler,8080);
    server->startForever();
    return 0;
}
