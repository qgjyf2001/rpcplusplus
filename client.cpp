#include "rpcClient.h"
using addFunc=std::function<std::vector<std::string>(std::vector<std::string>,std::vector<long long>)>;
using add2Func=std::function<std::vector<std::string>(std::vector<std::pair<int,std::string>> a,int b)>;

int main()
{    
    rpcClient client("127.0.0.1",8080);
    auto add=client.makeRpcCall<addFunc>("add");
    auto add2=client.makeRpcCall<add2Func>("add2");
    std::vector<std::string> a={"abcd","efgh","ijkl"};
    std::vector<long long> b={12345,12346,12347};
    std::vector<long long> c={54321,64321,74321};
    auto result=add(a,b);
    for (auto each:result)
    {
        std::cout<<each<<std::endl;
    }
    result=add(result,c);
    for (auto each:result)
    {
        std::cout<<each<<std::endl;
    }
    std::vector<std::pair<int,std::string>> d={{12345,"abcd"},{12346,"efgh"},{12347,"ijkl"}};
    result=add2(d,54321);
    for (auto each:result)
    {
        std::cout<<each<<std::endl;
    }
    return 0;
}
