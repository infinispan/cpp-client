#ifndef ISPN_HOTROD_TRANSPORT_CONNECTIONPOOL_H
#define ISPN_HOTROD_TRANSPORT_CONNECTIONPOOL_H

#include <map>
#include <iterator>
#include "hotrod/sys/types.h"
#include "hotrod/sys/Mutex.h"
#include "hotrod/impl/configuration/Configuration.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"
#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"

namespace infinispan {
namespace hotrod {
namespace transport {


class ConnectionPool
{
  public:
    ConnectionPool(
      HR_SHARED_PTR<TransportObjectFactory> factory_,
      const ConnectionPoolConfiguration& configuration_)
      : factory(factory_), configuration(configuration_), closed(false)
    { }

    ~ConnectionPool() {
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
    void returnObject(const InetSocketAddress& /*key*/, TcpTransport& /*val*/) { }
    TcpTransport& borrowObject(const InetSocketAddress& key);
    void invalidateObject(const InetSocketAddress& key, TcpTransport* val);
    void clear();
    void clear(const InetSocketAddress& key);
    void preparePool(const InetSocketAddress& key);
    void close();

  private:
    HR_SHARED_PTR<TransportObjectFactory> factory;
    const ConnectionPoolConfiguration& configuration;
    sys::Mutex lock;
    std::map<InetSocketAddress,TcpTransport*> poolMap;
    bool closed;

};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_CONNECTIONPOOL_H */
