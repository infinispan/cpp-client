/*
 * ClientListenerNotifier.h
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_EVENT_CLIENTLISTENERNOTIFIER_H_
#define SRC_HOTROD_IMPL_EVENT_CLIENTLISTENERNOTIFIER_H_

#include <infinispan/hotrod/ClientListener.h>
#include "hotrod/impl/protocol/Codec.h"
#include "infinispan/hotrod/EventMarshaller.h"
#include "hotrod/impl/event/EventDispatcher.h"
#include <map>

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

namespace infinispan {
namespace hotrod {
namespace event {

class ClientListenerNotifier {
public:
	virtual ~ClientListenerNotifier();
	void addClientListener(const std::vector<char> listenerId, const ClientListener& clientListener, const std::vector<char> cacheName,  Transport& t, const Codec20& codec20, std::shared_ptr<void> operationPtr, const std::function<void()> &recoveryCallback);
	std::shared_ptr<CounterDispatcher> addCounterListener(const std::vector<char> listenerId, const std::vector<char> cacheName,  Transport& t, const Codec20& codec20, const std::function<void()> &recoveryCallback);
	void removeClientListener(const std::vector<char> listenerId);
	void startClientListener(const std::vector<char> listenerId);
//	const ClientListener& findClientListener(const std::vector<char> listenerId);
	const Transport& findClientListenerTransport(const std::vector<char> listenerId);
	static ClientListenerNotifier* create(std::shared_ptr<TransportFactory> factory);
	void failoverClientListeners(const std::vector<transport::InetSocketAddress>& failedServers);
	void releaseTransport(const std::vector<char> listenerId);
	void stop();
protected:
	ClientListenerNotifier(std::shared_ptr<TransportFactory> factory);
private:
	std::map<std::vector<char>, std::shared_ptr<GenericDispatcher>> eventDispatchers;
	std::shared_ptr<TransportFactory> transportFactory;
};

} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_EVENT_CLIENTLISTENERNOTIFIER_H_ */
