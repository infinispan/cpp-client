#ifndef ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H
#define ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H

#include "hotrod/sys/types.h"
#include "infinispan/hotrod/Flag.h"

#include <set>
#include <stdint.h>

namespace infinispan {
namespace hotrod {

namespace transport {
class TransportFactory;
class Transport;
}

namespace protocol {
class Codec;
}

namespace operations {

class PingOperation;
class GetOperation;
class PutOperation;

class OperationsFactory
{
  public:
    OperationsFactory(
      infinispan::hotrod::transport::TransportFactory& transportFactory,
      const std::string& cacheName, uint32_t topologyId, bool forceReturnValue,
      const infinispan::hotrod::protocol::Codec& codec);

    PingOperation* newPingOperation(
      infinispan::hotrod::transport::Transport& transport);

    GetOperation* newGetKeyOperation(const hrbytes& key);

    PutOperation* newPutKeyValueOperation(
      const hrbytes& key, const hrbytes& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs);

    void addFlags(const std::set<Flag>& flags);
    void setFlags(const std::set<Flag>& flags);

  private:
    infinispan::hotrod::transport::TransportFactory& transportFactory;
    const infinispan::hotrod::protocol::Codec& codec;
    hrbytes cacheNameBytes;
    bool forceReturnValue;
    // TODO: atomic
    uint32_t topologyId;
    // TODO: thread local
    std::set<Flag> flags;

    std::set<Flag> getFlags();

  friend class RemoteCacheManagerImpl;

};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H

