#include <iostream>

#include "hotrod/impl/transport/tcp/RoundRobinBalancingStrategy.h"

namespace infinispan {
namespace hotrod {
namespace transport {

FailOverRequestBalancingStrategy* RoundRobinBalancingStrategy::newInstance() {
    return new RoundRobinBalancingStrategy();
}

RoundRobinBalancingStrategy::RoundRobinBalancingStrategy() : index(0) {}

void RoundRobinBalancingStrategy::setServers(const std::vector<transport::InetSocketAddress>& s) {
    servers = s;
    // keep the old index if possible so that we don't produce more requests for the first server
    if (index >= servers.size()) {
       index = 0;
    }
}

const transport::InetSocketAddress& RoundRobinBalancingStrategy::nextServer(const std::set<transport::InetSocketAddress>& failedServers) {
    sys::ScopedLock<sys::Mutex> scopedLock(lock);
    for (auto i = 0u; i < servers.size(); i++) {
        if (++index >= servers.size()) {
       index = 0;
    }
        const transport::InetSocketAddress &server = getServerByIndex(index);
        if (failedServers.find(server) == failedServers.end()) {
    return server;
        }
    }
    throw NoSuchElementException("Balancing: No more server available.");
}

const transport::InetSocketAddress& RoundRobinBalancingStrategy::getServerByIndex(size_t pos) {
    const transport::InetSocketAddress& server = servers[pos];
    return server;
}

}}} /* namespace */
