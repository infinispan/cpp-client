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
#include "infinispan/hotrod/Flag.h"

#include <set>

namespace infinispan {
namespace hotrod {
namespace operations {

template<class T> class AbstractKeyOperation : public RetryOnFailureOperation<T>
{
    protected:
        AbstractKeyOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            infinispan::hotrod::transport::TransportFactory& transportFactory_,
            const hrbytes&                                   key_,
            const hrbytes&                                   cacheName_,
            uint32_t                                         topologyId_,
            const std::set<Flag>&                                   flags_)
            : RetryOnFailureOperation<T>(codec_, transportFactory_,
                                      cacheName_,
                                      topologyId_, flags_), key(key_)
        {}

        infinispan::hotrod::transport::Transport* getTransport(int retryCount)
        {
            if (retryCount == 0) {
                return RetryOnFailureOperation<T>::transportFactory.getTransport(key);
            } else {
                return RetryOnFailureOperation<T>::transportFactory.getTransport();
            }
        }

        uint8_t sendKeyOperation(
            const hrbytes&                                      key_,
            infinispan::hotrod::transport::Transport&     transport,
            uint8_t                                       opCode,
            uint8_t                                       /*opRespCode*/)
        {
           // 1) write [header][key length][key]
           hr_scoped_ptr<infinispan::hotrod::protocol::HeaderParams> params(&(RetryOnFailureOperation<T>::writeHeader(transport, opCode)));
           transport.writeArray(key_);
           transport.flush();

           // 2) now read the header
           return RetryOnFailureOperation<T>::readHeaderAndValidate(transport, *params);
        }

        hrbytes returnPossiblePrevValue(infinispan::hotrod::transport::Transport& transport) {
            hrbytes result;
            if (hasForceReturn(HotRodOperation<T>::flags)) {
               result = transport.readArray();
            }
            result.setSmart(0,0);
            return result;
         }


           const hrbytes& key;

    private:
        bool hasForceReturn(const std::set<Flag>& flags_) {
            if (flags_.find(FORCE_RETURN_VALUE) != flags_.end()) {
                return true;
            }
            return false;
        }

};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_ABSTRACTKEYOPERATION_H

