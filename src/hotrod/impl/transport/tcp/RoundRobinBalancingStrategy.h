#ifndef ISPN_HOTROD_TRANSPORT_ROUNDROBINBALANCINGSTRATEGY_H
#define ISPN_HOTROD_TRANSPORT_ROUNDROBINBALANCINGSTRATEGY_H

#include "infinispan/hotrod/FailOverRequestBalancingStrategy.h"
#include <set>

namespace infinispan {
namespace hotrod {
namespace transport {

class RoundRobinBalancingStrategy : public FailOverRequestBalancingStrategy
{
  public:
	RoundRobinBalancingStrategy();
    void setServers(const std::vector<transport::InetSocketAddress>& servers);
    const transport::InetSocketAddress& nextServer(const std::set<transport::InetSocketAddress>& failedServer);
	static FailOverRequestBalancingStrategy* newInstance();

  private:
    std::vector<transport::InetSocketAddress> servers;
    size_t index;

    const transport::InetSocketAddress& getServerByIndex(size_t pos);
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_ROUNDROBINBALANCINGSTRATEGY_H */
