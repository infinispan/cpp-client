#ifndef ISPN_HOTROD_OPERATIONS_GETWITHMETADATAOPERATION_H
#define ISPN_HOTROD_OPERATIONS_GETWITHMETADATAOPERATION_H



#include "hotrod/impl/operations/AbstractKeyOperation.h"
#include "hotrod/impl/MetadataValueImpl.h"

namespace infinispan {
namespace hotrod {
class IntWrapper;
namespace operations {

class GetWithMetadataOperation
   : public AbstractKeyOperation<MetadataValueImpl<hrbytes> >
{
    protected:
        MetadataValueImpl<hrbytes> executeOperation(
            infinispan::hotrod::transport::Transport& transport);

    private:
        GetWithMetadataOperation(
            const infinispan::hotrod::protocol::Codec& codec,
            HR_SHARED_PTR<transport::TransportFactory> transportFactory,
            const hrbytes& key, const hrbytes& cacheName,
            IntWrapper& topologyId, uint32_t flags);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_GETWITHMETADATAOPERATION_H
