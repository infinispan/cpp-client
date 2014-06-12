

#include "infinispan/hotrod/RemoteCacheBase.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "hotrod/impl/RemoteCacheImpl.h"

#include <iostream>

namespace infinispan {
namespace hotrod {

#define IMPL ((RemoteCacheImpl *) impl.get())

void RemoteCacheBase::setMarshallers(void* rc, MarshallHelperFn kf, MarshallHelperFn vf, UnmarshallHelperFn ukf, UnmarshallHelperFn uvf) {
    remoteCachePtr = rc;
    baseKeyMarshallFn = kf;
    baseValueMarshallFn = vf;
    baseKeyUnmarshallFn = ukf;
    baseValueUnmarshallFn = uvf;
}
    
void RemoteCacheBase::baseKeyMarshall(const void* k, ScopedBuffer &buf) {
    baseKeyMarshallFn(remoteCachePtr, k, buf);
}

void RemoteCacheBase::baseValueMarshall(const void* v, ScopedBuffer &buf) {
    baseValueMarshallFn(remoteCachePtr, v, buf);
}

void* RemoteCacheBase::baseKeyUnmarshall(const ScopedBuffer &buf) {
    return baseKeyUnmarshallFn(remoteCachePtr, buf);
}

void* RemoteCacheBase::baseValueUnmarshall(const ScopedBuffer &buf) {
    return baseValueUnmarshallFn(remoteCachePtr, buf);
}

const char *RemoteCacheBase::base_getName() {
    return IMPL->getName();
}

void *RemoteCacheBase::base_get(const void *key) {
    return IMPL->get(*this, key);
}

void *RemoteCacheBase::base_put(const void *key, const void *val, int64_t life, int64_t idle) {
    return IMPL->put(*this, key, val, life, idle);
}

void *RemoteCacheBase::base_putIfAbsent(const void *key, const void *val, int64_t life, int64_t idle) {
    return IMPL->putIfAbsent(*this, key, val, life, idle);
}

void *RemoteCacheBase::base_replace(const void *key, const void *val, int64_t life, int64_t idle) {
    return IMPL->replace(*this, key, val, life, idle);
}

void *RemoteCacheBase::base_remove(const void *key) {
    return IMPL->remove(*this, key);
}

bool RemoteCacheBase::base_containsKey(const void *key){
    return IMPL->containsKey(*this, key);
}

bool RemoteCacheBase::base_replaceWithVersion(const void *key, const void *value, int64_t version, int64_t life, int64_t idle)
{
    return IMPL->replaceWithVersion(*this, key, value, version, life, idle);
}

bool RemoteCacheBase::base_removeWithVersion(const void *key, int64_t version)
{
    return IMPL->removeWithVersion(*this, key, version);
}

void *RemoteCacheBase::base_getWithVersion(const void *key, VersionedValue* version)
{
    return IMPL->getWithVersion(*this, key, version);
}

void *RemoteCacheBase::base_getWithMetadata(const void *key, MetadataValue* metadata)
{
    return IMPL->getWithMetadata(*this, key, metadata);
}

void RemoteCacheBase::base_getBulk(int size, portable::map<void*, void*> &mbuf)
{
    IMPL->getBulk(*this, size, mbuf);
}


void RemoteCacheBase::base_keySet(int scope, portable::vector<void*> &result)
{
    IMPL->keySet(*this, scope, result);
}

void RemoteCacheBase::base_stats(portable::map<portable::string,portable::string> &stats)
{
    IMPL->stats(stats);
}

void RemoteCacheBase::base_clear()
{
    IMPL->clear();
}

void RemoteCacheBase::base_ping() {
    IMPL->ping();
}

void RemoteCacheBase::base_withFlags(Flag flags) {
    IMPL->withFlags(flags);
}

}} /* namespace */
