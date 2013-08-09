#ifndef ISPN_HOTROD_REMOTECACHEBASE_H
#define ISPN_HOTROD_REMOTECACHEBASE_H

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
#include "infinispan/hotrod/Flag.h"
#include "infinispan/hotrod/MetadataValue.h"

#include <map>
#include <set>

namespace infinispan {
namespace hotrod {

namespace operations {
class OperationsFactory;
}

class RemoteCacheImpl;
typedef void (*MarshallHelperFn) (void*, const void*, void*);
typedef void* (*UnmarshallHelperFn) (void*, const void*);

class HR_EXTERN RemoteCacheBase
  : public infinispan::hotrod::Handle<RemoteCacheImpl>
{
  public:
    void base_get(const void *key, void *rbuf);
    void base_put(const void *key, const void *value, int64_t life, int64_t idle, void *buf);
    void base_putIfAbsent(const void *key, const void *value, int64_t life, int64_t idle, void *buf);
    void base_replace(const void *key, const void *value, int64_t life, int64_t idle, void *buf);
    void base_remove(const void *key, void *rbuf);
    void base_containsKey(const void *key, bool *res);
    void base_ping();
    void base_replaceWithVersion(const void *key, const void *value, int64_t version, int64_t life, int64_t idle, bool *res);
    void base_removeWithVersion(const void *key, int64_t version, bool *res);
    void base_getWithMetadata(const void* key, void* vbuf, MetadataValue* metadata);
    void base_getBulk(int size, std::map<void*, void*>* mbuf);
    void base_keySet(int scope, std::set<void*>* sbuf);
    void base_stats(std::map<std::string,std::string>* sbuf);
    void base_clear();
    void base_withFlags(Flag flag);

    void init(const std::string& name, operations::OperationsFactory* operationFactory);

 protected:
    RemoteCacheBase(const std::string& name);
    void setMarshallers(void* rc, MarshallHelperFn kf, MarshallHelperFn vf, UnmarshallHelperFn ukf, UnmarshallHelperFn uvf);

  private:
    void *remoteCachePtr;
    MarshallHelperFn baseKeyMarshallFn;
    MarshallHelperFn baseValueMarshallFn;
    void baseKeyMarshall(const void* k, void *buf);
    void baseValueMarshall(const void* v, void *buf);

    UnmarshallHelperFn baseKeyUnmarshallFn;
    UnmarshallHelperFn baseValueUnmarshallFn;
    void* baseKeyUnmarshall(const void* buf);
    void* baseValueUnmarshall(const void* buf);

  friend class RemoteCacheManager;
  friend class RemoteCacheImpl;
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHEBASE_H */
