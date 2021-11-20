## rpcplusplus
一个简易的c++ RPC框架

### 支持的类型

目前仅支持参数类型和返回类型如下的函数：

int,long long等整数类型

std::vector\<T\>

std::map\<std::string,T\>

std::pair\<U,V\>

支持诸如std::vector\<std::map\<std::string,int\>\>等嵌套类型

### 使用方法

在server端只需使用rpcHandler::addRpcHandler将函数和他的签名添加，或者直接使用宏AddRpcHandler进行添加。
在client端要先调用auto functor=client.makeRpcCall\<FunctionType\>(FunctionName);生成一个仿函数，然后调用这个仿函数即可。

详细使用方法可以参照server.cpp和client.cpp
