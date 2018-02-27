#ifndef ISPN_HOTROD_TRANSPORT_FAILOVERREQUESTBALANCINGSTRATEGY_H
#define ISPN_HOTROD_TRANSPORT_FAILOVERREQUESTBALANCINGSTRATEGY_H

#include <infinispan/hotrod/InetSocketAddress.h>
#include <vector>
#include <set>

namespace infinispan {
namespace hotrod {

/**
 * Abstract class for a balancing strategy on failover. An implementation of this class must
 * define a policy for the selection of the target server for a specific request.
 */
class FailOverRequestBalancingStrategy
{
  public:
    /** Signature spec for function that produce object of this class */
    typedef FailOverRequestBalancingStrategy* (*ProducerFn)();
    /**
     * Set the the list of the available servers
     * \param servers the servers list
     */
    virtual void setServers(const std::vector<transport::InetSocketAddress>& servers) = 0;
    /**
     * \param failedServers the list of the server with failure
     * \return the next target server
     */
    virtual const transport::InetSocketAddress& nextServer(const std::set<transport::InetSocketAddress>& failedServers) = 0;

    virtual ~FailOverRequestBalancingStrategy() {};
  private:
    std::vector<transport::InetSocketAddress> servers;
    size_t index;

};

}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_FAILOVERREQUESTBALANCINGSTRATEGY_H */
