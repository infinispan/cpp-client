/*
 * RemoteCountersImpl.cpp
 *
 *  Created on: May 4, 2018
 *      Author: rigazilla
 */

#include <hotrod/api/CountersImpl.h>
#include "hotrod/impl/operations/CounterOperations.h"

namespace infinispan {
namespace hotrod {

using namespace infinispan::hotrod::operations;

void BaseCounterImpl::reset() {
    ResetCounterOperation op(*rcm.codec, rcm.transportFactory, rcm.topology, 0, name);
    op.execute();
}

void BaseCounterImpl::remove() {
    RemoveCounterOperation op(*rcm.codec, rcm.transportFactory, rcm.topology, 0, name);
    op.execute();
    setRemoved();
}

long BaseCounterImpl::getValue() {
    GetCounterValueOperation op(*rcm.codec, rcm.transportFactory, rcm.topology, 0, name);
    return op.execute();
}

const void* BaseCounterImpl::addListener(const event::CounterListener* listener) {
    return rcm.addListener(getName(), listener);
}

void BaseCounterImpl::removeListener(const void* handler) {
    rcm.removeListener(getName(), handler);
}

long StrongCounterImpl::addAndGet(long delta) {
    AddAndGetCounterValueOperation op(*rcm.codec, rcm.transportFactory, rcm.topology, 0, name, delta);
    return op.execute();
}

long StrongCounterImpl::compareAndSwap(long expect, long update) {
    CompareAndSwapCounterValueOperation op(*rcm.codec, rcm.transportFactory, rcm.topology, 0, name, expect, update,
            configuration);
    return op.execute();
}

void WeakCounterImpl::add(long delta) {
    AddAndGetCounterValueOperation op(*rcm.codec, rcm.transportFactory, rcm.topology, 0, name, delta);
    op.execute();
}

}
}

