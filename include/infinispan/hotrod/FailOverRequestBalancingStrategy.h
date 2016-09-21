#ifndef ISPN_HOTROD_TRANSPORT_FAILOVERREQUESTBALANCINGSTRATEGY_H
#define ISPN_HOTROD_TRANSPORT_FAILOVERREQUESTBALANCINGSTRATEGY_H

#include <infinispan/hotrod/InetSocketAddress.h>
#include <vector>
#include <set>

namespace infinispan {
namespace hotrod {

//using infinispan::hotrod::transport::InetSocketAddress;

class FailOverRequestBalancingStrategy
{
  public:
    typedef FailOverRequestBalancingStrategy* (*ProducerFn)();
    virtual void setServers(const std::vector<transport::InetSocketAddress>& servers) = 0;
    virtual const transport::InetSocketAddress& nextServer(const std::set<transport::InetSocketAddress>& failedServer) = 0;

    virtual ~FailOverRequestBalancingStrategy() {};
  private:
    std::vector<transport::InetSocketAddress> servers;
    size_t index;

};

}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_FAILOVERREQUESTBALANCINGSTRATEGY_H */
