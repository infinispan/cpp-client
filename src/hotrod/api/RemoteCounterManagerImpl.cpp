#include <hotrod/api/CountersImpl.h>
#include "infinispan/hotrod/Counters.h"
#include "hotrod/impl/RemoteCounterManagerImpl.h"
#include "hotrod/impl/operations/CounterOperations.h"
#include <random>
#include <list>
#include <functional>

/*
 * RemoteCounterManagerImpl.cpp
 *
 *  Created on: May 3, 2018
 *      Author: rigazilla
 */

#include <memory>

namespace infinispan {
namespace hotrod {

using namespace infinispan::hotrod::operations;

void RemoteCounterManagerImpl::start(std::shared_ptr<transport::TransportFactory> t, protocol::Codec* c) {
    const static char COUNTERCACHENAME[] = "org.infinispan.counter";
    started = true;
    codec = c;
    transportFactory = t;
    topology = transportFactory->createTopologyId(
            std::vector<char>(COUNTERCACHENAME, COUNTERCACHENAME + sizeof(COUNTERCACHENAME) - 1));
}
void RemoteCounterManagerImpl::stop() {
    started = false;
    codec = nullptr;
    transportFactory.reset();
}

std::shared_ptr<StrongCounter> RemoteCounterManagerImpl::getStrongCounter(std::string name) {
    auto it = counters.find(name);
    if (it == counters.end()) {
        GetCounterConfigurationOperation op(*codec, transportFactory, topology, 0, name);
        CounterConfiguration cc = op.execute();
        if (cc.getType() == CounterType::WEAK) {
            throw HotRodClientException(name + " counter is the wrong type (weak instead of strong");
        }
        std::shared_ptr<StrongCounterImpl> sc(new StrongCounterImpl(*this, name, cc));
        counters[name] = sc;
        return sc;
    }
    return std::static_pointer_cast<StrongCounterImpl>(it->second);
}

std::shared_ptr<WeakCounter> RemoteCounterManagerImpl::getWeakCounter(std::string name) {
    auto it = counters.find(name);
    if (it == counters.end()) {
        GetCounterConfigurationOperation op(*codec, transportFactory, topology, 0, name);
        CounterConfiguration cc = op.execute();
        if (cc.getType() != CounterType::WEAK) {
            throw HotRodClientException(name + " counter is the wrong type (strong instead of weak");
        }
        std::shared_ptr<WeakCounterImpl> sc(new WeakCounterImpl(*this, name, cc));
        counters[name] = sc;
        return sc;
    }
    return std::static_pointer_cast<WeakCounterImpl>(it->second);
}

bool RemoteCounterManagerImpl::defineCounter(std::string name, CounterConfiguration configuration) {
    DefineCounterOperation op(*codec, transportFactory, topology, 0, name, configuration);
    return op.execute();
}

bool RemoteCounterManagerImpl::isDefined(std::string name) {
    IsCounterDefinedOperation op(*codec, transportFactory, topology, 0, name);
    return op.execute();
}

CounterConfiguration RemoteCounterManagerImpl::getConfiguration(std::string name) {
    GetCounterConfigurationOperation op(*codec, transportFactory, topology, 0, name);
    CounterConfiguration cc = op.execute();
    return cc;
}

void RemoteCounterManagerImpl::remove(std::string name) {
    RemoveCounterOperation op(*codec, transportFactory, topology, 0, name);
    op.execute();
    auto it = counters.find(name);
    if (it != counters.end()) {
        it->second->setRemoved();
        counters.erase(name);
    }
}

std::set<std::string> RemoteCounterManagerImpl::getCounterNames() {
    GetCounterNamesOperation op(*codec, transportFactory, topology, 0);
    return op.execute();
}

static std::vector<char> generateV4UUID()
{
    std::vector<char> tmp(16);
    static std::default_random_engine e { };
    static std::uniform_int_distribution<int> d { 0, 255 };
    auto i = 0;
    for (; i < 16; i++) {
        tmp[i] = (unsigned char) d(e);
    }
    tmp[6] = (tmp[6] & 0x0F) | 0x40;
    tmp[8] = (tmp[8] & 0x3F) | 0x80;
    return tmp;
}

std::function<void()> f;

void* RemoteCounterManagerImpl::addListener(const std::string counterName, const event::CounterListener& listener) {
    if (counterDispatcher == nullptr) {
        listenerId = generateV4UUID();
        AddCounterListenerOperation op(*codec, transportFactory, topology, 0, counterName, listenerId, true);
        Transport* t = op.execute();
        if (t == nullptr) {
            return (long) nullptr;
        }
        eventTransport = t;
        counterDispatcher = listenerNotifier->addCounterListener(listenerId, std::vector<char>(),
                *eventTransport, *(Codec27*) codec, f);
    }
    else {
        if (counterDispatcher->getListeners(counterName).empty()) {
            AddCounterListenerOperation op(*codec, transportFactory, topology, 0, counterName, listenerId, false);
            Transport* t = op.execute();
            if (t == nullptr) {
                return (long) nullptr;
            }
        }
    }
    CounterListener* p = new CounterListener(listener);
    counterDispatcher->getListeners(counterName).push_back(p);
    return p;
}

void RemoteCounterManagerImpl::removeListener(const std::string counterName, const void* handler) {
    if (counterDispatcher == nullptr)
        return;
    auto listeners = counterDispatcher->getListeners(counterName);
    listeners.remove((const CounterListener*) handler);
    delete ((const CounterListener*) handler);
    if (listeners.empty()) {
        RemoveCounterListenerOperation op(*codec, transportFactory, topology, 0, counterName, listenerId);
        if (op.execute()) {
            counterDispatcher->remove(counterName);
        }
    }
    if (counterDispatcher->empty()) {
        counterDispatcher->stop();
        counterDispatcher.reset();
    }
}

}
}
