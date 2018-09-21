#ifndef ISPN_HOTROD_OPERATIONS_CONTAINSKEYOPERATION_H
#define ISPN_HOTROD_OPERATIONS_CONTAINSKEYOPERATION_H

#include "hotrod/impl/operations/AbstractKeyOperation.h"

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

class ContainsKeyOperation : public AbstractKeyOperation<bool>
{
    protected:
        bool executeOperation(
            infinispan::hotrod::transport::Transport& transport);

    private:
        ContainsKeyOperation(
            const infinispan::hotrod::protocol::Codec& codec,
            std::shared_ptr<transport::TransportFactory> transportFactory,
            const std::vector<char>& key, const std::vector<char>& cacheName,
            Topology& topologyId, uint32_t flags, EntryMediaTypes* df);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_CONTAINSKEYOPERATION_H
