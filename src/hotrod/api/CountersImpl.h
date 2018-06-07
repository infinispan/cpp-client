/*
 * RemoteCountersImpl.h
 *
 *  Created on: May 4, 2018
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_API_COUNTERSIMPL_H_
#define SRC_HOTROD_API_COUNTERSIMPL_H_

#include "infinispan/hotrod/CounterConfiguration.h"
#include "hotrod/impl/RemoteCounterManagerImpl.h"
#include <string>
#include <future>

namespace infinispan {
namespace hotrod {

class BaseCounterImpl: public virtual Counter {
public:
    BaseCounterImpl(RemoteCounterManagerImpl& rcm, std::string name, CounterConfiguration configuration) :
            rcm(rcm), name(name), configuration(configuration), removed(false) {
    }

    std::string getName() {
        return name;
    }
    CounterConfiguration getConfiguration() {
        return configuration;
    }

    void setRemoved() {
        removed = true;
    }

    void reset();

    void remove();

    virtual long getValue();

    const void* addListener(const event::CounterListener* listener);

    void removeListener(const void* handler);

    virtual ~BaseCounterImpl() {
    }

protected:
    RemoteCounterManagerImpl& rcm;
    std::string name;
    CounterConfiguration configuration;
    bool removed;
};

class StrongCounterImpl: public BaseCounterImpl, public virtual StrongCounter {
public:
    StrongCounterImpl(RemoteCounterManagerImpl& rcm, std::string name, CounterConfiguration configuration) :
            BaseCounterImpl(rcm, name, configuration) {
    }

    long getValue() {
        return BaseCounterImpl::getValue();
    }

    long addAndGet(long delta);

    long incrementAndGet() {
        return addAndGet(1L);
    }

    long decrementAndGet() {
        return addAndGet(-1L);
    }

    long compareAndSwap(long expect, long update);

    bool compareAndSet(long expect, long update) {
        return compareAndSwap(expect, update) == expect;
    }
};

class WeakCounterImpl: public BaseCounterImpl, public virtual WeakCounter {
public:
    WeakCounterImpl(RemoteCounterManagerImpl& rcm, std::string name, CounterConfiguration configuration) :
            BaseCounterImpl(rcm, name, configuration) {
    }

    long getValue() {
        return BaseCounterImpl::getValue();
    }

    void increment() {
        return add(1L);
    }

    void decrement() {
        return add(-1L);
    }

    void add(long delta);
};

}
}

#endif /* SRC_HOTROD_API_COUNTERSIMPL_H_ */
