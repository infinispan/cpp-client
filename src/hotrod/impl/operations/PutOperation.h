#ifndef ISPN_HOTROD_OPERATIONS_PUTOPERATION_H
#define ISPN_HOTROD_OPERATIONS_PUTOPERATION_H



#include "hotrod/impl/operations/AbstractKeyValueOperation.h"

#include <set>

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

class PutOperation : public AbstractKeyValueOperation<std::vector<char>>
{
    public:
	std::vector<char> executeOperation(infinispan::hotrod::transport::Transport& transport);

    private:
        PutOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            std::shared_ptr<transport::TransportFactory> transportFactory_,
            const std::vector<char>&                                   key_,
            const std::vector<char>&                                   cacheName_,
            Topology&                                 topologyId_,
            uint32_t                                         flags_,
            const std::vector<char>&                                   value_,
            uint32_t                                         lifespan_,
            uint32_t                                         maxIdle_,
            EntryMediaTypes* df);

    friend class OperationsFactory;
};

}}} // namespace

#endif  /* ISPN_HOTROD_PUTOPERATION_H */
