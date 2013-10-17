#ifndef ISPN_HOTROD_BLOCKINGQUEUE_H
#define ISPN_HOTROD_BLOCKINGQUEUE_H

#include <list>
#include <algorithm>
#include "hotrod/sys/Condition.h"
#include "hotrod/sys/Mutex.h"

namespace infinispan {
namespace hotrod {
namespace sys {

template<class T> class BlockingQueue {
public:
    BlockingQueue<T>(int capacity_) :
            capacity(capacity_) {
    }

    BlockingQueue<T>() :
            capacity(0) {
    }

    void push(T const& value) {
        ScopedLock<Mutex> l(lock);

        if (capacity > 0 && queue.size() == capacity) {
            throw new Exception("Queue is full");
        }
        queue.push_back(value);
        condition.notify();
    }

    bool offer(T const& value) {
        ScopedLock<Mutex> l(lock);
        if (capacity > 0 && queue.size() == capacity) {
            return false;
        }
        queue.push_back(value);
        condition.notify();
        return true;
    }

    T pop() {
        ScopedLock<Mutex> l(lock);

        while (queue.empty()) {
            condition.wait(lock);
        }
        T element = queue.front();
        queue.pop_front();
        return element;
    }

    bool poll(T& value) {
        ScopedLock<Mutex> l(lock);

        if (queue.empty()) {
            return false;
        } else {
            value = queue.front();
            queue.pop_front();
            return true;
        }
    }

    bool remove(T const& value) {
        ScopedLock<Mutex> l(lock);

        int old_size = queue.size();
        queue.remove(value);
        return old_size != queue.size();
    }

    int size() {
        ScopedLock<Mutex> l(lock);

        return queue.size();
    }

private:
    int capacity;
    std::list<T> queue;
    Mutex lock;
    Condition condition;

};

}}} // namespace infinispan::hotrod::sys

#endif  /* ISPN_HOTROD_BLOCKINGQUEUE_H */
