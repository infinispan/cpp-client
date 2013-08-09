#ifndef ISPN_HOTROD_OPERATIONS_ABSTRACTKEYOPERATION_H
#define ISPN_HOTROD_OPERATIONS_ABSTRACTKEYOPERATION_H

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

#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/VersionedOperationResponse.h"
#include "infinispan/hotrod/Flag.h"
#include "hotrod/impl/protocol/HotRodConstants.h"

#include <set>
#include <sstream>

namespace infinispan {
namespace hotrod {
namespace operations {

template<class T> class AbstractKeyOperation : public RetryOnFailureOperation<T>
{
  protected:
    const hrbytes& key;

    AbstractKeyOperation(
        const protocol::Codec& _codec,
        transport::TransportFactory* _transportFactory,
        const hrbytes& _key, const hrbytes& _cacheName,
        uint32_t _topologyId, uint32_t  _flags) :
            RetryOnFailureOperation<T>(
                _codec, _transportFactory, _cacheName, _topologyId, _flags),
            key(_key)
        {}

    transport::Transport& getTransport(int retryCount) {
        if (retryCount == 0) {
            return RetryOnFailureOperation<T>::transportFactory->getTransport(key);
        } else {
            return RetryOnFailureOperation<T>::transportFactory->getTransport();
        }
    }

    uint8_t sendKeyOperation(
        const hrbytes& _key, transport::Transport& transport,
        uint8_t opCode, uint8_t /*opRespCode*/)
    {
        // 1) write [header][key length][key]
        hr_scoped_ptr<protocol::HeaderParams> params(
            &(RetryOnFailureOperation<T>::writeHeader(transport, opCode)));
        transport.writeArray(_key);
        transport.flush();

        // 2) now read the header
        return RetryOnFailureOperation<T>::readHeaderAndValidate(transport, *params);
    }

    hrbytes returnPossiblePrevValue(transport::Transport& transport) {
        hrbytes result;
        if (hasForceReturn(RetryOnFailureOperation<T>::flags)) {
            result = transport.readArray();
        }
        return result;
    }

    VersionedOperationResponse returnVersionedOperationResponse(
        transport::Transport& transport, protocol::HeaderParams& params)
    {
        uint8_t respStatus =
            RetryOnFailureOperation<T>::readHeaderAndValidate(transport, params);
        VersionedOperationResponse::RspCode code;
        if (respStatus == protocol::HotRodConstants::NO_ERROR_STATUS) {
           	code = VersionedOperationResponse::SUCCESS;
        } else if (respStatus == protocol::HotRodConstants::NOT_PUT_REMOVED_REPLACED_STATUS) {
            code = VersionedOperationResponse::MODIFIED_KEY;
        } else if (respStatus == protocol::HotRodConstants::KEY_DOES_NOT_EXIST_STATUS) {
            code = VersionedOperationResponse::NO_SUCH_KEY;
        } else {
            std::ostringstream message;
            message << "Unknown response status: " << std::hex << (int)respStatus;
            // TODO: check exception type
            throw InvalidResponseException(message.str());
        }
        hrbytes prevValue = returnPossiblePrevValue(transport);
        return VersionedOperationResponse(prevValue, code);
    }


  private:
    bool hasForceReturn(uint32_t _flags) {
        if ((_flags & FORCE_RETURN_VALUE) != FORCE_RETURN_VALUE) {
            return false;
        }
        return true;
    }
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_ABSTRACTKEYOPERATION_H

