#include <hotrod/impl/Topology.h>
#include "hotrod/impl/operations/OperationsFactory.h"
#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/operations/GetOperation.h"
#include "hotrod/impl/operations/GetAllOperation.h"
#include "hotrod/impl/operations/PutOperation.h"
#include "hotrod/impl/operations/PutIfAbsentOperation.h"
#include "hotrod/impl/operations/ReplaceOperation.h"
#include "hotrod/impl/operations/RemoveOperation.h"
#include "hotrod/impl/operations/ContainsKeyOperation.h"
#include "hotrod/impl/operations/PingOperation.h"
#include "hotrod/impl/operations/ReplaceIfUnmodifiedOperation.h"
#include "hotrod/impl/operations/RemoveIfUnmodifiedOperation.h"
#include "hotrod/impl/operations/GetWithMetadataOperation.h"
#include "hotrod/impl/operations/GetWithVersionOperation.h"
#include "hotrod/impl/operations/BulkGetOperation.h"
#include "hotrod/impl/operations/BulkGetKeysOperation.h"
#include "hotrod/impl/operations/StatsOperation.h"
#include "hotrod/impl/operations/ClearOperation.h"
#include "hotrod/impl/operations/SizeOperation.h"
#include "hotrod/impl/operations/FaultTolerantPingOperation.h"
#include "hotrod/impl/operations/ExecuteCmdOperation.h"
#include "hotrod/impl/operations/QueryOperation.h"
#include <hotrod/impl/operations/AddClientListenerOperation.h>
#include <hotrod/impl/operations/RemoveClientListenerOperation.h>
#include <hotrod/impl/operations/AdminOperation.h>
#include <hotrod/impl/operations/TransactionOperations.h>
#include "infinispan/hotrod/Flag.h"

#include <cstring>
#include <vector>
#include <functional>

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace protocol;
using namespace transport;

OperationsFactory::OperationsFactory(
		std::shared_ptr<infinispan::hotrod::transport::TransportFactory> tf,
		const std::string& cn, bool frv, infinispan::hotrod::protocol::Codec& c) :
		transportFactory(tf), codec(c), forceReturnValue(frv), flags(), cacheNameBytes(
				cn.begin(), cn.end()) {
	if (transportFactory) {
		topologyId = transportFactory->createTopologyId(cacheNameBytes);
	}
}

PingOperation* OperationsFactory::newPingOperation(Transport& transport,
		EntryMediaTypes* df) {
	infinispan::hotrod::operations::PingOperation* pingOperation =
			new PingOperation(codec, topologyId, transport, cacheNameBytes, df);
	return pingOperation;
}

GetOperation* OperationsFactory::newGetKeyOperation(
		const std::vector<char>& key, EntryMediaTypes* df) {
	infinispan::hotrod::operations::GetOperation* operation = new GetOperation(
			codec, transportFactory, key, cacheNameBytes, topologyId,
			getFlags(), df);
	return operation;
}

GetAllOperation* OperationsFactory::newGetAllOperation(
		const std::set<std::vector<char>>& keySet, EntryMediaTypes* df) {
	infinispan::hotrod::operations::GetAllOperation* allOperation =
			new GetAllOperation(codec, transportFactory, keySet, cacheNameBytes,
					topologyId, getFlags(), df);
	return allOperation;
}

PutOperation* OperationsFactory::newPutKeyValueOperation(
		const std::vector<char>& key, const std::vector<char>& value,
		uint32_t lifespanSecs, uint32_t maxIdleSecs, EntryMediaTypes* df) {
	infinispan::hotrod::operations::PutOperation* putOperation =
			new PutOperation(codec, transportFactory, key, cacheNameBytes,
					topologyId, getFlags(), value, lifespanSecs, maxIdleSecs, df);
	return putOperation;
}

PutIfAbsentOperation* OperationsFactory::newPutIfAbsentOperation(
		const std::vector<char>& key, const std::vector<char>& value,
		uint32_t lifespanSecs, uint32_t maxIdleSecs, EntryMediaTypes* df) {
	infinispan::hotrod::operations::PutIfAbsentOperation* putIfAbsentOperation =
			new PutIfAbsentOperation(codec, transportFactory, key,
					cacheNameBytes, topologyId, getFlags(), value, lifespanSecs,
					maxIdleSecs, df);
	return putIfAbsentOperation;
}

