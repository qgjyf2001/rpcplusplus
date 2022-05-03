#include "rpcClient.h"

#include <thread>
#include <cassert>

#include "testClass.h"

using addFunc=std::function<std::vector<std::string>(std::vector<std::string>,std::vector<long long>)>;
using add2Func=std::function<testClass(testClass,testClass)>;

int main()
{    
    auto client=std::make_shared<rpcClient>("test.service",rpcClient::CLIENT);
    auto add2=client->makeRpcCall<add2Func>("add2");
    testClass a(1,"2",3,"4");
    testClass b(2,"a");//b(2,"a",1234,"abcd")
    auto result=add2(a,b);
    assert(result.a==3);
    assert(result.b=="2a");
    assert(result.c==1237);
    assert(result.d=="4abcd");
    while (true) 
    {
        std::vector<std::future<std::vector<std::string>>> vec;
        std::vector<std::shared_ptr<rpcClient>> clients;
        for (int i=0;i<200;i++) {
            auto client=std::make_shared<rpcClient>("test.service",rpcClient::CLIENT);
            clients.push_back(client);
            auto add=client->makeRpcSyncCall<addFunc>("add");
            std::vector<std::string> a={"abcd","efgh","ijkl"};
            std::vector<long long> b={12345,12346,12347};
            std::vector<long long> c={54321,64321,74321};
            auto result=add(a,b);
            vec.push_back(std::move(result));
        }
        for (auto &v:vec) {
            auto result=v.get();
            std::cout<<serialize::doSerialize(result)<<std::endl;
        }
        std::cout<<"next..."<<std::endl;
    }
    return 0;
}