#ifndef ISPN_HOTROD_TIMEUNIT_H
#define ISPN_HOTROD_TIMEUNIT_H

namespace infinispan {
namespace hotrod {

/**
 * A convenient enumeration of various time units used
 * mostly when inserting key/value pairs in RemoteCache.
 *
 */

    enum  TimeUnit {
        SECONDS = 0x00,
        MILLISECONDS = 0x01,
        NANOSECONDS = 0x02,
        MICROSECONDS = 0x03,
        MINUTES = 0x04,
        DAYS = 0x06,
        HOURS = 0x05,
        DEFAULT = 0x07,
        INFINITUM = 0x08
    };
}} // namespace

#endif  /* ISPN_HOTROD_TIMEUNIT_H */
