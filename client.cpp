#include "rpcClient.h"
using addFunc=std::function<std::vector<std::string>(std::vector<std::string>,std::vector<long long>)>;
using add2Func=std::function<std::vector<std::string>(std::vector<std::pair<int,std::string>> a,int b)>;

#include <thread>
int main()
{    
    std::vector<std::thread*> vec;
    for (int i=0;i<1000;i++)
    vec.push_back(new std::thread([](){
        rpcClient client("test.service");
        auto add=client.makeRpcCall<addFunc>("add");
        auto add2=client.makeRpcCall<add2Func>("add2");
        std::vector<std::string> a={"abcd","efgh","ijkl"};
        std::vector<long long> b={12345,12346,12347};
        std::vector<long long> c={54321,64321,74321};
        auto result=add(a,b);
        result=add(result,c);
        std::vector<std::pair<int,std::string>> d={{12345,"abcd"},{12346,"efgh"},{12347,"ijkl"}};
        result=add2(d,54321);
        for (auto each:result)
        {
            std::cout<<each<<std::endl;
        }
    }));
    for (auto v:vec)
        v->join();
    return 0;
}