#ifndef ISPN_HOTROD_OPERATIONS_GETALLOPERATION_H
#define ISPN_HOTROD_OPERATIONS_GETALLOPERATION_H


#include "infinispan/hotrod/RemoteCacheBase.h"
#include "hotrod/impl/operations/RetryOnFailureOperation.h"

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

class GetAllOperation : public RetryOnFailureOperation<std::map<std::vector<char>, std::vector<char> > >
{
  protected:
    std::map<std::vector<char>, std::vector<char>> executeOperation(
        infinispan::hotrod::transport::Transport& transport);

  private:
    GetAllOperation(const Codec& _codec, std::shared_ptr<transport::TransportFactory> _transportFactory, const std::set<std::vector<char>>& keySet,
    	    const std::vector<char>& _cacheName, Topology& _topologyId, uint32_t _flags);
    virtual transport::Transport& getTransport(int /*retryCount*/, const std::set<transport::InetSocketAddress>& failedServers)
    {
            return transportFactory->getTransport(*keySet.begin(), this->cacheName, failedServers);
    }
    const std::set<std::vector<char>>& keySet;
  friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif  // ISPN_HOTROD_OPERATIONS_GETALLOPERATION_H
