#ifndef INFINISPAN_HOTROD_CPP_TIMEUNITPARAM_H
#define INFINISPAN_HOTROD_CPP_TIMEUNITPARAM_H

#include "infinispan/hotrod/TimeUnit.h"

namespace infinispan {
namespace hotrod {
    class TimeUnitParam {
        static TimeUnit encodeDuration(long duration, TimeUnit timeUnit) {
            if (duration == 0) return DEFAULT;
            if (duration < 0) return INFINITUM;
            return timeUnit;
        }

    public:
        static char encodeTimeUnits(long lifespan, TimeUnit lifespanTimeUnit, long maxIdle, TimeUnit maxIdleTimeUnit) {
            char encodedLifespan = static_cast<char>(encodeDuration(lifespan, lifespanTimeUnit));
            char encodedMaxIdle = static_cast<char>(encodeDuration(maxIdle, maxIdleTimeUnit));
            return encodedLifespan << 4 | encodedMaxIdle;
        }
    };
}}

#endif //INFINISPAN_HOTROD_CPP_TIMEUNITPARAM_H
