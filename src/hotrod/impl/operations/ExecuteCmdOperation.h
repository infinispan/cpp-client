#ifndef ISPN_HOTROD_OPERATIONS_EXECUTECMDOPERATION_H
#define ISPN_HOTROD_OPERATIONS_EXECUTECMDOPERATION_H



#include "hotrod/impl/operations/RetryOnFailureOperation.h"


#include <set>

namespace infinispan {
namespace hotrod {
class IntWrapper;
namespace operations {

class ExecuteCmdOperation : public RetryOnFailureOperation<hrbytes >
{
protected:
    infinispan::hotrod::transport::Transport& getTransport(int retryCount);
    ExecuteCmdOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            std::shared_ptr<transport::TransportFactory> transportFactory_,
            const hrbytes&                                   cacheName_,
            IntWrapper&                                 topologyId_,
            uint32_t                                         flags_,
            const hrbytes&                                   cmdName_,
            const portable::map<hrbytes,hrbytes>&   cmdArgs_);
            hrbytes sendExecuteOperation(
                    transport::Transport&     transport,
					uint8_t                                       opCode,
					uint8_t                                       opRespCode);
	hrbytes executeOperation(infinispan::hotrod::transport::Transport& transport);
    const hrbytes& cmdName;
    const portable::map<hrbytes,hrbytes>& cmdArgs;
    friend class OperationsFactory;
};

}}} // namespace

#endif  /* ISPN_HOTROD_EXECUTECMDOPERATION_H */
