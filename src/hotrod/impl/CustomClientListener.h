/*
 * CustomClientListener.h
 *
 *  Created on: Nov 29, 2016
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_CUSTOMCLIENTLISTENER_H_
#define SRC_HOTROD_IMPL_CUSTOMCLIENTLISTENER_H_

#include <functional>
#include <vector>
#include <list>

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

class CustomClientListener : public ClientListener
{
public:
    void add_listener(std::function<void(ClientCacheEntryCreatedEvent<std::vector<char> >)> callback) {
        createdCallbacks.push_back(callback);
    }
    void add_listener(std::function<void(ClientCacheEntryExpiredEvent<std::vector<char> >)> callback) {
        expiredCallbacks.push_back(callback);
    }
    void add_listener(std::function<void(ClientCacheEntryModifiedEvent<std::vector<char> >)> callback) {
        modifiedCallbacks.push_back(callback);
    }
    void add_listener(std::function<void(ClientCacheEntryRemovedEvent<std::vector<char> >)> callback) {
        removedCallbacks.push_back(callback);
    }
    void add_listener(std::function<void(ClientCacheEntryCustomEvent)> callback) {
        customCallbacks.push_back(callback);
    }



    virtual void processEvent(ClientCacheEntryCreatedEvent<std::vector<char>> marshEv, std::vector<char >listId, uint8_t isCustom) const
    {
          for (auto callable: createdCallbacks)
          {
              callable(marshEv);
          }
    }

    virtual void processEvent(ClientCacheEntryModifiedEvent<std::vector<char>> marshEv, std::vector<char >listId, uint8_t isCustom) const
    {
          for (auto callable: modifiedCallbacks)
          {
              callable(marshEv);
          }
    }

    virtual void processEvent(ClientCacheEntryRemovedEvent<std::vector<char>> marshEv, std::vector<char >listId, uint8_t isCustom) const
    {
          for (auto callable: removedCallbacks)
          {
              callable(marshEv);
          }
    }

    virtual void processEvent(ClientCacheEntryExpiredEvent<std::vector<char>> marshEv, std::vector<char >listId, uint8_t isCustom) const
    {
          for (auto callable: expiredCallbacks)
          {
              callable(marshEv);
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
    std::list<std::function<void(ClientCacheEntryCreatedEvent<std::vector<char>>)>> createdCallbacks;
    std::list<std::function<void(ClientCacheEntryExpiredEvent<std::vector<char>>)>> expiredCallbacks;
    std::list<std::function<void(ClientCacheEntryModifiedEvent<std::vector<char>>)>> modifiedCallbacks;
    std::list<std::function<void(ClientCacheEntryRemovedEvent<std::vector<char>>)>> removedCallbacks;
    std::list<std::function<void(ClientCacheEntryCustomEvent)>> customCallbacks;
};



}}}

#endif /* SRC_HOTROD_IMPL_CUSTOMCLIENTLISTENER_H_ */
