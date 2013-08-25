#include "hotrod/sys/Mutex.h"

namespace infinispan {
namespace hotrod {
namespace sys {

/**
 * Initialise a recursive mutex attr for use in creating mutexes later
 * (we use pthread_once to make sure it is initialised exactly once)
 */

namespace {
pthread_once_t  onceControl = PTHREAD_ONCE_INIT;
pthread_mutexattr_t mutexattr;

void initMutexattr()  {
    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
}
}

const pthread_mutexattr_t* Mutex::getAttribute() {
    pthread_once(&onceControl, initMutexattr);
    return &mutexattr;
}

}}} // namespace
