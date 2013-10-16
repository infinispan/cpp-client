#include "hotrod/impl/transport/tcp/ConnectionPool.h"

namespace infinispan {
namespace hotrod {
namespace transport {

TcpTransport& ConnectionPool::borrowObject(const InetSocketAddress& key) {
    sys::ScopedLock < sys::Mutex > l(lock);
    if (closed) {
        throw HotRodClientException("POOL CLOSED");
    }
    TcpTransport* val = poolMap[key];
    if (!val) {
        val = &factory->makeObject(key);
        poolMap[key] = val;
    }
    factory->activateObject(key, *val);
    return *val;
}

void ConnectionPool::invalidateObject(const InetSocketAddress& key, TcpTransport* val) {
    sys::ScopedLock < sys::Mutex > l(lock);
    poolMap[key] = NULL;
    if (val != NULL) {
        factory->destroyObject(key, *val);
    }
}

void ConnectionPool::clear() {
    sys::ScopedLock < sys::Mutex > l(lock);
    for (typename std::map<InetSocketAddress, TcpTransport*>::iterator iter = poolMap.begin(); iter != poolMap.end();
            ++iter) {
        clear(iter->first);
    }
}

void ConnectionPool::clear(const InetSocketAddress& key) {
    sys::ScopedLock < sys::Mutex > l(lock);
    TcpTransport* val = poolMap[key];
    if (val) {
        factory->destroyObject(key, *val);
    }
    poolMap[key] = 0;
}

void ConnectionPool::preparePool(const InetSocketAddress& key) {
    sys::ScopedLock < sys::Mutex > l(lock);
    poolMap[key] = NULL;
}

void ConnectionPool::close() {
    sys::ScopedLock < sys::Mutex > l(lock);
    clear();
    closed = true;
}

void ConnectionPool::addObject(const InetSocketAddress& key) {
    sys::ScopedLock < sys::Mutex > l(lock);
    TcpTransport& val = factory->makeObject(key);
    poolMap[key] = &val;
}

}}}

