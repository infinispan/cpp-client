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
#include "infinispan/hotrod/CounterEvent.h"
#include "hotrod/impl/protocol/Codec21.h"
#include "hotrod/impl/protocol/Codec27.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include <memory>
#include <functional>
#include <map>
#include <tuple>
#include <thread>
#include <iostream>
#include <list>

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

namespace infinispan {
namespace hotrod {
namespace event {


template <class T> using X =  std::function<void(T)>;
class GenericDispatcher {
public:
    GenericDispatcher(const std::vector<char> listenerId, std::vector<char> cacheName, Transport &t, const std::function<void()> &recoveryCallback) : listenerId(listenerId), cacheName(cacheName), transport(t), recoveryCallback(recoveryCallback)
    {}
    virtual ~GenericDispatcher() {
        waitThreadExit();
    }
    Transport& getTransport() {
        return transport;
    }
    virtual void run() = 0;
    virtual void failOver() = 0;
    virtual void start() = 0;
    void stop();
    void waitThreadExit();
    const std::vector<char> listenerId;

protected:
    std::vector<char> cacheName;
    Transport &transport;
    std::shared_ptr<std::thread> p_thread;
    const std::function<void()> &recoveryCallback;
};


class EventDispatcher: public GenericDispatcher {
public:
    EventDispatcher(const std::vector<char> listenerId, const ClientListener& cl, std::vector<char> cacheName,
            Transport &t, const Codec20& codec20, std::shared_ptr<void> addClientListenerOpPtr,
            const std::function<void()> &recoveryCallback) :
            GenericDispatcher(listenerId, cacheName, t, recoveryCallback), cl(cl), operationPtr(addClientListenerOpPtr), codec20(
                    codec20)
    {
    }
    virtual void run();
    virtual void failOver();
    virtual void start();
    const ClientListener& cl;
    const std::shared_ptr<void> operationPtr;
    const Codec20& codec20;
};

class CounterDispatcher: public GenericDispatcher {
public:
    CounterDispatcher(const std::vector<char> listenerId, std::vector<char> cacheName, Transport &t,
            const Codec20& codec20, const std::function<void()> &recoveryCallback) :
            GenericDispatcher(listenerId, cacheName, t, recoveryCallback), codec20(codec20)
    {
    }
    virtual void run();
    virtual void failOver();
    virtual void start();
    std::list<const CounterListener*>& getListeners(const std::string& counterName);
    bool empty() {
        return listeners.empty();
    }
    void remove(const std::string& counterName) {
        listeners.erase(counterName);
    }
    std::map<const std::string, std::list<const CounterListener*> > listeners;
    const Codec20& codec20;
};


} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_EVENT_EVENTDISPATCHER_H_ */
