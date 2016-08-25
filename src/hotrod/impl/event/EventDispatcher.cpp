/*
 * EventDispatcher.cpp
 *
 *  Created on: Aug 19, 2016
 *      Author: rigazilla
 */

#include "hotrod/impl/event/EventDispatcher.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include <thread>
#include <iostream>
#include <exception>

using namespace infinispan::hotrod::protocol;
namespace infinispan {
namespace hotrod {
namespace event {

void EventDispatcher::start()
{
  p_thread.reset(new std::thread(&EventDispatcher::run, this));
}

void EventDispatcher::stop()
{
   // This terminates the thread
//   std::cout << "EventDispatcher::stop()" << std::endl;
//   transport.release();
//   std::cout << "EventDispatcher::stop() end" << std::endl;
}

void EventDispatcher::run() {
	  try {
		while (1) {
			uint64_t messageId;
			EventHeaderParams params = codec20.readEventHeader(transport);
			if (!(HotRodConstants::isEvent(params.opCode))) {
				// TODO handle error in some way
				break;
			}
			std::vector<char> listId = codec20.readEventListenerId(transport);
			uint8_t isCustom = codec20.readEventIsCustomFlag(transport);
			uint8_t isRetried = codec20.readEventIsRetriedFlag(transport);
			if (isCustom != 0) {
		    	ClientCacheEntryCustomEvent ev = codec20.readCustomEvent(transport);
		    	cl.processEvent(ev, listId, isCustom);
			} else {
				switch (params.opCode) {
				case HotRodConstants::CACHE_ENTRY_CREATED_EVENT_RESPONSE: {
					ClientCacheEntryCreatedEvent<std::vector<char>> ev =
							codec20.readCreatedEvent(transport, isRetried);
					cl.processEvent(ev, listId, isCustom);
				}
					break;
				case HotRodConstants::CACHE_ENTRY_MODIFIED_EVENT_RESPONSE: {
					ClientCacheEntryModifiedEvent<std::vector<char>> ev =
							codec20.readModifiedEvent(transport, isRetried);
					cl.processEvent(ev, listId, isCustom);
				}
					break;
				case HotRodConstants::CACHE_ENTRY_REMOVED_EVENT_RESPONSE: {
					ClientCacheEntryRemovedEvent<std::vector<char>> ev =
							codec20.readRemovedEvent(transport, isRetried);
					cl.processEvent(ev, listId, isCustom);
				}
					break;
				}
			}
		}
	} catch (std::exception& ex) {
	}
}
} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */
