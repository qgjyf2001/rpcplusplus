#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "jsonParser.h"

#include <type_traits>
#include <set>
class baseSerializeClass;
static std::map<std::string,std::vector<std::function<void(baseSerializeClass*,JsonParser&)>>> serializeMap;
static std::map<std::string,std::vector<std::function<void(baseSerializeClass*,JsonParser&)>>> unSerializeMap;
inline auto &getSerializeMap(std::string name) {
    return serializeMap[name];
}
inline auto &getUnSerializeMap(std::string name) {
    return unSerializeMap[name];
}
class baseSerializeClass
{
public:
    virtual void unSerialize (JsonParser parser)
    {
        for (auto func:getUnSerializeMap(typeid(*this).name())) {
            func(this,parser);
        }
        return;
    }
    virtual operator JsonParser()
    {
        JsonParser json;
        for (auto func:getSerializeMap(typeid(*this).name())) {
            func(this,json);
        }
        return json;
    }
};
#define SERIALIZECLASS(className) class className:public baseSerializeClass
#define SERIALIZEOBJECT(objectType,objectName) objectType objectName;\
objectType objectName##Serialize=(getSerializeMap(typeid(*this).name()).emplace_back([](baseSerializeClass* mClass,JsonParser& parser){\
        parser[#objectName]=(dynamic_cast<decltype(this)>(mClass))->objectName;\
    }),objectType());\
objectType objectName##UnSerialize=(getUnSerializeMap(typeid(*this).name()).emplace_back([](baseSerializeClass* mClass,JsonParser& parser){\
        (dynamic_cast<decltype(this)>(mClass))->objectName=serialize::doUnSerialize<objectType>(parser[#objectName]);\
    }),objectType());
#define SERIALIZEOBJECTWITHVALUE(objectType,objectName,initValue) objectType objectName=initValue;\
objectType objectName##Serialize=(getSerializeMap(typeid(*this).name()).emplace_back([](baseSerializeClass* mClass,JsonParser& parser){\
        parser[#objectName]=(dynamic_cast<decltype(this)>(mClass))->objectName;\
    }),objectType(initValue));\
objectType objectName##UnSerialize=(getUnSerializeMap(typeid(*this).name()).emplace_back([](baseSerializeClass* mClass,JsonParser& parser){\
        (dynamic_cast<decltype(this)>(mClass))->objectName=serialize::doUnSerialize<objectType>(parser[#objectName]);\
    }),objectType(initValue));
class serialize
{
private:
    /* data */
public:
    serialize(/* args */);
    template <typename T>
    struct isVector
    {
        static constexpr auto value=false;
    };
    template <typename T>
    struct  isVector<std::vector<T>>
    {
        static constexpr auto value=true;
    };
    template <typename T>
    struct  isMap
    {
        static constexpr auto value=false;
    };
    template <typename U,typename V>
    struct  isMap<std::map<U,V>>
    {
        static constexpr auto value=true;
    };
    template <typename T>
    struct  isSet
    {
        static constexpr auto value=false;
    };
    template <typename U,typename V>
    struct  isSet<std::set<U,V>>
    {
        static constexpr auto value=true;
    };
    
    template <typename T>
    struct  isPair
    {
        static constexpr auto value=false;
    };
    template <typename U,typename V>
    struct  isPair<std::pair<U,V>>
    {
        static constexpr auto value=true;
    }; 
    template <typename T>
    struct  isInteger
    {
        using rT=typename std::remove_const<T>::type;
        static constexpr auto value=std::__is_integer<rT>::__value;
    };
    
    template <typename T>
    struct isString
    {
        private:
            template <typename U,typename=decltype(std::string(U()))>
            static char _isString(void*);
            template <typename>
            static int _isString(...);
        public:
            static constexpr auto value=std::is_same<decltype(_isString<T>(nullptr)),char>::value&&!isInteger<T>::value;
    };
    template <typename T>
    struct isSerializable
    {
        private:
            template <typename U,typename=decltype(&U::unSerialize)>
            static char _isSerializable(void*);
            template <typename>
            static int _isSerializable(...);
        public:
            static constexpr auto value=std::is_same<decltype(_isSerializable<T>(nullptr)),char>::value;
    };
    template <typename T>
    static JsonParser doSerialize(T data){   
        if constexpr(std::is_same<T,JsonParser>::value) {
            return data;
        }
        else if constexpr(isString<T>::value)
        {
            return JsonParser(std::make_shared<std::string>(std::string(data)));
        }
        else if constexpr(isSerializable<T>::value)
        {
            return (JsonParser)data;
        }
        else if constexpr(isSet<T>::value) 
        {
            std::string arr="[]";
            JsonParser jsonArray(&arr,JsonParser::ARRAY);
            for (auto &&each:data)
                jsonArray.add(doSerialize(each));
            return jsonArray;
        }
        else if constexpr(isInteger<T>::value)
        {
            return JsonParser(new std::string(std::to_string(data)),JsonParser::INT);
        }
        else if constexpr(isVector<T>::value)
        {
            std::string arr="[]";
            JsonParser jsonArray(&arr,JsonParser::ARRAY);
            for (auto &&each:data)
                jsonArray.add(doSerialize(each));
            return jsonArray;
        }
        else if constexpr(isMap<T>::value)
        {
            static_assert(isString<typename T::value_type::first_type>::value,"key should be string");
            JsonParser result;
            for (auto &&[u,v]:data)
            {
                result[std::string(u)]=doSerialize(v);
            }
            return result;
        }
        else if constexpr(isPair<T>::value)
        {
            auto &&[u,v]=data;
            std::string arr="[]";
            JsonParser jsonArray(&arr,JsonParser::ARRAY);
            jsonArray.add(doSerialize(u));
            jsonArray.add(doSerialize(v));
            return jsonArray;
        }
        else
            throw std::invalid_argument("unsupported type");
    }
    template <typename T> 
    static T doUnSerialize(JsonParser data)
    {
        if constexpr(std::is_same<T,JsonParser>::value) {
            return data;
        }
        else if constexpr(isString<T>::value)
        {
            return data.toString();
        }
        else if constexpr(isSerializable<T>::value)
        {
            T tmp;
            tmp.unSerialize(data);
            return tmp;
        }
        else if constexpr(isInteger<T>::value)
        {
            return data.toInt();
        }
        else if constexpr(isSet<T>::value)
        {
            T result;
            data.foreach([&](JsonParser& json){ 
                result.insert(doUnSerialize<typename T::value_type>(json));
            });
            return result;
        }
        else if constexpr(isVector<T>::value)
        {
            T result;
            data.foreach([&](JsonParser& json){ 
                result.push_back(doUnSerialize<typename T::value_type>(json));
            });
            return result;
        }
        else if constexpr(isMap<T>::value)
        {
            T result;
            for (auto &&[u,v]:data.getIterator())
            {
                result[u]=doUnSerialize<typename T::value_type::second_type>(v);
            }
            return result;
        }
        else if constexpr(isPair<T>::value)
        {
            std::vector<JsonParser> result;
            data.foreach([&](JsonParser &json){
                result.push_back(json);
            });
            return std::make_pair(doUnSerialize<typename T::first_type>(result[0]),doUnSerialize<typename T::second_type>(result[1]));
        }
        else
            throw std::invalid_argument("unsupported type");
    }
    ~serialize();
};


#endif