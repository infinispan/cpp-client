#ifndef ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H

#include "hotrod/impl/transport/Transport.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class TransportFactory
{
    public:
        // TODO: no singleton, one instance each RemoteCacheManager
        static TransportFactory& getTransportFactory();

        virtual Transport* getTransport() = 0;
        virtual Transport* getTransport(const hrbytes& key) = 0;
        virtual void releaseTransport(Transport* transport) = 0;
        virtual int getTransportCount() = 0;
        virtual void invalidateTransport(/*SocketAdress,*/Transport* transport) = 0;
        virtual ~TransportFactory() {}

    private:
        static TransportFactory* transportFactory;

    friend class RemoteCacheManager;
};

}}} // namespace

#endif  /* ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H */

