#ifndef ISPN_HOTROD_OPERATIONS_GETWITHVERSIONOPERATION_H
#define ISPN_HOTROD_OPERATIONS_GETWITHVERSIONOPERATION_H



#include "hotrod/impl/operations/AbstractKeyOperation.h"
#include "hotrod/impl/VersionedValueImpl.h"

namespace infinispan {
namespace hotrod {
class IntWrapper;
namespace operations {

class GetWithVersionOperation
   : public AbstractKeyOperation<VersionedValueImpl<std::vector<char>> >
{
    protected:
        VersionedValueImpl<std::vector<char>> executeOperation(
            infinispan::hotrod::transport::Transport& transport);

    private:
        GetWithVersionOperation(
            const infinispan::hotrod::protocol::Codec& codec,
            std::shared_ptr<transport::TransportFactory> transportFactory,
            const std::vector<char>& key, const std::vector<char>& cacheName,
            IntWrapper& topologyId, uint32_t flags);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_GETWITHVERSIONOPERATION_H
