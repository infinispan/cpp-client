/*
 * ClientListenerNotifier.cpp
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#include <hotrod/impl/event/ClientListenerNotifier.h>
#include "hotrod/impl/event/EventDispatcher.h"
#include <vector>


namespace infinispan {
namespace hotrod {
namespace event {

ClientListenerNotifier::ClientListenerNotifier() {
	// TODO Auto-generated constructor stub

}

ClientListenerNotifier::~ClientListenerNotifier() {
	// TODO Auto-generated destructor stub
}

ClientListenerNotifier* ClientListenerNotifier::create()
{
	return new ClientListenerNotifier();
}

void ClientListenerNotifier::addClientListener(const std::vector<char> listenerId, const ClientListener& clientListener, const std::vector<char> cacheName, Transport& t, const Codec20& codec20)
{
	EventDispatcher ed(listenerId, clientListener, cacheName, t, codec20);
	eventDispatchers.insert(std::make_pair(listenerId, ed));
}

void ClientListenerNotifier::stop()
{
	for(auto& kv : eventDispatchers)
	{
		kv.second.stop();
	}
}

void ClientListenerNotifier::removeClientListener(const std::vector<char> listenerId)
{
	eventDispatchers.erase(listenerId);
}

void ClientListenerNotifier::startClientListener(const std::vector<char> listenerId)
{
      auto it = eventDispatchers.find(listenerId);
      if (it != eventDispatchers.end())
    	  it->second.start();
}


} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */
