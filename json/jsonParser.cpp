#include "jsonParser.h"
JsonParser::JsonParser(std::string* message,Type type,bool checkEnd,std::string::iterator *beginPtr,std::string::iterator *endPtr)
{
            this->type=type;
        if (type==INT)
        {
            this->messsageInt=std::atoi(message->c_str());
            return;
        }
        auto begin=!checkEnd?*beginPtr:message->begin();
        auto end=message->end()-1;
        moveForwardPtr<' ','\n'>(begin,message->end());
        if (checkEnd)
            moveBackwardPtr<' ','\n'>(end,message->begin());
        if ((*begin=='{'&&(!checkEnd||*end=='}'))||(*begin=='['&&(!checkEnd||*end==']')))
        {
            this->type=*begin=='['?ARRAY:OBJECT;
            begin+=1;
            std::string::iterator tail,head;
            while (true)
            {
                if (this->type!=ARRAY)
                {
                moveForwardPtr<' ','\n'>(begin,end);
                if (begin==end)
                    break;
                if (*begin!='"')
                    JSONPanic("illegal syntax");
                head=++begin;
                moveForwardPtr<'"'>(begin,end,false);
                if (*begin!='"')
                    JSONPanic("illegal syntax");
                tail=begin++;
                moveForwardPtr<' ','\n'>(begin,end);
                if (begin==end||*begin!=':')
                    JSONPanic("illegal syntax");
                begin++;
                moveForwardPtr<' ','\n'>(begin,end);
                }
                moveForwardPtr<'{','"','0','1','2','3','4','5','6','7','8','9','['>(begin,end,false);
                if (begin==end)
                {
                    if (this->type==OBJECT)
                        JSONPanic("illegal syntax");
                    else
                        break;
                }
                if (*begin=='"')
                {
                    auto contentHead=++begin;
                    while (true)
                    {
                        moveForwardPtr<'"','\\'>(begin,end,false);
                        if (begin==end)
                            JSONPanic("illegal syntax");
                        if (*begin=='"')
                            break;
                        begin+=2;
                    }
                    auto contentTail=begin;
                    if (this->type==ARRAY)
                        jsonArray.push_back(JsonParser(std::make_shared<std::string>(std::string(contentHead,contentTail))));
                    else
                        json[std::string(head,tail)]=JsonParser(std::make_shared<std::string>(std::string(contentHead,contentTail)));
                    begin++;
                    if (((*begin==']'&&this->type==ARRAY)||(*begin=='}'&&this->type==OBJECT))&&!checkEnd)
                    {
                        *endPtr=begin+1;
                        return;
                    }
                }
                else if (*begin=='{')
                {
                    JsonParser newJson=JsonParser(message,OBJECT,false,&begin,&begin);
                    if (this->type==ARRAY)
                        jsonArray.push_back(newJson);
                    else
                        json[std::string(head,tail)]=newJson;
                }
                else if (*begin=='[')
                {
                    JsonParser newJson=JsonParser(message,OBJECT,false,&begin,&begin);
                    if (this->type==ARRAY)
                        jsonArray.push_back(newJson);
                    else
                        json[std::string(head,tail)]=newJson;
                }
                else{
                    auto contentHead=begin;
                    moveForwardPtr<'0','1','2','3','4','5','6','7','8','9'>(begin,end);
                    auto contentTail=begin;
                    if (this->type==ARRAY)
                        jsonArray.push_back(JsonParser(new std::string(contentHead,contentTail),INT));
                    else
                        json[std::string(head,tail)]=JsonParser(new std::string(contentHead,contentTail),INT);
                }
                moveForwardPtr<' ','\n'>(begin,end);
                if (*begin==',')
                    begin++;
                else if ((!((*begin=='}'&&this->type==OBJECT)||(*begin==']'&&this->type==ARRAY)))&&checkEnd)
                    JSONPanic("illegal syntax");
                else if (((*begin=='}'&&this->type==OBJECT)||(*begin==']'&&this->type==ARRAY))&&!checkEnd)
                {
                    *endPtr=begin+1;
                    return;
                }
            }
        }
        else
        {
           JSONPanic("illegal syntax");
        }
}