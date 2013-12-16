#include "hotrod/impl/transport/tcp/ConnectionPool.h"
#include <hotrod/sys/Log.h>

namespace infinispan {
namespace hotrod {
namespace transport {

void ConnectionPool::addObject(const InetSocketAddress& key) {
    sys::ScopedLock<sys::Mutex> l(lock);

    TransportQueuePtr idleQ(new BlockingQueue<TcpTransport *>()); // = TransportQueue(BlockingQueue<TcpTransport *>* idleQ = new BlockingQueue<TcpTransport *>();
    idle.insert(std::pair<InetSocketAddress, TransportQueuePtr>(key, idleQ));
    for (int i = 0; i < configuration.getMinIdle(); i++) {
        idleQ->push(&factory->makeObject(key));
    }
    TransportQueuePtr busyQ(new BlockingQueue<TcpTransport *>()); //BlockingQueue<TcpTransport *>* busyQ = new BlockingQueue<TcpTransport *>();
    busy.insert(std::pair<InetSocketAddress, TransportQueuePtr>(key, busyQ));
}

TcpTransport& ConnectionPool::borrowObject(const InetSocketAddress& key) {
    if (closed) {
        throw HotRodClientException("Pool is closed");
    }
    if (!idle.count(key) || !busy.count(key)) {
        throw HotRodClientException("Pool is closed");
    }
    TransportQueuePtr idleQ = idle[key];
    TransportQueuePtr busyQ = busy[key];

    // See if an object is readily available
    TcpTransport* obj;
    bool ok = idleQ->poll(obj);

    for (;;) {
        if (ok) {
            // Check if the object is still valid, if not destroy it
            if (configuration.isTestOnBorrow() && !factory->validateObject(key, *obj)) {
                factory->destroyObject(key, *obj);
                ok = false;
            }
            // We have a valid object
            if (ok) {
                busyQ->push(obj);
                break;
            }
        }
        // See if we can create a new one
        if (busyQ->size() < configuration.getMaxActive()) {
            obj = &factory->makeObject(key);
        } else {
            // Wait for an object to become idle
            obj = idleQ->pop();
        }
        ok = true;
    }
    factory->activateObject(key, *obj);
    return *obj;
}

void ConnectionPool::invalidateObject(const InetSocketAddress& key, TcpTransport* val) {
    sys::ScopedLock<sys::Mutex> l(lock);
    if (val != NULL) {
        factory->destroyObject(key, *val);
    }
}

void ConnectionPool::returnObject(const InetSocketAddress& key, TcpTransport& val) {
    sys::ScopedLock<sys::Mutex> l(lock);
    bool ok = true;

    // Remove the object from the busy queue
    if (busy.count(key)) {
        busy[key]->remove(&val);
    }

    // If necessary validate the object, then passivate it
    if (closed || (configuration.isTestOnReturn() && !factory->validateObject(key, val))) {
        ok = false;
    } else {
        factory->passivateObject(key, val);
    }

    if (ok) {
        // The object is still valid, see if the idle queue wants it
        if (idle.count(key)) {
            ok = idle[key]->offer(&val);
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
}

void ConnectionPool::clear(std::map<InetSocketAddress, TransportQueuePtr>& queue) {
    for (std::map<InetSocketAddress, TransportQueuePtr>::iterator it = queue.begin(); it != queue.end(); ++it) {
        clear(it->first, it->second);
    }
    queue.clear();
}

void ConnectionPool::clear(const InetSocketAddress& key) {
    sys::ScopedLock<sys::Mutex> l(lock);
    TransportQueuePtr idleQ = idle[key];
    clear(key, idleQ);
    idle.erase(key);
}

void ConnectionPool::clear(const InetSocketAddress& key, TransportQueuePtr queue) {
    while (queue->size() > 0) {
        TcpTransport* transport = queue->pop();
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

void ConnectionPool::preparePool(const InetSocketAddress& key) {
    addObject(key);
}

void ConnectionPool::close() {
    closed = true;
    sys::ScopedLock<sys::Mutex> l(lock);
    clear();
}

void PoolWorker::run() {
    while(!pool->closed) {
        pool->checkIdle();
        pool->testIdle();
        // Sleep in 1 second bursts to let us be cancellable
        for(long t = 0; t < pool->getConfiguration().getTimeBetweenEvictionRuns() && !pool->closed; t+=1000) {
            sys::Thread::sleep(1000);
        }
    }
}

}}}

