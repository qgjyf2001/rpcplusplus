#ifndef RPCSERVER_H
#define RPCSERVER_H
#include <signal.h>
#include <unistd.h>

#include "threadPool.h"
#include "rpcParser.h"
#include "rpcHandler.h"
class rpcServer
{
private:
    rpcHandler *handler;
    threadPool* pool;
    std::map<int,std::pair<std::string,int>> uncompleted; 
    std::map<int,std::future<std::string>> futureMap;
public:
    rpcServer(rpcHandler* handler,int maxThreads=4);
    void doRpc(int sockfd,std::string httpRequest);
    void free(int sockfd);
    std::string getResult(int sockfd,bool& result);
    void registService(std::string service,std::string ip,int port);
    ~rpcServer();
};

#endif