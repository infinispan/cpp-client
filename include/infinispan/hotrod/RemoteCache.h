#ifndef ISPN_HOTROD_REMOTECACHE_H
#define ISPN_HOTROD_REMOTECACHE_H

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
#include "infinispan/hotrod/Marshaller.h"

namespace infinispan {
namespace hotrod {

template <class K, class V>
class RemoteCache : private RemoteCacheBase
{
  public:
    V* get(const K& k) {
        ScopedBuffer vbuf;
        base_get(&k, &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }
    void put(const K& k, const V& v) {
        base_put(&k, &v);;
    }
    // Need separate signature for ForceReturnPreviousValue:   V put(const K& k, const V& v)
    void ping() {
        base_ping();
    }

  private:
    RemoteCache() {
        setMarshallers(this, &keyMarshall, &valueMarshall);
    }
    // type-hiding and resurrecting support
    static void keyMarshall(void *thisp, const void* key, void* buf) {
        ((RemoteCache<K, V> *) thisp)->keyMarshaller->marshall(*(K *) key, *(ScopedBuffer *) buf);
    }
    static void valueMarshall(void* thisp, const void* val, void* buf) {
        ((RemoteCache<K, V> *)thisp)->valueMarshaller->marshall(*(V *) val, *(ScopedBuffer *) buf);
    }

    HR_SHARED_PTR<Marshaller<K> > keyMarshaller;
    HR_SHARED_PTR<Marshaller<V> > valueMarshaller;

  friend class RemoteCacheManager;
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHE_H */
