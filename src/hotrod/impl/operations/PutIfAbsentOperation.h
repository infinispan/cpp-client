#ifndef ISPN_HOTROD_OPERATIONS_PUTIFABSENTOPERATION_H
#define ISPN_HOTROD_OPERATIONS_PUTIFABSENTOPERATION_H



#include "hotrod/impl/operations/AbstractKeyValueOperation.h"

#include <set>

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

class PutIfAbsentOperation : public AbstractKeyValueOperation<std::vector<char>>
{
    protected:
        std::vector<char> executeOperation(
            infinispan::hotrod::transport::Transport& transport);

    private:
        PutIfAbsentOperation(
            const infinispan::hotrod::protocol::Codec& codec,
            std::shared_ptr<transport::TransportFactory> transportFactory_,
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

#endif  // ISPN_HOTROD_OPERATIONS_PUTIFABSENTOPERATION_H
