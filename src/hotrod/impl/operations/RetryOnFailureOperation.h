#ifndef ISPN_HOTROD_OPERATIONS_RETRYONFAILUREOPERATION_H
#define ISPN_HOTROD_OPERATIONS_RETRYONFAILUREOPERATION_H

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

#include "hotrod/impl/operations/HotRodOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"

namespace infinispan {
namespace hotrod {
namespace operations {

template<class T> class RetryOnFailureOperation : public HotRodOperation<T>
{
  public:
    T execute() {
        int retryCount = 0;
        while (shouldRetry(retryCount)) {
            transport::Transport* transport = NULL;
            try {
                // Transport retrieval should be retried
                transport = &getTransport(retryCount);
                const T& result = executeOperation(*transport);
                releaseTransport(transport);
                return result;
            } catch(const TransportException& te) {
                // Invalidate transport since this exception means that this
                // instance is no longer usable and should be destroyed
                transportFactory->invalidateTransport(te.getServerAddress(), transport);
                releaseTransport(transport);
                logErrorAndThrowExceptionIfNeeded(retryCount, te);
            } catch(const RemoteNodeSuspectException& rnse) {
                // Do not invalidate transport because this exception is caused
                // as a result of a server finding out that another node has
                // been suspected, so there's nothing really wrong with the server
                // from which this node was received
                releaseTransport(transport);
                logErrorAndThrowExceptionIfNeeded(retryCount, rnse);
            }
            ++retryCount;
        }
        throw InternalException("We should not reach here!");
    }

  protected:
    RetryOnFailureOperation(
        const protocol::Codec& _codec,
        transport::TransportFactory* _transportFactory,
        const hrbytes& _cacheName, uint32_t _topologyId, uint32_t _flags) :
            HotRodOperation<T>(_codec, _flags, _cacheName, _topologyId),
            transportFactory(_transportFactory) {}

    bool shouldRetry(int retryCount) {
       return retryCount < transportFactory->getTransportCount();
    }

    void releaseTransport(transport::Transport* transport) {
        if (transport) {
            transportFactory->releaseTransport(*transport);
        }
    }

    void logErrorAndThrowExceptionIfNeeded(int i, const HotRodClientException&) {
        if (i >= transportFactory->getTransportCount() - 1
            || transportFactory->getTransportCount() < 0)
        {
            // log
    	    throw;
        } else {
            // log
        }
    }

    virtual transport::Transport& getTransport(int retryCount) = 0;
    virtual T executeOperation(transport::Transport& transport) = 0;

    virtual ~RetryOnFailureOperation() {}

    transport::TransportFactory* transportFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_RETRYONFAILUREOPERATION_H
