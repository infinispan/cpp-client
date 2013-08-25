#ifndef ISPN_HOTROD_POSIX_CONDITION_H
#define ISPN_HOTROD_POSIX_CONDITION_H

#include "hotrod/sys/Mutex.h"

#include <limits>
#include <time.h>
#include <sys/errno.h>

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
    pthread_cond_t condition;
};

Condition::Condition() {
    HOTROD_POSIX_ASSERT_THROW_IF(pthread_cond_init(&condition, 0));
}

Condition::~Condition() {
    HOTROD_POSIX_ABORT_IF(pthread_cond_destroy(&condition));
}

void Condition::wait(Mutex& mutex) {
    HOTROD_POSIX_ASSERT_THROW_IF(pthread_cond_wait(&condition, &mutex.mutex));
}

namespace {
/** type conversion helper: an infinite timeout for time_t sized types **/
const time_t TIME_T_MAX = std::numeric_limits<time_t>::max();
const uint64_t TIME_T_MAX_U64 = static_cast<uint64_t>(TIME_T_MAX);
}

bool Condition::wait(Mutex& mutex, uint64_t microsecs){
    struct timespec ts;
    ::clock_gettime(CLOCK_REALTIME, &ts);
    uint64_t secs = (microsecs / 1000000) + ts.tv_sec;
    ts.tv_nsec += static_cast<long>(microsecs % 1000000);
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_nsec -= 1000000000;
        secs++;
    }
    ts.tv_sec = (secs > TIME_T_MAX_U64) ? TIME_T_MAX : static_cast<time_t>(secs);

    int status = pthread_cond_timedwait(&condition, &mutex.mutex, &ts);
    if (status != 0) {
        if (status == ETIMEDOUT) return false;
        throw HOTROD_POSIX_ERROR(status);
    }
    return true;
}

void Condition::notify(){
    HOTROD_POSIX_ASSERT_THROW_IF(pthread_cond_signal(&condition));
}

void Condition::notifyAll(){
    HOTROD_POSIX_ASSERT_THROW_IF(pthread_cond_broadcast(&condition));
}

}}} // namespace

#endif  /* ISPN_HOTROD_POSIX_CONDITION_H */
