/*
 * EventDispatcher.h
 *
 *  Created on: Aug 19, 2016
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_EVENT_EVENTDISPATCHER_H_
#define SRC_HOTROD_IMPL_EVENT_EVENTDISPATCHER_H_

#include "hotrod/impl/event/ClientEvent.h"
#include "infinispan/hotrod/ClientListener.h"
#include "hotrod/impl/protocol/Codec20.h"
#include "hotrod/impl/transport/Transport.h"
#include <memory>
#include <functional>
#include <map>
#include <tuple>
#include <thread>
#include <iostream>

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

namespace infinispan {
namespace hotrod {
namespace event {


template <class T> using X =  std::function<void(T)>;

class EventDispatcher {
public:
	EventDispatcher(const std::vector<char> listenerId, const ClientListener& cl, std::vector<char> cacheName, Transport& t, const Codec20& codec20, void* addClientListenerOpPtr) : listenerId(listenerId), cl(cl), operationPtr(addClientListenerOpPtr), cacheName(cacheName), transport(t), codec20(codec20)
    {}
	virtual ~EventDispatcher() { std::cout << "~EventDispatcher()" << std::endl;};
	const Transport& getTransport() {
		return transport;
	}
    void run();
    void start();
    void stop();
	const std::vector<char> listenerId;
	const ClientListener& cl;
	void* const operationPtr;

private:
	std::vector<char> cacheName;
	Transport& transport;
	const Codec20& codec20;
	std::shared_ptr<std::thread> p_thread;
};

} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_EVENT_EVENTDISPATCHER_H_ */
