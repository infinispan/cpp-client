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