ReplaceOperation* OperationsFactory::newReplaceOperation(
		const std::vector<char>& key, const std::vector<char>& value,
		uint32_t lifespanSecs, uint32_t maxIdleSecs, EntryMediaTypes* df) {
	infinispan::hotrod::operations::ReplaceOperation* replaceOperation =
			new ReplaceOperation(codec, transportFactory, key, cacheNameBytes,
					topologyId, getFlags(), value, lifespanSecs, maxIdleSecs, df);
	return replaceOperation;
}

RemoveOperation* OperationsFactory::newRemoveOperation(
		const std::vector<char>& key, EntryMediaTypes* df) {
	infinispan::hotrod::operations::RemoveOperation* removeOperation =
			new RemoveOperation(codec, transportFactory, key, cacheNameBytes,
					topologyId, getFlags(), df);
	return removeOperation;
}

ContainsKeyOperation* OperationsFactory::newContainsKeyOperation(
		const std::vector<char>& key, EntryMediaTypes* df) {
	infinispan::hotrod::operations::ContainsKeyOperation* containsKeyOperation =
			new ContainsKeyOperation(codec, transportFactory, key,
					cacheNameBytes, topologyId, getFlags(), df);
	return containsKeyOperation;
}

ReplaceIfUnmodifiedOperation* OperationsFactory::newReplaceIfUnmodifiedOperation(
		const std::vector<char>& key, const std::vector<char>& value,
		uint32_t lifespanSecs, uint32_t maxIdleSecs, int64_t version,
		EntryMediaTypes* df) {
	infinispan::hotrod::operations::ReplaceIfUnmodifiedOperation* replaceIfUnmodifiedOperation =
			new ReplaceIfUnmodifiedOperation(codec, transportFactory, key,
					cacheNameBytes, topologyId, getFlags(), value, lifespanSecs,
					maxIdleSecs, version, df);
	return replaceIfUnmodifiedOperation;
}

RemoveIfUnmodifiedOperation* OperationsFactory::newRemoveIfUnmodifiedOperation(
		const std::vector<char>& key, int64_t version, EntryMediaTypes* df) {
	infinispan::hotrod::operations::RemoveIfUnmodifiedOperation* removeIfUnmodifiedOperation =
			new RemoveIfUnmodifiedOperation(codec, transportFactory, key,
					cacheNameBytes, topologyId, getFlags(), version, df);
	return removeIfUnmodifiedOperation;
}

GetWithMetadataOperation* OperationsFactory::newGetWithMetadataOperation(
		const std::vector<char>& key, EntryMediaTypes* df) {
	infinispan::hotrod::operations::GetWithMetadataOperation* withMetadataOperation =
			new GetWithMetadataOperation(codec, transportFactory, key,
					cacheNameBytes, topologyId, getFlags(), df);
	return withMetadataOperation;
}

GetWithVersionOperation* OperationsFactory::newGetWithVersionOperation(
		const std::vector<char>& key, EntryMediaTypes* df) {
	infinispan::hotrod::operations::GetWithVersionOperation* withVersionOperation =
			new GetWithVersionOperation(codec, transportFactory, key,
					cacheNameBytes, topologyId, getFlags(), df);
	return withVersionOperation;
}

BulkGetOperation* OperationsFactory::newBulkGetOperation(int size,
		EntryMediaTypes* df) {
	infinispan::hotrod::operations::BulkGetOperation* bulkGetOperation =
			new BulkGetOperation(codec, transportFactory, cacheNameBytes,
					topologyId, getFlags(), size, df);
	return bulkGetOperation;
}

BulkGetKeysOperation* OperationsFactory::newBulkGetKeysOperation(int scope,
		EntryMediaTypes* df) {
	infinispan::hotrod::operations::BulkGetKeysOperation* bulkGetKeysOperation =
			new BulkGetKeysOperation(codec, transportFactory, cacheNameBytes,
					topologyId, getFlags(), scope, df);
	return bulkGetKeysOperation;
}

StatsOperation* OperationsFactory::newStatsOperation(EntryMediaTypes* df) {
	infinispan::hotrod::operations::StatsOperation* statsOperation =
			new StatsOperation(codec, transportFactory, cacheNameBytes,
					topologyId, getFlags(), df);
	return statsOperation;
}

ClearOperation* OperationsFactory::newClearOperation(EntryMediaTypes* df) {
	return new ClearOperation(codec, transportFactory, cacheNameBytes,
			topologyId, getFlags(), df);
}

SizeOperation* OperationsFactory::newSizeOperation(EntryMediaTypes* df) {
	infinispan::hotrod::operations::SizeOperation* sizeOperation =
			new SizeOperation(codec, transportFactory, cacheNameBytes,
					topologyId, getFlags(), df);
	return sizeOperation;
}

