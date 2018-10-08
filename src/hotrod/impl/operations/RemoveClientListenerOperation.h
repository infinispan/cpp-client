/*
 * RemoveClientListenerOperation.h
 *
 *  Created on: Sep 21, 2016
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_OPERATIONS_REMOVECLIENTLISTENEROPERATION_H_
#define SRC_HOTROD_IMPL_OPERATIONS_REMOVECLIENTLISTENEROPERATION_H_

#include <hotrod/impl/operations/RetryOnFailureOperation.h>

namespace infinispan {
namespace hotrod {
namespace operations {

class RemoveClientListenerOperation: public RetryOnFailureOperation<char> {
public:
	RemoveClientListenerOperation(const Codec &codec, std::shared_ptr<TransportFactory> transportFactory,
                             std::vector<char> cacheName, Topology& topologyId, int flags,
                             ClientListenerNotifier &listenerNotifier, const ClientListener& clientListener, EntryMediaTypes* df)
                           : RetryOnFailureOperation<char>(codec, transportFactory, cacheName, topologyId, flags, df), listenerNotifier(listenerNotifier), clientListener(clientListener)
							 {};
    virtual void releaseTransport(transport::Transport* transport);
    virtual transport::Transport& getTransport(int retryCount, const std::set<transport::InetSocketAddress>& failedServers);
	virtual char executeOperation(transport::Transport& transport);
    ClientListenerNotifier& listenerNotifier;
    const ClientListener& clientListener;
    friend class OperationsFactory;
};

} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_OPERATIONS_REMOVECLIENTLISTENEROPERATION_H_ */
