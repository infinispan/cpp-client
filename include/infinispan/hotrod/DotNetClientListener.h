/*
 * ClientListener.h
 *
 *  Created on: Aug 3, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_CACHECLIENTLISTENER_H_
#define INCLUDE_INFINISPAN_HOTROD_CACHECLIENTLISTENER_H_

#include "infinispan/hotrod/ClientEvent.h"
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
class DotNetClientListener
{
public:
	
	DotNetClientListener(RemoteCache<K,V>& cache) {};
	virtual void processEvent(ClientCacheEntryCreatedEvent<std::vector<char>> marshEv, std::vector<char >listId, uint8_t isCustom) const
	{
		if (createdEventListener)
		    createdEventListener(marshEv);
	}

	virtual void processEvent(ClientCacheEntryModifiedEvent<std::vector<char>> marshEv, std::vector<char >listId, uint8_t isCustom) const
	{
        if (modifiedEventListener)
            modifiedEventListener(marshEv);
	}

	virtual void processEvent(ClientCacheEntryRemovedEvent<std::vector<char>> marshEv, std::vector<char >listId, uint8_t isCustom) const
	{
        if (removedEventListener)
            removedEventListener(marshEv);
	}
	virtual void processEvent(ClientCacheEntryCustomEvent ev, std::vector<char >listId, uint8_t isCustom) const
	{
        if (customEventListener)
            customEventListener(ev);
	}
	virtual ~DotNetClientListener() {};
private:
	typedef void (__stdcall *MessageHandler)(void *);
public:
	void(*createdEventListener)(ClientCacheEntryCreatedEvent<std::vector<char> >) = nullptr;
    void(*expiredEventListener)(ClientCacheEntryExpiredEvent<std::vector<char> >) = nullptr;
    void(*modifiedEventListener)(ClientCacheEntryModifiedEvent<std::vector<char> >) = nullptr;
    void(*removedEventListener)(ClientCacheEntryRemovedEvent<std::vector<char> >) = nullptr;
    void(*customEventListener)(ClientCacheEntryCustomEvent) = nullptr;
    bool haveDotNetGuest = false;
};

}}}


#endif /* INCLUDE_INFINISPAN_HOTROD_CACHECLIENTLISTENER_H_ */
