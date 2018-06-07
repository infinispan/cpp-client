/*
 * RemoteCounterManagerImpl.h
 *
 *  Created on: May 4, 2018
 *      Author: rigazilla
 */

#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/protocol/Codec.h"
#include "hotrod/impl/Topology.h"
#include "infinispan/hotrod/RemoteCounterManager.h"
#include <memory>
#include <string>
#include <map>

#ifndef SRC_HOTROD_IMPL_REMOTECOUNTERMANAGERIMPL_H_
#define SRC_HOTROD_IMPL_REMOTECOUNTERMANAGERIMPL_H_

namespace infinispan {
namespace hotrod {

class BaseCounterImpl;
class StrongCounterImpl;
class WeakCounterImpl;

class RemoteCounterManagerImpl: public RemoteCounterManager {
public:
    RemoteCounterManagerImpl(std::shared_ptr<ClientListenerNotifier>& listenerNotifier) :
            transportFactory(), codec(nullptr), started(false), listenerNotifier(listenerNotifier), eventTransport(
                    nullptr) {
    }
    ~RemoteCounterManagerImpl() {
    }

    void start(std::shared_ptr<transport::TransportFactory> t, protocol::Codec* c);
    void stop();

    std::shared_ptr<StrongCounter> getStrongCounter(std::string name);
    std::shared_ptr<WeakCounter> getWeakCounter(std::string name);
    bool defineCounter(std::string name, CounterConfiguration configuration);
    bool isDefined(std::string name);
    CounterConfiguration getConfiguration(std::string name);
    void remove(std::string name);
    std::set<std::string> getCounterNames();
    const void* addListener(const std::string counterName, const event::CounterListener* listener);
    void removeListener(const std::string counterName, const void* handler);

private:
    std::shared_ptr<transport::TransportFactory> transportFactory;
    protocol::Codec* codec;
    bool started;
    std::shared_ptr<ClientListenerNotifier>& listenerNotifier;
    Topology topology;
    std::map<std::string, std::shared_ptr<BaseCounterImpl>> counters;
    std::shared_ptr<CounterDispatcher> counterDispatcher;
    Transport* eventTransport;
    std::vector<char> listenerId;
    friend BaseCounterImpl;
    friend StrongCounterImpl;
    friend WeakCounterImpl;
};
}
}

#endif /* SRC_HOTROD_IMPL_REMOTECOUNTERMANAGERIMPL_H_ */
