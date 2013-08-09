#ifndef ISPN_HOTROD_OPERATIONS_BULKGETOPERATION_H
#define ISPN_HOTROD_OPERATIONS_BULKGETOPERATION_H



#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/VersionedOperationResponse.h"
#include "infinispan/hotrod/Flag.h"
#include "hotrod/impl/protocol/HotRodConstants.h"

#include <map>

namespace infinispan {
namespace hotrod {
namespace operations {

class BulkGetOperation : public RetryOnFailureOperation<std::map<hrbytes, hrbytes> >
{
    protected:
        BulkGetOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            infinispan::hotrod::transport::TransportFactory* transportFactory,
            const hrbytes&                                   cacheName_,
            uint32_t                                         topologyId_,
            uint32_t                                   flags_,
            int                                    entryCount_);

        infinispan::hotrod::transport::Transport& getTransport(int retryCount);

        std::map<hrbytes,hrbytes> executeOperation(infinispan::hotrod::transport::Transport& transport);
    private:
        int entryCount;

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_BULKGETOPERATION_H

