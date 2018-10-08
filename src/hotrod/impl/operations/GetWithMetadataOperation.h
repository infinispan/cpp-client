#ifndef ISPN_HOTROD_OPERATIONS_GETWITHMETADATAOPERATION_H
#define ISPN_HOTROD_OPERATIONS_GETWITHMETADATAOPERATION_H



#include "hotrod/impl/operations/AbstractKeyOperation.h"
#include "hotrod/impl/MetadataValueImpl.h"

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

class GetWithMetadataOperation
   : public AbstractKeyOperation<MetadataValueImpl<std::vector<char>> >
{
    protected:
        MetadataValueImpl<std::vector<char>> executeOperation(
            infinispan::hotrod::transport::Transport& transport);

    private:
        GetWithMetadataOperation(
            const infinispan::hotrod::protocol::Codec& codec,
            std::shared_ptr<transport::TransportFactory> transportFactory,
            const std::vector<char>& key, const std::vector<char>& cacheName,
            Topology& topologyId, uint32_t flags, EntryMediaTypes* df);

    friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_GETWITHMETADATAOPERATION_H
