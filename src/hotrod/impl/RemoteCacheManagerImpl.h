#ifndef ISPN_HOTROD_REMOTECACHEMANAGERIMPL_H
#define ISPN_HOTROD_REMOTECACHEMANAGERIMPL_H

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

#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/Handle.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "hotrod/impl/configuration/Configuration.h"
#include "hotrod/impl/protocol/Codec.h"
#include "hotrod/impl/transport/TransportFactory.h"

#include <map>

namespace infinispan {
namespace hotrod {

class RemoteCacheManagerImpl
{
  public:
    RemoteCacheManagerImpl(bool start = true);
    //RemoteCacheManagerImpl(const Configuration& configuration, bool start = true);
	RemoteCacheManagerImpl(const std::map<std::string,std::string>& properties, bool start = true);

    void start();
    void stop();
    bool isStarted();
    const Configuration& getConfiguration();
    void initCache(RemoteCacheImpl& cache, bool forceReturnValue);

  private:
    transport::TransportFactory* transportFactory;
    // TODO: volatile
    bool started;
    Configuration configuration;
    protocol::Codec* codec;
    // TODO: atomic, initialized to 1
    int64_t topologyId;
};

}} // namespace infinispan::hotrod

#endif  /* ISPN_HOTROD_REMOTECACHEMANAGERIMPL_H */
