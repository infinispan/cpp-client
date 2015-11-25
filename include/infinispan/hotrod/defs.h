#ifndef ISPN_HOTROD_DEFS_H
#define ISPN_HOTROD_DEFS_H

#include "infinispan/hotrod/ImportExport.h"

#include <memory>

/*
 * Handle special cases for stdint.h and the definition for ssize_t.
 * Third party libraries (e.g. Boost) may provide competing solutions.
 *
 * The effects of this include file may be controlled by overrides:
 *  PN_DEFINE_STDINT/PN_NODEFINE_STDINT   : turn on/off definition of int64_t etc.
 *  PN_DEFINE_SSIZE_T/PN_NODEFINE_SSIZE_T : turn on/off definition of ssize_t
 *  PN_INCLUDE_STDINT/PN_NOINCLUDE_STDINT : include (or not) stdint.h
 */

// Honor positive overrides
#if defined(PN_DEFINE_STDINT)
#define PNI_DEFINE_STDINT
#endif
#if defined(PN_INCLUDE_STDINT)
#define PNI_INCLUDE_STDINT)
#endif
#if defined(PN_DEFINE_SSIZE_T)
#define PNI_DEFINE_SSIZE_T
#endif

// Determinine default action
#ifndef _MSC_VER
// Not Windows and not using Visual Studio
#ifndef PNI_INCLUDE_STDINT
#define PNI_INCLUDE_STDINT
#endif
#else
// all versions of Visual Studio
#ifndef PNI_DEFINE_SSIZE_T
// ssie_t def is needed, unless third party definition interferes, e.g. python/swig
#ifndef Py_CONFIG_H
#define PNI_DEFINE_SSIZE_T
#endif
#endif

#if (_MSC_VER < 1600)
// VS 2008 and earlier
#ifndef PNI_DEFINE_STDINT
#define PNI_DEFINE_STDINT
#endif
#else
// VS 2010 and newer
#ifndef PNI_INCLUDE_STDINT
#define PNI_INCLUDE_STDINT
#endif

#endif // (_MSC_VER < 1600)
#endif //_MSC_VER

// Honor negative overrides
#ifdef PN_NODEFINE_SSIZE_T
#undef PNI_DEFINE_SSIZE_T
#endif
#ifdef PN_NODEFINE_STDINT
#undef PNI_DEFINE_STDINT
#endif
#ifdef PN_NOINCLUDE_STDINT
#undef PNI_INCLUDE_STDINT
#endif

#ifdef PNI_INCLUDE_STDINT
#include <stdint.h>
#endif

#ifdef PNI_DEFINE_SSIZE_T
#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
#error ssize_t definition not kown
#endif
#endif // PNI_DEFINE_SSIZE_T

#ifdef PNI_DEFINE_STDINT
#ifdef _MSC_VER

typedef signed __int8 int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef signed __int64 int64_t;

typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

#else // _MSC_VER
#error stdint.h definitions not kown
#endif
#endif // PNI_DEFINE_SSIZE_T

#ifdef _MSC_VER
#   define strtoull _strtoui64
#endif

// warnings
#ifndef _MSC_VER
#   define __pragma(...)
#endif

#endif  /* ISPN_HOTROD_DEFS_H */
