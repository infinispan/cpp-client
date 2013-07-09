#ifndef ISPN_HOTROD_REMOTECACHEMANAGER_H
#define ISPN_HOTROD_REMOTECACHEMANAGER_H

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

#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/Handle.h"
#include "infinispan/hotrod/RemoteCache.h"

//TODO: make the marshaller configurable
#include "hotrod/sys/BasicMarshaller.h"

namespace infinispan {
namespace hotrod {

class RemoteCacheManagerImpl;

class HR_EXTERN RemoteCacheManager
    : public infinispan::hotrod::Handle<RemoteCacheManagerImpl>
{
  public:
    RemoteCacheManager();

    template <class K, class V> RemoteCache<K, V> getCache() {
        RemoteCache<K, V> rcache;
        initCache(rcache);
        rcache.keyMarshaller.reset(new sys::BasicMarshaller<K>());
        rcache.valueMarshaller.reset(new sys::BasicMarshaller<V>());
        return rcache;
    }

    template <class K, class V> RemoteCache<K, V> 
	getCache(HR_SHARED_PTR<Marshaller<K> > km, HR_SHARED_PTR<Marshaller<V> > vm) {
        RemoteCache<K, V> rcache;
        initCache(rcache);
        rcache.keyMarshaller = km;
        rcache.valueMarshaller = vm;
        return rcache;
    }

  private:
    void initCache (RemoteCacheBase &cache);
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHEMANAGER_H */
