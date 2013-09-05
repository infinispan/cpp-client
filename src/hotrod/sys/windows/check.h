#ifndef INFINISPAN_HOTROD_CHECK_H
#define INFINISPAN_HOTROD_CHECK_H

#include "infinispan/hotrod/exceptions.h"
#include "hotrod/sys/Msg.h"
#include "hotrod/sys/platform.h"


#define HOTROD_WINDOWS_ERROR(ERRVAL) ::infinispan::hotrod::Exception(HOTROD_MSG(infinispan::hotrod::sys::strError(ERRVAL)))
#define HOTROD_WINDOWS_CRT_ERROR(ERRNO) ::infinispan::hotrod::Exception(HOTROD_MSG(infinispan::hotrod::sys::strError(ERRNO)))

/** THROW HOTROD_WINDOWS_ERROR(::GetLastError()) if RESULT is NULL */
#define HOTROD_WINDOWS_CHECK_NULL(RESULT)                        \
    if ((RESULT) == NULL) throw HOTROD_WINDOWS_ERROR((::GetLastError()))

#define HOTROD_WINDOWS_CHECK_NOT(RESULT,VAL)                                 \
  if ((RESULT) == (VAL)) throw HOTROD_WINDOWS_ERROR((::GetLastError()))

#define HOTROD_WINDOWS_CHECK_ASYNC_START(STATUS)                 \
    if (!(STATUS) && ::WSAGetLastError() != ERROR_IO_PENDING)  \
        throw HOTROD_WINDOWS_ERROR((::WSAGetLastError()))

#define HOTROD_WINDOWS_CHECK_CRT_NZ(VAL)   \
    if ((VAL) == 0) throw HOTROD_WINDOWS_CRT_ERROR(errno)

#define HOTROD_WINSOCK_CHECK(OP)                        \
    if ((OP) == SOCKET_ERROR) throw HOTROD_WINDOWS_ERROR((::WSAGetLastError()))

#endif  /* INFINISPAN_HOTROD_CHECK_H */
