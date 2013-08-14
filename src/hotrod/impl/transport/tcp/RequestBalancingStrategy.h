#ifndef ISPN_HOTROD_TRANSPORT_REQUESTBALANCINGSTRATEGY_H
#define ISPN_HOTROD_TRANSPORT_REQUESTBALANCINGSTRATEGY_H



#include "hotrod/impl/transport/tcp/InetSocketAddress.h"

#include <vector>

namespace infinispan {
namespace hotrod {
namespace transport {

class TcpTransportFactory;

class RequestBalancingStrategy
{
  public:
    virtual void setServers(const std::vector<InetSocketAddress>& servers) = 0;
    virtual const InetSocketAddress& nextServer() = 0;

    virtual ~RequestBalancingStrategy() {}

  private:
    static RequestBalancingStrategy* newInstance();

  friend class TcpTransportFactory;
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_REQUESTBALANCINGSTRATEGY_H */
