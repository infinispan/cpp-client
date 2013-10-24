#ifndef ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H
#define ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H

#include "infinispan/hotrod/Flag.h"

#include <set>

namespace infinispan {
namespace hotrod {

class RemoteCacheManagerImpl;

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
class PutIfAbsentOperation;
class ReplaceOperation;
class RemoveOperation;
class ContainsKeyOperation;
class ReplaceIfUnmodifiedOperation;
class RemoveIfUnmodifiedOperation;
class GetWithMetadataOperation;
class GetWithVersionOperation;
class BulkGetOperation;
class BulkGetKeysOperation;
class StatsOperation;
class ClearOperation;
class FaultTolerantPingOperation;

class OperationsFactory
{
  public:

    PingOperation* newPingOperation(
      infinispan::hotrod::transport::Transport& transport);

    GetOperation* newGetKeyOperation(const hrbytes& key);

    PutOperation* newPutKeyValueOperation(
      const hrbytes& key, const hrbytes& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs);

    PutIfAbsentOperation* newPutIfAbsentOperation(
      const hrbytes& key, const hrbytes& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs);

    ReplaceOperation* newReplaceOperation(
      const hrbytes& key, const hrbytes& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs);

    RemoveOperation* newRemoveOperation(const hrbytes& key);

    ContainsKeyOperation* newContainsKeyOperation(const hrbytes& key);

    ReplaceIfUnmodifiedOperation* newReplaceIfUnmodifiedOperation(
      const hrbytes& key, const hrbytes& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs, int64_t version);

    RemoveIfUnmodifiedOperation* newRemoveIfUnmodifiedOperation(const hrbytes& key, int64_t version);

    GetWithMetadataOperation* newGetWithMetadataOperation(const hrbytes& key);

    GetWithVersionOperation* newGetWithVersionOperation(const hrbytes& key);

    BulkGetOperation* newBulkGetOperation(int size);

    BulkGetKeysOperation* newBulkGetKeysOperation(int scope);

    StatsOperation* newStatsOperation();

    ClearOperation* newClearOperation();

    FaultTolerantPingOperation* newFaultTolerantPingOperation();

    void addFlags(uint32_t flags);
    void setFlags(uint32_t flags);

  private:
    HR_SHARED_PTR<infinispan::hotrod::transport::TransportFactory> transportFactory;
    const infinispan::hotrod::protocol::Codec& codec;
    hrbytes cacheNameBytes;
    bool forceReturnValue;
    uint32_t topologyId;
    uint32_t flags;

    uint32_t getFlags();

    OperationsFactory(
      HR_SHARED_PTR<infinispan::hotrod::transport::TransportFactory> transportFactory,
      const std::string& cacheName, uint32_t topologyId, bool forceReturnValue,
      const infinispan::hotrod::protocol::Codec& codec);

  friend class infinispan::hotrod::RemoteCacheManagerImpl;

};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H

