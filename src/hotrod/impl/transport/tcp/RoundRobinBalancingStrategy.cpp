#include <iostream>

#include "hotrod/impl/transport/tcp/RoundRobinBalancingStrategy.h"

namespace infinispan {
namespace hotrod {
namespace transport {

FailOverRequestBalancingStrategy* RoundRobinBalancingStrategy::newInstance() {
    return new RoundRobinBalancingStrategy();
}

RoundRobinBalancingStrategy::RoundRobinBalancingStrategy() : index(0) {}

void RoundRobinBalancingStrategy::setServers(const std::vector<ServerNameId>& s) {
    servers = s;
    // keep the old index if possible so that we don't produce more requests for the first server
    if (index >= servers.size()) {
       index = 0;
    }
}

const ServerNameId& RoundRobinBalancingStrategy::nextServer() {
    const ServerNameId& server = getServerByIndex(index++);
    if (index >= servers.size()) {
       index = 0;
    }
    return server;
}

const ServerNameId& RoundRobinBalancingStrategy::getServerByIndex(size_t pos) {
    const ServerNameId& server = servers[pos];
    return server;
}

}}} /* namespace */
