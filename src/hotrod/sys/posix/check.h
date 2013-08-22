#ifndef INFINISPAN_HOTROD_CHECK_H
#define INFINISPAN_HOTROD_CHECK_H

#include "infinispan/hotrod/exceptions.h"
#include "hotrod/sys/Msg.h"
#include "hotrod/sys/platform.h"

#include <cerrno>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define HOTROD_POSIX_ERROR(ERRNO) ::infinispan::hotrod::Exception(HOTROD_MSG(infinispan::hotrod::sys::strError(ERRNO)))

/** THROW HOTROD_POSIX_ERROR(errno) if RESULT is less than zero */
#define HOTROD_POSIX_CHECK(RESULT)                        \
    if ((RESULT) < 0) throw HOTROD_POSIX_ERROR((errno))

/** Throw a posix error if ERRNO is non-zero */
#define HOTROD_POSIX_THROW_IF(ERRNO)              \
    do { int e=(ERRNO); if (e) throw HOTROD_POSIX_ERROR(e); } while(0)

/** Same as _THROW_IF in a release build, but abort a debug build */
#ifdef NDEBUG
#define HOTROD_POSIX_ASSERT_THROW_IF(ERRNO) HOTROD_POSIX_THROW_IF(ERRNO)
#else
#define HOTROD_POSIX_ASSERT_THROW_IF(ERRNO)                               \
    do { int e=(ERRNO); if (e) { errno=e; ::perror(0); assert(0); } } while(0)
#endif

#define HOTROD_POSIX_ABORT_IF(ERRNO) if ((int) ERRNO) { errno=ERRNO; ::perror(0); abort(); }

#endif  /* INFINISPAN_HOTROD_CHECK_H */
