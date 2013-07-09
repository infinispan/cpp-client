#ifndef ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H
#define ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H

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
#include "infinispan/hotrod/Flag.h"

#include <set>
#include <stdint.h>

namespace infinispan {
namespace hotrod {

namespace transport {
class TransportFactory;
class Transport;
}

namespace protocol {
class Codec;
}

namespace operations {

class PingOperation;
class GetOperation;
class PutOperation;

class OperationsFactory
{
  public:
    OperationsFactory(
      infinispan::hotrod::transport::TransportFactory& transportFactory,
      const std::string& cacheName, uint32_t topologyId, bool forceReturnValue,
      const infinispan::hotrod::protocol::Codec& codec);

    PingOperation* newPingOperation(
      infinispan::hotrod::transport::Transport& transport);

    GetOperation* newGetKeyOperation(const hrbytes& key);

    PutOperation* newPutKeyValueOperation(
      const hrbytes& key, const hrbytes& value,
      uint32_t lifespanSecs, uint32_t maxIdleSecs);

    void addFlags(const std::set<Flag>& flags);
    void setFlags(const std::set<Flag>& flags);

  private:
    infinispan::hotrod::transport::TransportFactory& transportFactory;
    const infinispan::hotrod::protocol::Codec& codec;
    hrbytes cacheNameBytes;
    bool forceReturnValue;
    // TODO: atomic
    uint32_t topologyId;
    // TODO: thread local
    std::set<Flag> flags;

    std::set<Flag> getFlags();

  friend class RemoteCacheManagerImpl;

};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_OPERATIONSFACTORY_H

