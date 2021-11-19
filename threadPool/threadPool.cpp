#include "threadPool.h"
threadPool::threadPool(int threadNum)
{
    this->threadNum=threadNum;
    ready.reserve(threadNum);
    terminate.reserve(threadNum);
    for (int i=0;i<threadNum;i++)
    {
        ready[i]=false;
        terminate[i]=false;
        std::mutex* mtx=new std::mutex();
        mtx->lock();
        available.push_back(mtx);
        finished.push_back(new std::mutex());
    }
    args.reserve(threadNum);
    f.resize(threadNum);
    for (int i=0;i<threadNum;i++)
        threads.push_back(threadLoop(i));
}
std::thread* threadPool::threadLoop(int _num)
{
    return new std::thread([&](int num){
        //signal(SIGPIPE , SIG_IGN);
        while (!terminate[num])
        {
            available[num]->lock();
            if (terminate[num])
                break;
            std::lock_guard<std::mutex> lck(*finished[num]);            
            ready[num]=true;
            f[num](args[num]);
        }
    },_num);
}
void threadPool::addThread(std::function<void(void*)>function,void *arg)
{
    for (int i=0;;i=(i+1)%threadNum)
    {
        if (finished[i]->try_lock())
        {

            args[i]=arg;
            f[i]=function;
            finished[i]->unlock();
            available[i]->unlock();
            while (!ready[i]);
            ready[i]=false;
            break;
        }
    }
}
void threadPool::waitAll()
{
    for (int i=0;i<threadNum;i++)
        finished[i]->lock();
    for (int i=0;i<threadNum;i++)
        finished[i]->unlock();
}
threadPool::~threadPool()
{
    for (int i=0;i<threadNum;i++)
    {
        terminate[i]=true;
        available[i]->try_lock();
        available[i]->unlock();
        if (threads[i]->joinable())
            threads[i]->join();
    }
    for (auto *val:available)
        delete val;
    for (auto *val:finished)
        delete val;
}
