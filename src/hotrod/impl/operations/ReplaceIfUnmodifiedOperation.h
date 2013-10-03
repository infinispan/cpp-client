#ifndef ISPN_HOTROD_OPERATIONS_REPLACEIFUNMODIFIEDOPERATION_H
#define ISPN_HOTROD_OPERATIONS_REPLACEIFUNMODIFIEDOPERATION_H



#include "hotrod/impl/operations/AbstractKeyValueOperation.h"
#include "hotrod/impl/VersionedOperationResponse.h"

namespace infinispan {
namespace hotrod {
namespace operations {

class ReplaceIfUnmodifiedOperation
    : public AbstractKeyValueOperation<VersionedOperationResponse>
{
  protected:
    VersionedOperationResponse executeOperation(transport::Transport& transport);

  private:
    ReplaceIfUnmodifiedOperation(
        const protocol::Codec& codec,
        HR_SHARED_PTR<transport::TransportFactory> transportFactory,
        const hrbytes& key,
        const hrbytes& cacheName,
        uint32_t topologyId,
        uint32_t flags,
        const hrbytes& value,
        uint32_t lifespan,
        uint32_t maxIdle,
        int64_t version);

    int64_t version;

  friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_REPLACEIFUNMODIFIEDOPERATION_H
