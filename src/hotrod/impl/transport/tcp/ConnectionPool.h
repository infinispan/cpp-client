#ifndef ISPN_HOTROD_TRANSPORT_CONNECTIONPOOL_H
#define ISPN_HOTROD_TRANSPORT_CONNECTIONPOOL_H

#include <infinispan/hotrod/InetSocketAddress.h>
#include <map>
#include <iterator>
#include <queue>
#include "infinispan/hotrod/defs.h"
#include "hotrod/sys/BlockingQueue.h"
#include "hotrod/sys/Mutex.h"
#include "hotrod/sys/Runnable.h"
#include "hotrod/sys/Thread.h"
#include "infinispan/hotrod/Configuration.h"
#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"

using infinispan::hotrod::sys::BlockingQueue;

namespace infinispan {
namespace hotrod {
namespace transport {

class ConnectionPool;

class PoolWorker: public sys::Runnable {
  public:
    PoolWorker() : pool(0) {
    }

    void run();

    void setPool(ConnectionPool *pool_) {
        this->pool = pool_;
    }

  private:
    ConnectionPool *pool;

};

typedef std::shared_ptr<BlockingQueue<TcpTransport *> > TransportQueuePtr;

class ConnectionPool
{
  public:
    ConnectionPool(
      std::shared_ptr<AbstractObjectFactory> factory_,
      const ConnectionPoolConfiguration& configuration_)
      : factory(factory_), configuration(configuration_), closed(false),
		totalIdle(0), totalActive(0)
    {
        poolWorker.setPool(this);
        poolWorkerThread = new sys::Thread(poolWorker);
    }

    ~ConnectionPool() {
        close();
        poolWorkerThread->join();
        delete poolWorkerThread;
        clear();
    }

    const ConnectionPoolConfiguration& getConfiguration() const {
        return configuration;
    }

    int getNumActive() {
        sys::ScopedLock<sys::Mutex> l(lock);
        return totalActive;
    }

    int getNumActive(const InetSocketAddress& key) {
        sys::ScopedLock<sys::Mutex> l(lock);
        if (busy.find(key) != busy.end()) {
            return (int)busy[key]->size();
        }
        return 0;
    }

    int getNumIdle() {
        sys::ScopedLock<sys::Mutex> l(lock);
        return totalIdle;
    }

    int getNumIdle(const InetSocketAddress& key) {
        sys::ScopedLock<sys::Mutex> l(lock);
        if (idle.find(key) != idle.end()) {
            return (int)idle[key]->size();
        }
        return 0;
    }

    void addObject(const InetSocketAddress& key);
    void returnObject(const InetSocketAddress& key, TcpTransport& val);
    TcpTransport& borrowObject(const InetSocketAddress& key);
    bool tryRemoveIdleOrAskAllocate(const InetSocketAddress& key);
    void invalidateObject(const InetSocketAddress& key, TcpTransport* val);
    void clear();
    void clear(const InetSocketAddress& key);
    void preparePool(const InetSocketAddress& key);
    void close();

    void checkIdle();
    void testIdle();

    friend class PoolWorker;

  private:
    void clear(std::map<InetSocketAddress, TransportQueuePtr>& queue);
    void clear(const InetSocketAddress& key, TransportQueuePtr queue);
    void ensureMinIdle(const InetSocketAddress& key);
    int calculateMinIdleGrow(const InetSocketAddress& key);
    bool hasReachedMaxTotal();
    bool tryRemoveIdle();
	bool isValidOrDestroy(const InetSocketAddress& key,
			const TransportQueuePtr& busyQ, TcpTransport* obj);

    std::shared_ptr<AbstractObjectFactory> factory;
    const ConnectionPoolConfiguration& configuration;
    sys::Mutex lock;
    std::map<InetSocketAddress, TransportQueuePtr > busy;
    std::map<InetSocketAddress, TransportQueuePtr > idle;
    std::queue<InetSocketAddress> allocationQueue;
    bool closed;
    int totalIdle;
    int totalActive;
    PoolWorker poolWorker;
    sys::Thread *poolWorkerThread;

};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_CONNECTIONPOOL_H */
