#ifndef SRC_HOTROD_IMPL_OPERATIONS_ADMINOPERATION_H_
#define SRC_HOTROD_IMPL_OPERATIONS_ADMINOPERATION_H_

#include "hotrod/impl/operations/ExecuteCmdOperation.h"
#include "hotrod/impl/transport/Transport.h"
namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::transport;
using namespace infinispan::hotrod::protocol;

class AdminOperation: public ExecuteCmdOperation {
protected:
    AdminOperation(const infinispan::hotrod::protocol::Codec&  codec_,
                   std::shared_ptr<transport::TransportFactory> transportFactory_,
                   const std::vector<char>& cacheName_,
                   Topology& topologyId_,
                   uint32_t flags_,
                   const std::vector<char>& cmdName_,
                   const std::map<std::vector<char>,std::vector<char>>& cmdArgs_)
        : ExecuteCmdOperation(codec_, transportFactory_, cacheName_, topologyId_, flags_, cmdName_, cmdArgs_, nullptr) {}

    friend class OperationsFactory;
};
}
}
}

#endif /* SRC_HOTROD_IMPL_OPERATIONS_ADMINOPERATION_H_ */
