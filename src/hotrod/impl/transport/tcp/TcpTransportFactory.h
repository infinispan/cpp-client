#ifndef ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H

#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/TransportFactory.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class TcpTransportFactory : public TransportFactory
{
    public:
        ~TcpTransportFactory() {};

        Transport* getTransport();
        Transport* getTransport(const hrbytes& key);
        void releaseTransport(Transport* transport);
        int getTransportCount();
        void invalidateTransport(/*SocketAdress,*/Transport* transport);
};

}}} // namespace

#endif  /* ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H */

