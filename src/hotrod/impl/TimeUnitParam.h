#ifndef INFINISPAN_HOTROD_CPP_TIMEUNITPARAM_H
#define INFINISPAN_HOTROD_CPP_TIMEUNITPARAM_H

#include "infinispan/hotrod/TimeUnit.h"

namespace infinispan {
namespace hotrod {
    class TimeUnitParam {
        static TimeUnit encodeDuration(uint64_t duration, TimeUnit timeUnit) {
            if (duration == 0) return DEFAULT;
            return timeUnit;
        }

    public:
        static char encodeTimeUnits(uint64_t lifespan, TimeUnit lifespanTimeUnit, uint64_t maxIdle, TimeUnit maxIdleTimeUnit) {
            char encodedLifespan = static_cast<char>(encodeDuration(lifespan, lifespanTimeUnit));
            char encodedMaxIdle = static_cast<char>(encodeDuration(maxIdle, maxIdleTimeUnit));
            return encodedLifespan << 4 | encodedMaxIdle;
        }
    };
}}

#endif //INFINISPAN_HOTROD_CPP_TIMEUNITPARAM_H
