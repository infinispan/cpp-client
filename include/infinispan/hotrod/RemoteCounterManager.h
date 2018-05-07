/*
 * CounterManager.h
 *
 *  Created on: May 3, 2018
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_REMOTECOUNTERMANAGER_H_

#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/CounterConfiguration.h"
#include "infinispan/hotrod/Counters.h"

#include <string>
#include <set>

namespace infinispan {
namespace hotrod {

class HR_EXTERN RemoteCounterManager {
public:
    /**
     * Returns the {@link StrongCounter} with that specific name.
     * <p>
     * If the {@link StrongCounter} does not exists, it is created based on the {@link CounterConfiguration}.
     * <p>
     * Note that the counter must be defined prior to this method invocation using {@link
     * RemoteCounterManager::defineCounter(std::string, CounterConfiguration)} or via global configuration. This method only supports
     * CounterType::BOUNDED_STRONG and CounterType::UNBOUNDED_STRONG counters.
     *
     * @param name the counter name.
     * @return the StrongCounter instance.
     * @throws org.infinispan.counter.exception.CounterException              if unable to retrieve the counter.
     * @throws org.infinispan.counter.exception.CounterConfigurationException if the counter configuration is not valid
     *                                                                        or it does not exists.
     */
    virtual std::shared_ptr<StrongCounter> getStrongCounter(std::string name) = 0;

    /**
     * Returns the {@link WeakCounter} with that specific name.
     * <p>
     * If the {@link WeakCounter} does not exists, it is created based on the {@link CounterConfiguration}.
     * <p>
     * Note that the counter must be defined prior to this method invocation using {@link
     * RemoteCounterManager::defineCounter(std::string, CounterConfiguration)} or via global configuration. This method only supports
     * CounterType::WEAK counters.
     *
     * @param name the counter name.
     * @return the WeakCounter instance.
     * @throws org.infinispan.counter.exception.CounterException              if unable to retrieve the counter.
     * @throws org.infinispan.counter.exception.CounterConfigurationException if the counter configuration is not valid
     *                                                                        or it does not exists.
     */
    virtual std::shared_ptr<WeakCounter> getWeakCounter(std::string name) = 0;

    /**
     * Defines a counter with the specific {@code name} and {@link CounterConfiguration}.
     * <p>
     * It does not overwrite existing configurations.
     *
     * @param name          the counter name.
     * @param configuration the counter configuration
     * @return {@code true} if successfully defined or {@code false} if the counter exists or fails to defined.
     */
    virtual bool defineCounter(std::string name, CounterConfiguration configuration) = 0;

    /**
     * @param name the counter name.
     * @return {@code true} if the counter is defined or {@code false} if the counter is not defined or fails to check.
     */
    virtual bool isDefined(std::string name) = 0;

    /**
     * @param counterName the counter name.
     * @return the counter's {@link CounterConfiguration} or {@code null} if the counter is not defined.
     */
    virtual CounterConfiguration getConfiguration(std::string counterName) = 0;

    /**
     * It removes the counter from the cluster.
     * <p>
     * All instances returned by getWeakCounter(std::string) or getStrongCounter(std::tring) are destroyed and
     * they shouldn't be used anymore. Also, the registered CounterListener are removed and they aren't invoked
     * anymore.
     *
     * @param counterName The counter's name to remove.
     */
    virtual void remove(std::string counterName) = 0;

    /**
     * Returns a set of defined counter names.
     *
     * @return a set of defined counter names.
     */
    virtual std::set<std::string> getCounterNames() = 0;

    virtual ~RemoteCounterManager() {
    }
    ;
};

}
}
#define INCLUDE_INFINISPAN_HOTROD_REMOTECOUNTERMANAGER_H_

#endif /* INCLUDE_INFINISPAN_HOTROD_COUNTERMANAGER_H_ */
