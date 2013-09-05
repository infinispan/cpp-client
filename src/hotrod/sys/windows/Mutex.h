#ifndef ISPN_HOTROD_WINDOWS_MUTEX_H
#define ISPN_HOTROD_WINDOWS_MUTEX_H

#include <windows.h>

namespace infinispan {
namespace hotrod {
namespace sys {

class Condition;

/**
 * Mutex lock. Non-copyable.
 */
class Mutex {
    friend class Condition;

public:
    typedef ::infinispan::hotrod::sys::ScopedLock<Mutex> ScopedLock;
    typedef ::infinispan::hotrod::sys::ScopedUnlock<Mutex> ScopedUnlock;

    inline Mutex();
    inline ~Mutex();
    inline void lock();
    inline void unlock();
    inline bool trylock();

protected:
    CRITICAL_SECTION mutex;

private:
    Mutex(Mutex const &);
    Mutex& operator=(Mutex const &);
};

Mutex::Mutex() {
    ::InitializeCriticalSection(&mutex);
}

Mutex::~Mutex(){
    ::DeleteCriticalSection(&mutex);
}

void Mutex::lock() {
    ::EnterCriticalSection(&mutex);
}

void Mutex::unlock() {
    ::LeaveCriticalSection(&mutex);
}

bool Mutex::trylock() {
    return ::TryEnterCriticalSection(&mutex) != 0;
}

}}}
#endif  /* ISPN_HOTROD_WINDOWS_MUTEX_H */
