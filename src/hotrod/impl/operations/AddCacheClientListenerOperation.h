/*
 * AddClientListenerOperation.h
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_OPERATIONS_ADDCACHECLIENTLISTENEROPERATION_H_
#define SRC_HOTROD_IMPL_OPERATIONS_ADDCACHECLIENTLISTENEROPERATION_H_

#include <hotrod/impl/event/ClientListenerNotifier.h>
#include <infinispan/hotrod/ClientListener.h>
#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/protocol/Codec.h"

using namespace infinispan::hotrod::transport;
using namespace infinispan::hotrod::event;
using namespace infinispan::hotrod::protocol;


using namespace infinispan::hotrod::event;

namespace infinispan {
namespace hotrod {
namespace operations {

class AddClientListenerOperation: public RetryOnFailureOperation<char> {
public:
	AddClientListenerOperation(const Codec &codec, std::shared_ptr<TransportFactory> transportFactory,
        std::vector<char> cacheName, Topology& topologyId, int flags,
        ClientListenerNotifier &listenerNotifier, const ClientListener& clientListener,
		std::vector<std::vector<char> > filterFactoryParams,
		std::vector<std::vector<char> > converterFactoryParams)
                           : RetryOnFailureOperation<char>(codec, transportFactory, cacheName, topologyId, flags), listenerNotifier(listenerNotifier),
							 listenerId(generateV4UUID()), clientListener(clientListener), filterFactoryParams(filterFactoryParams), converterFactoryParams(converterFactoryParams),
							 cacheName(cacheName)
							 {};
    virtual void releaseTransport(transport::Transport* transport);
    virtual transport::Transport& getTransport(int retryCount);
    transport::Transport& getDedicatedTransport();
	virtual ~AddClientListenerOperation();
	char executeOperation(transport::Transport& transport);
    ClientListenerNotifier& listenerNotifier;
	const std::vector<char> listenerId;
    const ClientListener& clientListener;
    const std::vector<std::vector<char> > filterFactoryParams;
    const std::vector<std::vector<char> > converterFactoryParams;

private:
    const std::vector<char> cacheName;
    std::vector<char> generateV4UUID();
    /**
     * Dedicated transport instance for adding client listener. This transport
     * is used to send events back to client and it's only released when the
     * client listener is removed.
     */
    Transport *dedicatedTransport;

};

} /* namespace operations */
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_OPERATIONS_ADDCACHECLIENTLISTENEROPERATION_H_ */
