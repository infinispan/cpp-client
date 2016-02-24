#include <time.h>
#include "hotrod/sys/Time.h"

using namespace infinispan::hotrod::sys;

Time Time::utcNow() {
    struct timespec now = getClockNow();
    struct tm *local = gmtime(&(now.tv_sec));
    return Time((uint16_t) local->tm_year, (uint8_t) local->tm_mon, (uint8_t) local->tm_mday,
        (uint8_t) local->tm_hour, (uint8_t) local->tm_min, (uint8_t) local->tm_sec,
        (uint16_t) (now.tv_nsec/1000000));
}

Time Time::localNow() {
    struct timespec now = getClockNow();
    struct tm *utc = localtime(&(now.tv_sec));
    return Time((uint16_t) utc->tm_year, (uint8_t) utc->tm_mon, (uint8_t) utc->tm_mday,
        (uint8_t) utc->tm_hour, (uint8_t) utc->tm_min, (uint8_t) utc->tm_sec,
        (uint16_t) (now.tv_nsec/1000000));
}