FaultTolerantPingOperation* OperationsFactory::newFaultTolerantPingOperation(
		EntryMediaTypes* df) {
	infinispan::hotrod::operations::FaultTolerantPingOperation* faultTolerantPingOperation =
			new FaultTolerantPingOperation(codec, transportFactory,
					cacheNameBytes, topologyId, getFlags(), df);
	return faultTolerantPingOperation;
}

ExecuteCmdOperation* OperationsFactory::newExecuteCmdOperation(
		const std::vector<char>& cmdName,
		const std::map<std::vector<char>, std::vector<char>>& values,
		EntryMediaTypes* df) {
	infinispan::hotrod::operations::ExecuteCmdOperation* executeCmdOperation =
			new ExecuteCmdOperation(codec, transportFactory, cacheNameBytes,
					topologyId, getFlags(), cmdName, values, df);
	return executeCmdOperation;
}

QueryOperation* OperationsFactory::newQueryOperation(const QueryRequest& qr,
		EntryMediaTypes* df) {
	infinispan::hotrod::operations::QueryOperation* queryOperation =
			new QueryOperation(codec, transportFactory, cacheNameBytes,
					topologyId, getFlags(), qr, df);
	return queryOperation;
}

uint32_t OperationsFactory::getFlags() {
	uint32_t result = flags;
	if (forceReturnValue) {
		result |= FORCE_RETURN_VALUE;
	}
	flags = 0;
	return result;
}
AddClientListenerOperation* OperationsFactory::newAddClientListenerOperation(
		ClientListener& listener, ClientListenerNotifier& listenerNotifier,
		const std::vector<std::vector<char> > filterFactoryParam,
		const std::vector<std::vector<char> > converterFactoryParams,
		const std::function<void()> &recoveryCallback, EntryMediaTypes* df) {
	infinispan::hotrod::operations::AddClientListenerOperation* addClientListenerOperation =
			new AddClientListenerOperation(codec, transportFactory,
					cacheNameBytes, topologyId, getFlags(), listenerNotifier,
					listener, filterFactoryParam, converterFactoryParams,
					recoveryCallback, df);
	return addClientListenerOperation;
}

RemoveClientListenerOperation* OperationsFactory::newRemoveClientListenerOperation(
		ClientListener& listener, ClientListenerNotifier& listenerNotifier,
		EntryMediaTypes* df) {
	infinispan::hotrod::operations::RemoveClientListenerOperation* removeClientListenerOperation =
			new RemoveClientListenerOperation(codec, transportFactory,
					cacheNameBytes, topologyId, getFlags(), listenerNotifier,
					listener, df);
	return removeClientListenerOperation;
}

PrepareCommitOperation* OperationsFactory::newPrepareCommitOperation(XID xid,
		TransactionContext& tctx, EntryMediaTypes* df) {
	infinispan::hotrod::operations::PrepareCommitOperation* prepareCommitOperation =
			new PrepareCommitOperation(codec, transportFactory, cacheNameBytes,
					topologyId, getFlags(), xid, tctx, df);
	return prepareCommitOperation;
}

CommitOperation* OperationsFactory::newCommitOperation(XID xid,
		TransactionContext& tctx, EntryMediaTypes* df) {
	infinispan::hotrod::operations::CommitOperation* commitOperation =
			new CommitOperation(codec, transportFactory, cacheNameBytes,
					topologyId, getFlags(), xid, tctx, df);
	return commitOperation;
}

RollbackOperation* OperationsFactory::newRollbackOperation(XID xid,
		TransactionContext& tctx, EntryMediaTypes* df) {
	infinispan::hotrod::operations::RollbackOperation* rollbackOperation =
			new RollbackOperation(codec, transportFactory, cacheNameBytes,
					topologyId, getFlags(), xid, tctx, df);
	return rollbackOperation;
}

void OperationsFactory::addFlags(uint32_t f) {
	flags |= f;
}

void OperationsFactory::setFlags(uint32_t f) {
	flags = f;
}

AdminOperation* OperationsFactory::newAdminOperation(
		const std::vector<char>& cmdName,
		const std::map<std::vector<char>, std::vector<char>>& values) {
	infinispan::hotrod::operations::AdminOperation* adminOperation =
			new AdminOperation(codec, transportFactory, cacheNameBytes,
					topologyId, getFlags(), cmdName, values);
	return adminOperation;
}

CacheTopologyInfo infinispan::hotrod::operations::OperationsFactory::getCacheTopologyInfo() {
	return transportFactory->getCacheTopologyInfo(cacheNameBytes);
}

}
}
} // namespace infinispan::hotrod::operations

