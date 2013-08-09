#ifndef ISPN_HOTROD_OPERATIONS_PUTOPERATION_H
#define ISPN_HOTROD_OPERATIONS_PUTOPERATION_H



#include "hotrod/impl/operations/AbstractKeyValueOperation.h"

#include <set>

namespace infinispan {
namespace hotrod {
namespace operations {

class PutOperation : public AbstractKeyValueOperation<hrbytes>
{
    public:
	hrbytes executeOperation(infinispan::hotrod::transport::Transport& transport);

    private:
        PutOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            infinispan::hotrod::transport::TransportFactory* transportFactory_,
            const hrbytes&                                   key_,
            const hrbytes&                                   cacheName_,
            uint32_t                                         topologyId_,
            uint32_t                            flags_,
            const hrbytes&                                   value_,
            uint32_t                                         lifespan_,
            uint32_t                                         maxIdle_);

    friend class OperationsFactory;
};

}}} // namespace

#endif  /* ISPN_HOTROD_PUTOPERATION_H */
