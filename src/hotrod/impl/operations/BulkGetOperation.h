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
class Topology;
namespace operations {

class BulkGetOperation : public RetryOnFailureOperation<std::map<std::vector<char>, std::vector<char>> >
{
    protected:
        BulkGetOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            std::shared_ptr<transport::TransportFactory> transportFactory,
            const std::vector<char>&                                   cacheName_,
            Topology&                                 topologyId_,
            uint32_t                                   flags_,
            int                                    entryCount_,
            EntryMediaTypes* df);

        std::map<std::vector<char>,std::vector<char>> executeOperation(infinispan::hotrod::transport::Transport& transport);
    private:
        int entryCount;

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_BULKGETOPERATION_H

