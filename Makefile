all:main

CC=g++
CXXFLAGS=-std=c++17 -g
INCLUDE=./include

THREADPOOL=./threadPool
THREADPOOLTARGET=$(THREADPOOL)/threadPool.o
JSONPARSER=./json
JSONPARSERTARGET=$(JSONPARSER)/jsonParser.o
RPC=./rpc
RPCTARGET=$(RPC)/rpcParser.o $(RPC)/rpcServer.o
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

main.o:main.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
main:main.o $(JSONPARSERTARGET) $(RPCTARGET) $(THREADPOOLTARGET) $(TCPTARGET) -lpthread
	$(CC) -o $@ $^
clean:
	find . -name '*.o' -type f -print -exec rm -rf {} \;
	rm main

