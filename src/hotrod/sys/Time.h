#ifndef ISPN_HOTROD_TIME_H
#define ISPN_HOTROD_TIME_H

#include <stdint.h>

namespace infinispan {
namespace hotrod {
namespace sys {

class Time {
public:
    const uint16_t year;
    const uint8_t month;
    const uint8_t day;
    const uint8_t hour;
    const uint8_t minute;
    const uint8_t second;
    const uint16_t millisecond;
       
    static Time utcNow(void);
    static Time localNow(void);

    Time(uint16_t Y, uint8_t M, uint8_t D, uint8_t h, uint8_t m, uint8_t s, uint16_t ms):
        year(Y), month(M), day(D), hour(h), minute(m), second(s), millisecond(ms) {}   
};

}}}

#endif
