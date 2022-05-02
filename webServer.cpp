#include "./CppHttpServer/include/tcpServer.h"
#include "./CppHttpServer/include/webServer.h"

#include "dotterClient.h"

#include "boost/algorithm/string.hpp"

#include <sstream>
#include <iomanip>
#include <cmath>
#define MAXPOINT 2000
int64_t str2TimeStamp(std::string &s,const std::string& fmt="%Y-%m-%d %H:%M:%S")
{
    boost::replace_all(s,"%20"," ");
    std::stringstream ss;
    ss<<s;
    std::tm tm={};
    ss>>std::get_time(&tm,fmt.c_str());
    return mktime(&tm);
}
std::string timeStamp2Str(int64_t timestamp,const std::string& fmt="%Y-%m-%d %H:%M:%S")
{
    std::stringstream ss;
    auto tm=std::localtime(&timestamp);
    ss<<std::put_time(tm,fmt.c_str());
    return ss.str();
}
template <typename U,typename V>
V interpolate(std::vector<U> &x,std::vector<V> &y,U target)
{
    auto pos=std::lower_bound(x.begin(),x.end(),target)-x.begin();
    if (pos == x.size()) {
        return 0;
    }
    if (pos == 0 && target<x[0]) {
        pos+=1;
        return 0;
    }
    U x0=x[pos-1];
    U x1=x[pos];
    V y0=y[pos-1];
    V y1=y[pos];
    return y0+1.0*(y1-y0)/(x1-x0)*(target-x0);
}
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
        auto fromTimeStamp=str2TimeStamp(params["from"]);
        auto toTimeStamp=str2TimeStamp(params["to"]);
        auto result=dotterClient::getThroughput(key,fromTimeStamp,toTimeStamp);
        std::map<int64_t,int> countMap;
        std::vector<std::pair<int64_t,int>> timestamps;
        for (auto &[key,value]:result)
        {
            auto timestamp=std::atoi(key.c_str());
            countMap[timestamp]=value;
            if (countMap.find(timestamp+1)==countMap.end())
            {
                countMap[timestamp+1]=0;
            }
            if (countMap.find(timestamp-1)==countMap.end())
            {
                countMap[timestamp-1]=0;
            }
        }
        timestamps.reserve(countMap.size());
        for (auto &[key,value]:countMap)
        {
            timestamps.emplace_back(std::make_pair(key,value));
        }
        std::sort(timestamps.begin(),timestamps.end(),[](std::pair<int64_t,int> &a,std::pair<int64_t,int> &b){
            return a.first<b.first;
        });
        std::vector<int64_t> x;
        std::vector<int> y;
        x.reserve(timestamps.size());
        y.reserve(timestamps.size());
        for (auto &[xn,yn]:timestamps)
        {
            x.push_back(xn);
            y.push_back(yn);
        }
        std::string arr="[]";
        JsonParser jsonArray(&arr,JsonParser::ARRAY);
        int timeInterval=ceil(1.0*(toTimeStamp-fromTimeStamp+1)/(std::min(toTimeStamp-fromTimeStamp+1,(int64_t)MAXPOINT)));
        for (int64_t now=fromTimeStamp;now<toTimeStamp;now+=timeInterval)
        {
            JsonParser json;
            json["time"]=timeStamp2Str(now);
            json["count"]=interpolate(x,y,now);
            jsonArray.add(json);
        }
        response.setJson(jsonArray);
        return response;
    });
    server->startForever();
}