#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <iostream>
#include <string.h>

#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <signal.h>

#include "rpcServer.h"
#define MAXLINE 1024
#define LISTENQ 5

class poolServer
{
private:
    int port;
    std::string ipAddress;
    int maxClient;
    rpcHandler* handler;
public:
    poolServer(rpcHandler* handler,int port,std::string ipAddress="127.0.0.1",int maxClient=65536);
    void startForever();
    ~poolServer();
};

#endif