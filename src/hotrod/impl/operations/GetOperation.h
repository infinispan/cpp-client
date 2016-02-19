#ifndef ISPN_HOTROD_OPERATIONS_GETOPERATION_H
#define ISPN_HOTROD_OPERATIONS_GETOPERATION_H

#include "infinispan/hotrod/RemoteCacheBase.h"
#include "hotrod/types.h"
#include "hotrod/impl/operations/AbstractKeyOperation.h"

namespace infinispan {
namespace hotrod {
class IntWrapper;
namespace operations {

class GetOperation : public AbstractKeyOperation<std::vector<char>>
{
  protected:
    std::vector<char> executeOperation(
        infinispan::hotrod::transport::Transport& transport);

  private:
    GetOperation(
        const infinispan::hotrod::protocol::Codec& codec,
        std::shared_ptr<transport::TransportFactory> transportFactory,
        const std::vector<char>& key, const std::vector<char>& cacheName,
        IntWrapper& topologyId, uint32_t flags);

  friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_GETOPERATION_H
