#ifndef ISPN_HOTROD_PROTOCOL_CODEC12_H
#define ISPN_HOTROD_PROTOCOL_CODEC12_H

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

#include "hotrod/impl/protocol/Codec.h"

#include <stdint.h>

namespace infinispan {
namespace hotrod {

namespace transport {
class Transport;
}

namespace protocol {

class HeaderParams;

class Codec12 : public Codec
{
    public:
        HeaderParams& writeHeader(
            infinispan::hotrod::transport::Transport& transport,
            HeaderParams& params) const;

        uint8_t readHeader(
            infinispan::hotrod::transport::Transport& transport,
            const HeaderParams& params) const;

    protected:
        HeaderParams& writeHeader(
            infinispan::hotrod::transport::Transport& transport,
            HeaderParams& params, uint8_t version) const;

        void readNewTopologyIfPresent(
            infinispan::hotrod::transport::Transport& transport,
            const HeaderParams& params) const;

        void readNewTopologyAndHash(
            infinispan::hotrod::transport::Transport& transport,
            uint32_t topologyId) const;

        void checkForErrorsInResponseStatus(
            infinispan::hotrod::transport::Transport& transport,
            const HeaderParams& params, uint8_t status) const;

        // TODO : multithread management
        static long msgId;

    private:
        Codec12() {}

    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC12_H
