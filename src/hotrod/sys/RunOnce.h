#ifndef ISPN_HOTROD_RUNONCE_H
#define ISPN_HOTROD_RUNONCE_H

/*
 * Lowest common denominator of std::call_once, pthread_once, and Windows InitOnceXX
 * Usage:
 * void myFooInitFn();
 * RunOnce fooInitializer(myFooInitFn);  // static decl
 * 
 * void useFooFromAnyThread() {
 *   fooInitializer.runOnce();
 *   // safe to use foo
 * }
 */  

#if defined (_WIN32)
#include "windows/RunOnce.h"
#else
#include "posix/RunOnce.h"
#endif

#endif  /* ISPN_HOTROD_RUNONCE_H */
