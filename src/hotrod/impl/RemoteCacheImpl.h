#ifndef ISPN_HOTROD_REMOTECACHEIMPL_H
#define ISPN_HOTROD_REMOTECACHEIMPL_H

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

#include "infinispan/hotrod/types.h"
#include "infinispan/hotrod/ScopedBuffer.h"
#include "infinispan/hotrod/RemoteCacheBase.h"
#include "hotrod/impl/MetadataValueImpl.h"

namespace infinispan {
namespace hotrod {

namespace operations {
class OperationsFactory;
}

class RemoteCacheImpl
{
  public:
    RemoteCacheImpl(RemoteCacheBase& base, const std::string& name);
    void get(const void* key, void *buf);
    void put(const void *key, const void* val, uint64_t life, uint64_t idle, void* b);
    void putIfAbsent(const void *key, const void* val, uint64_t life, uint64_t idle, void* b);
    void replace(const void *key, const void* val, uint64_t life, uint64_t idle, void* b);
    void remove(const void* key, void* buf);
    void containsKey(const void* key, bool* res);
    void replaceWithVersion(const void* k, const void* v, uint64_t version, uint64_t life, uint64_t idle, bool* res);
    void removeWithVersion(const void* k, uint64_t version, bool* res);
    void getWithMetadata(const void *key, void* vbuf, MetadataValue* metadata);
    void getBulk(int size, std::map<void*, void*>* mbuf);
    void keySet(int scope, std::set<void*>* result);
    void stats(std::map<std::string,std::string>* stats);
    void clear();

    void ping();

    void init(const std::string& name, operations::OperationsFactory* operationsFactory);

    void withFlags(Flag flag);

    const std::string& getName() const;

  private:
    RemoteCacheBase& remoteCacheBase;

    infinispan::hotrod::operations::OperationsFactory* operationsFactory;
    std::string name;

    void applyDefaultExpirationFlags(uint64_t lifespan, uint64_t maxIdle);

};

}} // namespace infinispan::hotrod

#endif  /* ISPN_HOTROD_REMOTECACHEIMPL_H */

