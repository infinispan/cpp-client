#ifndef ISPN_HOTROD_OPERATIONS_CONTAINSKEYOPERATION_H
#define ISPN_HOTROD_OPERATIONS_CONTAINSKEYOPERATION_H



#include "infinispan/hotrod/types.h"
#include "infinispan/hotrod/ScopedBuffer.h"
#include "infinispan/hotrod/RemoteCacheBase.h"
#include "hotrod/impl/operations/AbstractKeyOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

class ContainsKeyOperation : public AbstractKeyOperation<bool>
{
    protected:
        bool executeOperation(
            infinispan::hotrod::transport::Transport& transport);

    private:
        ContainsKeyOperation(
            const infinispan::hotrod::protocol::Codec& codec,
            infinispan::hotrod::transport::TransportFactory* transportFactory,
            const hrbytes& key, const hrbytes& cacheName,
            uint32_t topologyId, uint32_t flags);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_CONTAINSKEYOPERATION_H
