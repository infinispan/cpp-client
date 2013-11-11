#ifndef ISPN_HOTROD_OPERATIONS_PUTIFABSENTOPERATION_H
#define ISPN_HOTROD_OPERATIONS_PUTIFABSENTOPERATION_H



#include "hotrod/impl/operations/AbstractKeyValueOperation.h"

#include <set>

namespace infinispan {
namespace hotrod {
class IntWrapper;
namespace operations {

class PutIfAbsentOperation : public AbstractKeyValueOperation<hrbytes>
{
    protected:
        hrbytes executeOperation(
            infinispan::hotrod::transport::Transport& transport);

    private:
        PutIfAbsentOperation(
            const infinispan::hotrod::protocol::Codec& codec,
            HR_SHARED_PTR<transport::TransportFactory> transportFactory_,
            const hrbytes& key,
            const hrbytes& cacheName,
            IntWrapper& topologyId,
            uint32_t flags,
            const hrbytes& value,
            uint32_t lifespan,
            uint32_t maxIdle);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_PUTIFABSENTOPERATION_H
