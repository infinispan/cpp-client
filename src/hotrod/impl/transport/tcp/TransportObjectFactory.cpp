#include <hotrod/impl/transport/tcp/SSLTcpTransport.h>
#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"

namespace infinispan {
namespace hotrod {

using protocol::Codec;
using namespace operations;

namespace transport {


TransportObjectFactory::TransportObjectFactory(
    Codec& c, TcpTransportFactory& factory)
    : tcpTransportFactory(factory),
      codec(c)
{ }

TcpTransport& TransportObjectFactory::makeObject(const InetSocketAddress& address) {
    if(tcpTransportFactory.isSslEnabled()) {
        return *(new SSLTcpTransport(address, tcpTransportFactory,
                tcpTransportFactory.getSslServerCAPath(), tcpTransportFactory.getSslServerCAFile(), tcpTransportFactory.getSslClientCertificateFile()));
    } else {
        return *(new TcpTransport(address, tcpTransportFactory));
    }
}

bool TransportObjectFactory::validateObject(const InetSocketAddress& /*address*/, TcpTransport& transport) {
    return ping(transport) == SUCCESS;
}

void TransportObjectFactory::destroyObject(const InetSocketAddress& /*address*/, TcpTransport& transport) {
    transport.destroy();
    delete &transport;
}

void TransportObjectFactory::activateObject(const InetSocketAddress& /*address*/, TcpTransport& /*transport*/) {
    // empty
}

void TransportObjectFactory::passivateObject(
    const InetSocketAddress& /*address*/, TcpTransport& /*transport*/)
{
    // empty
}

PingResult TransportObjectFactory::ping(
    TcpTransport& tcpTransport)
{
    Topology tid(-1);
    PingOperation po(codec, tid, tcpTransport);
    return po.execute();
}

}}} // namespace infinispan::hotrod::transport
