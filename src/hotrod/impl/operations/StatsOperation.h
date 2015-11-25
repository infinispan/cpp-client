#ifndef ISPN_HOTROD_OPERATIONS_STATSOPERATION_H
#define ISPN_HOTROD_OPERATIONS_STATSOPERATION_H



#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/VersionedOperationResponse.h"
#include "infinispan/hotrod/Flag.h"
#include "hotrod/impl/protocol/HotRodConstants.h"

#include <string>
#include <map>

namespace infinispan {
namespace hotrod {
class IntWrapper;
namespace operations {

class StatsOperation : public RetryOnFailureOperation<std::map<std::string, std::string> >
{
    protected:
        StatsOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            std::shared_ptr<transport::TransportFactory> transportFactory_,
            const hrbytes&                                   cacheName_,
            IntWrapper&                                 topologyId_,
            uint32_t                                   flags_);

        infinispan::hotrod::transport::Transport& getTransport(int retryCount);

        std::map<std::string, std::string> executeOperation(infinispan::hotrod::transport::Transport& transport);
    private:

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_STATSOPERATION_H

