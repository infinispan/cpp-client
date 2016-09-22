/*
 * ClientListenerNotifier.cpp
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#include <hotrod/impl/event/ClientListenerNotifier.h>
#include <hotrod/impl/operations/AddClientListenerOperation.h>
#include "hotrod/impl/event/EventDispatcher.h"
#include <vector>


#include "infinispan/hotrod/InetSocketAddress.h"
#include <vector>


using namespace infinispan::hotrod::operations;

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

void ClientListenerNotifier::addClientListener(const std::vector<char> listenerId, const ClientListener& clientListener, const std::vector<char> cacheName, Transport& t, const Codec20& codec20, void* operationPtr, const std::function<void()> &recoveryCallback)
{
	EventDispatcher ed(listenerId, clientListener, cacheName, t, codec20, operationPtr, recoveryCallback);
	eventDispatchers.insert(std::make_pair(listenerId, ed));
}

void ClientListenerNotifier::failoverClientListeners(const std::vector<transport::InetSocketAddress>& failedServers)
{
	for (auto server: failedServers)
	{
		for (auto &edPair : eventDispatchers)
		{
			if (edPair.second.getTransport().targets(server))
			{
				//removeClientListener(edPair.second.listenerId);
//				edPair.second.cl.processFailoverEvent();
				auto op = (AddClientListenerOperation*)edPair.second.operationPtr;
				// Add the listener to the failover servers
				op->execute();
			}
		}
	}
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

const ClientListener& ClientListenerNotifier::findClientListener(const std::vector<char> listenerId)
{
    auto it = eventDispatchers.find(listenerId);
    if (it!=eventDispatchers.end())
    	return it->second.cl;
    throw HotRodClientException("Internal: client listener not found");
}

const Transport& ClientListenerNotifier::findClientListenerTransport(const std::vector<char> listenerId)
{
    auto it = eventDispatchers.find(listenerId);
    if (it!=eventDispatchers.end())
    	return it->second.getTransport();
    throw HotRodClientException("Internal: client listener not found");
}



} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */
