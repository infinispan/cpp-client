#ifndef ISPN_HOTROD_CONDITION_H
#define ISPN_HOTROD_CONDITION_H

#if defined (_WIN32)
#include "windows/Condition.h"
#else
#include "posix/Condition.h"
#endif

#endif  /* ISPN_HOTROD_CONDITION_H */
