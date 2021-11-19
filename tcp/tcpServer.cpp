#include "tcpServer.h"
poolServer::poolServer(rpcHandler* handler,int port,std::string ipAddress,int maxClient)
{
    signal(SIGPIPE , SIG_IGN);
    this->port=port;
    this->ipAddress=ipAddress;
    this->maxClient=maxClient;
    this->handler=handler;
}
void poolServer::startForever()
{
    auto RpcSever=rpcServer(handler);
    //init
    sockaddr_in servaddr,clientaddr;
    auto listenfd=socket(AF_INET,SOCK_STREAM,0);
    if (listenfd==-1)
        throw std::runtime_error("socket error");
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    inet_pton(AF_INET,ipAddress.c_str(),&servaddr.sin_addr);
    servaddr.sin_port=htons(port);
    if (bind(listenfd,(sockaddr*)&servaddr,sizeof(servaddr))==-1)
        throw std::runtime_error("bind error");

    //listen
    listen(listenfd,LISTENQ);

    //accept
    auto *clientfd=new pollfd[maxClient];
    for (int i=1;i<maxClient;i++)
    {
        clientfd[i].fd=-1;//ignored
        clientfd[i].events=0;
    }
    clientfd[0].fd=listenfd;
    clientfd[0].events=POLLIN;
    clientfd[0].revents=0;
    int totfd=0;
    while (true)
    {
        auto nready=poll(clientfd,totfd+1,-1);
        if (nready==-1)
            throw std::runtime_error("poll error");
        if (clientfd[0].revents&POLLIN)
        {
            socklen_t clientAddrLen=sizeof(clientaddr);
            auto connfd=accept(listenfd,(sockaddr*)&clientaddr,&clientAddrLen);
            if (connfd==-1)
                if(errno == EINTR)
                    continue;
                else
                    throw std::runtime_error("accept error");
            int i;
            for (i=1;i<maxClient;i++)
                if (clientfd[i].fd<0)
                {            
                    clientfd[i].events=POLLIN;
                    clientfd[i].fd=connfd;
                    break;
                }
            if (i==maxClient)//server busy
            {
                close(connfd);
                continue;
            }
            totfd=std::max(i,totfd);
        }
        else
        {
            char buf[MAXLINE];
            for (int i=1;i<maxClient;i++)
            {
                if (clientfd[i].fd<0)
                    continue;
                if (clientfd[i].revents&POLLIN)
                {
                    auto n=read(clientfd[i].fd,buf,MAXLINE);
                    if (n<=0)
                    {
                        RpcSever.free(clientfd[i].fd);
                        close(clientfd[i].fd);
                        clientfd[i].fd=-1;
                        continue;
                    }
                    RpcSever.doRpc(&clientfd[i].fd,std::string(buf,n),[](int* sockfd){
                            close(*sockfd);
                            *sockfd=-1;
                    });                    
                }
            }
        }
    }
    
}
poolServer::~poolServer()
{
}
