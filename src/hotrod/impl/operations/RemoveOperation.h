#ifndef ISPN_HOTROD_OPERATIONS_REMOVEOPERATION_H
#define ISPN_HOTROD_OPERATIONS_REMOVEOPERATION_H

#include "infinispan/hotrod/RemoteCacheBase.h"
#include "hotrod/impl/operations/AbstractKeyOperation.h"

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

class RemoveOperation : public AbstractKeyOperation<std::vector<char>>
{
    protected:
        std::vector<char> executeOperation(
            infinispan::hotrod::transport::Transport& transport);

    private:
        RemoveOperation(
            const infinispan::hotrod::protocol::Codec& codec,
            std::shared_ptr<transport::TransportFactory> transportFactory,
            const std::vector<char>& key, const std::vector<char>& cacheName,
            Topology& topologyId, uint32_t flags, EntryMediaTypes* df);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_REMOVEOPERATION_H
