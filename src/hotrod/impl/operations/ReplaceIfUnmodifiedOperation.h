#ifndef ISPN_HOTROD_OPERATIONS_REPLACEIFUNMODIFIEDOPERATION_H
#define ISPN_HOTROD_OPERATIONS_REPLACEIFUNMODIFIEDOPERATION_H



#include "hotrod/impl/operations/AbstractKeyValueOperation.h"
#include "hotrod/impl/VersionedOperationResponse.h"

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

class ReplaceIfUnmodifiedOperation
    : public AbstractKeyValueOperation<VersionedOperationResponse>
{
  protected:
    VersionedOperationResponse executeOperation(transport::Transport& transport);

  private:
    ReplaceIfUnmodifiedOperation(
        const protocol::Codec& codec,
        std::shared_ptr<transport::TransportFactory> transportFactory,
        const std::vector<char>& key,
        const std::vector<char>& cacheName,
        Topology& topologyId,
        uint32_t flags,
        const std::vector<char>& value,
        uint32_t lifespan,
        uint32_t maxIdle,
        int64_t version);

    int64_t version;

  friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_REPLACEIFUNMODIFIEDOPERATION_H
