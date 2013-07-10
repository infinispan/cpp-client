#include "infinispan/hotrod/RemoteCacheBase.h"
#include "hotrod/impl/RemoteCacheImpl.h"
#include <iostream>

namespace infinispan {
namespace hotrod {

RemoteCacheBase::RemoteCacheBase() : 
    Handle<RemoteCacheImpl>(new RemoteCacheImpl(*this)) {}
    
void RemoteCacheBase::setMarshallers(void* rc, MarshallHelperFn kf, MarshallHelperFn vf) {
    remoteCachePtr = rc;
    baseKeyMarshallFn = kf;
    baseValueMarshallFn = vf;
}
    
void RemoteCacheBase::baseKeyMarshall(const void* k, void *buf) {
    baseKeyMarshallFn(remoteCachePtr, k, buf);
}

void RemoteCacheBase::baseValueMarshall(const void* v, void *buf) {
    baseValueMarshallFn(remoteCachePtr, v, buf);
}

void RemoteCacheBase::base_get(const void *key, void *buf) {
    impl->get(key, buf);
}

void RemoteCacheBase::base_put(const void *key, const void *val) {
    impl->put(key, val);
}

void RemoteCacheBase::base_ping() {
    impl->ping();
}

}} /* namespace */
