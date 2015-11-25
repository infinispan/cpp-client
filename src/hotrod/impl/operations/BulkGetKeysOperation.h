#ifndef ISPN_HOTROD_OPERATIONS_BULKGETKEYSOPERATION_H
#define ISPN_HOTROD_OPERATIONS_BULKGETKEYSOPERATION_H



#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/VersionedOperationResponse.h"
#include "infinispan/hotrod/Flag.h"
#include "hotrod/impl/protocol/HotRodConstants.h"

#include <set>

namespace infinispan {
namespace hotrod {
class IntWrapper;
namespace operations {

class BulkGetKeysOperation : public RetryOnFailureOperation<std::set<hrbytes> >
{
    protected:
        BulkGetKeysOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            std::shared_ptr<transport::TransportFactory> transportFactory_,
            const hrbytes&                                   cacheName_,
            IntWrapper&                                 topologyId_,
            uint32_t                                   flags_,
            int                                    entryCount_);

        infinispan::hotrod::transport::Transport& getTransport(int retryCount);

        std::set<hrbytes> executeOperation(infinispan::hotrod::transport::Transport& transport);
    private:
        int scope;

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_BULKGETKEYSOPERATION_H

