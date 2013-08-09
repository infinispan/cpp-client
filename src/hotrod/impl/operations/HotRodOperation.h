#ifndef ISPN_HOTROD_OPERATIONS_HOTRODOPERATION_H
#define ISPN_HOTROD_OPERATIONS_HOTRODOPERATION_H

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

#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/protocol/Codec.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "infinispan/hotrod/Flag.h"

namespace infinispan {
namespace hotrod {
namespace operations {


template<class T> class HotRodOperation : public protocol::HotRodConstants
{
  public:
    virtual T execute() = 0;

  protected:
    HotRodOperation(
        const protocol::Codec& _codec,
        uint32_t _flags, const hrbytes& _cacheName, uint32_t _topologyId) :
            codec(_codec), flags(_flags),
            cacheName(_cacheName), topologyId(_topologyId) {
    }

    protocol::HeaderParams& writeHeader(
        transport::Transport& transport, uint8_t opCode)
    {
       // TODO: CLIENT_INTELLIGENCE_HASH_DISTRIBUTION_AWARE
        protocol::HeaderParams* params =
            new protocol::HeaderParams();
        (*params).setOpCode(opCode).setCacheName(cacheName)
            .setFlags(flags).setClientIntel(CLIENT_INTELLIGENCE_BASIC)
            .setTopologyId(topologyId).setTxMarker(NO_TX);

        return codec.writeHeader(transport, *params);
    }

    uint8_t readHeaderAndValidate(
        transport::Transport& transport,
        const protocol::HeaderParams& params)
    {
        return codec.readHeader(transport, params);
    }

    virtual ~HotRodOperation() {}

    const protocol::Codec& codec;
    uint32_t flags;
    hrbytes cacheName;
    // TODO: atomic
    uint32_t topologyId;

  private:
    static const uint8_t NO_TX = 0x0;

};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_HOTRODOPERATION_H
