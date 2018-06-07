/*
 * CounterEvent.h
 *
 *  Created on: May 9, 2018
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_COUNTEREVENT_H_
#define INCLUDE_INFINISPAN_HOTROD_COUNTEREVENT_H_

namespace infinispan {
namespace hotrod {
namespace event {

enum CounterState {
    /**
     * The counter value is valid.
     */
    VALID,
    /**
     * The counter value has reached its min threshold, i.e. no thresholds has been reached.
     */
    LOWER_BOUND_REACHED,
    /**
     * The counter value has reached its max threshold.
     */
    UPPER_BOUND_REACHED
};

class CounterEvent {
public:
    CounterEvent(std::string counterName, const long oldValue, const CounterState oldState, const long newValue,
            const CounterState newState) :
            counterName(counterName), oldValue(oldValue), oldState(oldState), newValue(newValue), newState(newState) {
    }

    /**
     * the counter name.
     */
    const std::string counterName;
    /**
     * the previous value.
     */
    const long oldValue;

    /**
     * the previous state.
     */
    const CounterState oldState;

    /**
     * the counter value.
     */
    const long newValue;

    /**
     * the counter state.
     */
    const CounterState newState;
};

class CounterListener {
public:
    CounterListener(const std::string counterName, std::function<void(const CounterEvent)> action) :
            counterName(counterName), action(action) {
    }
    CounterListener(const std::string counterName) :
            counterName(counterName) {
    }
    virtual void onUpdate(const CounterEvent entry) const {
        action(entry);
    }
    virtual ~CounterListener() {}
private:
    const std::string counterName;
    const std::function<void(const CounterEvent)> action;
};

}
}
}
#endif /* INCLUDE_INFINISPAN_HOTROD_COUNTEREVENT_H_ */
