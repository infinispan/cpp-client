#include "hotrod/sys/Msg.h"
#include "hotrod/impl/RemoteCacheImpl.h"
#include "hotrod/impl/RemoteCacheManagerImpl.h"
#include "hotrod/impl/operations/OperationsFactory.h"
#include "hotrod/impl/operations/GetOperation.h"
#include "hotrod/impl/operations/GetAllOperation.h"
#include "hotrod/impl/operations/PutOperation.h"
#include "hotrod/impl/operations/PingOperation.h"
#include "hotrod/impl/operations/PutIfAbsentOperation.h"
#include "hotrod/impl/operations/ReplaceOperation.h"
#include "hotrod/impl/operations/RemoveOperation.h"
#include "hotrod/impl/operations/ContainsKeyOperation.h"
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
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/VersionedOperationResponse.h"
#include "hotrod/impl/MetadataValueImpl.h"
#include "hotrod/impl/operations/ExecuteCmdOperation.h"
#include "hotrod/impl/operations/QueryOperation.h"
#include <hotrod/impl/operations/AddClientListenerOperation.h>
#include <hotrod/impl/operations/RemoveClientListenerOperation.h>
#include <hotrod/impl/operations/TransactionOperations.h>
#include <iostream>

namespace infinispan {
namespace hotrod {

using namespace operations;
using namespace transport;
using namespace protocol;
using namespace sys;

RemoteCacheImpl::RemoteCacheImpl(RemoteCacheManagerImpl& rcm, const std::string& n)
    :remoteCacheManager(rcm), name(n) {}

void *RemoteCacheImpl::get(RemoteCacheBase& remoteCacheBase, const void *k) {
    assertRemoteCacheManagerIsStarted();
	std::vector<char> kbuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    std::vector<char> keyBytes(kbuf.data(), kbuf.data()+kbuf.size());

    std::unique_ptr<GetOperation> gco(operationsFactory->newGetKeyOperation(keyBytes));
    std::vector<char> bytes = gco->execute();
    return bytes.data() ? remoteCacheBase.baseValueUnmarshall(bytes) : NULL;
}

std::map<std::vector<char>,std::vector<char>> RemoteCacheImpl::getAll(const std::set<std::vector<char>>& keySet) {
    assertRemoteCacheManagerIsStarted();
    // split key set according to server address
    std::map<const InetSocketAddress, std::set<std::vector<char> > > splittedKeySet;
    for ( auto key : keySet)
    {
       std::vector<char> cacheNameBytes(name.begin(), name.end());
       const InetSocketAddress& addr = operationsFactory->getTransportFactory()->getTopologyInfo()->getHashAwareServer(key, cacheNameBytes);
       splittedKeySet[addr].insert(key);
    }
    std::map<std::vector<char>,std::vector<char>> result;
    // Execute separated GetAllOperations and merge the result
    for (auto item: splittedKeySet)
    {
        std::unique_ptr<GetAllOperation> gco(operationsFactory->newGetAllOperation(item.second));
        std::map<std::vector<char>,std::vector<char>> splittedResult = gco->execute();
        result.insert(splittedResult.begin(), splittedResult.end());
    }
    return result;
}

std::vector<char> RemoteCacheImpl::putraw(const std::vector<char> &k, const std::vector<char> &v, uint64_t life, uint64_t idle) {
    assertRemoteCacheManagerIsStarted();
    applyDefaultExpirationFlags(life, idle);
    std::unique_ptr<PutOperation> op(operationsFactory->newPutKeyValueOperation(k, v,life,idle));
    return op->execute();
}

void *RemoteCacheImpl::put(RemoteCacheBase& remoteCacheBase, const void *k, const void* v, uint64_t life, uint64_t idle) {
	assertRemoteCacheManagerIsStarted();
	std::vector<char> kbuf, vbuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    remoteCacheBase.baseValueMarshall(v, vbuf);
    std::vector<char> keyBytes(kbuf.data(), kbuf.data()+kbuf.size());
    std::vector<char> valueBytes(vbuf.data(), vbuf.data()+vbuf.size());
    applyDefaultExpirationFlags(life, idle);
    std::unique_ptr<PutOperation> op(operationsFactory->newPutKeyValueOperation(keyBytes, valueBytes,life,idle));
    std::vector<char> bytes = op->execute();
    return bytes.data() ? remoteCacheBase.baseValueUnmarshall(bytes) : NULL;
}

void *RemoteCacheImpl::putIfAbsent(RemoteCacheBase& remoteCacheBase, const void *k, const void* v, uint64_t life, uint64_t idle) {
    assertRemoteCacheManagerIsStarted();
    std::vector<char> kbuf, vbuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    remoteCacheBase.baseValueMarshall(v, vbuf);
    std::vector<char> keyBytes(kbuf.data(), kbuf.data()+kbuf.size());
    std::vector<char> valueBytes(vbuf.data(), vbuf.data()+vbuf.size());
    applyDefaultExpirationFlags(life, idle);
    std::unique_ptr<PutIfAbsentOperation> op(operationsFactory->newPutIfAbsentOperation(keyBytes, valueBytes,life,idle));
    std::vector<char> bytes = op->execute();
    return bytes.data() ? remoteCacheBase.baseValueUnmarshall(bytes) : NULL;
}

PingResult RemoteCacheImpl::ping() {
	std::unique_ptr<FaultTolerantPingOperation> op(operationsFactory->newFaultTolerantPingOperation());
    return op->execute();
}

void *RemoteCacheImpl::replace(RemoteCacheBase& remoteCacheBase, const void *k, const void* v, uint64_t life, uint64_t idle) {
    assertRemoteCacheManagerIsStarted();
    std::vector<char> kbuf, vbuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    remoteCacheBase.baseValueMarshall(v, vbuf);
    std::vector<char> keyBytes(kbuf.data(), kbuf.data()+kbuf.size());
    std::vector<char> valueBytes(vbuf.data(), vbuf.data()+vbuf.size());
    applyDefaultExpirationFlags(life, idle);
    std::unique_ptr<ReplaceOperation> op(operationsFactory->newReplaceOperation(keyBytes, valueBytes,life,idle));
    std::vector<char> bytes = op->execute();
    return bytes.data() ? remoteCacheBase.baseValueUnmarshall(bytes) : NULL;
}

void *RemoteCacheImpl::remove(RemoteCacheBase& remoteCacheBase, const void* k) {
    assertRemoteCacheManagerIsStarted();
    std::vector<char> kbuf, obuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    std::vector<char> keyBytes(kbuf.data(), kbuf.data()+kbuf.size());

    std::unique_ptr<RemoveOperation> gco(operationsFactory->newRemoveOperation(keyBytes));
    std::vector<char> bytes = gco->execute();
    return bytes.data() ? remoteCacheBase.baseValueUnmarshall(bytes) : NULL;
}

bool RemoteCacheImpl::containsKey(RemoteCacheBase& remoteCacheBase, const void* k) {
    assertRemoteCacheManagerIsStarted();
    std::vector<char> kbuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    std::vector<char> keyBytes(kbuf.data(), kbuf.data()+kbuf.size());
    std::unique_ptr<ContainsKeyOperation> gco(
        operationsFactory->newContainsKeyOperation(keyBytes));
    return gco->execute();
}

bool RemoteCacheImpl::replaceWithVersion(RemoteCacheBase& remoteCacheBase,
    const void* k, const void* v, uint64_t version, uint64_t life, uint64_t idle)
{
    assertRemoteCacheManagerIsStarted();
    std::vector<char> kbuf, vbuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    remoteCacheBase.baseValueMarshall(v, vbuf);
    std::vector<char> keyBytes(kbuf.data(), kbuf.data()+kbuf.size());
    std::vector<char> valueBytes(vbuf.data(), vbuf.data()+vbuf.size());

    std::unique_ptr<ReplaceIfUnmodifiedOperation> op(operationsFactory->newReplaceIfUnmodifiedOperation(keyBytes, valueBytes,life,idle,version));
    VersionedOperationResponse response = op->execute();
    return response.isUpdated();
}

bool RemoteCacheImpl::removeWithVersion(RemoteCacheBase& remoteCacheBase, const void* k, uint64_t version) {
    assertRemoteCacheManagerIsStarted();
    std::vector<char> kbuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    std::vector<char> keyBytes(kbuf.data(), kbuf.data()+kbuf.size());

    std::unique_ptr<RemoveIfUnmodifiedOperation> gco(operationsFactory->newRemoveIfUnmodifiedOperation(keyBytes, version));
    VersionedOperationResponse response = gco->execute();
    return response.isUpdated();
}

void *RemoteCacheImpl::getWithVersion(RemoteCacheBase& remoteCacheBase, const void *k, VersionedValue* version)
{
    assertRemoteCacheManagerIsStarted();
    std::vector<char> kbuf, obuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    std::vector<char> keyBytes(kbuf.data(), kbuf.data()+kbuf.size());
    std::unique_ptr<GetWithVersionOperation> gco(operationsFactory->newGetWithVersionOperation(keyBytes));
    VersionedValueImpl<std::vector<char>> m = gco->execute();
    obuf=m.getValue();
    version->version = m.version;
    return obuf.data() ? remoteCacheBase.baseValueUnmarshall(obuf) : NULL;
}

void *RemoteCacheImpl::getWithMetadata(RemoteCacheBase& remoteCacheBase, const void *k, MetadataValue* metadata)
{
    assertRemoteCacheManagerIsStarted();
    std::vector<char> kbuf, obuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    std::vector<char> keyBytes(kbuf.data(), kbuf.data()+kbuf.size());
    std::unique_ptr<GetWithMetadataOperation> gco(operationsFactory->newGetWithMetadataOperation(keyBytes));
    MetadataValueImpl<std::vector<char>> m = gco->execute();
    obuf=m.getValue();
    metadata->version = m.version;
    metadata->created = m.created;
    metadata->lifespan = m.lifespan;
    metadata->lastUsed = m.lastUsed;
    metadata->maxIdle = m.maxIdle;
    return obuf.data() ? remoteCacheBase.baseValueUnmarshall(obuf) : NULL;
}

void RemoteCacheImpl::getBulk(RemoteCacheBase& remoteCacheBase, std::map<void*, void*> &map) {
    getBulk(remoteCacheBase, 0, map);
}

void RemoteCacheImpl::getBulk(RemoteCacheBase& remoteCacheBase, int isize, std::map<void*, void*> &map) {
    assertRemoteCacheManagerIsStarted();
    std::unique_ptr<BulkGetOperation> gco(operationsFactory->newBulkGetOperation(isize));
    std::map<std::vector<char>,std::vector<char>> res = gco->execute();
    for (auto it = res.begin(); it != res.end(); ++it)
    {
        map[KeyUnmarshallerFtor(remoteCacheBase)(it->first)]=ValueUnmarshallerFtor(remoteCacheBase)(it->second);
    }
}

void RemoteCacheImpl::keySet(RemoteCacheBase& remoteCacheBase, int scope, std::vector<void*> &result) {
    assertRemoteCacheManagerIsStarted();
    std::unique_ptr<BulkGetKeysOperation> gco(operationsFactory->newBulkGetKeysOperation(scope));
    std::set<std::vector<char>> res = gco->execute();
    for(auto it=res.begin(); it!=res.end(); it++)
    {
        result.push_back(KeyUnmarshallerFtor(remoteCacheBase)(*it));
    }
}

void RemoteCacheImpl::stats(std::map<std::string, std::string> &statistics) {
    assertRemoteCacheManagerIsStarted();
    std::unique_ptr<StatsOperation> gco(operationsFactory->newStatsOperation());
    statistics = gco->execute();
}

void RemoteCacheImpl::clear() {
    assertRemoteCacheManagerIsStarted();
    std::unique_ptr<ClearOperation> gco(operationsFactory->newClearOperation());
    gco->execute();
}

uint64_t RemoteCacheImpl::size() {
    assertRemoteCacheManagerIsStarted();
    std::unique_ptr<SizeOperation> szo(operationsFactory->newSizeOperation());
    return szo->execute();
}

const char *RemoteCacheImpl::getName() const {
    return name.c_str();
}

const std::string& RemoteCacheImpl::getNameAsString() const {
    return name;
}


void RemoteCacheImpl::init(OperationsFactory* of) {
	operationsFactory.reset(of);
}

void RemoteCacheImpl::withFlags(Flag flags) {
	operationsFactory->setFlags(flags);
}

void RemoteCacheImpl::applyDefaultExpirationFlags(uint64_t lifespan, uint64_t maxIdle) {
   if (lifespan == 0) {
      operationsFactory->addFlags(DEFAULT_LIFESPAN);
   }
   if (maxIdle == 0) {
      operationsFactory->addFlags(DEFAULT_MAXIDLE);
   }
}

void RemoteCacheImpl::assertRemoteCacheManagerIsStarted() {
   if (!remoteCacheManager.isStarted()) {
	   // TODO: log
       throw RemoteCacheManagerNotStartedException(
           Msg() << "Cannot perform operations on a cache associated with an unstarted RemoteCacheManager. "
                 << "Use RemoteCacheManager.start before using the remote cache.");
   }
}
std::vector<char> RemoteCacheImpl::execute(std::vector<char> cmdNameBytes, const std::map<std::vector<char>,std::vector<char>>& args) {
	assertRemoteCacheManagerIsStarted();

    std::unique_ptr<ExecuteCmdOperation> op(operationsFactory->newExecuteCmdOperation(cmdNameBytes, args));
    return op->execute();

}

QueryResponse RemoteCacheImpl::query(const QueryRequest &qr) {
	std::unique_ptr<QueryOperation> op(operationsFactory->newQueryOperation(qr));
	return op->execute();
}

CacheTopologyInfo RemoteCacheImpl::getCacheTopologyInfo() {
	return operationsFactory->getCacheTopologyInfo();
}

void RemoteCacheImpl::addClientListener(ClientListener& clientListener, const std::vector<std::vector<char> > filterFactoryParam, const std::vector<std::vector<char> > converterFactoryParams, const std::function<void()> &recoveryCallback)
{
	// Special behaviour for this operation. op will be keep by the dispatcher and reused if needed
	// so op is not to be destroyed here.
	// TODO: Maybe a good move semantic implementation for Add operation can uniform the code
    auto op = std::shared_ptr<AddClientListenerOperation>(operationsFactory->newAddClientListenerOperation(clientListener, remoteCacheManager.getListenerNotifier(), filterFactoryParam, converterFactoryParams, recoveryCallback));
    op->execute();
}

void RemoteCacheImpl::removeClientListener(ClientListener& clientListener) {
    RemoveClientListenerOperation *rclo = operationsFactory->newRemoveClientListenerOperation(clientListener, remoteCacheManager.getListenerNotifier());
    std::unique_ptr<RemoveClientListenerOperation> op(rclo);
    op->execute();
}

uint32_t RemoteCacheImpl::prepareCommit(XID xid, TransactionContext& tctx) {
    auto pco =  std::unique_ptr<PrepareCommitOperation>(operationsFactory->newPrepareCommitOperation(xid, tctx));
    return pco->execute();
}

uint32_t RemoteCacheImpl::commit(XID xid, TransactionContext& tctx) {
    auto co =  std::unique_ptr<CommitOperation>(operationsFactory->newCommitOperation(xid, tctx));
    return co->execute();
}

uint32_t RemoteCacheImpl::rollback(XID xid, TransactionContext& tctx) {
    auto co =  std::unique_ptr<RollbackOperation>(operationsFactory->newRollbackOperation(xid, tctx));
    return co->execute();
}

}
} /* namespace */

