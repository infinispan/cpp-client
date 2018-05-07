/*
 * Counters.h
 *
 *  Created on: May 3, 2018
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_COUNTERS_H_
#define INCLUDE_INFINISPAN_HOTROD_COUNTERS_H_

#include <string>
#include "infinispan/hotrod/CounterConfiguration.h"

namespace infinispan {
namespace hotrod {

class Counter {
public:
    /**
     * @return The counter name.
     */
    virtual std::string getName() = 0;

    /**
     * @return the {@link CounterConfiguration} used by this counter.
     */
    virtual CounterConfiguration getConfiguration() = 0;

    /**
     * Resets the counter to its initial value.
     */
    virtual void reset() = 0;

    /**
     * It removes this counter from the cluster.
     * <p>
     * Note that it doesn't remove the counter from the {@link CounterManager}. If you want to remove the counter from
     * the {@link CounterManager} use {@link CounterManager#remove(String)}.
     *
     * @return The {@link CompletableFuture} that is completed when the counter is removed from the cluster.
     */
    virtual void remove() = 0;

};

class StrongCounter: public virtual Counter {
public:
    /**
     * It fetches the current value.
     * <p>
     * It may go remotely to fetch the current value.
     *
     * @return The current value.
     */
    virtual long getValue() = 0;

    /**
     * Atomically adds the given value and return the new value.
     *
     * @param delta The non-zero value to add. It can be negative.
     * @return The new value.
     */
    virtual long addAndGet(long delta) = 0;

    /**
     * Atomically increments the counter and returns the new value.
     *
     * @return The new value.
     */
    virtual long incrementAndGet() = 0;

    /**
     * Atomically decrements the counter and returns the new value
     *
     * @return The new value.
     */
    virtual long decrementAndGet() = 0;

    /**
     * Atomically sets the value to the given updated value if the current value {@code ==} the expected value.
     *
     * The operation is successful if the return value is equals to the expected value.
     *
     * @param expect the expected value.
     * @param update the new value.
     * @return the previous counter's value.
     */
    virtual long compareAndSwap(long expect, long update) = 0;

    /**
     * Atomically sets the value to the given updated value if the current value {@code ==} the expected value.
     *
     * It is the same as {@code return compareAndSwap(expect, update).thenApply(value -> value == expect);}
     *
     * @param expect the expected value
     * @param update the new value
     * @return {@code true} if successful, {@code false} otherwise.
     */
    virtual bool compareAndSet(long expect, long update) = 0;

    virtual ~StrongCounter() {
    }
};

class WeakCounter: public virtual Counter {
public:
    /**
     * It returns the counter's value.
     * <p>
     * This value may be not the mot up-to-data value.
     *
     * @return The counter's value.
     */
    virtual long getValue() = 0;

    /**
     * Increments the counter.
     */
    virtual void increment() = 0;

    /**
     * Decrements the counter.
     */
    virtual void decrement() = 0;

    /**
     * Adds the given value to the new value.
     *
     * @param delta the value to add.
     */
    virtual void add(long delta) = 0;

    virtual ~WeakCounter() {
    }
};
}
}

#endif /* INCLUDE_INFINISPAN_HOTROD_COUNTERS_H_ */
