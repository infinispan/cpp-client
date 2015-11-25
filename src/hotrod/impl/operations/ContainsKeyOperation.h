#ifndef ISPN_HOTROD_OPERATIONS_CONTAINSKEYOPERATION_H
#define ISPN_HOTROD_OPERATIONS_CONTAINSKEYOPERATION_H

#include "hotrod/impl/operations/AbstractKeyOperation.h"

namespace infinispan {
namespace hotrod {
class IntWrapper;
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
            const hrbytes& key, const hrbytes& cacheName,
            IntWrapper& topologyId, uint32_t flags);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_CONTAINSKEYOPERATION_H
