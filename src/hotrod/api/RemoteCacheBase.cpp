

#include "infinispan/hotrod/RemoteCacheBase.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "hotrod/impl/RemoteCacheImpl.h"

#include <iostream>

namespace infinispan {
namespace hotrod {

RemoteCacheBase::RemoteCacheBase() : Handle<RemoteCacheImpl>(NULL) {}

void RemoteCacheBase::setMarshallers(void* rc, MarshallHelperFn kf, MarshallHelperFn vf, UnmarshallHelperFn ukf, UnmarshallHelperFn uvf) {
    remoteCachePtr = rc;
    baseKeyMarshallFn = kf;
    baseValueMarshallFn = vf;
    baseKeyUnmarshallFn = ukf;
    baseValueUnmarshallFn = uvf;
}
    
void RemoteCacheBase::baseKeyMarshall(const void* k, void *buf) {
    baseKeyMarshallFn(remoteCachePtr, k, buf);
}

void RemoteCacheBase::baseValueMarshall(const void* v, void *buf) {
    baseValueMarshallFn(remoteCachePtr, v, buf);
}

void* RemoteCacheBase::baseKeyUnmarshall(const void* buf) {
    return baseKeyUnmarshallFn(remoteCachePtr, buf);
}

void* RemoteCacheBase::baseValueUnmarshall(const void* buf) {
    return baseValueUnmarshallFn(remoteCachePtr, buf);
}

void RemoteCacheBase::init(operations::OperationsFactory* operationFactory) {
    impl->init(operationFactory);
}

void RemoteCacheBase::base_get(const void *key, void *buf) {
    impl->get(*this, key, buf);
}

void RemoteCacheBase::base_put(const void *key, const void *val, int64_t life, int64_t idle, void *buf) {
    impl->put(*this, key, val, life, idle, buf);
}

void RemoteCacheBase::base_putIfAbsent(const void *key, const void *val, int64_t life, int64_t idle, void *buf) {
    impl->putIfAbsent(*this, key, val, life, idle, buf);
}

void RemoteCacheBase::base_replace(const void *key, const void *val, int64_t life, int64_t idle, void *buf) {
    impl->replace(*this, key, val, life, idle, buf);
}

void RemoteCacheBase::base_remove(const void *key, void *rbuf) {
    impl->remove(*this, key, rbuf);
}

void RemoteCacheBase::base_containsKey(const void *key, bool *res){
    impl->containsKey(*this, key, res);
}

void RemoteCacheBase::base_replaceWithVersion(const void *key, const void *value, int64_t version, int64_t life, int64_t idle, bool *res)
{
    impl->replaceWithVersion(*this, key, value, version, life, idle, res);
}

void RemoteCacheBase::base_removeWithVersion(const void *key, int64_t version, bool *res)
{
    impl->removeWithVersion(*this, key, version, res);
}

void RemoteCacheBase::base_getWithMetadata(
    const void *key, void* vbuf, MetadataValue* metadata)
{
	impl->getWithMetadata(*this, key, vbuf, metadata);
}

void RemoteCacheBase::base_getBulk(int size, std::map<void*, void*>* mbuf)
{
	impl->getBulk(*this, size, mbuf);
}


void RemoteCacheBase::base_keySet(int scope, std::set<void*>* result)
{
	impl->keySet(*this, scope, result);
}

void RemoteCacheBase::base_stats(std::map<std::string,std::string>* stats)
{
	impl->stats(stats);
}

void RemoteCacheBase::base_clear()
{
	impl->clear();
}

void RemoteCacheBase::base_ping() {
    impl->ping();
}

void RemoteCacheBase::base_withFlags(Flag flags) {
    impl->withFlags(flags);
}

}} /* namespace */
