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

#include "hotrod/impl/transport/tcp/RoundRobinBalancingStrategy.h"


namespace infinispan {
namespace hotrod {
namespace transport {

RequestBalancingStrategy* RequestBalancingStrategy::newInstance() {
    return new RoundRobinBalancingStrategy();
}

RoundRobinBalancingStrategy::RoundRobinBalancingStrategy() : index(0) {}

void RoundRobinBalancingStrategy::setServers(const std::vector<InetSocketAddress>& s) {
    servers = s;
    // keep the old index if possible so that we don't produce more requests for the first server
    if (index >= servers.size()) {
       index = 0;
    }
}

const InetSocketAddress& RoundRobinBalancingStrategy::nextServer() {
    const InetSocketAddress& server = getServerByIndex(index++);
    if (index >= servers.size()) {
       index = 0;
    }
    return server;
}

const InetSocketAddress& RoundRobinBalancingStrategy::getServerByIndex(size_t pos) {
    const InetSocketAddress& server = servers[pos];
    // TODO: log?
    return server;
}

}}} /* namespace */
