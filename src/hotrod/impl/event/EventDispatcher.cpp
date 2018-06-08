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
#include <hotrod/impl/operations/AddClientListenerOperation.h>
#include <thread>
#include <iostream>
#include <exception>

using namespace infinispan::hotrod::protocol;
namespace infinispan {
namespace hotrod {
namespace event {

using namespace infinispan::hotrod::sys;

void GenericDispatcher::stop()
{
    // This terminates the thread
    transport.release();
    waitThreadExit();
}

void GenericDispatcher::waitThreadExit()
{
    if (p_thread)
    {
        if (p_thread->joinable())
        {
            p_thread->join();
        }
    }
}

void EventDispatcher::start()
{
  p_thread.reset(new std::thread(&EventDispatcher::run, this));
}

void EventDispatcher::run() {
    while (true) {
        try {
            EventHeaderParams params = codec20.readEventHeader(transport);
            if (!(HotRodConstants::isEvent(params.opCode))) {
                // Just ignore malformed messages
                break;
            }
            std::vector<char> listId = codec20.readEventListenerId(transport);
            uint8_t isCustom = codec20.readEventIsCustomFlag(transport);
            uint8_t isRetried = codec20.readEventIsRetriedFlag(transport);
            if (isCustom != 0) {
                ClientCacheEntryCustomEvent ev = codec20.readCustomEvent(transport, isRetried);
                cl.processEvent(ev, listId, isCustom);
            } else {
                switch (params.opCode) {
                case HotRodConstants::CACHE_ENTRY_CREATED_EVENT_RESPONSE: {
                    ClientCacheEntryCreatedEvent<std::vector<char>> ev = codec20.readCreatedEvent(transport, isRetried);
                    cl.processEvent(ev, listId, isCustom);
                }
                    break;
                case HotRodConstants::CACHE_ENTRY_MODIFIED_EVENT_RESPONSE: {
                    ClientCacheEntryModifiedEvent<std::vector<char>> ev = codec20.readModifiedEvent(transport,
                            isRetried);
                    cl.processEvent(ev, listId, isCustom);
                }
                    break;
                case HotRodConstants::CACHE_ENTRY_REMOVED_EVENT_RESPONSE: {
                    ClientCacheEntryRemovedEvent<std::vector<char>> ev = codec20.readRemovedEvent(transport, isRetried);
                    cl.processEvent(ev, listId, isCustom);
                }
                    break;
                case HotRodConstants::CACHE_ENTRY_EXPIRED_EVENT_RESPONSE: {
                    if (codec20.getProtocolVersion() >= HotRodConstants::VERSION_21) { // ok codec has expired events
                        ClientCacheEntryExpiredEvent<std::vector<char>> ev =
                                ((const Codec21&) codec20).readExpiredEvent(transport);
                        cl.processEvent(ev, listId, isCustom);
                    } else {
                        ERROR("Received an Expired Entry Events but codec is %d<21", codec20.getProtocolVersion());
                    }
                }
                    break;
                }
            }
        } catch (const TransportException& ) {
            if (recoveryCallback) {
                recoveryCallback();
            }
            break;
        }
    }
}

void EventDispatcher::failOver() {
    auto op = (infinispan::hotrod::operations::AddClientListenerOperation*) operationPtr.get();
    // Add the listener to the failover servers
    op->execute();

}

void CounterDispatcher::start()
{
    p_thread.reset(new std::thread(&CounterDispatcher::run, this));
}

static CounterState encoded2OldState(uint8_t encoded) {
    switch (encoded & 0x03) {
    case 0x01:
        return LOWER_BOUND_REACHED;
    case 0x02:
        return UPPER_BOUND_REACHED;
    case 0x00:
        default:
        return VALID;
    }
}

static CounterState encoded2NewState(uint8_t encoded) {
    switch ((encoded >> 0x02) & 0x03) {
    case 0x01:
        return LOWER_BOUND_REACHED;
    case 0x02:
        return UPPER_BOUND_REACHED;
    case 0x00:
        default:
        return VALID;
    }
}

void CounterDispatcher::run() {
    while (true) {
        try {
            EventHeaderParams params = codec20.readEventHeader(transport);
            if (!(HotRodConstants::isCounterEvent(params.opCode))) {
                // Just ignore malformed messages
                break;
            }
            std::string counterName = transport.readString();
            std::vector<char> listId = codec20.readEventListenerId(transport);
            uint8_t encodedState = transport.readByte();
            long oldValue = transport.readLong();
            long newValue = transport.readLong();
            switch (params.opCode) {
            case HotRodConstants::COUNTER_EVENT_RESPONSE:
                CounterEvent ev(counterName, oldValue, encoded2OldState(encodedState), newValue,
                        encoded2NewState(encodedState));
                if (listeners.find(counterName) != listeners.end()) {
                    for (auto cl : listeners[counterName]) {
                        try {
                            cl->onUpdate(ev);
                        }
                        catch (const std::exception &) {
                            // just ignore failure on callback;
                        }
                    }
                }
                break;
            }
        } catch (const TransportException& ex) {
            if (recoveryCallback) {
                recoveryCallback();
            }
            break;
        }
    }
}

void CounterDispatcher::failOver() {

}

std::list<const CounterListener*>& CounterDispatcher::getListeners(const std::string& counterName) {
    return listeners[counterName];
}

} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */
