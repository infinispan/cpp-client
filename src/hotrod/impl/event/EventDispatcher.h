/*
 * EventDispatcher.h
 *
 *  Created on: Aug 19, 2016
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_EVENT_EVENTDISPATCHER_H_
#define SRC_HOTROD_IMPL_EVENT_EVENTDISPATCHER_H_

#include "infinispan/hotrod/ClientEvent.h"
#include "infinispan/hotrod/ClientListener.h"
#include "hotrod/impl/protocol/Codec21.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include <memory>
#include <functional>
#include <map>
#include <tuple>
#include <thread>
#include <iostream>

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

namespace infinispan {
namespace hotrod {
namespace event {


template <class T> using X =  std::function<void(T)>;

class EventDispatcher {
public:
    EventDispatcher(const std::vector<char> listenerId, const ClientListener& cl, std::vector<char> cacheName, Transport &t, const Codec20& codec20, std::shared_ptr<void> addClientListenerOpPtr, const std::function<void()> &recoveryCallback) : listenerId(listenerId), cl(cl), operationPtr(addClientListenerOpPtr), cacheName(cacheName), transport(t), codec20(codec20), recoveryCallback(recoveryCallback)
    {}
    virtual ~EventDispatcher() {
        waitThreadExit();
    }
    Transport& getTransport() {
        return transport;
    }
    void run();
    void start();
    void stop();
    void waitThreadExit();
    const std::vector<char> listenerId;
    const ClientListener& cl;
    const std::shared_ptr<void> operationPtr;

private:
    std::vector<char> cacheName;
    Transport &transport;
    const Codec20& codec20;
    std::shared_ptr<std::thread> p_thread;
    const std::function<void()> &recoveryCallback;
};

} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_EVENT_EVENTDISPATCHER_H_ */
