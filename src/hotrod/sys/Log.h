#ifndef ISPN_HOTROD_LOG_H
#define ISPN_HOTROD_LOG_H

#include "infinispan/hotrod/ImportExport.h"
#include "hotrod/sys/Mutex.h"
#include "hotrod/sys/Path.h"
#include <stdexcept>
#include <iostream>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


namespace infinispan {
namespace hotrod {
namespace sys {

typedef enum LogLevel { LEVEL_TRACE, LEVEL_DEBUG, LEVEL_INFO, LEVEL_WARN, LEVEL_ERROR } LogLevel;

class HR_EXTERN Log
{
  public:
    Log() {
        const char *level = getenv("HOTROD_LOG_LEVEL");
        if (level == 0) {
            m_level = LEVEL_ERROR;
        } else if (!strcmp(level, "TRACE")) {
            m_level = LEVEL_TRACE;
        } else if (!strcmp(level, "DEBUG")) {
            m_level = LEVEL_DEBUG;
        } else if (!strcmp(level, "INFO")) {
            m_level = LEVEL_INFO;
        } else if (!strcmp(level, "WARN")) {
            m_level = LEVEL_WARN;
        } else if (!strcmp(level, "ERROR")) {
            m_level = LEVEL_ERROR;
        } else {
            throw std::runtime_error("Invalid HOTROD_LOG_LEVEL environment variable");
        }
    }

    Log(LogLevel level) {
        m_level = level;
    }

    ~Log() {}

    void trace(const char *fname, const int lineno, const char *format, ...) { va_list vl; va_start(vl, format); log("TRACE", fname, lineno, format, vl); va_end(vl); }
    void debug(const char *fname, const int lineno, const char *format, ...) { va_list vl; va_start(vl, format); log("DEBUG", fname, lineno, format, vl); va_end(vl); }
    void info(const char *fname, const int lineno, const char *format, ...) { va_list vl; va_start(vl, format); log("INFO", fname, lineno, format, vl); va_end(vl); }
    void warn(const char *fname, const int lineno, const char *format, ...) { va_list vl; va_start(vl, format); log("WARN", fname, lineno, format, vl); va_end(vl); }
    void error(const char *fname, const int lineno, const char *format, ...) { va_list vl; va_start(vl, format); log("ERROR", fname, lineno, format, vl); va_end(vl); }

    bool isTraceEnabled() { return m_level == LEVEL_TRACE; }
    bool isDebugEnabled() { return m_level <= LEVEL_DEBUG; }
    bool isInfoEnabled() { return m_level <= LEVEL_INFO; }
    bool isWarnEnabled() { return m_level <= LEVEL_WARN; }
    bool isErrorEnabled() { return m_level <= LEVEL_ERROR; }
  private:
    Mutex lock;
    LogLevel m_level;

    void log(const char *level, const char *fname, const int lineno, const char *format, va_list vl) {
        char buf[2048];
        vsnprintf(buf, 2048, format, vl);
        ScopedLock<Mutex> sl(lock);

        const char* pfname = strrchr(fname, PATH_SEP);
        if (pfname == NULL) {
          /* Use the full fname if no separator is found. */
          pfname = fname;
        }
        std::cerr << level << "[" << (const char *)(pfname + 1) << ":" << lineno << "] " << buf << std::endl;
    }
};

#define TRACE(...) if (logger.isTraceEnabled()) logger.trace(__FILE__, __LINE__, __VA_ARGS__)
#define DEBUG(...) if (logger.isDebugEnabled()) logger.debug(__FILE__, __LINE__, __VA_ARGS__)
#define INFO(...) if (logger.isInfoEnabled()) logger.info(__FILE__, __LINE__, __VA_ARGS__)
#define WARN(...) if (logger.isWarnEnabled()) logger.warn(__FILE__, __LINE__, __VA_ARGS__)
#define ERROR(...) if (logger.isErrorEnabled()) logger.error(__FILE__, __LINE__, __VA_ARGS__)

}}}

static infinispan::hotrod::sys::Log logger;

#endif  /* ISPN_HOTROD_LOG_H */

