#include "infinispan/hotrod/types.h"
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

#include <iostream>

namespace infinispan {
namespace hotrod {

using namespace operations;
using namespace transport;
using namespace protocol;
using namespace sys;

RemoteCacheImpl::RemoteCacheImpl(RemoteCacheManagerImpl& rcm, const std::string& n)
    :remoteCacheManager(rcm), name(n) {}

void RemoteCacheImpl::get(RemoteCacheBase& remoteCacheBase, const void *k, void* b) {
    assertRemoteCacheManagerIsStarted();
	ScopedBuffer kbuf;
    ScopedBuffer& vbuf(*(ScopedBuffer *)b);
    remoteCacheBase.baseKeyMarshall(k, &kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());

    hr_scoped_ptr<GetOperation> gco(operationsFactory->newGetKeyOperation(keyBytes));
    hrbytes bytes = gco->execute();
    bytes.releaseTo(vbuf);
}

void RemoteCacheImpl::put(RemoteCacheBase& remoteCacheBase, const void *k, const void* v, uint64_t life, uint64_t idle, void* b) {
	assertRemoteCacheManagerIsStarted();
	ScopedBuffer kbuf, vbuf;
    ScopedBuffer& obuf(*(ScopedBuffer *)b);
    remoteCacheBase.baseKeyMarshall(k, &kbuf);
    remoteCacheBase.baseValueMarshall(v, &vbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());

    hrbytes valueBytes(vbuf.getBytes(), vbuf.getLength());
    applyDefaultExpirationFlags(life, idle);
    hr_scoped_ptr<PutOperation> op(operationsFactory->newPutKeyValueOperation(keyBytes, valueBytes,life,idle));
    hrbytes bytes = op->execute();
    bytes.releaseTo(obuf);
}

void RemoteCacheImpl::putIfAbsent(RemoteCacheBase& remoteCacheBase, const void *k, const void* v, uint64_t life, uint64_t idle, void* b) {
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf, vbuf;
    ScopedBuffer& obuf(*(ScopedBuffer *)b);
    remoteCacheBase.baseKeyMarshall(k, &kbuf);
    remoteCacheBase.baseValueMarshall(v, &vbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hrbytes valueBytes(vbuf.getBytes(), vbuf.getLength());
    applyDefaultExpirationFlags(life, idle);
    hr_scoped_ptr<PutIfAbsentOperation> op(operationsFactory->newPutIfAbsentOperation(keyBytes, valueBytes,life,idle));
    hrbytes bytes = op->execute();
    bytes.releaseTo(obuf);
}

PingResult RemoteCacheImpl::ping() {
	hr_scoped_ptr<FaultTolerantPingOperation> op(operationsFactory->newFaultTolerantPingOperation());
    return op->execute();
}

void RemoteCacheImpl::replace(RemoteCacheBase& remoteCacheBase, const void *k, const void* v, uint64_t life, uint64_t idle, void* b) {
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf, vbuf;
    ScopedBuffer& obuf(*(ScopedBuffer *)b);
    remoteCacheBase.baseKeyMarshall(k, &kbuf);
    remoteCacheBase.baseValueMarshall(v, &vbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hrbytes valueBytes(vbuf.getBytes(), vbuf.getLength());
    applyDefaultExpirationFlags(life, idle);
    hr_scoped_ptr<ReplaceOperation> op(operationsFactory->newReplaceOperation(keyBytes, valueBytes,life,idle));
    hrbytes bytes = op->execute();
    bytes.releaseTo(obuf);
}

void RemoteCacheImpl::remove(RemoteCacheBase& remoteCacheBase, const void* k, void* b) {
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf;
    ScopedBuffer& vbuf(*(ScopedBuffer *)b);
    remoteCacheBase.baseKeyMarshall(k, &kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());

    hr_scoped_ptr<RemoveOperation> gco(operationsFactory->newRemoveOperation(keyBytes));
    hrbytes bytes = gco->execute();
    bytes.releaseTo(vbuf);
}

void RemoteCacheImpl::containsKey(RemoteCacheBase& remoteCacheBase, const void* k, bool* r) {
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf;
    remoteCacheBase.baseKeyMarshall(k, &kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hr_scoped_ptr<ContainsKeyOperation> gco(
        operationsFactory->newContainsKeyOperation(keyBytes));
    *r = gco->execute();
}

void RemoteCacheImpl::replaceWithVersion(RemoteCacheBase& remoteCacheBase,
    const void* k, const void* v, uint64_t version, uint64_t life, uint64_t idle, bool* res)
{
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf, vbuf;
    remoteCacheBase.baseKeyMarshall(k, &kbuf);
    remoteCacheBase.baseValueMarshall(v, &vbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hrbytes valueBytes(vbuf.getBytes(), vbuf.getLength());

    hr_scoped_ptr<ReplaceIfUnmodifiedOperation> op(operationsFactory->newReplaceIfUnmodifiedOperation(keyBytes, valueBytes,life,idle,version));
    VersionedOperationResponse response = op->execute();
    *res = response.isUpdated();
}

void RemoteCacheImpl::removeWithVersion(RemoteCacheBase& remoteCacheBase, const void* k, uint64_t version, bool* res) {
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf;
    remoteCacheBase.baseKeyMarshall(k, &kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());

    hr_scoped_ptr<RemoveIfUnmodifiedOperation> gco(operationsFactory->newRemoveIfUnmodifiedOperation(keyBytes, version));
    VersionedOperationResponse response = gco->execute();
    *res = response.isUpdated();
}

void RemoteCacheImpl::getWithVersion(RemoteCacheBase& remoteCacheBase, const void *k, void* b, VersionedValue* version)
{
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf;
    ScopedBuffer& vbuf(*(ScopedBuffer *)b);
    remoteCacheBase.baseKeyMarshall(k, &kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hr_scoped_ptr<GetWithVersionOperation> gco(operationsFactory->newGetWithVersionOperation(keyBytes));
    VersionedValueImpl<hrbytes> m = gco->execute();
    m.getValue().releaseTo(vbuf);
    version->version = m.version;
}

void RemoteCacheImpl::getWithMetadata(RemoteCacheBase& remoteCacheBase, const void *k, void* b, MetadataValue* metadata)
{
    assertRemoteCacheManagerIsStarted();
    ScopedBuffer kbuf;
    ScopedBuffer& vbuf(*(ScopedBuffer *)b);
    remoteCacheBase.baseKeyMarshall(k, &kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hr_scoped_ptr<GetWithMetadataOperation> gco(operationsFactory->newGetWithMetadataOperation(keyBytes));
    MetadataValueImpl<hrbytes> m = gco->execute();
    m.getValue().releaseTo(vbuf);
    metadata->version = m.version;
    metadata->created = m.created;
    metadata->lifespan = m.lifespan;
    metadata->lastUsed = m.lastUsed;
    metadata->maxIdle = m.maxIdle;
}

void RemoteCacheImpl::getBulk(RemoteCacheBase& remoteCacheBase, int size, std::map<void*, void*>* mbuf) {
    assertRemoteCacheManagerIsStarted();
    hr_scoped_ptr<BulkGetOperation> gco(operationsFactory->newBulkGetOperation(size));
    std::map<hrbytes,hrbytes> res = gco->execute();
    for(std::map<hrbytes,hrbytes>::iterator i = res.begin(); i != res.end(); i++) {
    	ScopedBuffer kbuf, vbuf;
        i->first.releaseTo(kbuf);
        void* k = remoteCacheBase.baseKeyUnmarshall(&kbuf);
        i->second.releaseTo(vbuf);
        void* v = remoteCacheBase.baseValueUnmarshall(&vbuf);
        mbuf->insert(std::make_pair(k,v));
    }
}

void RemoteCacheImpl::keySet(RemoteCacheBase& remoteCacheBase, int scope, std::set<void*>* result) {
    assertRemoteCacheManagerIsStarted();
    hr_scoped_ptr<BulkGetKeysOperation> gco(operationsFactory->newBulkGetKeysOperation(scope));
    std::set<hrbytes> res = gco->execute();
    for(std::set<hrbytes>::const_iterator i = res.begin(); i != res.end(); i++) {
    	ScopedBuffer kbuf;
    	i->releaseTo(kbuf);
        void* k = remoteCacheBase.baseKeyUnmarshall(&kbuf);
        result->insert(k);
    }
}

void RemoteCacheImpl::stats(std::map<std::string,std::string>* statistics) {
    assertRemoteCacheManagerIsStarted();
    hr_scoped_ptr<StatsOperation> gco(operationsFactory->newStatsOperation());
    *statistics = gco->execute();
}

void RemoteCacheImpl::clear() {
    assertRemoteCacheManagerIsStarted();
    hr_scoped_ptr<ClearOperation> gco(operationsFactory->newClearOperation());
    gco->execute();
}

const std::string& RemoteCacheImpl::getName() const {
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

}} /* namespace */
