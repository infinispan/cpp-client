#ifndef ISPN_HOTROD_TRANSPORT_CONNECTIONPOOL_H
#define ISPN_HOTROD_TRANSPORT_CONNECTIONPOOL_H

#include <map>
#include <iterator>
#include "infinispan/hotrod/types.h"
#include "hotrod/sys/BlockingQueue.h"
#include "hotrod/sys/Mutex.h"
#include "hotrod/sys/Runnable.h"
#include "hotrod/sys/Thread.h"
#include "infinispan/hotrod/Configuration.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"
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

class ConnectionPool
{
  public:
    ConnectionPool(
      HR_SHARED_PTR<TransportObjectFactory> factory_,
      const ConnectionPoolConfiguration& configuration_)
      : factory(factory_), configuration(configuration_), closed(false)
    {
        poolWorker.setPool(this);
        poolWorkerThread = new sys::Thread(poolWorker);
    }

    ~ConnectionPool() {
        // FIXME: stop poolWorker
        close();
        clear();
    }

    const ConnectionPoolConfiguration& getConfiguration() const {
        return configuration;
    }

    int getNumActive() { return 0; }
    int getNumActive(const InetSocketAddress& /*key*/) { return 0; }
    int getNumIdle() { return 0; }
    int getNumIdle(const InetSocketAddress& /*key*/) { return 0; }

    void addObject(const InetSocketAddress& key);
    void returnObject(const InetSocketAddress& key, TcpTransport& val);
    TcpTransport& borrowObject(const InetSocketAddress& key);
    void invalidateObject(const InetSocketAddress& key, TcpTransport* val);
    void clear();
    void clear(const InetSocketAddress& key);
    void preparePool(const InetSocketAddress& key);
    void close();

    void checkIdle();
    void testIdle();

    friend class PoolWorker;

  private:
    void clear(const InetSocketAddress& key, BlockingQueue<TcpTransport *>* queue);

    HR_SHARED_PTR<TransportObjectFactory> factory;
    const ConnectionPoolConfiguration& configuration;
    sys::Mutex lock;
    std::map<InetSocketAddress, BlockingQueue<TcpTransport*>*> busy;
    std::map<InetSocketAddress, BlockingQueue<TcpTransport*>*> idle;
    bool closed;
    PoolWorker poolWorker;
    sys::Thread *poolWorkerThread;

};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_CONNECTIONPOOL_H */
