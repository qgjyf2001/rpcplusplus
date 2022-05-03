## rpcplusplus
一个简易的c++ RPC框架

### 支持的类型

目前支持参数类型和返回类型如下的函数：

int,long long等整数类型

std::vector\<T\>

std::map\<std::string,T\>

std::set\<T\>

std::pair\<U,V\>

支持诸如std::vector\<std::map\<std::string,int\>\>等嵌套类型

使用SERIALIZECLASS和SERIALIZEOBJECT进行声明的类（声明方式详见testClass.h）

### 编译方式

编译后端和各类链接库

make

编译前端

make web

### 使用方法

balancer是一个简易的负载均衡器，可以采用服务发现方式进行rpc调用（见server.cpp和client.cpp），服务发现的负载均衡策略可以选择在server端进行或者在client端（rpcClient::CLIENT）进行，也可以采用直接指定ip:port集合的方式进行rpc调用。

dotServer和dotWebServer是打点服务的后台和前台，启动dotServer后，所有的服务都可以先使用dotterClient::init()进行初始化,然后使用dotterClient::throughput(serviceName,count)进行打点。运行dotWebServer，便可以通过访问 http://localhost:3000/index.html 进行打点的查询（前提是需要先启动dotServer）。

RPC的具体使用方式上，在server端只需使用rpcHandler::addRpcHandler将函数和他的签名添加，或者直接使用宏AddRpcHandler进行添加。
在client端要先调用auto functor=client.makeRpcCall\<FunctionType\>(FunctionName);生成一个仿函数，然后调用这个仿函数即可。详细使用方法可以参照server.cpp和client.cpp。
