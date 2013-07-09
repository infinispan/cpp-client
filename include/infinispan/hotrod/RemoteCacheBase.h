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

namespace infinispan {
namespace hotrod {

class RemoteCacheImpl;
typedef void (*MarshallHelperFn) (void*, const void *, void*);

class HR_EXTERN RemoteCacheBase
    : public infinispan::hotrod::Handle<RemoteCacheImpl>
{
 public:
    void base_get(const void *key, void *rbuf);
    void base_put(const void *key, const void *value);
    void base_ping();
  protected:
    RemoteCacheBase();
    void setMarshallers(void* rc, MarshallHelperFn kf, MarshallHelperFn vf);

  private:
    void *remoteCachePtr;
    MarshallHelperFn baseKeyMarshallFn;
    MarshallHelperFn baseValueMarshallFn;
    void baseKeyMarshall(const void* k, void *buf);
    void baseValueMarshall(const void* v, void *buf);

  friend class RemoteCacheManagerImpl;
  friend class RemoteCacheImpl;
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHEBASE_H */
