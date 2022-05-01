#include "./CppHttpServer/include/tcpServer.h"
#include "./CppHttpServer/include/webServer.h"

#include "dotterClient.h"
int main()
{
    dotterClient::init();
    auto *handler=new httpHandler();
    tcpServer *server=new poolServer(handler,3000,"0.0.0.0");
    auto *web=new webServer(handler);
    web->addHttpHook("/","./app/build/");
    handler->setGetHandler("/api/getDot",[](httpRequestType &request,std::map<std::string,std::string>&params){
        auto response=http200BasicResponse();
        auto key=params["key"];
        auto fromTimeStamp=params["from"];
        auto toTimeStamp=params["to"];
        auto result=dotterClient::getThroughput(key,std::atoi(fromTimeStamp.c_str()),std::atoi(toTimeStamp.c_str()));
        response.setJson(serialize::doSerialize(result));
        return response;
    });
    server->startForever();
}