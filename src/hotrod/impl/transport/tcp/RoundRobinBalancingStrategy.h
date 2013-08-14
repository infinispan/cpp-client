#ifndef ISPN_HOTROD_TRANSPORT_ROUNDROBINBALANCINGSTRATEGY_H
#define ISPN_HOTROD_TRANSPORT_ROUNDROBINBALANCINGSTRATEGY_H

#include "hotrod/impl/transport/tcp/RequestBalancingStrategy.h"
#include "infinispan/hotrod/types.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class RoundRobinBalancingStrategy : public RequestBalancingStrategy
{
  public:
	RoundRobinBalancingStrategy();
    void setServers(const std::vector<InetSocketAddress>& servers);
    const InetSocketAddress& nextServer();

  private:
    std::vector<InetSocketAddress> servers;
    size_t index;

    const InetSocketAddress& getServerByIndex(size_t pos);
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_ROUNDROBINBALANCINGSTRATEGY_H */
