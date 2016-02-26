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
class Topology;
namespace operations {

class BulkGetKeysOperation : public RetryOnFailureOperation<std::set<std::vector<char>> >
{
    protected:
        BulkGetKeysOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            std::shared_ptr<transport::TransportFactory> transportFactory_,
            const std::vector<char>&                                   cacheName_,
            Topology&                                 topologyId_,
            uint32_t                                   flags_,
            int                                    entryCount_);

        infinispan::hotrod::transport::Transport& getTransport(int retryCount);

        std::set<std::vector<char>> executeOperation(infinispan::hotrod::transport::Transport& transport);
    private:
        int scope;

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_BULKGETKEYSOPERATION_H

