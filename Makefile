all:server client

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

$(THREADPOOLTARGET):$(THREADPOOL)/threadPool.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(TCP)/%.o:$(TCP)/%.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(JSONPARSERTARGET):$(JSONPARSER)/jsonParser.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(RPC)/%.o:$(RPC)/%.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@

client.o:client.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
client:client.o $(JSONPARSERTARGET) $(RPCTARGET) $(THREADPOOLTARGET) $(TCPTARGET) -lpthread
	$(CC) -o $@ $^
server.o:server.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
server:server.o $(JSONPARSERTARGET) $(RPCTARGET) $(THREADPOOLTARGET) $(TCPTARGET) -lpthread
	$(CC) -o $@ $^
clean:
	find . -name '*.o' -type f -print -exec rm -rf {} \;
	rm client
	rm server

