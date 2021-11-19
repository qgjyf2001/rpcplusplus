#include "rpcParser.h"
rpcMessage rpcParser::parse(std::string rpcRequest)
{
    for (int i=0;i<5;i++)
        if (!isdigit(rpcRequest[i]))
            throw std::invalid_argument("invalid request");
    int length=std::atoi(rpcRequest.substr(0,5).c_str());
    if (rpcRequest[5]!='\r'||rpcRequest[6]!='\n')
        throw std::invalid_argument("invalid request");
    int len=rpcRequest.length();
    if (len-9>length)
        throw std::invalid_argument("invalid request");
    else if (len-9==length)
    {
        if (rpcRequest[len-2]!='\r'||rpcRequest[len-1]!='\n')
            throw std::invalid_argument("invalid request");
        return rpcMessage(len-9,rpcRequest.substr(7,length));
    }
    else
        return rpcMessage(len-7,rpcRequest.substr(7));
}