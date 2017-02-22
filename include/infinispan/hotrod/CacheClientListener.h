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
    void add_listener(std::function<void(ClientCacheEntryExpiredEvent<K>)> callback) {
        expiredCallbacks.push_back(callback);
    }
    void add_listener(std::function<void(ClientCacheEntryModifiedEvent<K>)> callback) {
        modifiedCallbacks.push_back(callback);
    }
    void add_listener(std::function<void(ClientCacheEntryRemovedEvent<K>)> callback) {
        removedCallbacks.push_back(callback);
    }
    void add_listener(std::function<void(ClientCacheEntryCustomEvent)> callback) {
        customCallbacks.push_back(callback);
    }

    virtual void processEvent(ClientCacheEntryCreatedEvent<std::vector<char>> marshEv, std::vector<char >listId, uint8_t isCustom) const
    {
        K* key= (K*)cache.baseKeyUnmarshall(marshEv.getKey());
        ClientCacheEntryCreatedEvent<K> typedEvent(*key, marshEv.getVersion(), marshEv.isCommandRetried());
        delete key;
        for (auto callable: createdCallbacks)
        {
          callable(typedEvent);
        }
    }

    virtual void processEvent(ClientCacheEntryModifiedEvent<std::vector<char>> marshEv, std::vector<char >listId, uint8_t isCustom) const
    {
        K* key= (K*)cache.baseKeyUnmarshall(marshEv.getKey());
        ClientCacheEntryModifiedEvent<K> typedEvent(*key, marshEv.getVersion(), marshEv.isCommandRetried());
        delete key;
        for (auto callable: modifiedCallbacks)
        {
          callable(typedEvent);
        }
    }

    virtual void processEvent(ClientCacheEntryRemovedEvent<std::vector<char>> marshEv, std::vector<char >listId, uint8_t isCustom) const
    {
        K* key= (K*)cache.baseKeyUnmarshall(marshEv.getKey());
        ClientCacheEntryRemovedEvent<K> typedEvent(*key, marshEv.isCommandRetried());
        delete key;
        for (auto callable: removedCallbacks)
        {
            callable(typedEvent);
        }
    }

    virtual void processEvent(ClientCacheEntryExpiredEvent<std::vector<char>> marshEv, std::vector<char >listId, uint8_t isCustom) const
    {
        K* key= (K*)cache.baseKeyUnmarshall(marshEv.getKey());
        ClientCacheEntryExpiredEvent<K> typedEvent(*key);
        delete key;
        for (auto callable: expiredCallbacks)
        {
            callable(typedEvent);
        }
    }

    virtual void processEvent(ClientCacheEntryCustomEvent ev, std::vector<char >listId, uint8_t isCustom) const
    {
        for (auto callable: customCallbacks)
        {
          callable(ev);
        }
    }
private:
    std::list<std::function<void(ClientCacheEntryCreatedEvent<K>)>> createdCallbacks;
    std::list<std::function<void(ClientCacheEntryExpiredEvent<K>)>> expiredCallbacks;
    std::list<std::function<void(ClientCacheEntryModifiedEvent<K>)>> modifiedCallbacks;
    std::list<std::function<void(ClientCacheEntryRemovedEvent<K>)>> removedCallbacks;
    std::list<std::function<void(ClientCacheEntryCustomEvent)>> customCallbacks;
};

}}}


#endif /* INCLUDE_INFINISPAN_HOTROD_CACHECLIENTLISTENER_H_ */
