#ifndef ISPN_HOTROD_OPERATIONS_FAULTTOLERANTPINGOPERATION_H
#define ISPN_HOTROD_OPERATIONS_FAULTTOLERANTPINGOPERATION_H



#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/operations/PingOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "infinispan/hotrod/Flag.h"
#include "hotrod/impl/protocol/HotRodConstants.h"

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

class FaultTolerantPingOperation : public RetryOnFailureOperation<PingResult>
{
    protected:
        FaultTolerantPingOperation(
            const protocol::Codec&       codec_,
            std::shared_ptr<transport::TransportFactory> transportFactory_,
            const std::vector<char>&                                   cacheName_,
            Topology&                                 topologyId_,
            uint32_t                                   flags_,
            EntryMediaTypes* df);

        PingResult executeOperation(transport::Transport& transport);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_FAULTTOLERANTPINGOPERATION_H

