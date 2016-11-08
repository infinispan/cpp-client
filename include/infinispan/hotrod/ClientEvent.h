/*
 * ClientEvent.h
 *
 *  Created on: Aug 19, 2016
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_EVENT_CLIENTEVENT_H_
#define SRC_HOTROD_IMPL_EVENT_CLIENTEVENT_H_

#include <vector>
#include <cstdint>

namespace infinispan {
namespace hotrod {
namespace event {

struct EventHeaderParams {
    uint8_t magic;
    uint64_t messageId;
    uint8_t opCode;
    uint8_t status;
};

class ClientEvent {
public:
   enum Type {
      CLIENT_CACHE_ENTRY_CREATED, CLIENT_CACHE_ENTRY_MODIFIED,
      CLIENT_CACHE_ENTRY_REMOVED, CLIENT_CACHE_ENTRY_EXPIRED, CLIENT_CACHE_ENTRY_CUSTOM,
      CLIENT_CACHE_FAILOVER
   };

   virtual uint8_t getType() = 0;
   virtual ~ClientEvent() {}
};

template <class K>
class ClientCacheEntryCreatedEvent : public ClientEvent
{
public:
	   ClientCacheEntryCreatedEvent(K key, uint64_t version, int commandRetried) : key(key), version(version), commandRetried(commandRetried!=0) {}
	   /**
	    * Created cache entry's key.
	    * @return an instance of the key with which a cache entry has been
	    * created in the remote server(s).
	    */
	   K getKey() { return key; }

	   /**
	    * Provides access to the version of the created cache entry. This version
	    * can be used to invoke conditional operations on the server, such as
	    * {@link org.infinispan.client.hotrod.RemoteCache#replaceWithVersion(Object, Object, long)}
	    * or {@link org.infinispan.client.hotrod.RemoteCache#removeWithVersion(Object, long)}
	    *
	    * @return a long containing the version of the created cache entry.
	    */
	   uint64_t getVersion() { return version; }

	   /**
	    * This will be true if the write command that caused this had to be retried
	    * again due to a topology change.  This could be a sign that this event
	    * has been duplicated or another event was dropped and replaced
	    * (eg: ModifiedEvent replaced CreateEvent)
	    *
	    * @return Whether the command that caused this event was retried
	    */
	   bool isCommandRetried() { return commandRetried; }
	   uint8_t getType() { return CLIENT_CACHE_ENTRY_CREATED; }
private:
	   const K key;
	   const uint64_t version;
	   const bool commandRetried;
};

template <class K>
class ClientCacheEntryModifiedEvent : public ClientEvent {
public:
	   ClientCacheEntryModifiedEvent(K key, uint64_t version, int commandRetried) : key(key), version(version), commandRetried(commandRetried!=0) {}

   /**
    * Modified cache entry's key.
    * @return an instance of the key with which a cache entry has been
    * modified in the remote server(s).
    */
	   K getKey() { return key; }

   /**
    * Provides access to the version of the modified cache entry. This version
    * can be used to invoke conditional operations on the server, such as
    * {@link org.infinispan.client.hotrod.RemoteCache#replaceWithVersion(Object, Object, long)}
    * or {@link org.infinispan.client.hotrod.RemoteCache#removeWithVersion(Object, long)}
    *
    * @return a long containing the version of the modified cache entry.
    */
	   uint64_t getVersion() { return version; }

   /**
    * This will be true if the write command that caused this had to be retried
    * again due to a topology change.  This could be a sign that this event
    * has been duplicated or another event was dropped and replaced
    * (eg: ModifiedEvent replaced CreateEvent)
    *
    * @return Whether the command that caused this event was retried
    */
	   bool isCommandRetried() { return commandRetried; }
   uint8_t getType() { return CLIENT_CACHE_ENTRY_MODIFIED; }
private:
	   const K key;
	   const uint64_t version;
	   const bool commandRetried;
};

template <class K>
class ClientCacheEntryExpiredEvent : public ClientEvent {
public:
	   ClientCacheEntryExpiredEvent(K key): key(key) {}
	   /**
	    * Created cache entry's key.
	    * @return an instance of the key with which a cache entry has been
	    * created in remote server.
	    */
	   K getKey() { return key; }

	   uint8_t getType() { return CLIENT_CACHE_ENTRY_EXPIRED; }
private:
   const K key;
};

template <class K>
class ClientCacheEntryRemovedEvent : ClientEvent {
public:
	   ClientCacheEntryRemovedEvent(K key, int commandRetried) : key(key), commandRetried(commandRetried!=0) {}
   /**
    * Created cache entry's key.
    * @return an instance of the key with which a cache entry has been
    * created in remote server.
    */
	   K getKey() { return key; }

   /**
    * This will be true if the write command that caused this had to be retried
    * again due to a topology change.  This could be a sign that this event
    * has been duplicated or another event was dropped and replaced
    * (eg: ModifiedEvent replaced CreateEvent)
    *
    * @return Whether the command that caused this event was retried
    */
	   bool isCommandRetried() { return commandRetried; }
   uint8_t getType() { return CLIENT_CACHE_ENTRY_REMOVED; }
private:
   const K key;
   const bool commandRetried;
};

class ClientCacheEntryCustomEvent : ClientEvent {
public:
	ClientCacheEntryCustomEvent(std::vector<char> data, int commandRetried) : data(data), commandRetried(commandRetried) {}
   /**
    * Customized event data. It can be any type as long as it can be converted
    * to binary format for shipping between the server and client.
    *
    * @return an instance of the customised event data.
    */
   std::vector<char> getEventData() { return data; }

   /**
    * This will be true if the write command that caused this had to be retried
    * again due to a topology change.  This could be a sign that this event
    * has been duplicated or another event was dropped and replaced
    * (eg: ModifiedEvent replaced CreateEvent)
    *
    * @return Whether the command that caused this event was retried
    */
   bool isCommandRetried() { return commandRetried; }
   uint8_t getType() { return CLIENT_CACHE_ENTRY_CUSTOM; }
private:
   const std::vector<char> data;
   bool commandRetried;
};

class ClientCacheFailoverEvent : ClientEvent {
	   uint8_t getType() { return CLIENT_CACHE_FAILOVER; }

};


} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_EVENT_CLIENTEVENT_H_ */
