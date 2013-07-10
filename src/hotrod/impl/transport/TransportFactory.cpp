#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"
#include "hotrod/impl/transport/Transport.h"


namespace infinispan {
namespace hotrod {
namespace transport {

TransportFactory* TransportFactory::transportFactory = 0;
TransportFactory& TransportFactory::getTransportFactory() {
    if (!transportFactory) {
    	transportFactory = new TcpTransportFactory();
    }
    return *transportFactory;
}

}}} /* namespace */
