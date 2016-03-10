#ifndef ISPN_HOTROD_OPERATIONS_EXECUTECMDOPERATION_H
#define ISPN_HOTROD_OPERATIONS_EXECUTECMDOPERATION_H



#include "hotrod/impl/operations/RetryOnFailureOperation.h"


#include <set>

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

class ExecuteCmdOperation : public RetryOnFailureOperation<std::vector<char> >
{
protected:
    infinispan::hotrod::transport::Transport& getTransport(int retryCount);
    ExecuteCmdOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            std::shared_ptr<transport::TransportFactory> transportFactory_,
            const std::vector<char>&                                   cacheName_,
            Topology&                                 topologyId_,
            uint32_t                                         flags_,
            const std::vector<char>&                                   cmdName_,
            const portable::map<std::vector<char>,std::vector<char>>&   cmdArgs_);
            std::vector<char> sendExecuteOperation(
                    transport::Transport&     transport,
					uint8_t                                       opCode,
					uint8_t                                       opRespCode);
	std::vector<char> executeOperation(infinispan::hotrod::transport::Transport& transport);
    const std::vector<char>& cmdName;
    const portable::map<std::vector<char>,std::vector<char>>& cmdArgs;
    friend class OperationsFactory;
};

}}} // namespace

#endif  /* ISPN_HOTROD_EXECUTECMDOPERATION_H */
