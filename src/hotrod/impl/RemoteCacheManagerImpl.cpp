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

#include "hotrod/impl/RemoteCacheManagerImpl.h"
#include "hotrod/impl/RemoteCacheImpl.h"
#include "hotrod/impl/configuration/ConfigurationBuilder.h"
#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/operations/OperationsFactory.h"

namespace infinispan {
namespace hotrod {

//using namespace configuration;
using namespace protocol;
using namespace transport;
using namespace operations;

const std::string ISPN_CLIENT_HOTROD_SERVER_LIST("infinispan.client.hotrod.server_list");

RemoteCacheManagerImpl::RemoteCacheManagerImpl(bool start_)
  : transportFactory(0), started(false),
    configuration(ConfigurationBuilder().build()), codec(0), topologyId(0)
{
	if (start_) start();
}

RemoteCacheManagerImpl::RemoteCacheManagerImpl(const std::map<std::string,std::string>& properties, bool start_)
  : transportFactory(0), started(false),
    configuration(ConfigurationBuilder().build()), codec(0), topologyId(0)
{
  std::map<std::string,std::string>::const_iterator server_prop;

  server_prop = properties.find(ISPN_CLIENT_HOTROD_SERVER_LIST);
  if(server_prop != properties.end()) {
      std::string serverList = server_prop->second;
      configuration = ConfigurationBuilder().addServers(serverList).build();
  }

  if (start_) start();
}

void RemoteCacheManagerImpl::start() {
	codec = CodecFactory::getCodec(configuration.getProtocolVersion().c_str());
    if (!isStarted()) {
        transportFactory = TransportFactory::newInstance();
        transportFactory->start(*codec, configuration, topologyId);

        started = true;
	}
}

void RemoteCacheManagerImpl::stop() {
    if (isStarted()) {
        // TODO
        started = false;
    }
}

bool RemoteCacheManagerImpl::isStarted() {
    return started;
}

const Configuration& RemoteCacheManagerImpl::getConfiguration() {
    return configuration;
}

void RemoteCacheManagerImpl::initCache(
    RemoteCacheImpl& cache,  bool forceReturnValue)
{
    OperationsFactory* operationsFactory = new OperationsFactory(
        transportFactory,
        cache.getName(),
        topologyId,
        forceReturnValue,
        *CodecFactory::getCodec(configuration.getProtocolVersion().c_str()));

	cache.init(cache.getName(), operationsFactory);
}

}} // namespace infinispan::hotrod
