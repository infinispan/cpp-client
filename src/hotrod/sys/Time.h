#ifndef ISPN_HOTROD_TIME_H
#define ISPN_HOTROD_TIME_H

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#else
#include <stdint.h>
#endif

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

private:
    static timespec getClockNow(void) {
        struct timespec now;

    	#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    		clock_serv_t cclock;
    		mach_timespec_t mts;
    		host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    		clock_get_time(cclock, &mts);
    		mach_port_deallocate(mach_task_self(), cclock);
    		now.tv_sec = mts.tv_sec;
    		now.tv_nsec = mts.tv_nsec;

    	#else
    		clock_gettime(CLOCK_REALTIME, &now);
    	#endif
    	return now;
    }
};

}}}

#endif
