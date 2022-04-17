#include "threadPool.h"

threadPool::threadPool(int threadNum)
{
    this->threadNum=threadNum;

    for (int i=0;i<threadNum;i++)
        threads.push_back(threadLoop(i));
}
std::thread* threadPool::threadLoop(int num)
{
    return new std::thread([&,num](){
        //signal(SIGPIPE , SIG_IGN);
        while (!terminate)
        {
            std::unique_lock<std::mutex> lck(mutex);
            if (queue.empty()) {
                consumer.wait(lck);
            }
            bool result;
            std::function<void()> f;
            result=queue.pop(f);
            if (result) {
                f();
            }
        }
    });
}

threadPool::~threadPool()
{
    terminate=true;
    consumer.notify_all();
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    consumer.notify_all();
    for (auto *thread:threads) {
        if (thread->joinable())
            thread->join();
    }
}
