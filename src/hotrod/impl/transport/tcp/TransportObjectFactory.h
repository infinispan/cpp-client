#ifndef ISPN_HOTROD_TRANSPORT_TRANSPORTOBJECTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_TRANSPORTOBJECTFACTORY_H

#include <infinispan/hotrod/InetSocketAddress.h>
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/operations/PingOperation.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class TcpTransportFactory;

class TransportObjectFactory
{
  public:
    TransportObjectFactory(
        protocol::Codec& codec,
        TcpTransportFactory& tcpTransportFactory);

    virtual TcpTransport& makeObject(const InetSocketAddress& address);
    bool validateObject(const InetSocketAddress& address, TcpTransport& transport);
    virtual void destroyObject(const InetSocketAddress& address, TcpTransport& transport);

    void activateObject(const InetSocketAddress& address, TcpTransport& transport);
    void passivateObject(const InetSocketAddress& address, TcpTransport& transport);

    virtual operations::PingResult ping(TcpTransport& tcpTransport);
  private:
    TcpTransportFactory& tcpTransportFactory;
    infinispan::hotrod::protocol::Codec& codec;


};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_TRANSPORTOBJECTFACTORY_H */
