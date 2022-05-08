#include "rpcClient.h"
#include "kafka.h"

#include <boost/algorithm/string.hpp>
#include <gflags/gflags.h>

#include "signal.h"

DEFINE_string(service,"test.service","service");
DEFINE_int32(qps,1,"qps");

kafkaConsumer* consumer;
volatile bool isTerminate=false;

void exitHandler(int signal)
{
    isTerminate=true;
}
int main(int argc,char** argv)
{
    google::ParseCommandLineFlags(&argc,&argv,true);
    signal(SIGINT,exitHandler);
    std::string service(FLAGS_service);
    rpcClient client(service);
    boost::replace_all(service,".","_");
    consumer=new kafkaConsumer(service);
    int timeout=1000/FLAGS_qps;
    while (true)
    {
        if (isTerminate)
        {
            std::cout<<"[deleting consumer]"<<std::endl;
            delete consumer;
            std::cout<<"[deleting consumer]done"<<std::endl;
            break;
        }
        auto asyncResult=consumer->consume(timeout);
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
        auto result=asyncResult.get();
        if (!result.empty())
        {
            JsonParser json(new std::string(result));
            json["pressure"]="tagged";
            std::cout<<client.remoteCall(json)<<std::endl;
        }
    }
    return 0;
}
