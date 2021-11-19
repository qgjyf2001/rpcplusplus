#ifndef RPCHANDLER_H
#define RPCHANDLER_H
#include <iostream>
#include <map>
#include "serialization.h"
#include <functional>
template <typename function>
struct  getParamsLength;

template<typename returnType,typename... args>
struct  getParamsLength<returnType(args...)>
{
    using type=std::tuple<args...>;
    static constexpr auto value=sizeof...(args);
};
template <int index,typename function>
struct getParamsIndex;
template <int index,typename returnType,typename first,typename... args>
struct getParamsIndex<index,returnType(first,args...)>
{
    using type=typename getParamsIndex<index-1,returnType(args...)>::type;
};
template <typename returnType,typename first,typename... args>
struct  getParamsIndex<0,returnType(first,args...)>
{
    using type=first;
};

class rpcHandler
{
private:
    template <int length,typename returnType,typename... args>
    std::tuple<args...> _genType(JsonParser& json,returnType(args...))
    {
        return genType<length,args...>(json);
    }
    template <int length,typename first,typename... args>
    std::tuple<first,args...> genType(JsonParser& json)
    {
        auto now=serialize::doUnSerialize<typename std::remove_reference<first>::type>(json["param"+std::to_string(length)]);
        return std::tuple_cat(std::make_tuple(now),genType<length-1,args...>(json));
    }
    template <int length>
    std::tuple<> genType(JsonParser&json)
    {
        return std::make_tuple();
    }
    std::map<std::string,std::function<JsonParser(JsonParser&)>> rpcMap;
public:
    JsonParser handleRPC(JsonParser json)
    {
        return rpcMap[json["name"].toString()](json);
    }
    template <typename F>
    void addRpcHandler(std::string name,F& func)
    {
        constexpr auto length=getParamsLength<F>::value;
        rpcMap[name]=[&](JsonParser& json)->JsonParser{
            auto tuple=_genType<length>(json,func);
            JsonParser result;
            std::apply([&](auto ...args){
                result=serialize::doSerialize(func(args...));
            },tuple);
            return result;
        };
    }
    void removeRpcHandler(std::string name){
        rpcMap.erase(name);
    }
};
class rpcSender
{
private:
    template <typename first,typename... args>
    void setParam(JsonParser& json,int depth,first param1,args... params)
    {
        if (depth==0)
            return;
        json["param"+std::to_string(depth)]=serialize::doSerialize(param1);
        setParam(json,depth-1,params...);
    }
    void setParam(JsonParser&json,int depth)
    {

    }
public:
    template <typename ...args>
    JsonParser sendRPC(args... params)
    {
        constexpr auto length=sizeof...(args);
        JsonParser result;
        setParam(result,length,params...);
        return result;
    }
    template <typename returnType>
    returnType getRPC(JsonParser &rpc)
    {
        return serialize::doUnSerialize<returnType>(rpc);
    }
};


#endif