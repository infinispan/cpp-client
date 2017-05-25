#ifndef ISPN_HOTROD_TRANSPORT_TRANSPORTOBJECTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_TRANSPORTOBJECTFACTORY_H

#include <infinispan/hotrod/InetSocketAddress.h>
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/operations/PingOperation.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class TcpTransportFactory;

class AbstractObjectFactory {
public:
  AbstractObjectFactory() {}
  virtual TcpTransport& makeObject(const InetSocketAddress& address)=0;
  bool validateObject(const InetSocketAddress& address, TcpTransport& transport);
  virtual void destroyObject(const InetSocketAddress& address, TcpTransport& transport)=0;
  void activateObject(const InetSocketAddress& address, TcpTransport& transport);
  void passivateObject(const InetSocketAddress& address, TcpTransport& transport);
  virtual operations::PingResult ping(TcpTransport& tcpTransport)=0;
  virtual ~AbstractObjectFactory() {}
};

class TransportObjectFactory : public AbstractObjectFactory
{
  public:
    TransportObjectFactory(
        protocol::Codec& codec,
        TcpTransportFactory& tcpTransportFactory);

    virtual TcpTransport& makeObject(const InetSocketAddress& address);
    virtual void destroyObject(const InetSocketAddress& address, TcpTransport& transport);
    virtual operations::PingResult ping(TcpTransport& tcpTransport);
    virtual ~TransportObjectFactory() {};
  private:
    TcpTransportFactory& tcpTransportFactory;
    infinispan::hotrod::protocol::Codec& codec;
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_TRANSPORTOBJECTFACTORY_H */
