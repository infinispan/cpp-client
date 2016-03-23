#ifndef ISPN_HOTROD_OPERATIONS_REPLACEOPERATION_H
#define ISPN_HOTROD_OPERATIONS_REPLACEOPERATION_H



#include "hotrod/impl/operations/AbstractKeyValueOperation.h"

#include <set>

namespace infinispan {
namespace hotrod {
namespace operations {

class ReplaceOperation : public AbstractKeyValueOperation<std::vector<char>>
{
    protected:
        std::vector<char> executeOperation(
            infinispan::hotrod::transport::Transport& transport);

    private:
        ReplaceOperation(
            const infinispan::hotrod::protocol::Codec& codec,
            std::shared_ptr<transport::TransportFactory> transportFactory,
            const std::vector<char>& key,
            const std::vector<char>& cacheName,
            Topology& topologyId,
            uint32_t flags,
            const std::vector<char>& value,
            uint32_t lifespan,
            uint32_t maxIdle);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_REPLACEOPERATION_H
