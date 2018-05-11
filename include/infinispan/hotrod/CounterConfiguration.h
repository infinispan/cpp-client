/*
 * CounterConfiguration.h
 *
 *  Created on: May 3, 2018
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_COUNTERCONFIGURATION_H_
#define INCLUDE_INFINISPAN_HOTROD_COUNTERCONFIGURATION_H_

#include "infinispan/hotrod/ImportExport.h"
#include <sstream>

namespace infinispan {
namespace hotrod {

enum Storage {
    VOLATILE,
    PERSISTENT
};

enum CounterType {
    UNBOUNDED_STRONG,
    BOUNDED_STRONG,
    WEAK
};

class HR_EXTERN CounterConfiguration {
public:

    CounterConfiguration(long initialValue, long lowerBound, long upperBound, int concurrencyLevel,
            CounterType type, Storage storage)
    :
            initialValue(initialValue),
            upperBound(upperBound),
            lowerBound(lowerBound),
            concurrencyLevel(concurrencyLevel),
            type(type),
            storage(storage) {
    }

    long getInitialValue() const {
        return initialValue;
    }

    long getUpperBound() const {
        return upperBound;
    }

    long getLowerBound() const {
        return lowerBound;
    }

    CounterType getType() const {
        return type;
    }

    int getConcurrencyLevel() const {
        return concurrencyLevel;
    }

    Storage getStorage() const {
        return storage;
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "CounterConfiguration{" <<
                "initialValue=" << initialValue <<
                ", upperBound=" << upperBound <<
                ", lowerBound=" << lowerBound <<
                ", concurrencyLevel=" << concurrencyLevel <<
                ", type=" << type <<
                ", storage=" << storage <<
                "}";
        return ss.str();
    }

private:
    const long initialValue;
    const long upperBound;
    const long lowerBound;
    const int concurrencyLevel;
    const CounterType type;
    const Storage storage;
};
}
}

#endif /* INCLUDE_INFINISPAN_HOTROD_COUNTERCONFIGURATION_H_ */
