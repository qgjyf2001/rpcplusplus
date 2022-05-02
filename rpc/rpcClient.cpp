#include "rpcClient.h"
#include "tcpServer.h"

#include <boost/algorithm/string.hpp>
rpcClient::rpcClient(std::vector<std::pair<std::string,int>> ipports)
{
    for (auto ipport:ipports) {
        connectOne(ipport.first,ipport.second,fdCache);
    }
    if (fdCache.get("connected").size()==0) {
        throw std::runtime_error("connect error");
    }
}
rpcClient::rpcClient(std::string hostIp,int hostPort):rpcClient({std::make_pair(hostIp,hostPort)}) {

}
rpcClient::rpcClient(std::string service,connectionType type,std::vector<std::pair<std::string,int>> ipports){
    if (type==SERVER) {
        new (this) rpcClient(ipports);
        this->service=service;
    } else {
        for (auto ipport:ipports) {
            connectOne(ipport.first,ipport.second,serviceFdCache);
        }
        service_=service;
        refreshFdCache();
        if (fdCache.get("connected").size()==0) {
            throw std::runtime_error("connect error");
        }
        refreshThread=new std::thread([&](){
            auto shutdown=shutdownRefersh.get_future();
            while (shutdown.wait_for(std::chrono::seconds(10))==std::future_status::timeout) {
                refreshFdCache();
            }
        });
    }

}
void rpcClient::refreshFdCache() {
    rpcSender sender;
    auto rpc=sender.sendRPC(service_);
    rpc["name"]="getService";
    auto rpcResult=remoteCall(rpc,serviceFdCache);
    auto ipports=sender.getRPC<std::set<std::string>>(rpcResult);
    for (auto ipport:ipports) {
        std::vector<std::string> ipAndPort;
        boost::split(ipAndPort,ipport,boost::is_any_of(":"));
        connectOne(ipAndPort[0],std::atoi(ipAndPort[1].c_str()),fdCache);
    }
    
}
bool rpcClient::connectOne(std::string &ip,int port,fdCacheType &mCache) {
    std::lock_guard<std::mutex> lck(mutex);
    std::string ipport=ip+":"+std::to_string(port);
    if (connectedIpports.find(ipport)!=connectedIpports.end()) {
        return true;
    }
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if (sockfd==-1)
        return false;
    sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    inet_pton(AF_INET,ip.c_str(),&addr.sin_addr);
    if (connect(sockfd,(sockaddr*)&addr,sizeof(addr))<0) {
        close(sockfd);
        return false;
    }
    sockaddr_in sa;
    socklen_t len=sizeof(sa);
    if (!getsockname(sockfd,(sockaddr*)&sa,&len)) {
        ipPort=std::string(inet_ntoa(sa.sin_addr))+":"+std::to_string(ntohs(sa.sin_port));
    }
    connectedIpportsMap[sockfd]=ipport;
    connectedIpports.insert(ipport);
    mCache.insert("connected",sockfd);
    return true;
}
JsonParser rpcClient::remoteCall(JsonParser json) {
    return remoteCall(json,fdCache);
}
JsonParser rpcClient::remoteCall(JsonParser json,rpcClient::fdCacheType& mCache)
{
    std::lock_guard<std::mutex> rpcLck(rpcMutex);
    while (true) {
        auto sockfds=mCache.get("connected");
        auto sz=sockfds.size();
        if (sz==0) {
            throw std::runtime_error("connect error");
        }
        int sockfd=*(sockfds.begin());
        for (auto fd:sockfds) {
            if (rand()%sz==0)
            {
                sockfd=fd;
                break;
            }
        }
        try {
            if (service!="") {
                rpcSender sender;
                JsonParser tmp;
                tmp=sender.sendRPC(service,json);
                tmp["name"]="requestService";
                tmp["param1"]["ip"]=ipPort;
                json=tmp;
            }
            std::string msg=rpcMessage(json).toString();
            write(sockfd,msg.c_str(),msg.length());
            char buf[MAXLINE];
            auto n=read(sockfd,buf,MAXLINE);
            std::string message(buf,n);
            auto rpcRequest=rpcParser::parse(message);
            if (rpcRequest.length>rpcRequest.message.length())
            {
                std::string now=message;
                int length=rpcRequest.length+RPCMESSAGELENGTH+4;
                while (now.length()<length)
                {
                    auto n=read(sockfd,buf,MAXLINE);
                    now+=std::string(buf,n);
                }
                rpcRequest=rpcParser::parse(now);
            }
            return JsonParser(&rpcRequest.message);
        }
        catch (std::exception& e) {
            bool alive=false;
            for (int i=0;i<2;i++)
                try
                {
                    rpcSender sender;
                    auto pingRequest=sender.sendRPC("ping").toString();
                    write(sockfd,pingRequest.c_str(),pingRequest.length());
                    char buf[MAXLINE];
                    auto n=read(sockfd,buf,MAXLINE);
                    std::string message(buf,n);
                    auto rpcMsg=rpcParser::parse(message).message;
                    auto rpcResponse=JsonParser(&rpcMsg);
                    auto pong=sender.getRPC<std::string>(rpcResponse);
                    if (pong=="pong") {
                        alive=true;
                        break;
                    }
                }
                catch(const std::exception& e)
                {
                }
                
            if (alive) {
                throw e;
            } else {

                std::lock_guard<std::mutex> lck(mutex);
                std::string ipport=connectedIpportsMap[sockfd];
                connectedIpports.erase(ipport);
                connectedIpportsMap.erase(sockfd);
                close(sockfd);
                mCache.erase("connected",sockfd);
            }
        }
    }
}
rpcClient::~rpcClient()
{
    if (refreshThread!=nullptr) {
        shutdownRefersh.set_value(true);
        refreshThread->join();
    }
    for (auto sockfd:fdCache.get("connected"))
        close(sockfd);
    for (auto sockfd:serviceFdCache.get("connected"))
        close(sockfd);
}