#include "hotrod/sys/Msg.h"
#include "hotrod/impl/RemoteCacheImpl.h"
#include "hotrod/impl/RemoteCacheManagerImpl.h"
#include "hotrod/impl/operations/OperationsFactory.h"
#include "hotrod/impl/operations/GetOperation.h"
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

void RemoteCacheImpl::getBulk(RemoteCacheBase& remoteCacheBase, portable::map<void*, void*> &map) {
    getBulk(remoteCacheBase, 0, map);
}

void RemoteCacheImpl::getBulk(RemoteCacheBase& remoteCacheBase, int isize, portable::map<void*, void*> &map) {
    assertRemoteCacheManagerIsStarted();
    std::unique_ptr<BulkGetOperation> gco(operationsFactory->newBulkGetOperation(isize));
    std::map<std::vector<char>,std::vector<char>> res = gco->execute();
    portable::map<void *, void *> tmpMap(res, KeyUnmarshallerFtor(remoteCacheBase), ValueUnmarshallerFtor(remoteCacheBase));
    map = tmpMap.move();
}

void RemoteCacheImpl::keySet(RemoteCacheBase& remoteCacheBase, int scope, portable::vector<void*> &result) {
    assertRemoteCacheManagerIsStarted();
    std::unique_ptr<BulkGetKeysOperation> gco(operationsFactory->newBulkGetKeysOperation(scope));
    std::set<std::vector<char>> res = gco->execute();
    portable::vector<void *> tmpVector(res, KeyUnmarshallerFtor(remoteCacheBase));
    result = tmpVector.move();
}

void RemoteCacheImpl::stats(portable::map<portable::string,portable::string> &statistics) {
    assertRemoteCacheManagerIsStarted();
    std::unique_ptr<StatsOperation> gco(operationsFactory->newStatsOperation());
    portable::map<portable::string,portable::string> tmpMap(gco->execute(), portable::string::convert(), portable::string::convert());
    statistics = tmpMap.move();
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
std::vector<char> RemoteCacheImpl::execute(RemoteCacheBase& /*remoteCacheBase*/, std::vector<char> cmdNameBytes, const portable::map<std::vector<char>,std::vector<char>>& args) {
	assertRemoteCacheManagerIsStarted();

    std::unique_ptr<ExecuteCmdOperation> op(operationsFactory->newExecuteCmdOperation(cmdNameBytes, args));
    return op->execute();

}

CacheTopologyInfo RemoteCacheImpl::getCacheTopologyInfo() {
	return operationsFactory->getCacheTopologyInfo();
}


}
} /* namespace */

