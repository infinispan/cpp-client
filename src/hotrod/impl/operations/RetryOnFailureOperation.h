#ifndef ISPN_HOTROD_OPERATIONS_RETRYONFAILUREOPERATION_H
#define ISPN_HOTROD_OPERATIONS_RETRYONFAILUREOPERATION_H


#include <infinispan/hotrod/InetSocketAddress.h>
#include "infinispan/hotrod/exceptions.h"
#include "hotrod/impl/operations/HotRodOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"


namespace infinispan {
namespace hotrod {
namespace operations {

template<class T> class RetryOnFailureOperation : public HotRodOperation<T>
{
  public:
    T execute() {
        int retryCount = 0;
        std::set<transport::InetSocketAddress> failedServers;
        while (shouldRetry(retryCount)) {
            transport::Transport* transport = NULL;
            try {
                // Transport retrieval should be retried
                transport = &getTransport(retryCount, failedServers);
                const T& result = executeOperation(*transport);
                releaseTransport(transport);
                return result;
            } catch (const TransportException& te) {
                // Invalidate transport since this exception means that this
                // instance is no longer usable and should be destroyed
            	transport::InetSocketAddress isa(te.getHostCString(),te.getPort());
            	failedServers.insert(isa);
                transportFactory->invalidateTransport(isa, transport);
                logErrorAndThrowExceptionIfNeeded(retryCount, te);
            } catch (const RemoteNodeSuspectException& rnse) {
                // Do not invalidate transport because this exception is caused
                // as a result of a server finding out that another node has
                // been suspected, so there's nothing really wrong with the server
                // from which this node was received
                releaseTransport(transport);
                logErrorAndThrowExceptionIfNeeded(retryCount, rnse);
            }
            ++retryCount;
        }
        throw InternalException("We should not reach here!");
    }

  protected:
    RetryOnFailureOperation(
        const protocol::Codec& _codec,
        std::shared_ptr<transport::TransportFactory> _transportFactory,
        const std::vector<char>& _cacheName, Topology& _topologyId, uint32_t _flags) :
            HotRodOperation<T>(_codec, _flags, _cacheName, _topologyId),
            transportFactory(_transportFactory) {}

    bool shouldRetry(int retryCount) {
       return retryCount <= transportFactory->getMaxRetries();
    }

    virtual void releaseTransport(transport::Transport* transport) {
        if (transport) {
            transportFactory->releaseTransport(*transport);
        }
    }

    void logErrorAndThrowExceptionIfNeeded(int& retryCount, const HotRodClientException& e) {
        if (retryCount >= transportFactory->getMaxRetries() - 1) {
        	if (transportFactory->switchOnFailoverCluster() == ClusterStatus::SWITCHED)
        	{
        		retryCount=0; // reset retry counter
        	}
        	else
        	{
            	ERROR("Exception encountered, retry %d of %d: %s",
            			retryCount, transportFactory->getMaxRetries(), e.what());
            	throw; // Rethrow. The exception is rethrown as const!
        	}
        } else {
            TRACE("Exception encountered, retry %d of %d: %s",
                retryCount, transportFactory->getMaxRetries(), e.what());
        }
    }

    virtual transport::Transport& getTransport(int /*retryCount*/, const std::set<transport::InetSocketAddress>& failedServers)
    {
            return transportFactory->getTransport(this->cacheName, failedServers);
    }

    virtual T executeOperation(transport::Transport& transport) = 0;

    virtual ~RetryOnFailureOperation() {}

    std::shared_ptr<infinispan::hotrod::transport::TransportFactory> transportFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_RETRYONFAILUREOPERATION_H
