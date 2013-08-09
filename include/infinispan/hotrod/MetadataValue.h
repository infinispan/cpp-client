#ifndef ISPN_HOTROD_METADATAVALUE_H
#define ISPN_HOTROD_METADATAVALUE_H

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

#include <stdint.h>

namespace infinispan {
namespace hotrod {

class MetadataValue
{
  public:
	MetadataValue() :
        created(0), lifespan(0), lastUsed(0), maxIdle(0), version(0) {}

    int64_t created;
    uint32_t lifespan;
    int64_t lastUsed;
    uint32_t maxIdle;
    int64_t version;
};

}} // namespace infinispan::hotrod

#endif // ISPN_HOTROD_METADATAVALUE_H
