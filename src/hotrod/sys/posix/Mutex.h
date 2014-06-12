#ifndef ISPN_HOTROD_POSIX_MUTEX_H
#define ISPN_HOTROD_POSIX_MUTEX_H

#include "infinispan/hotrod/ImportExport.h"
#include "hotrod/sys/posix/check.h"

#include <pthread.h>

namespace infinispan {
namespace hotrod {
namespace sys {

class Condition;

/**
 * Mutex lock. Non-copyable.
 */
class Mutex {
    friend class Condition;
    static const pthread_mutexattr_t* getAttribute();

public:
    typedef ::infinispan::hotrod::sys::ScopedLock<Mutex> ScopedLock;
    typedef ::infinispan::hotrod::sys::ScopedUnlock<Mutex> ScopedUnlock;

    inline Mutex();
    inline ~Mutex();
    inline void lock();
    inline void unlock();
    inline bool trylock();

protected:
    pthread_mutex_t mutex;

private:
    Mutex(Mutex const &);
    Mutex& operator=(Mutex const &);
};

Mutex::Mutex() {
    HOTROD_POSIX_ASSERT_THROW_IF(pthread_mutex_init(&mutex, getAttribute()));
}

Mutex::~Mutex(){
    HOTROD_POSIX_ABORT_IF(pthread_mutex_destroy(&mutex));
}

void Mutex::lock() {
    HOTROD_POSIX_ASSERT_THROW_IF(pthread_mutex_lock(&mutex));
}

void Mutex::unlock() {
    HOTROD_POSIX_ASSERT_THROW_IF(pthread_mutex_unlock(&mutex));
}

bool Mutex::trylock() {
    return pthread_mutex_trylock(&mutex) == 0;
}

}}}
#endif  /* ISPN_HOTROD_POSIX_MUTEX_H */
