/*
 * JBoss, Home of Professional Open Source
 * Copyright 2010 Red Hat Inc. and/or its affiliates and other
 * contributors as indicated by the @author tags. All rights reserved.
 * See the copyright.txt in the distribution for a full listing of
 * individual contributors.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */

#include "infinispan/hotrod/RemoteCacheBase.h"
#include "hotrod/impl/RemoteCacheImpl.h"
//#include "hotrod/impl/MetadataValueImpl.h"

#include <iostream>

namespace infinispan {
namespace hotrod {

RemoteCacheBase::RemoteCacheBase(const std::string& name) :
    Handle<RemoteCacheImpl>(new RemoteCacheImpl(*this, name)) {}
    
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

void RemoteCacheBase::init(const std::string& name, operations::OperationsFactory* operationFactory) {
    impl->init(name, operationFactory);
}


void RemoteCacheBase::base_get(const void *key, void *buf) {
    impl->get(key, buf);
}

void RemoteCacheBase::base_put(const void *key, const void *val, int64_t life, int64_t idle, void *buf) {
    impl->put(key, val, life, idle, buf);
}

void RemoteCacheBase::base_putIfAbsent(const void *key, const void *val, int64_t life, int64_t idle, void *buf) {
    impl->putIfAbsent(key, val, life, idle, buf);
}

void RemoteCacheBase::base_replace(const void *key, const void *val, int64_t life, int64_t idle, void *buf) {
    impl->replace(key, val, life, idle, buf);
}

void RemoteCacheBase::base_remove(const void *key, void *rbuf) {
    impl->remove(key, rbuf);
}

void RemoteCacheBase::base_containsKey(const void *key, bool *res){
    impl->containsKey(key, res);
}

void RemoteCacheBase::base_replaceWithVersion(const void *key, const void *value, int64_t version, int64_t life, int64_t idle, bool *res)
{
    impl->replaceWithVersion(key, value, version, life, idle, res);
}

void RemoteCacheBase::base_removeWithVersion(const void *key, int64_t version, bool *res)
{
    impl->removeWithVersion(key, version, res);
}

void RemoteCacheBase::base_getWithMetadata(
    const void *key, void* vbuf, MetadataValue* metadata)
{
	impl->getWithMetadata(key, vbuf, metadata);
}

void RemoteCacheBase::base_getBulk(int size, std::map<void*, void*>* mbuf)
{
	impl->getBulk(size, mbuf);
}


void RemoteCacheBase::base_keySet(int scope, std::set<void*>* result)
{
	impl->keySet(scope, result);
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
