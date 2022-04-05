all:balancer testClient testServer

CC=g++
CXXFLAGS=-std=c++17 -g
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
CACHETARGET=$(CACHE)/redisSet.o $(CACHE)/cache.o
BALANCER=./balancers
BALANCERTARGET=$(BALANCER)/baseBalancer.o $(BALANCER)/minimumBalancer.o $(BALANCER)/hashBalancer.o $(BALANCER)/connectionBalancer.o

$(BALANCER)/%.o:$(BALANCER)/%.cpp
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

balancer.o:balancer.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
server.o:server.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
client.o:client.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
balancer:balancer.o $(JSONPARSERTARGET) $(RPCTARGET) $(THREADPOOLTARGET) $(TCPTARGET) $(CACHETARGET) $(BALANCERTARGET)
	$(CC) -o $@ $^ -lhiredis -lpthread -lcrypto
testServer:server.o $(JSONPARSERTARGET) $(RPCTARGET) $(THREADPOOLTARGET) $(TCPTARGET) 
	$(CC) -o $@ $^ -lpthread -lgflags
testClient:client.o $(JSONPARSERTARGET) $(RPCTARGET) $(THREADPOOLTARGET) $(TCPTARGET) 
	$(CC) -o $@ $^ -lpthread

clean:
	find . -name '*.o' -type f -print -exec rm -rf {} \;
	rm balancer
	rm testServer
	rm testClient