#include "hotrod/types.h"
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
	ScopedBuffer kbuf, obuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());

    hr_scoped_ptr<GetOperation> gco(operationsFactory->newGetKeyOperation(keyBytes));
    hrbytes bytes = gco->execute();
    bytes.releaseTo(obuf);
    return obuf.getBytes() ? remoteCacheBase.baseValueUnmarshall(obuf) : NULL;
}

void *RemoteCacheImpl::put(RemoteCacheBase& remoteCacheBase, const void *k, const void* v, uint64_t life, uint64_t idle) {
	assertRemoteCacheManagerIsStarted();
	ScopedBuffer kbuf, vbuf, obuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    remoteCacheBase.baseValueMarshall(v, vbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());

    hrbytes valueBytes(vbuf.getBytes(), vbuf.getLength());
    applyDefaultExpirationFlags(life, idle);
    hr_scoped_ptr<PutOperation> op(operationsFactory->newPutKeyValueOperation(keyBytes, valueBytes,life,idle));
    hrbytes bytes = op->execute();
    bytes.releaseTo(obuf);
    return obuf.getBytes() ? remoteCacheBase.baseValueUnmarshall(obuf) : NULL;
}

void *RemoteCacheImpl::putIfAbsent(RemoteCacheBase& remoteCacheBase, const void *k, const void* v, uint64_t life, uint64_t idle) {
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf, vbuf, obuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    remoteCacheBase.baseValueMarshall(v, vbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hrbytes valueBytes(vbuf.getBytes(), vbuf.getLength());
    applyDefaultExpirationFlags(life, idle);
    hr_scoped_ptr<PutIfAbsentOperation> op(operationsFactory->newPutIfAbsentOperation(keyBytes, valueBytes,life,idle));
    hrbytes bytes = op->execute();
    bytes.releaseTo(obuf);
    return obuf.getBytes() ? remoteCacheBase.baseValueUnmarshall(obuf) : NULL;
}

PingResult RemoteCacheImpl::ping() {
	hr_scoped_ptr<FaultTolerantPingOperation> op(operationsFactory->newFaultTolerantPingOperation());
    return op->execute();
}

void *RemoteCacheImpl::replace(RemoteCacheBase& remoteCacheBase, const void *k, const void* v, uint64_t life, uint64_t idle) {
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf, vbuf, obuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    remoteCacheBase.baseValueMarshall(v, vbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hrbytes valueBytes(vbuf.getBytes(), vbuf.getLength());
    applyDefaultExpirationFlags(life, idle);
    hr_scoped_ptr<ReplaceOperation> op(operationsFactory->newReplaceOperation(keyBytes, valueBytes,life,idle));
    hrbytes bytes = op->execute();
    bytes.releaseTo(obuf);
    return obuf.getBytes() ? remoteCacheBase.baseValueUnmarshall(obuf) : NULL;
}

void *RemoteCacheImpl::remove(RemoteCacheBase& remoteCacheBase, const void* k) {
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf, obuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());

    hr_scoped_ptr<RemoveOperation> gco(operationsFactory->newRemoveOperation(keyBytes));
    hrbytes bytes = gco->execute();
    bytes.releaseTo(obuf);
    return obuf.getBytes() ? remoteCacheBase.baseValueUnmarshall(obuf) : NULL;
}

bool RemoteCacheImpl::containsKey(RemoteCacheBase& remoteCacheBase, const void* k) {
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hr_scoped_ptr<ContainsKeyOperation> gco(
        operationsFactory->newContainsKeyOperation(keyBytes));
    return gco->execute();
}

bool RemoteCacheImpl::replaceWithVersion(RemoteCacheBase& remoteCacheBase,
    const void* k, const void* v, uint64_t version, uint64_t life, uint64_t idle)
{
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf, vbuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    remoteCacheBase.baseValueMarshall(v, vbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hrbytes valueBytes(vbuf.getBytes(), vbuf.getLength());

    hr_scoped_ptr<ReplaceIfUnmodifiedOperation> op(operationsFactory->newReplaceIfUnmodifiedOperation(keyBytes, valueBytes,life,idle,version));
    VersionedOperationResponse response = op->execute();
    return response.isUpdated();
}

bool RemoteCacheImpl::removeWithVersion(RemoteCacheBase& remoteCacheBase, const void* k, uint64_t version) {
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());

    hr_scoped_ptr<RemoveIfUnmodifiedOperation> gco(operationsFactory->newRemoveIfUnmodifiedOperation(keyBytes, version));
    VersionedOperationResponse response = gco->execute();
    return response.isUpdated();
}

void *RemoteCacheImpl::getWithVersion(RemoteCacheBase& remoteCacheBase, const void *k, VersionedValue* version)
{
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf, obuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hr_scoped_ptr<GetWithVersionOperation> gco(operationsFactory->newGetWithVersionOperation(keyBytes));
    VersionedValueImpl<hrbytes> m = gco->execute();
    m.getValue().releaseTo(obuf);
    version->version = m.version;
    return obuf.getBytes() ? remoteCacheBase.baseValueUnmarshall(obuf) : NULL;
}

void *RemoteCacheImpl::getWithMetadata(RemoteCacheBase& remoteCacheBase, const void *k, MetadataValue* metadata)
{
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf, obuf;
    remoteCacheBase.baseKeyMarshall(k, kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hr_scoped_ptr<GetWithMetadataOperation> gco(operationsFactory->newGetWithMetadataOperation(keyBytes));
    MetadataValueImpl<hrbytes> m = gco->execute();
    m.getValue().releaseTo(obuf);
    metadata->version = m.version;
    metadata->created = m.created;
    metadata->lifespan = m.lifespan;
    metadata->lastUsed = m.lastUsed;
    metadata->maxIdle = m.maxIdle;
    return obuf.getBytes() ? remoteCacheBase.baseValueUnmarshall(obuf) : NULL;
}

void RemoteCacheImpl::getBulk(RemoteCacheBase& remoteCacheBase, portable::map<void*, void*> &map) {
    getBulk(remoteCacheBase, 0, map);
}

void RemoteCacheImpl::getBulk(RemoteCacheBase& remoteCacheBase, int size, portable::map<void*, void*> &map) {
    assertRemoteCacheManagerIsStarted();
    hr_scoped_ptr<BulkGetOperation> gco(operationsFactory->newBulkGetOperation(size));
    std::map<hrbytes,hrbytes> res = gco->execute();
    portable::map<void *, void *> tmpMap(res, KeyUnmarshallerFtor(remoteCacheBase), ValueUnmarshallerFtor(remoteCacheBase));
    map = tmpMap.move();
}

void RemoteCacheImpl::keySet(RemoteCacheBase& remoteCacheBase, int scope, portable::vector<void*> &result) {
    assertRemoteCacheManagerIsStarted();
    hr_scoped_ptr<BulkGetKeysOperation> gco(operationsFactory->newBulkGetKeysOperation(scope));
    std::set<hrbytes> res = gco->execute();
    portable::vector<void *> tmpVector(res, KeyUnmarshallerFtor(remoteCacheBase));
    result = tmpVector.move();
}

void RemoteCacheImpl::stats(portable::map<portable::string,portable::string> &statistics) {
    assertRemoteCacheManagerIsStarted();
    hr_scoped_ptr<StatsOperation> gco(operationsFactory->newStatsOperation());
    portable::map<portable::string,portable::string> tmpMap(gco->execute(), portable::string::convert(), portable::string::convert());
    statistics = tmpMap.move();
}

void RemoteCacheImpl::clear() {
    assertRemoteCacheManagerIsStarted();
    hr_scoped_ptr<ClearOperation> gco(operationsFactory->newClearOperation());
    gco->execute();
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
hrbytes RemoteCacheImpl::execute(RemoteCacheBase& /*remoteCacheBase*/, hrbytes cmdNameBytes, const portable::map<hrbytes,hrbytes>& args) {
	assertRemoteCacheManagerIsStarted();

    hr_scoped_ptr<ExecuteCmdOperation> op(operationsFactory->newExecuteCmdOperation(cmdNameBytes, args));
    return op->execute();

}



}} /* namespace */
