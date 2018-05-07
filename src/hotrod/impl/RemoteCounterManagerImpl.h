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
    RemoteCounterManagerImpl() :
            transportFactory(), codec(nullptr), started(false) {
    }
    ~RemoteCounterManagerImpl() {
    }
    ;

    void start(std::shared_ptr<transport::TransportFactory> t, protocol::Codec* c);
    void stop();

    std::shared_ptr<StrongCounter> getStrongCounter(std::string name);
    std::shared_ptr<WeakCounter> getWeakCounter(std::string name);
    bool defineCounter(std::string name, CounterConfiguration configuration);
    bool isDefined(std::string name);
    CounterConfiguration getConfiguration(std::string name);
    void remove(std::string name);
    std::set<std::string> getCounterNames();

private:
    std::shared_ptr<transport::TransportFactory> transportFactory;
    protocol::Codec* codec;
    bool started;
    Topology topology;
    std::map<std::string, std::shared_ptr<BaseCounterImpl>> counters;
    friend BaseCounterImpl;
    friend StrongCounterImpl;
    friend WeakCounterImpl;
};
}
}

#endif /* SRC_HOTROD_IMPL_REMOTECOUNTERMANAGERIMPL_H_ */
