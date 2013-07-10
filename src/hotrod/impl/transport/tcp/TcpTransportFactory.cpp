#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"


namespace infinispan {
namespace hotrod {
namespace transport {

// TODO

Transport* TcpTransportFactory::getTransport()
{
    return new TcpTransport();
}

Transport* TcpTransportFactory::getTransport(const hrbytes& key)
{
    Transport* transport = 0;
    hrbytes key_ = const_cast<hrbytes&>(key);
    if (key_.length() < 1000) {
    	transport = new TcpTransport();
    }
    return transport;
}

void TcpTransportFactory::releaseTransport(Transport* transport)
{
    delete transport;
}

int TcpTransportFactory::getTransportCount()
{
    return 0;
}

void TcpTransportFactory::invalidateTransport(/*SocketAdress,*/Transport* transport)
{
    delete transport;
}

}}} /* namespace */
