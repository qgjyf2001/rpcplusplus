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
int main()
{    
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
    rpcHandler* handler=new rpcHandler();
    handler->AddRpcHandler(add);
    handler->AddRpcHandler(add2);
    poolServer *server=new poolServer(handler,8080);
    server->startForever();
    return 0;
}
