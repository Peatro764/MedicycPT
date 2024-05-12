#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <queue>
#include <thread>
#include <mutex>

namespace utils {

template <typename T>
class ThreadSafeQueue
{

 public:

    // IMPORTANT: There is only one reader of this queue (but several pushers). Thus, the below implementation
    // is thread safe. If one they there are more readers (unlikely), this class needs to be modified.

    T& front() {
        std::unique_lock<std::mutex> mlock(mutex_);
        if (queue_.empty()) throw std::runtime_error("Cannot call front on empty queue");
        return queue_.front();
    }

    T pop() {
        std::unique_lock<std::mutex> mlock(mutex_);
        if (queue_.empty()) throw std::runtime_error("Cannot call front on empty queue");
        auto item = queue_.front();
        queue_.pop();
        return item;
    }

    void push(const T& item) {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(item);
    }

    void push(std::queue<T>& items) {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (!items.empty()) {
            queue_.push(items.front());
            items.pop();
        }
    }

    bool empty() {
        std::unique_lock<std::mutex> mlock(mutex_);
        return queue_.empty();
    }

    size_t size() {
        std::unique_lock<std::mutex> mlock(mutex_);
        return queue_.size();
    }

    void clear() {
        std::unique_lock<std::mutex> mlock(mutex_);
        std::queue<T> empty;
        std::swap(queue_, empty);
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
};

}

#endif // THREADSAFEQUEUE_H
