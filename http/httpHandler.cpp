#include "httpHandler.h"
std::map<std::string,std::string> httpHandler::getParams(std::string &params,int offset)
{
    std::map<std::string,std::string> requestParam;
    int len=params.length();
    for (int forwardPtr=len-1,backwardPtr=len-1,midPtr=-1;forwardPtr>=offset;forwardPtr--)
        {
            if (params[forwardPtr]=='=')
                midPtr=forwardPtr;
            else if (params[forwardPtr]=='&'||params[forwardPtr]=='?')
            {
                if (midPtr==-1)
                    requestParam[params.substr(forwardPtr+1,backwardPtr-forwardPtr)]="";
                else
                    requestParam[params.substr(forwardPtr+1,midPtr-forwardPtr-1)]=params.substr(midPtr+1,backwardPtr-midPtr);
                backwardPtr=forwardPtr-1;
                midPtr=-1;
            }
        }
    return requestParam;
}
httpResponse httpHandler::handleRequest(httpRequestType request)
{
    httpResponse response;
    auto& url=request["url"];
    int i;
    auto len=url.length();
    std::map<std::string,std::string> requestParam;
    for (i=len-1;i>=0;i--)
        if (url[i]=='?')
            break;
    if (i>0)
    {
        requestParam=getParams(url,i);
        url=url.substr(0,i);
    }        
    if (auto &method=request["method"];method=="GET")
    {
        
        auto it=getHandlers.find(url);
        if (it==getHandlers.end())
        {
            response=http404BasicResponse();
        }
        else
        {
            return getHandlers[url](request,requestParam);
        }
        return response;
    }
    else if (method=="POST")
    {
        auto it=postHandlers.find(url);
        auto &type=request["Content-Type"];
        auto content=httpPostRequestContent();
        if (it==postHandlers.end())
        {
            response=http404BasicResponse();
        }        
        else
        {
            if (type.find("application/x-www-form-urlencoded")!=std::string::npos)
            {
                content.type=httpPostRequestContent::FORM; 
                std::string params=std::string("?")+request["text"];
                content.form=getParams(params,0);
            }
            if (type.find("application/json")!=std::string::npos)
            {
                
                try
                {
                    content.type=httpPostRequestContent::JSON;
                    content.json=new JsonParser(&request["text"]);
                }
                catch(const std::exception& e)
                {
                    return http400BasicResponse();
                }
            }
            if (type.find("multipart/form-data")!=std::string::npos)
            {
                auto pos=type.find("boundary=");
                auto boundary=type.substr(pos+9);
                auto unknown=std::string("unknown");
                int unknownNum=0;
                auto &text=request["text"];
                pos=text.find(boundary);
                if (pos==std::string::npos)
                    return http400BasicResponse();
                while (text.find(boundary,pos+1)!=std::string::npos)
                {
                    auto pos1=text.find(boundary,pos+1);
                    if (pos1==std::string::npos||text[pos1-1]!='-'||text[pos1-2]!='-')
                        return http400BasicResponse();
                    std::string fileName;
                    auto filePosl=text.find("filename=\"",pos);
                    if (filePosl==std::string::npos)
                        fileName=unknown+std::to_string(unknownNum++);
                    else
                    {
                        auto posl=filePosl+10;
                        auto posr=text.find("\"",posl);
                        if (posr==std::string::npos||posr>=pos1)
                            return http400BasicResponse();
                        fileName=text.substr(posl,posr-posl);
                    }
                    pos=text.find("\r\n\r\n",pos);
                    content.type=httpPostRequestContent::FILES;
                    content.files[fileName]=text.substr(pos+4,pos1-4-(pos+4));
                    pos=pos1;
                }
                
            }
            response=postHandlers[url](request,content,requestParam);
            if (content.json!=NULL)
                delete content.json;
        }
        return response;
    }
    return http400BasicResponse();
}