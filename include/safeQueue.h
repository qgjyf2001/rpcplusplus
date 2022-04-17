#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H

#include <queue>
#include <mutex>
template <typename T>
class safeQueue : protected std::queue<T>
{
public:
    void push(T value) {
        std::lock_guard<std::mutex> lck(mutex);
        std::queue<T>::push(value);
    }
    bool pop(T& value) {
        std::lock_guard<std::mutex> lck(mutex);
        if (std::queue<T>::empty()) {
            return false;
        }
        else {
            value=std::move(std::queue<T>::front());
            std::queue<T>::pop();
        }
        return true;
    }
    bool empty() {
        std::lock_guard<std::mutex> lck(mutex);
        return std::queue<T>::empty();
    }
private:
    std::mutex mutex;
};


#endif