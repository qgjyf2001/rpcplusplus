#ifndef WEBSERVER_H
#define WEBSERVER_H
#include "httpHandler.h"
#include "folderHook.h"
class webServer
{
private:
    httpHandler* handler;
    folderHook* hook;
    void scanDir(std::string &serveUrl,std::string &servePath,std::string path);
public:
    webServer(httpHandler* handler);
    void addHttpHook(std::string serveUrl="/",std::string servePath="./app/");
    ~webServer();
};

#endif