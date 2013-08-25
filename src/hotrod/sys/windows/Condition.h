#ifndef ISPN_HOTROD_WINDOWS_CONDITION_H
#define ISPN_HOTROD_WINDOWS_CONDITION_H

#include "hotrod/sys/Mutex.h"
#include "check.h"

namespace infinispan {
namespace hotrod {
namespace sys {

/**
 * A condition variable for thread synchronization.
 */
class Condition
{
  public:
    inline Condition();
    inline ~Condition();
    inline void wait(Mutex&);
    inline bool wait(Mutex&, uint64_t microsecs);
    inline void notify();
    inline void notifyAll();

  private:
    CONDITION_VARIABLE condition;
};

Condition::Condition() {
    ::InitializeConditionVariable(&condition);
}

Condition::~Condition() {
    // no-op on Windows
}

void Condition::wait(Mutex& mutex) {
    HOTROD_WINDOWS_CHECK_NOT(::SleepConditionVariableCS(&condition, &mutex.mutex, INFINITE), 0);
}

bool Condition::wait(Mutex& mutex, uint64_t microsecs){
    uint64_t millisecs = microsecs / 1000;
    if (microsecs && !millisecs)
        millisecs = 1;  // force some wait time if microsecs non-zero
    DWORD winms = (millisecs >= INFINITE) ? INFINITE : static_cast<DWORD>(millisecs);
    if (::SleepConditionVariableCS(&condition, &mutex.mutex, winms) == 0) {
        if (GetLastError() == ERROR_TIMEOUT) return false;
        throw HOTROD_WINDOWS_ERROR(::GetLastError());
    }
    return true;
}

void Condition::notify(){
    ::WakeConditionVariable(&condition);
}

void Condition::notifyAll(){
    ::WakeAllConditionVariable(&condition);
}

}}} // namespace

#endif  /* ISPN_HOTROD_WINDOWS_CONDITION_H */
