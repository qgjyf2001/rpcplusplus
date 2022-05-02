#include "rpcParser.h"
rpcMessage rpcParser::parse(std::string rpcRequest)
{
    for (int i=0;i<RPCMESSAGELENGTH;i++)
        if (!isdigit(rpcRequest[i]))
            throw std::invalid_argument("invalid request");
    int length=std::atoi(rpcRequest.substr(0,RPCMESSAGELENGTH).c_str());
    if (rpcRequest[RPCMESSAGELENGTH]!='\r'||rpcRequest[RPCMESSAGELENGTH+1]!='\n')
        throw std::invalid_argument("invalid request");
    int len=rpcRequest.length();
    if (len-RPCMESSAGELENGTH-4>length)
        throw std::invalid_argument("invalid request");
    else if (len-RPCMESSAGELENGTH-4==length)
    {
        if (rpcRequest[len-2]!='\r'||rpcRequest[len-1]!='\n')
            throw std::invalid_argument("invalid request");
        return rpcMessage(length,rpcRequest.substr(RPCMESSAGELENGTH+2,length));
    }
    else
        return rpcMessage(length,rpcRequest.substr(RPCMESSAGELENGTH+2));
}