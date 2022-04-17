#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <future>
#include <functional>

#include "safeQueue.h"
class threadPool
{
private:
    bool terminate=false;
    int threadNum;
    safeQueue<std::function<void()>> queue;
    std::thread *threadLoop(int num);
    std::vector<std::thread*> threads;
    std::condition_variable consumer;
    std::mutex mutex;
public:
    threadPool(int threadNum);
    template <typename F,typename ...args>
    auto addThread(F&& function,args... arg) {
        using returnType=decltype(function(arg...));
        std::function<returnType()> functor=std::bind(std::forward<F>(function),arg...);
        auto taskPtr=std::make_shared<std::packaged_task<returnType()>>(functor);
        std::function<void()> warpper_func = [taskPtr]()
        {
            (*taskPtr)();
        };
        queue.push(warpper_func);
        consumer.notify_one();
        return taskPtr->get_future();
    }
    ~threadPool();
};
#endif