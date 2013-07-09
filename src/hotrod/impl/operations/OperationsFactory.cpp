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

#include "hotrod/sys/types.h"
#include "hotrod/impl/operations/OperationsFactory.h"
#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/operations/GetOperation.h"
#include "hotrod/impl/operations/PutOperation.h"
#include "hotrod/impl/operations/PingOperation.h"
#include "infinispan/hotrod/Flag.h"

#include <cstring>

namespace infinispan {
namespace hotrod {

using namespace protocol;
using namespace transport;

namespace operations {

OperationsFactory::OperationsFactory(
  TransportFactory& tf, const std::string& cn,
  uint32_t tid, bool frv, const Codec& c) :
	transportFactory(tf), codec(c), forceReturnValue(frv), topologyId(tid), flags()
{
  cacheNameBytes.reserve(cn.length());
  memcpy(cacheNameBytes.bytes(), cn.c_str(), cn.length());
}

PingOperation* OperationsFactory::newPingOperation(Transport& transport)
{
  return new PingOperation(codec, topologyId, transport, cacheNameBytes);
}

GetOperation* OperationsFactory::newGetKeyOperation(const hrbytes& key)
{
  return new GetOperation(
    codec, transportFactory, key, cacheNameBytes, topologyId, getFlags());
}

PutOperation* OperationsFactory::newPutKeyValueOperation(
  const hrbytes& key, const hrbytes& value,
  uint32_t lifespanSecs, uint32_t maxIdleSecs)
{
  return new PutOperation(
    codec, transportFactory, key, cacheNameBytes,
    topologyId, getFlags(), value, lifespanSecs, maxIdleSecs);
}

std::set<Flag> OperationsFactory::getFlags()
{
  std::set<Flag> result(flags);
  if (forceReturnValue) {
    result.insert(FORCE_RETURN_VALUE);
  }
  flags.clear();
  return result;
}

}}} // namespace infinispan::hotrod::operations
