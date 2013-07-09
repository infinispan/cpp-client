#ifndef ISPN_HOTROD_TRANSPORT_ABSTRACTTRANSPORT_H
#define ISPN_HOTROD_TRANSPORT_ABSTRACTTRANSPORT_H

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

#include <sstream>

#include "hotrod/impl/transport/Transport.h"

#include "hotrod/sys/types.h"
#include "hotrod/sys/Socket.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class AbstractTransport : public Transport
{
  public:
    void writeArray(const hrbytes& bytes);
    hrbytes readArray();

  protected:
    virtual void writeBytes(const hrbytes& bytes) = 0;
    virtual void readBytes(hrbytes& bytes, uint32_t size) = 0;
};

}}} // namespace infinispan::hotrod::transport

#endif  // ISPN_HOTROD_TRANSPORT_ABSTRACTTRANSPORT_H

