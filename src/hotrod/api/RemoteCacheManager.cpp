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

#include "infinispan/hotrod/RemoteCacheManager.h"
#include "hotrod/impl/RemoteCacheManagerImpl.h"
#include "hotrod/impl/configuration/ConfigurationBuilder.h"
#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/operations/OperationsFactory.h"

namespace infinispan {
namespace hotrod {


using namespace protocol;
using namespace transport;
using namespace operations;


RemoteCacheManager::RemoteCacheManager(bool start_)
  : Handle<RemoteCacheManagerImpl>(new RemoteCacheManagerImpl(start_)) { }


RemoteCacheManager::RemoteCacheManager(const std::map<std::string,std::string>& properties, bool start_)
  : Handle<RemoteCacheManagerImpl>(new RemoteCacheManagerImpl(properties,start_)) { }

void RemoteCacheManager::initCache(
  RemoteCacheBase& cache, bool forceReturnValue) {
    impl->initCache(*cache.impl, forceReturnValue);
}

void RemoteCacheManager::start() {
    impl->start();
}

void RemoteCacheManager::stop() {
    impl->stop();
}

bool RemoteCacheManager::isStarted() {
    return impl->isStarted();
}

const Configuration& RemoteCacheManager::getConfiguration() {
    return impl->getConfiguration();
}

}} // namespace infinispan::hotrod
