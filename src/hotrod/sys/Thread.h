/*
 * THREADING NOTE:
 *
 * Threading behaviour is undefined during the hotrod library "tear
 * down", e.g. after exit() or FreeLibrary().  In particular, any
 * threading code called from a static destructor is likely to
 * segfault or hang, even the simplest synchronization/locking
 * primitive.
 *
 * Hotrod subsystems that use threading should have a shutdown entry
 * point for joining all threads and setting a guard against further
 * use of any synchronization.
 */

#ifndef ISPN_HOTROD_THREAD_H
#define ISPN_HOTROD_THREAD_H


#ifdef _WIN32
#  ifdef _MSC_VER
#    define HOTROD_TSS __declspec(thread)
#  else
#    define HOTROD_TSS __thread
#  endif
#elif defined (__GNUC__)
#  define HOTROD_TSS __thread
#elif defined (__SUNPRO_CC)
#  define HOTROD_TSS __thread
#else
#  error "Dont know how to define HOTROD_TSS for this platform"
#endif

#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/types.h"

namespace infinispan {
namespace hotrod {
namespace sys {

class Runnable;
class ThreadPrivate;

class HR_EXTERN Thread
{
    HR_SHARED_PTR<ThreadPrivate> impl;

  public:
    Thread();
    explicit Thread(infinispan::hotrod::sys::Runnable*);
    explicit Thread(infinispan::hotrod::sys::Runnable&);

    operator bool();
    bool operator==(const Thread&) const;
    bool operator!=(const Thread&) const;

    void join();

    static Thread current();
    static void sleep(long millis);
};

}}}
#endif  /* ISPN_HOTROD_THREAD_H */
