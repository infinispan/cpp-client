#ifndef ISPN_HOTROD_LOG_H
#define ISPN_HOTROD_LOG_H

#include "infinispan/hotrod/types.h"
#include "infinispan/hotrod/ImportExport.h"
#include "hotrod/sys/Mutex.h"
#include <stdexcept>
#include <stdarg.h>
#include <stdlib.h>

#define LOG_LEVEL_TRACE "TRACE"
#define LOG_LEVEL_DEBUG "DEBUG"
#define LOG_LEVEL_INFO  "INFO"
#define LOG_LEVEL_WARN  "WARN"
#define LOG_LEVEL_ERROR "ERROR"

#define ENV_HOTROD_LOG_LEVEL    "HOTROD_LOG_LEVEL"
#define ENV_HOTROD_LOG_THREADS  "HOTROD_LOG_THREADS"
#define ENV_HOTROD_LOG_TIME     "HOTROD_LOG_TIME"
#define ENV_HOTROD_LOG_TRACE_BYTES_MAX  "HOTROD_LOG_TRACE_BYTES_MAX"

namespace infinispan {
namespace hotrod {
namespace sys {

typedef enum LogLevel { LEVEL_TRACE, LEVEL_DEBUG, LEVEL_INFO, LEVEL_WARN, LEVEL_ERROR } LogLevel;

/* The class is used in some of the tests, therefore, it is exported */
class HR_EXTERN Log
{
  public:
    Log();

    Log(LogLevel level) {
        m_level = level;
    }

    ~Log() {}

    void trace(const char *fname, const int lineno, const char *format, ...) { va_list vl; va_start(vl, format); log(LOG_LEVEL_TRACE, fname, lineno, format, vl); va_end(vl); }
    void trace(const char *fname, const int lineno, const char *message, const infinispan::hotrod::hrbytes &bytes) { log(LOG_LEVEL_TRACE, fname, lineno, message, bytes); }
    void debug(const char *fname, const int lineno, const char *format, ...) { va_list vl; va_start(vl, format); log(LOG_LEVEL_DEBUG, fname, lineno, format, vl); va_end(vl); }
    void info(const char *fname, const int lineno, const char *format, ...) { va_list vl; va_start(vl, format); log(LOG_LEVEL_INFO, fname, lineno, format, vl); va_end(vl); }
    void warn(const char *fname, const int lineno, const char *format, ...) { va_list vl; va_start(vl, format); log(LOG_LEVEL_WARN, fname, lineno, format, vl); va_end(vl); }
    void error(const char *fname, const int lineno, const char *format, ...) { va_list vl; va_start(vl, format); log(LOG_LEVEL_ERROR, fname, lineno, format, vl); va_end(vl); }

    bool isTraceEnabled() { return m_level == LEVEL_TRACE; }
    bool isDebugEnabled() { return m_level <= LEVEL_DEBUG; }
    bool isInfoEnabled() { return m_level <= LEVEL_INFO; }
    bool isWarnEnabled() { return m_level <= LEVEL_WARN; }
    bool isErrorEnabled() { return m_level <= LEVEL_ERROR; }
  private:
    Mutex lock;
    LogLevel m_level;
    bool m_logThread;
    bool m_logTime;
    size_t m_traceBytesMax;

    void log(const char *level, const char *fname, const int lineno);
    void log(const char *level, const char *fname, const int lineno, const char *format, va_list vl);
    void log(const char *level, const char *fname, const int lineno, const char *message, const infinispan::hotrod::hrbytes &bytes);
};

#define TRACE(...) if (logger.isTraceEnabled()) logger.trace(__FILE__, __LINE__, __VA_ARGS__)
#define TRACEBYTES(message, bytes) if (logger.isTraceEnabled()) logger.trace(__FILE__, __LINE__, message, bytes)
#define DEBUG(...) if (logger.isDebugEnabled()) logger.debug(__FILE__, __LINE__, __VA_ARGS__)
#define INFO(...) if (logger.isInfoEnabled()) logger.info(__FILE__, __LINE__, __VA_ARGS__)
#define WARN(...) if (logger.isWarnEnabled()) logger.warn(__FILE__, __LINE__, __VA_ARGS__)
#define ERROR(...) if (logger.isErrorEnabled()) logger.error(__FILE__, __LINE__, __VA_ARGS__)

}}}

extern HR_EXTERN infinispan::hotrod::sys::Log logger;

#endif  /* ISPN_HOTROD_LOG_H */

