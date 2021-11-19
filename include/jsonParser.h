#ifndef JSONPARSER_H
#define JSONPARSER_H
#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <memory>
class JsonParser
{
public:
    enum Type{INT,STRING,OBJECT,ARRAY};
    JsonParser(std::string* message,Type type=OBJECT,bool checkEnd=true,std::string::iterator *beginPtr=nullptr,std::string::iterator *endPtr=nullptr);
    JsonParser():JsonParser(&emptyJSON)
    {
        
    }
    JsonParser(int num)
    {
        this->messsageInt=num;
        this->type=INT;
    }
    JsonParser& operator=(std::string str)
    {
        *this=JsonParser(std::make_shared<std::string>(str));
        return *this;
    }
    JsonParser& operator=(int num)
    {
        *this=JsonParser(num);
        return *this;
    }
    JsonParser(std::shared_ptr<std::string> str)
    {
        this->messageStr=str;
        this->type=STRING;
    }
    JsonParser &operator [](std::string index)
    {
        return json[index];
    }
    std::string toString()
    {
        if (this->type!=STRING)
            JSONPanic("illegal operation");
        return *messageStr;
    }
    int toInt()
    {
        if (this->type!=INT)
            JSONPanic("illegal operation");
        return messsageInt;
    }
    void foreach(std::function<void(JsonParser&)> function)
    {
        if (this->type!=ARRAY)
            JSONPanic("illegal operation");
        for (auto obj:jsonArray)
            function(obj);
    }
    Type type;
    operator std::string()
    {
        if (this->type==STRING)
            return "\""+this->toString()+"\"";
        else if (this->type==INT)
            return std::to_string(this->toInt());
        else if (this->type==ARRAY){
            std::string result="[";
            this->foreach([&](JsonParser& json){
                 if (result=="[")
                          result+=json;
                 else
                          result+=std::string(",")+(std::string)json;
        });
            result+="]";
            return result;
        }
        else{
            std::string result="{";
            for (auto &&it:json)
            {
                if (result=="{")
                    result+="\""+it.first+"\":"+(std::string)(it.second);
                else
                    result+=std::string(",")+"\""+it.first+"\":"+(std::string)(it.second);
            }
            result+="}";
            return result;
        }
    }
    void add(int num)
    {
        if (this->type!=ARRAY)
            throw std::invalid_argument("invalid type");
        jsonArray.push_back(JsonParser(new std::string(std::to_string(num)),INT));
    }
    void add(std::string str)
    {
        if (this->type!=ARRAY)
            throw std::invalid_argument("invalid type");
        jsonArray.push_back(JsonParser(new std::string(str),STRING));
    }
    void add(JsonParser data)
    {
        if (this->type!=ARRAY)
            throw std::invalid_argument("invalid type");
        jsonArray.push_back(data);
    }
    std::map<std::string,JsonParser> &getIterator()
    {
        return json;
    }
    ~JsonParser()
    {
        freeAll();
    }
private:
    template<char... ch>
    static inline void moveForwardPtr(std::string::iterator& pos,std::string::iterator end,bool equal=true)
    {
        while (pos!=end&&(((*pos==ch))||...)^(!equal))
            pos++;
    }
    template<char... ch>
    static inline void moveBackwardPtr(std::string::iterator& pos,std::string::iterator begin,bool equal=true)
    {
        while (pos!=begin&&(((*pos==ch))||...)^(!equal))
            pos--;
    }
    void freeAll()
    {
        
    }
    void JSONPanic(std::string description)
    {
        freeAll();
        throw std::invalid_argument(description);
    }
    std::map<std::string,JsonParser> json;
    std::vector<JsonParser> jsonArray;
    std::shared_ptr<std::string> messageStr;
    std::string emptyJSON="{}";
    int messsageInt;
};
#endif // JSONPARSER_H
