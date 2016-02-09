#ifndef ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H
#define ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H

#include "infinispan/hotrod/Flag.h"
#include "hotrod/types.h"

#include <set>
#include <infinispan/hotrod/portable.h>

namespace infinispan {
namespace hotrod {

class RemoteCacheManagerImpl;
class IntWrapper;

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
class ExecuteCmdOperation;

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

    ExecuteCmdOperation* newExecuteCmdOperation(
        const hrbytes& cmdName, const portable::map<hrbytes,hrbytes>& values);

    FaultTolerantPingOperation* newFaultTolerantPingOperation();

    void addFlags(uint32_t flags);
    void setFlags(uint32_t flags);

    virtual ~OperationsFactory() { }

private:
    std::shared_ptr<infinispan::hotrod::transport::TransportFactory> transportFactory;
    IntWrapper topologyId;
    const infinispan::hotrod::protocol::Codec& codec;
    bool forceReturnValue;
    uint32_t flags;
    hrbytes cacheNameBytes;

    uint32_t getFlags();

    OperationsFactory(
      std::shared_ptr<infinispan::hotrod::transport::TransportFactory> transportFactory,
      const std::string& cacheName, bool forceReturnValue,
      const infinispan::hotrod::protocol::Codec& codec);

  friend class infinispan::hotrod::RemoteCacheManagerImpl;

};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H

