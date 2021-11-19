#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
class threadPool
{
private:
    std::vector<std::thread*> threads;
    std::vector<std::mutex*> available;
    std::vector<std::mutex*> finished;
    std::vector<bool> ready;
    std::vector<bool> terminate;
    std::vector<std::function<void(void*)>> f;
    std::vector<void*> args;
    int threadNum;
    std::thread *threadLoop(int num);
public:
    threadPool(int threadNum);
    void addThread(std::function<void(void*)>function,void *arg);
    void waitAll();
    ~threadPool();
};
#endif