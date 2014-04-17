#include <windows.h>
#include "hotrod/sys/Time.h"

using namespace infinispan::hotrod::sys;

Time Time::utcNow() {
    SYSTEMTIME time;
    GetSystemTime(&time);
    return Time((uint16_t) time.wYear, (uint8_t) time.wMonth, (uint8_t) time.wDay,
        (uint8_t) time.wHour, (uint8_t) time.wMinute, (uint8_t) time.wSecond,
        (uint16_t) time.wMilliseconds);
}

Time Time::localNow() {
    SYSTEMTIME time;
    GetLocalTime(&time);
    return Time((uint16_t) time.wYear, (uint8_t) time.wMonth, (uint8_t) time.wDay,
        (uint8_t) time.wHour, (uint8_t) time.wMinute, (uint8_t) time.wSecond,
        (uint16_t) time.wMilliseconds);
}
