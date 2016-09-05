/*
 * EventDispatcher.cpp
 *
 *  Created on: Aug 19, 2016
 *      Author: rigazilla
 */

#include "hotrod/impl/event/EventDispatcher.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "infinispan/hotrod/exceptions.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
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
	int retryCount = 0;
	while (true) {
		try {
			while (1) {
				//uint64_t messageId;
				std::cout << "run(" << this << "); while" << std::endl;
				status = 1;
				std::cout << "reading(" << &transport << ")" << std::endl;
				std::cout << "transport is valid = " << (dynamic_cast<TcpTransport&>(transport).isValid() ? 1 : 0) << std::endl;
				EventHeaderParams params = codec20.readEventHeader(transport);
				status = 2;
				std::cout << "run(); read" << std::endl;
				if (!(HotRodConstants::isEvent(params.opCode))) {
					// TODO handle error in some way
					break;
				}
				std::vector<char> listId = codec20.readEventListenerId(
						transport);
				uint8_t isCustom = codec20.readEventIsCustomFlag(transport);
				uint8_t isRetried = codec20.readEventIsRetriedFlag(transport);
				status = 3;
				if (isCustom != 0) {
					ClientCacheEntryCustomEvent ev = codec20.readCustomEvent(
							transport);
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
		} catch (TransportException& ex) {

			std::cout << "EXCEPTION: " << ex.getErrnum() << " : " << ex.what() << std::endl;
			if (ex.getErrnum() == EINTR) {
				// count a retry
				bool b = dynamic_cast<TcpTransport&>(transport).isValid();
				if (retryCount++ <= 2 && b)
					continue;
			}
			if (*ex.what() == 'r') {
				std::cout << "r" << ex.what() << std::endl;
			}
			transport.setValid(false);

			status = 99;
			if (recoveryCallback) {
				recoveryCallback();
			}
			break;
		}
	}
}
} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */
