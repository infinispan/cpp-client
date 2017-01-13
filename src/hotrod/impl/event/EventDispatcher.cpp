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
#include "hotrod/sys/Log.h"
#include <thread>
#include <iostream>
#include <exception>

using namespace infinispan::hotrod::protocol;
namespace infinispan {
namespace hotrod {
namespace event {

using namespace infinispan::hotrod::sys;

void EventDispatcher::start()
{
  p_thread.reset(new std::thread(&EventDispatcher::run, this));
}

void EventDispatcher::stop()
{
   // This terminates the thread
   transport.release();
}

void EventDispatcher::run() {
	int retryCount = 0;
	while (true) {
		try {
			while (1) {
				EventHeaderParams params = codec20.readEventHeader(transport);
				if (!(HotRodConstants::isEvent(params.opCode))) {
					// TODO handle error in some way
					break;
				}
				std::vector<char> listId = codec20.readEventListenerId(transport);
				uint8_t isCustom = codec20.readEventIsCustomFlag(transport);
				uint8_t isRetried = codec20.readEventIsRetriedFlag(transport);
				if (isCustom != 0) {
					ClientCacheEntryCustomEvent ev = codec20.readCustomEvent(
							transport, isRetried);
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
                    case HotRodConstants::CACHE_ENTRY_EXPIRED_EVENT_RESPONSE: {
                        if (codec20.getProtocolVersion() >= HotRodConstants::VERSION_21)
                        { // ok codec has expired events
                            ClientCacheEntryExpiredEvent<std::vector<char>> ev =
                                    ((const Codec21&)codec20).readExpiredEvent(transport);
                            cl.processEvent(ev, listId, isCustom);
                        }
                        else
                        {
                            ERROR("Received an Expired Entry Events but codec is %d<21",codec20.getProtocolVersion());
                        }
                    }
                        break;
					}
				}
			}
		} catch (TransportException& ex) {

			if (ex.getErrnum() == EINTR) {
				// count a retry
				bool b = dynamic_cast<TcpTransport&>(transport).isValid();
				if (retryCount++ <= 1 && b)
					continue;
			}
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
