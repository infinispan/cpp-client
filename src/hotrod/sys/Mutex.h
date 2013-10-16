#ifndef ISPN_HOTROD_MUTEX_H
#define ISPN_HOTROD_MUTEX_H

namespace infinispan {
namespace hotrod {
namespace sys {

/**
 * Scoped lock template: calls lock() in ctor, unlock() in dtor.
 * L can be any class with lock() and unlock() functions.
 */
template <class L>
class ScopedLock
{
  public:
    ScopedLock(L& l) : mutex(l) { l.lock(); }
    ~ScopedLock() { mutex.unlock(); }
  private:
    L& mutex;
};

template <class L>
class ScopedUnlock
{
  public:
    ScopedUnlock(L& l) : mutex(l) { l.unlock(); }
    ~ScopedUnlock() { mutex.lock(); }
  private:
    L& mutex;
};


}}}


// The Mutex class is "recursive" in pthread terms, i.e. the same
// thread can acquire a mutex it already owns without blocking.

#if defined (_WIN32)
#include "windows/Mutex.h"
#else
#include "posix/Mutex.h"
#endif

#define synchronized(M) bool M##_locked = true; for(ScopedLock<Mutex> M##_sl(M); M##_locked; M##_locked = false)

#endif  /* ISPN_HOTROD_MUTEX_H */
