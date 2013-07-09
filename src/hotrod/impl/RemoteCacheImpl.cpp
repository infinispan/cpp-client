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
#include "hotrod/impl/RemoteCacheImpl.h"
#include "hotrod/impl/operations/OperationsFactory.h"
#include "hotrod/impl/operations/GetOperation.h"
#include "hotrod/impl/operations/PutOperation.h"
#include "hotrod/impl/operations/PingOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/protocol/CodecFactory.h"

#include <iostream>

namespace infinispan {
namespace hotrod {

using namespace operations;
using namespace transport;
using namespace protocol;
//using namespace sys;

RemoteCacheImpl::RemoteCacheImpl(RemoteCacheBase& base)
    :remoteCacheBase(base) /*, operationsFactory(OperationsFactory::getTestFactory())*/ {
	operationsFactory = new OperationsFactory(TransportFactory::getTransportFactory(),"",0,false,*CodecFactory::getCodec("1.2"));
}


void RemoteCacheImpl::get(const void *k, void* b) {
    ScopedBuffer kbuf;
    ScopedBuffer& vbuf(*(ScopedBuffer *)b);
    remoteCacheBase.baseKeyMarshall(k, &kbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());

    hr_scoped_ptr<GetOperation> gco(operationsFactory->newGetKeyOperation(keyBytes));
    hrbytes bytes = gco->execute();
    bytes.releaseTo(vbuf);
}

void RemoteCacheImpl::put(const void *k, const void* v) {
    ScopedBuffer kbuf, vbuf;
    remoteCacheBase.baseKeyMarshall(k, &kbuf);
    remoteCacheBase.baseValueMarshall(v, &vbuf);
    hrbytes keyBytes(kbuf.getBytes(), kbuf.getLength());
    hrbytes valueBytes(vbuf.getBytes(), vbuf.getLength());

    hr_scoped_ptr<PutOperation> op(operationsFactory->newPutKeyValueOperation(keyBytes, valueBytes,0,0));
    op->execute();
}

void RemoteCacheImpl::ping() {
    TransportFactory& transportFactory = TransportFactory::getTransportFactory();
    Transport* transport = transportFactory.getTransport();
	hr_scoped_ptr<PingOperation> op(operationsFactory->newPingOperation(*transport));
    op->execute();
    transportFactory.releaseTransport(transport);
}

}} /* namespace */
