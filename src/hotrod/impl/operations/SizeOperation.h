#ifndef ISPN_HOTROD_OPERATIONS_SIZEOPERATION_H
#define ISPN_HOTROD_OPERATIONS_SIZEOPERATION_H



#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/VersionedOperationResponse.h"
#include "infinispan/hotrod/Flag.h"
#include "hotrod/impl/protocol/HotRodConstants.h"

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

class SizeOperation : public RetryOnFailureOperation<uint64_t>
{
    protected:
	SizeOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            std::shared_ptr<transport::TransportFactory> transportFactory_,
            const std::vector<char>&                                   cacheName_,
            Topology&                                 topologyId_,
            uint32_t                                   flags_);

        infinispan::hotrod::transport::Transport& getTransport(int retryCount);

        uint64_t executeOperation(infinispan::hotrod::transport::Transport& transport);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_SIZEOPERATION_H

