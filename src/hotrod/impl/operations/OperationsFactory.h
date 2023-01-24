#ifndef ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H
#define ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H

#include <infinispan/hotrod/CacheTopologyInfo.h>
#include <infinispan/hotrod/DataFormat.h>
#include "infinispan/hotrod/Flag.h"

#include "infinispan/hotrod/Query.h"
#include "infinispan/hotrod/ClientListener.h"
#include "infinispan/hotrod/TransactionManager.h"
#include "infinispan/hotrod/Transactions.h"
#include <set>
#include <functional>
#include <memory>
using namespace org::infinispan::query::remote::client;
using namespace infinispan::hotrod::event;

namespace infinispan {
namespace hotrod {

class RemoteCacheManagerImpl;
class RemoteCacheImpl;
class Topology;

namespace transport {
class TransportFactory;
class Transport;
}

namespace protocol {
class Codec;
}

namespace event
{
class ClientListenerNotifier;
}
namespace operations {

class PingOperation;
class GetOperation;
class GetAllOperation;
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
class ExecuteCmdKeyOperation;
class QueryOperation;
class AddClientListenerOperation;
class RemoveClientListenerOperation;
class AdminOperation;
class PrepareCommitOperation;
class CommitOperation;
class RollbackOperation;


class OperationsFactory
{
  public:

    PingOperation* newPingOperation(
      infinispan::hotrod::transport::Transport& transport, EntryMediaTypes* df);

    GetOperation* newGetKeyOperation(const std::vector<char>& key, EntryMediaTypes* df);

    GetAllOperation* newGetAllOperation(const std::set<std::vector<char>>& keySet, EntryMediaTypes* df);

    PutOperation* newPutKeyValueOperation(
      const std::vector<char>& key, const std::vector<char>& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs, EntryMediaTypes* df);

    PutIfAbsentOperation* newPutIfAbsentOperation(
      const std::vector<char>& key, const std::vector<char>& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs, EntryMediaTypes* df);

    ReplaceOperation* newReplaceOperation(
      const std::vector<char>& key, const std::vector<char>& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs, EntryMediaTypes* df);

    RemoveOperation* newRemoveOperation(const std::vector<char>& key, EntryMediaTypes* df);

    ContainsKeyOperation* newContainsKeyOperation(const std::vector<char>& key, EntryMediaTypes* df);

    ReplaceIfUnmodifiedOperation* newReplaceIfUnmodifiedOperation(
      const std::vector<char>& key, const std::vector<char>& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs, int64_t version, EntryMediaTypes* df);

    RemoveIfUnmodifiedOperation* newRemoveIfUnmodifiedOperation(const std::vector<char>& key, int64_t version, EntryMediaTypes* df);

    GetWithMetadataOperation* newGetWithMetadataOperation(const std::vector<char>& key, EntryMediaTypes* df);

    GetWithVersionOperation* newGetWithVersionOperation(const std::vector<char>& key, EntryMediaTypes* df);

    BulkGetOperation* newBulkGetOperation(int size, EntryMediaTypes* df);

    BulkGetKeysOperation* newBulkGetKeysOperation(int scope, EntryMediaTypes* df);

    StatsOperation* newStatsOperation(EntryMediaTypes* df);

    ClearOperation* newClearOperation(EntryMediaTypes* df);

    SizeOperation* newSizeOperation(EntryMediaTypes* df);

    ExecuteCmdOperation* newExecuteCmdOperation(
        const std::vector<char>& cmdName, const std::map<std::vector<char>,std::vector<char>>& values, EntryMediaTypes* df);

    ExecuteCmdKeyOperation* newExecuteCmdKeyOperation(
        const std::vector<char>& key, const std::vector<char>& cmdName, const std::map<std::vector<char>,std::vector<char>>& values, EntryMediaTypes* df);

    QueryOperation* newQueryOperation(const QueryRequest& qr, EntryMediaTypes* df);

    FaultTolerantPingOperation* newFaultTolerantPingOperation(EntryMediaTypes* df);

    AddClientListenerOperation* newAddClientListenerOperation(ClientListener& listener, ClientListenerNotifier& listenerNotifier, const std::vector<std::vector<char> > filterFactoryParam, const std::vector<std::vector<char> > converterFactoryParams,const std::function<void()> &recoveryCallback, EntryMediaTypes* df);
    RemoveClientListenerOperation* newRemoveClientListenerOperation(ClientListener& listener, ClientListenerNotifier& listenerNotifier, EntryMediaTypes* df);

    PrepareCommitOperation* newPrepareCommitOperation(XID xid, TransactionContext& tctx, EntryMediaTypes* df);
    CommitOperation* newCommitOperation(XID xid, TransactionContext& tctx, EntryMediaTypes* df);
    RollbackOperation* newRollbackOperation(XID xid, TransactionContext& tctx, EntryMediaTypes* df);

    void addFlags(uint32_t flags);
    void setFlags(uint32_t flags);

    AdminOperation* newAdminOperation(
        const std::vector<char>& cmdName, const std::map<std::vector<char>,std::vector<char>>& values);

    CacheTopologyInfo getCacheTopologyInfo();

    std::shared_ptr<infinispan::hotrod::transport::TransportFactory> getTransportFactory() { return transportFactory; }


    virtual ~OperationsFactory() { }

private:
    std::shared_ptr<infinispan::hotrod::transport::TransportFactory> transportFactory;
    Topology topologyId;
    std::shared_ptr<infinispan::hotrod::protocol::Codec> codecPtr;
    const infinispan::hotrod::protocol::Codec& codec;
    bool forceReturnValue;
    uint32_t flags;
    std::vector<char> cacheNameBytes;

    uint32_t getFlags();

    OperationsFactory(
      std::shared_ptr<infinispan::hotrod::transport::TransportFactory> transportFactory,
      const std::string& cacheName, bool forceReturnValue, infinispan::hotrod::protocol::Codec& codec);

  friend class infinispan::hotrod::RemoteCacheManagerImpl;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H

