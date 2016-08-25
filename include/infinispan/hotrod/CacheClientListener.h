/*
 * ClientListener.h
 *
 *  Created on: Aug 3, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_CACHECLIENTLISTENER_H_
#define INCLUDE_INFINISPAN_HOTROD_CACHECLIENTLISTENER_H_

#include <hotrod/impl/event/ClientEvent.h>
#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/ClientListener.h"
#include "infinispan/hotrod/RemoteCacheBase.h"
#include "hotrod/impl/transport/Transport.h"
#include <vector>
#include <list>
#include <functional>

using namespace infinispan::hotrod;

namespace infinispan {
namespace hotrod {

namespace protocol {
class Codec20;
}
template <class K, class V>
class RemoteCache;

namespace event {

template <class K, class V>
class CacheClientListener : public ClientListener
{
public:
	RemoteCacheBase& cache;
	
	CacheClientListener(RemoteCache<K,V>& cache) : cache((RemoteCacheBase&)cache) {};
	void add_listener(std::function<void(ClientCacheEntryCreatedEvent<K>)> callback) {
		createdCallbacks.push_back(callback);
	}
	void add_listener(std::function<void(ClientCacheEntryExpiredEvent)> callback) {
		expiredCallbacks.push_back(callback);
	}
	void add_listener(std::function<void(ClientCacheEntryModifiedEvent)> callback) {
		modifiedCallbacks.push_back(callback);
	}
	void add_listener(std::function<void(ClientCacheEntryRemovedEvent)> callback) {
		removedCallbacks.push_back(callback);
	}
	void add_listener(std::function<void(ClientCacheEntryCustomEvent)> callback) {
		customCallbacks.push_back(callback);
	}
	void add_listener(std::function<void(ClientCacheFailoverEvent)> callback) {
		failoverCallbacks.push_back(callback);
	}

	virtual void processEvent(ClientCacheEntryCreatedEvent<std::vector<char>> marshEv, std::vector<char >listId, uint8_t isCustom) const
	{
		ClientCacheEntryCreatedEvent<K> typedEvent(*(K*)cache.baseKeyUnmarshall(marshEv.getKey()), marshEv.getVersion(), marshEv.isCommandRetried());
		  for (auto callable: createdCallbacks)
		  {
			  callable(typedEvent);
		  }
	}


	virtual void processEvent(ClientEvent::Type t, std::vector<char >listId, uint8_t isCustom, uint8_t isRetried, transport::Transport& transport, const protocol::Codec20& codec20) const
	{
	  switch (t)
	  {
	  case ClientEvent::Type::CLIENT_CACHE_ENTRY_CREATED:
	  {
		  K* key = (K*)cache.baseKeyUnmarshall(transport.readArray());
          int64_t version = transport.readLong();
		  ClientCacheEntryCreatedEvent<K> ev(*key, version, isRetried);
		  for (auto callable: createdCallbacks)
		  {
			  callable(ev);
		  }
	  }
		  break;
	  default:
		  break;
	  }
	}
private:
	std::list<std::function<void(ClientCacheEntryCreatedEvent<K>)>> createdCallbacks;
	std::list<std::function<void(ClientCacheEntryExpiredEvent)>> expiredCallbacks;
	std::list<std::function<void(ClientCacheEntryModifiedEvent)>> modifiedCallbacks;
	std::list<std::function<void(ClientCacheEntryRemovedEvent)>> removedCallbacks;
	std::list<std::function<void(ClientCacheEntryCustomEvent)>> customCallbacks;
	std::list<std::function<void(ClientCacheFailoverEvent)>> failoverCallbacks;
};

}}}


#endif /* INCLUDE_INFINISPAN_HOTROD_CACHECLIENTLISTENER_H_ */
