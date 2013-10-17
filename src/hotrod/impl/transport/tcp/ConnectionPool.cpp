#include "hotrod/impl/transport/tcp/ConnectionPool.h"

namespace infinispan {
namespace hotrod {
namespace transport {

void ConnectionPool::addObject(const InetSocketAddress& key) {
    sys::ScopedLock<sys::Mutex> l(lock);
    BlockingQueue<TcpTransport *>* idleQ = new BlockingQueue<TcpTransport *>();
    idle.insert(std::pair<InetSocketAddress, BlockingQueue<TcpTransport *> *>(key, idleQ));
    for (int i = 0; i < configuration.getMinIdle(); i++) {
        idleQ->push(&factory->makeObject(key));
    }
    BlockingQueue<TcpTransport *>* busyQ = new BlockingQueue<TcpTransport *>();
    busy.insert(std::pair<InetSocketAddress, BlockingQueue<TcpTransport *> *>(key, busyQ));
}

TcpTransport& ConnectionPool::borrowObject(const InetSocketAddress& key) {
    if (closed) {
        throw new HotRodClientException("Pool is closed");
    }
    BlockingQueue<TcpTransport *>* idleQ = idle[key];
    BlockingQueue<TcpTransport *>* busyQ = busy[key];

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
    bool ok = true;

    BlockingQueue<TcpTransport *>* idleQ = idle[key];
    BlockingQueue<TcpTransport *>* busyQ = busy[key];

    // Remove the object from the busy queue
    busyQ->remove(&val);

    // If necessary validate the object, then passivate it
    if (closed || (configuration.isTestOnReturn() && !factory->validateObject(key, val))) {
        ok = false;
    } else {
        factory->passivateObject(key, val);
    }

    if (ok) {
        // The object is still valid, see if the idle queue wants it
        ok = idleQ->offer(&val);
    }

    if (!ok) {
        // The object is now useless
        factory->destroyObject(key, val);
    }
}

void ConnectionPool::clear() {
    sys::ScopedLock<sys::Mutex> l(lock);
    for (std::map<InetSocketAddress, BlockingQueue<TcpTransport*>*>::iterator it = idle.begin(); it != idle.end(); ++it) {
        clear(it->first);
    }
}

void ConnectionPool::clear(const InetSocketAddress& key) {
    sys::ScopedLock<sys::Mutex> l(lock);
    BlockingQueue<TcpTransport *>* idleQ = idle[key];
    clear(key, idleQ);
    idle.erase(key);
}

void ConnectionPool::clear(const InetSocketAddress& key, BlockingQueue<TcpTransport *>* queue) {
    while (queue->size() > 0) {
        TcpTransport* transport = queue->pop();
        factory->destroyObject(key, *transport);
    }
    delete queue;
}

void ConnectionPool::checkIdle() {
    std::cout << "Ensuring idle connection limits" << std::endl;
    // TODO
}

void ConnectionPool::testIdle() {
    std::cout << "Testing idle connections" << std::endl;
    // TODO
}

void ConnectionPool::preparePool(const InetSocketAddress& key) {
    addObject(key);
}

void ConnectionPool::close() {
    sys::ScopedLock<sys::Mutex> l(lock);
    clear();
    closed = true;
}

void PoolWorker::run() {
    sys::Thread::sleep(pool->getConfiguration().getTimeBetweenEvictionRuns());
    pool->checkIdle();
    pool->testIdle();
}

}}}

