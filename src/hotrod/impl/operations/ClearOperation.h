#ifndef ISPN_HOTROD_OPERATIONS_CLEAROPERATION_H
#define ISPN_HOTROD_OPERATIONS_CLEAROPERATION_H



#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/VersionedOperationResponse.h"
#include "infinispan/hotrod/Flag.h"
#include "hotrod/impl/protocol/HotRodConstants.h"

namespace infinispan {
namespace hotrod {
namespace operations {

class ClearOperation : public RetryOnFailureOperation<hrbytes>
{
    protected:
	ClearOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            infinispan::hotrod::transport::TransportFactory* transportFactory_,
            const hrbytes&                                   cacheName_,
            uint32_t                                         topologyId_,
            uint32_t                                   flags_);

        infinispan::hotrod::transport::Transport& getTransport(int retryCount);

        hrbytes executeOperation(infinispan::hotrod::transport::Transport& transport);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_CLEAROPERATION_H

