#ifndef ISPN_HOTROD_OPERATIONS_GETWITHVERSIONOPERATION_H
#define ISPN_HOTROD_OPERATIONS_GETWITHVERSIONOPERATION_H



#include "hotrod/impl/operations/AbstractKeyOperation.h"
#include "hotrod/impl/VersionedValueImpl.h"

namespace infinispan {
namespace hotrod {
namespace operations {

class GetWithVersionOperation
   : public AbstractKeyOperation<VersionedValueImpl<hrbytes> >
{
    protected:
        VersionedValueImpl<hrbytes> executeOperation(
            infinispan::hotrod::transport::Transport& transport);

    private:
        GetWithVersionOperation(
            const infinispan::hotrod::protocol::Codec& codec,
            HR_SHARED_PTR<transport::TransportFactory> transportFactory,
            const hrbytes& key, const hrbytes& cacheName,
            uint32_t topologyId, uint32_t flags);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_GETWITHVERSIONOPERATION_H
