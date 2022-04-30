#include "rpcClient.h"
using addFunc=std::function<std::vector<std::string>(std::vector<std::string>,std::vector<long long>)>;
using add2Func=std::function<std::vector<std::string>(std::vector<std::pair<int,std::string>> a,int b)>;

#include <thread>
int main()
{    
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