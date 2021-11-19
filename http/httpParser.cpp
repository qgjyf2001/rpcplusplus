#include "httpParser.h"
std::string httpParser::parseToken(std::string& str,int &pos,int &length,char token)
{
    int origin=pos;
    while (pos<length&&str[pos]!=token)
        pos++;
    if (pos==length&&str[pos]!=token)
        throw std::runtime_error("invalid http request!");
    std::string result=str.substr(origin,pos-origin);
    pos+=1;
    return result;
}
httpRequestType httpParser::parse(std::string httpRequest)
{
    httpRequestType result;
    int pos=0;
    int length=httpRequest.length();
    result["method"]=parseToken(httpRequest,pos,length,' ');
    result["url"]=parseToken(httpRequest,pos,length,' ');
    result["version"]=parseToken(httpRequest,pos,length,'\r');
    pos+=1;           
    while (++pos!=length&&httpRequest[pos]!='\r')
    {
        auto key=parseToken(httpRequest,pos,length,':');
        result[key]=parseToken(httpRequest,pos,length,'\r');
    }
    pos+=2;
    if (pos!=length)
        result["text"]=httpRequest.substr(pos,length-pos);
    return result;
}