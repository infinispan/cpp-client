#ifndef ISPN_HOTROD_POSIX_RUNONCE_H
#define ISPN_HOTROD_POSIX_RUNONCE_H

#include "hotrod/sys/posix/check.h"

#include <pthread.h>

namespace infinispan {
namespace hotrod {
namespace sys {

/**
 * RunOnce. Non-copyable.
 */
class RunOnce {
    pthread_once_t once_control;
    void (*init_routine)(void);

public:
    inline RunOnce(void(*fn)()) : init_routine(fn) { once_control = PTHREAD_ONCE_INIT; }
    inline void runOnce();

private:
    RunOnce(RunOnce const &);
    RunOnce& operator=(RunOnce const &);
};

void RunOnce::runOnce() {
    HOTROD_POSIX_ASSERT_THROW_IF(pthread_once(&once_control, init_routine));
}

}}}
#endif  /* ISPN_HOTROD_POSIX_RUNONCE_H */
