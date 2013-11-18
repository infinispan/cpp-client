#ifndef ISPN_HOTROD_TIMEUNIT_H
#define ISPN_HOTROD_TIMEUNIT_H

namespace infinispan {
namespace hotrod {

/**
 * A convenient enumeration of various time units used
 * mostly when inserting key/value pairs in RemoteCache.
 *
 */
enum TimeUnit
{
    NANOSECONDS,
    MICROSECONDS,
    MILLISECONDS,
    SECONDS,
    MINUTES,
    HOURS,
    DAYS
};

}} // namespace

#endif  /* ISPN_HOTROD_TIMEUNIT_H */
