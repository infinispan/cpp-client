#include "hotrod/impl/transport/tcp/ConnectionPool.h"
#include <hotrod/sys/Log.h>

namespace infinispan {
namespace hotrod {
namespace transport {

void ConnectionPool::addObject(const InetSocketAddress &key) {
    sys::ScopedLock<sys::Mutex> l(lock);

    if (idle.find(key) != idle.end()) {
        return; //key already existed.
    }
    TransportQueuePtr idleQ(new BlockingQueue<TcpTransport*>()); // = TransportQueue(BlockingQueue<TcpTransport *>* idleQ = new BlockingQueue<TcpTransport *>();
    idle.insert(std::pair<InetSocketAddress, TransportQueuePtr>(key, idleQ));
    TransportQueuePtr busyQ(new BlockingQueue<TcpTransport*>()); // = TransportQueue(BlockingQueue<TcpTransport *>* idleQ = new BlockingQueue<TcpTransport *>();
    busy.insert(std::pair<InetSocketAddress, TransportQueuePtr>(key, busyQ));
    ensureMinIdle(key);
}

void ConnectionPool::ensureMinIdle(const InetSocketAddress &key) {
    int grown = calculateMinIdleGrow(key);
    while (grown > 0) {
        idle[key]->push(&factory->makeObject(key));
        grown--;
        totalIdle++;
    }
}

int ConnectionPool::calculateMinIdleGrow(const InetSocketAddress &key) {
    TransportQueuePtr idleQ = idle[key];
    int grown = configuration.getMinIdle() - (int) idleQ->size();
    //Note: if we need to check maxActive, uncomment the code above
    /*if (configuration.getMaxActive() > 0) {
     int growLimit = std::max(0, configuration.getMaxActive() - (int) busy[key]->size() - (int) idleQ->size());
     grown = std::min(grown, growLimit);
     }*/
    if (configuration.getMaxTotal() > 0) {
        int growLimit = std::max(0, configuration.getMaxTotal() - totalIdle - totalActive);
        grown = std::min(grown, growLimit);
    }
    return grown;
}

bool ConnectionPool::hasReachedMaxTotal() {
    int maxTotal = configuration.getMaxTotal();
    return maxTotal > 0 && (maxTotal <= totalActive + totalIdle);
}

bool ConnectionPool::tryRemoveIdle() {
    //first, if we find an idle queue with more the minIdle connection, we destroy one of them.
    //otherwise, we try to destroy and connection from the longer idle queue.
    const int minIdle = configuration.getMinIdle();

    do {
        const InetSocketAddress *keyToRemove = NULL;
        size_t longerQueueSize = 0;

        for (std::map<InetSocketAddress, TransportQueuePtr>::iterator it = idle.begin(); it != idle.end(); ++it) {
            TransportQueuePtr idleQ = it->second;
            if (minIdle > 0 && (int) idleQ->size() > minIdle) {
                keyToRemove = &it->first;
                break;
            } else if (idleQ->size() > longerQueueSize) {
                keyToRemove = &it->first;
                longerQueueSize = idleQ->size();
            }
        }

        if (keyToRemove != NULL) {
            TcpTransport *t = NULL;
            if (idle[*keyToRemove]->poll(t)) { //in case of concurrent removal, avoid blocking
                factory->destroyObject(*keyToRemove, *t);
                totalIdle--;
                return true;
            }
        } else {
            return false;
        }
    } while (true);
}

bool ConnectionPool::tryRemoveIdleOrAskAllocate(const InetSocketAddress &key) {
    if (!tryRemoveIdle()) {
        allocationQueue.push(key);
        return false;
    }
    return true;
}

TcpTransport& ConnectionPool::borrowObject(const InetSocketAddress &key) {
    sys::ScopedLock<sys::Mutex> l(lock);

    if (closed) {
        throw HotRodClientException("Pool is closed");
    }
    if (!idle.count(key) || !busy.count(key)) {
        throw HotRodClientException("Pool has no idle or no busy transports.");
    }
    TransportQueuePtr idleQ = idle[key];
    TransportQueuePtr busyQ = busy[key];

    // See if an object is readily available
    TcpTransport *obj = NULL;
    bool ok = idleQ->poll(obj);
    if (ok) {
        totalIdle--;
    }
    // Loop for a valid object in the pool
    while (obj == NULL || (configuration.isTestOnBorrow() && !factory->validateObject(key, *obj))) {
        // obj is invalid here
        if (obj != NULL) {
            factory->destroyObject(key, *obj);
        }
        // See if we can create a new one
        if (idleQ->size() == 0
        && (configuration.getMaxActive() < 0 || busyQ->size() < (size_t)configuration.getMaxActive())
        && (!hasReachedMaxTotal() || tryRemoveIdleOrAskAllocate(key)))
        {
            try
            {
                obj = &factory->makeObject(key); // then create new object
            } catch (const Exception& ex)
            {
                // Unable to create new transport
                // free waiting threads and rise the exception
                idleQ->notifyAll();
                throw;
            }
        } else {
            if (busyQ->size() == 0)
            {
                // If here, idleQ and busyQ are empty and cannot create new transport
                // just, free waiting threads
                idleQ->notifyAll();
            }
            // if the idle queue is empty
            sys::ScopedUnlock<sys::Mutex> u(lock);
            if (this->getConfiguration().getExhaustedAction() == EXCEPTION) {
                obj = idleQ->popOrThrow();   // else get a transport or throw an exc
            } else {
                obj = idleQ->pop();          // else wait for the first available
            }

            totalIdle--;
        }
    }

    busyQ->push(obj);
    totalActive++;

    factory->activateObject(key, *obj);
    return *obj;
}

void ConnectionPool::invalidateObject(const InetSocketAddress &key, TcpTransport *val) {
    sys::ScopedLock<sys::Mutex> l(lock);
    bool maxTotalReached = hasReachedMaxTotal();
    if (val != NULL) {
        // Remove from busy queue
        std::map<InetSocketAddress, TransportQueuePtr>::iterator busyIt = busy.find(key);
        if (busyIt == busy.end()) {
            throw HotRodClientException("No busy queue for address!");
        }

        busyIt->second->remove(val);
        totalActive--;

        if (!busyIt->second->size()) {
            std::map<InetSocketAddress, TransportQueuePtr>::iterator idleIt = idle.find(key);
            if (idleIt == idle.end()) {
                throw HotRodClientException("No idle queue for address!");
            } else {
                if (!idleIt->second->size()) {
                    // idleQ busyQ are empty, awake all waiting threads
                    idleIt->second->notifyAll();
                }
            }
        }
        if (maxTotalReached && !allocationQueue.empty()) {
            InetSocketAddress keyToAllocate = allocationQueue.front();
            allocationQueue.pop(); //front does not remove it...
            //we need to allocate a new connection for other key.
            idle[keyToAllocate]->push(&factory->makeObject(keyToAllocate));
            totalIdle++;
        }

        // Destroy object
        factory->destroyObject(key, *val);
    }
}

void ConnectionPool::returnObject(const InetSocketAddress &key, TcpTransport &val) {
    sys::ScopedLock<sys::Mutex> l(lock);
    bool ok = true;
    bool maxTotalReached = hasReachedMaxTotal();

    // Remove the object from the busy queue
    if (busy.count(key)) {
        busy[key]->remove(&val);
        totalActive--;
    }

    // If necessary validate the object, then passivate it
    if (closed || (configuration.isTestOnReturn() && !factory->validateObject(key, val))) {
        ok = false;
    } else {
        factory->passivateObject(key, val);
    }

    if (maxTotalReached && !allocationQueue.empty()) {
        InetSocketAddress keyToAllocate = allocationQueue.front();
        allocationQueue.pop(); //front does not remove it...
        //we need to allocate a new connection for other key.
        idle[keyToAllocate]->push(&factory->makeObject(keyToAllocate));
        totalIdle++;
        if (!idle[key]->size() && !busy[key]->size()) {
            // idleQ busyQ are empty, awake all waiting threads
            idle[key]->notifyAll();
        }
        ok = false; //we need to destroy the object
    }

    if (ok) {
        // The object is still valid, see if the idle queue wants it
        if (idle.count(key)) {
            if ((ok = idle[key]->offer(&val))) {
                totalIdle++;
            }
        } else {
            ok = false;
        }
    }

    if (!ok) {
        // The object is now useless
        factory->destroyObject(key, val);
    }
}

void ConnectionPool::clear() {
    sys::ScopedLock<sys::Mutex> l(lock);
    clear(idle);
    clear(busy);
    totalIdle = 0;
    totalActive = 0;
}

void ConnectionPool::clear(std::map<InetSocketAddress, TransportQueuePtr> &queue) {
    for (std::map<InetSocketAddress, TransportQueuePtr>::iterator it = queue.begin(); it != queue.end(); ++it) {
        clear(it->first, it->second);
    }
    queue.clear();
}

void ConnectionPool::clear(const InetSocketAddress &key) {
    sys::ScopedLock<sys::Mutex> l(lock);
    TransportQueuePtr idleQ = idle[key];
    if (!idleQ)
        return;
    totalIdle -= (int) idleQ->size();
    clear(key, idleQ);
    idle.erase(key);
}

void ConnectionPool::clear(const InetSocketAddress &key, TransportQueuePtr queue) {
    if (!queue)
        return;
    while (queue->size() > 0) {
        TcpTransport *transport = queue->pop();
        factory->destroyObject(key, *transport);
    }
}

void ConnectionPool::checkIdle() {
    DEBUG("Ensuring idle connection limits");
    // TODO
}

void ConnectionPool::testIdle() {
    DEBUG("Testing idle connections");
    // TODO
}

void ConnectionPool::preparePool(const InetSocketAddress &key) {
    addObject(key);
}

void ConnectionPool::close() {
    closed = true;
    sys::ScopedLock<sys::Mutex> l(lock);
    clear();
}

void PoolWorker::run() {
    long totalTime = pool->getConfiguration().getTimeBetweenEvictionRuns();
    if (totalTime <= 0) {
        // When non-positive, no eviction thread will be launched.
        return;
    }

    while (!pool->closed) {
        pool->checkIdle();
        pool->testIdle();
        // Sleep in 1 second bursts to let us be cancellable
        for (long t = 0; t < totalTime && !pool->closed; t += 1000) {
            sys::Thread::sleep(1000);
        }
    }
}

}
}
}

