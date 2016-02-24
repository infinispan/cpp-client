#ifndef ISPN_HOTROD_TRANSPORT_FAILOVERREQUESTBALANCINGSTRATEGY_H
#define ISPN_HOTROD_TRANSPORT_FAILOVERREQUESTBALANCINGSTRATEGY_H

#include <infinispan/hotrod/ServerNameId.h>
#include <vector>

namespace infinispan {
namespace hotrod {

class FailOverRequestBalancingStrategy
{
  public:
    typedef FailOverRequestBalancingStrategy* (*ProducerFn)();
    virtual void setServers(const std::vector<ServerNameId>& servers) = 0;
    virtual const ServerNameId& nextServer() = 0;

    virtual ~FailOverRequestBalancingStrategy() {};
  private:
    std::vector<ServerNameId> servers;
    size_t index;

};

}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_FAILOVERREQUESTBALANCINGSTRATEGY_H */
