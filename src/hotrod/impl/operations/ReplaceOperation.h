#ifndef ISPN_HOTROD_OPERATIONS_REPLACEOPERATION_H
#define ISPN_HOTROD_OPERATIONS_REPLACEOPERATION_H



#include "hotrod/impl/operations/AbstractKeyValueOperation.h"

#include <set>

namespace infinispan {
namespace hotrod {
namespace operations {

class ReplaceOperation : public AbstractKeyValueOperation<hrbytes>
{
    protected:
        hrbytes executeOperation(
            infinispan::hotrod::transport::Transport& transport);

    private:
        ReplaceOperation(
            const infinispan::hotrod::protocol::Codec& codec,
            infinispan::hotrod::transport::TransportFactory* transportFactory,
            const hrbytes& key,
            const hrbytes& cacheName,
            uint32_t topologyId,
            uint32_t flags,
            const hrbytes& value,
            uint32_t lifespan,
            uint32_t maxIdle);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_REPLACEOPERATION_H
