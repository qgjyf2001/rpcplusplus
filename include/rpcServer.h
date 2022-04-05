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
public:
    rpcServer(rpcHandler* handler,int maxThreads=4);
    void doRpc(int* sockfd,std::string httpRequest,std::function<void(int*)> handleClose);
    void free(int sockfd);
    void registService(std::string service,std::string ip,int port);
    ~rpcServer();
};

#endif