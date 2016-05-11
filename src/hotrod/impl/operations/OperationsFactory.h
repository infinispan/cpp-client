#ifndef ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H
#define ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H

#include <infinispan/hotrod/CacheTopologyInfo.h>
#include "infinispan/hotrod/Flag.h"

#include <set>
#include <infinispan/hotrod/portable.h>
#include <query.pb.h>

using namespace org::infinispan::query::remote::client;

namespace infinispan {
namespace hotrod {

class RemoteCacheManagerImpl;
class Topology;

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
class SizeOperation;
class FaultTolerantPingOperation;
class ExecuteCmdOperation;
class QueryOperation;


class OperationsFactory
{
  public:

    PingOperation* newPingOperation(
      infinispan::hotrod::transport::Transport& transport);

    GetOperation* newGetKeyOperation(const std::vector<char>& key);

    PutOperation* newPutKeyValueOperation(
      const std::vector<char>& key, const std::vector<char>& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs);

    PutIfAbsentOperation* newPutIfAbsentOperation(
      const std::vector<char>& key, const std::vector<char>& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs);

    ReplaceOperation* newReplaceOperation(
      const std::vector<char>& key, const std::vector<char>& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs);

    RemoveOperation* newRemoveOperation(const std::vector<char>& key);

    ContainsKeyOperation* newContainsKeyOperation(const std::vector<char>& key);

    ReplaceIfUnmodifiedOperation* newReplaceIfUnmodifiedOperation(
      const std::vector<char>& key, const std::vector<char>& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs, int64_t version);

    RemoveIfUnmodifiedOperation* newRemoveIfUnmodifiedOperation(const std::vector<char>& key, int64_t version);

    GetWithMetadataOperation* newGetWithMetadataOperation(const std::vector<char>& key);

    GetWithVersionOperation* newGetWithVersionOperation(const std::vector<char>& key);

    BulkGetOperation* newBulkGetOperation(int size);

    BulkGetKeysOperation* newBulkGetKeysOperation(int scope);

    StatsOperation* newStatsOperation();

    ClearOperation* newClearOperation();

    SizeOperation* newSizeOperation();

    ExecuteCmdOperation* newExecuteCmdOperation(
        const std::vector<char>& cmdName, const std::map<std::vector<char>,std::vector<char>>& values);

    QueryOperation* newQueryOperation(const QueryRequest& qr);

    FaultTolerantPingOperation* newFaultTolerantPingOperation();

    void addFlags(uint32_t flags);
    void setFlags(uint32_t flags);

    CacheTopologyInfo getCacheTopologyInfo();

    virtual ~OperationsFactory() { }

private:
    std::shared_ptr<infinispan::hotrod::transport::TransportFactory> transportFactory;
    Topology topologyId;
    const infinispan::hotrod::protocol::Codec& codec;
    bool forceReturnValue;
    uint32_t flags;
    std::vector<char> cacheNameBytes;

    uint32_t getFlags();

    OperationsFactory(
      std::shared_ptr<infinispan::hotrod::transport::TransportFactory> transportFactory,
      const std::string& cacheName, bool forceReturnValue,
      const infinispan::hotrod::protocol::Codec& codec);

  friend class infinispan::hotrod::RemoteCacheManagerImpl;

};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H

