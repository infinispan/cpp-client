/*
 * ClientListener.h
 *
 *  Created on: Aug 3, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_CLIENTLISTENER_H_
#define INCLUDE_INFINISPAN_HOTROD_CLIENTLISTENER_H_

#include "infinispan/hotrod/ClientEvent.h"
#include "infinispan/hotrod/ImportExport.h"
#include <vector>
#include <list>
#include <functional>

using namespace infinispan::hotrod;

namespace infinispan {
namespace hotrod {

namespace transport {
class Transport;
}

namespace protocol {
class Codec20;
}
class RemoteCacheBase;
template <class K, class V>
class RemoteCache;

namespace event {

class ClientListener
{
public:
	bool includeCurrentState = false;
	std::vector<char> filterFactoryName;
	std::vector<char> converterFactoryName;
	bool useRawData = false ;
    unsigned char interestFlag=0;

    void setInterestFlag(unsigned char flag) { interestFlag=flag; }
	virtual void processEvent(ClientCacheEntryCreatedEvent<std::vector<char> >, std::vector<char >listId, uint8_t isCustom) const = 0;
	virtual void processEvent(ClientCacheEntryModifiedEvent<std::vector<char> >, std::vector<char >listId, uint8_t isCustom) const = 0;
	virtual void processEvent(ClientCacheEntryRemovedEvent<std::vector<char> >, std::vector<char >listId, uint8_t isCustom) const = 0;
    virtual void processEvent(ClientCacheEntryExpiredEvent<std::vector<char> >, std::vector<char >listId, uint8_t isCustom) const = 0;
	virtual void processEvent(ClientCacheEntryCustomEvent, std::vector<char >listId, uint8_t isCustom) const = 0;
	virtual void processFailoverEvent() const
	{
		for (auto callable : failoverCallbacks) {
			callable();
		}
	}
	void add_listener(std::function<void()> callback) {
		failoverCallbacks.push_back(callback);
	}

	virtual ~ClientListener() {}

	const std::vector<char>& getListenerId() const {
		return listenerId;
	}

	void setListenerId(const std::vector<char>& listenerId) {
		this->listenerId = listenerId;
	}

private:
	std::list<std::function<void()> > failoverCallbacks;
	std::vector<char> listenerId;
};

}}}


#endif /* INCLUDE_INFINISPAN_HOTROD_CLIENTLISTENER_H_ */
