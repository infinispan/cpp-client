#ifndef ISPN_HOTROD_OPERATIONS_RETRYONFAILUREOPERATION_H
#define ISPN_HOTROD_OPERATIONS_RETRYONFAILUREOPERATION_H

#include "hotrod/impl/operations/HotRodOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"

namespace infinispan {
namespace hotrod {
namespace operations {

template<class T> class RetryOnFailureOperation : public HotRodOperation<T>
{
    public:
        T execute()
        {
            // TODO : retry on failure management
        	int retryCount = 0;
        	infinispan::hotrod::transport::Transport* transport = getTransport(retryCount);
            T result = executeOperation(transport);
            transportFactory.releaseTransport(transport);
            return result;
        }

    protected:
        RetryOnFailureOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            infinispan::hotrod::transport::TransportFactory& transportFactory_,
            const hrbytes&                                         cacheName_,
            uint32_t                                         topologyId_,
            const std::set<Flag>&                                   flags_)
            : HotRodOperation<T>(codec_, flags_, cacheName_, topologyId_),
              transportFactory(transportFactory_)
        {}

        infinispan::hotrod::transport::TransportFactory& transportFactory;

        virtual infinispan::hotrod::transport::Transport* getTransport(int retryCount) = 0;
        virtual T executeOperation(
            infinispan::hotrod::transport::Transport* transport) = 0;

        virtual ~RetryOnFailureOperation() {}
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_RETRYONFAILUREOPERATION_H
