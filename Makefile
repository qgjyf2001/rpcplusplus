all:balancer dotServer testClient testServer dotWebServer

CC=g++
CXXFLAGS=-std=c++17 -g
MAKE=make
INCLUDE=./include
THREADPOOL=./threadPool
THREADPOOLTARGET=$(THREADPOOL)/threadPool.o
JSONPARSER=./json
JSONPARSERTARGET=$(JSONPARSER)/jsonParser.o
RPC=./rpc
RPCTARGET=$(RPC)/rpcParser.o $(RPC)/rpcServer.o $(RPC)/rpcClient.o
TCP=./tcp
TCPTARGET=$(TCP)/tcpServer.o
CACHE=./cache
CACHETARGET=$(CACHE)/redisSet.o $(CACHE)/cache.o $(CACHE)/redisZset.o
BALANCER=./balancers
BALANCERTARGET=$(BALANCER)/baseBalancer.o $(BALANCER)/minimumBalancer.o $(BALANCER)/hashBalancer.o $(BALANCER)/connectionBalancer.o
DOTTER=./dotter
DOTTERTARGET=$(DOTTER)/dotterClient.o
WEBSERVER=./CppHttpServer
WEBSERVERTARGET=$(WEBSERVER)/libCppServer.a

$(BALANCER)/%.o:$(BALANCER)/%.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(DOTTER)/%.o:$(DOTTER)/%.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(CACHE)/%.o:$(CACHE)/%.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(THREADPOOLTARGET):$(THREADPOOL)/threadPool.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(TCP)/%.o:$(TCP)/%.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(JSONPARSERTARGET):$(JSONPARSER)/jsonParser.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(RPC)/%.o:$(RPC)/%.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(WEBSERVERTARGET):
	cd $(WEBSERVER) && $(MAKE) libCppServer.a

balancer.o:balancer.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
server.o:server.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
client.o:client.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
dotter.o:dotter.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@

web:
	cd ./app&&yarn build
dotWebServer.o:webServer.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
balancer:balancer.o $(JSONPARSERTARGET) $(RPCTARGET) $(THREADPOOLTARGET) $(TCPTARGET) $(CACHETARGET) $(BALANCERTARGET) $(DOTTERTARGET)
	$(CC) -o $@ $^ -lhiredis -lpthread -lcrypto
dotServer:dotter.o $(JSONPARSERTARGET) $(RPCTARGET) $(THREADPOOLTARGET) $(TCPTARGET) $(CACHETARGET) $(DOTTERTARGET)
	$(CC) -o $@ $^ -lhiredis -lpthread -lgflags
testServer:server.o $(JSONPARSERTARGET) $(RPCTARGET) $(THREADPOOLTARGET) $(TCPTARGET) $(DOTTERTARGET)
	$(CC) -o $@ $^ -lpthread -lgflags
testClient:client.o $(JSONPARSERTARGET) $(RPCTARGET) $(THREADPOOLTARGET) $(TCPTARGET) $(DOTTERTARGET)
	$(CC) -o $@ $^ -lpthread
dotWebServer:dotWebServer.o $(WEBSERVERTARGET) $(RPCTARGET) $(DOTTERTARGET)
	$(CC) -o $@ $^ -lpthread -L$(WEBSERVER) -lCppServer
	
clean:
	find . -name '*.o' -type f -print -exec rm -rf {} \;
	rm balancer
	rm testServer
	rm testClient
	rm dotServer
	rm dotWebServer
	cd $(WEBSERVER) && $(MAKE) clean