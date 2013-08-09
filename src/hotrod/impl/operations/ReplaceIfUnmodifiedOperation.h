#ifndef ISPN_HOTROD_OPERATIONS_REPLACEIFUNMODIFIEDOPERATION_H
#define ISPN_HOTROD_OPERATIONS_REPLACEIFUNMODIFIEDOPERATION_H

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

#include "hotrod/impl/operations/AbstractKeyValueOperation.h"
#include "hotrod/impl/VersionedOperationResponse.h"

namespace infinispan {
namespace hotrod {
namespace operations {

class ReplaceIfUnmodifiedOperation
    : public AbstractKeyValueOperation<VersionedOperationResponse>
{
  protected:
    VersionedOperationResponse executeOperation(transport::Transport& transport);

  private:
    ReplaceIfUnmodifiedOperation(
        const protocol::Codec& codec,
        transport::TransportFactory* transportFactory,
        const hrbytes& key,
        const hrbytes& cacheName,
        uint32_t topologyId,
        uint32_t flags,
        const hrbytes& value,
        uint32_t lifespan,
        uint32_t maxIdle,
        int64_t version);

    int64_t version;

  friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_REPLACEIFUNMODIFIEDOPERATION_H
