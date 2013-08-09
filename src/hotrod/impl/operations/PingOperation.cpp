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

#include "hotrod/impl/operations/PingOperation.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/transport/Transport.h"

namespace infinispan {
namespace hotrod {

using namespace protocol;
using transport::Transport;

namespace operations {

PingOperation::PingOperation(const Codec& c, uint32_t id,
		Transport& t, const hrbytes& n)
    : HotRodOperation<PingResult>(c, 0, n, id), transport(t)
{}

PingOperation::PingOperation(const Codec& c, uint32_t id,
		Transport& t)
    : HotRodOperation<PingResult>(c, 0, hrbytes(), id), transport(t)
{}

PingResult PingOperation::execute() {
    //HeaderParams params;
	hr_scoped_ptr<infinispan::hotrod::protocol::HeaderParams> params(&writeHeader(transport, HotRodConstants::PING_REQUEST));
	transport.flush();

    uint8_t respStatus = readHeaderAndValidate(transport, *params);
    if (respStatus == HotRodConstants::NO_ERROR_STATUS) {
    	return SUCCESS;
    } else {
    	return FAIL;
    }
}

}}} // namespace
