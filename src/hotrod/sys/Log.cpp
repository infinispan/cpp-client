#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h>

#include "hotrod/sys/Log.h"
#include "hotrod/sys/Path.h"
#include "hotrod/sys/Thread.h"
#include "hotrod/sys/Time.h"

using namespace infinispan::hotrod::sys;

Log logger;

#ifdef _MSC_VER
#   define getenv_safe getenv_s
#   define vsnprintf_safe vsnprintf_s
#else
static int getenv_safe(size_t *required_size, char *buffer, size_t numberOfElements, const char *variable) {
    if (!required_size) return -1;
    const char *value = getenv(variable);    
    size_t i;
    if (!value) {
        *required_size = 0;
        if (buffer && numberOfElements > 0) buffer[0] = '\0';
        return 0;
    } else if (buffer && numberOfElements) {        
        for (i = 0; i < numberOfElements - 1 && value[i]; ++i) {
            buffer[i] = value[i];
        }        
        buffer[i] = '\0';
        *required_size = i;
    } else {
        for (i = 0; value[i]; ++i);
        *required_size = i;       
    }
    return 0;
}

static int vsnprintf_safe(char *buffer, size_t sizeOfBuffer, size_t count, const char *format, va_list argptr) {
    (void) count;
    return vsnprintf(buffer, sizeOfBuffer, format, argptr);
}
#endif

#define ENV_BUFFER_SIZE 256

Log::Log() {
    char buf[ENV_BUFFER_SIZE];
    size_t sz;
    getenv_safe(&sz, buf, ENV_BUFFER_SIZE, ENV_HOTROD_LOG_LEVEL);
    if (sz == 0) {
        m_level = LEVEL_ERROR;
    } else if (!strcmp(buf, LOG_LEVEL_TRACE)) {
        m_level = LEVEL_TRACE;
    } else if (!strcmp(buf, LOG_LEVEL_DEBUG)) {
        m_level = LEVEL_DEBUG;
    } else if (!strcmp(buf, LOG_LEVEL_INFO)) {
        m_level = LEVEL_INFO;
    } else if (!strcmp(buf, LOG_LEVEL_WARN)) {
        m_level = LEVEL_WARN;
    } else if (!strcmp(buf, LOG_LEVEL_ERROR)) {
        m_level = LEVEL_ERROR;
    } else {
        throw std::runtime_error("Invalid " ENV_HOTROD_LOG_LEVEL " environment variable");
    }
    getenv_safe(&sz, NULL, 0, ENV_HOTROD_LOG_THREADS);
    m_logThread = sz != 0;
    getenv_safe(&sz, NULL, 0, ENV_HOTROD_LOG_TIME);
    m_logTime = sz != 0;
    getenv_safe(&sz, buf, ENV_BUFFER_SIZE, ENV_HOTROD_LOG_TRACE_BYTES_MAX);
    m_traceBytesMax = sz == 0 ? 32 : atoi(buf);        
}

static const char *pfname(const char* fname) {
    const char* name = strrchr(fname, PATH_SEP);
    if (name == NULL) {
        /* Use the full fname if no separator is found. */
        return fname;
    } else {
        return name + 1;
    }
}

void Log::log(const char *level, const char *fname, const int lineno) {
    if (m_logTime) {
        Time now = Time::localNow();
        std::cerr << std::setfill('0') << std::setw(2) << (unsigned) now.hour << ':'
            << std::setw(2) << (unsigned) now.minute << ':' << std::setw(2) << (unsigned) now.second << ',' 
            << std::setw(3) << (unsigned) now.millisecond << ' ';
    }
    std::cerr << level;        
    if (m_logThread) {
        std::cerr << " (" << Thread::current().id().c_str() << ") [";
    } else {
        std::cerr << " [";
    }
    std::cerr << pfname(fname) << ':' << lineno << "] ";
}

void Log::log(const char *level, const char *fname, const int lineno, const char *format, va_list vl) {
    char buf[2048];
    vsnprintf_safe(buf, 2048, _TRUNCATE, format, vl);
    ScopedLock<Mutex> sl(lock);

    log(level, fname, lineno);
    std::cerr << buf << std::endl;
}

void Log::log(const char *level, const char *fname, const int lineno, const char *message, const infinispan::hotrod::hrbytes &bytes) {    
    std::ostringstream buf;         
    buf << message << "[length=" << bytes.length() << " string=";    	   	
    unsigned maxLength = std::min(m_traceBytesMax, (size_t) bytes.length());
    for (unsigned tbi = 0; tbi < maxLength; ++tbi) {
        unsigned char byte = (unsigned char) bytes.bytes()[tbi];    		
        buf << (char) (byte >= 0x20 && byte < 0x7F ? byte : '?');
    }
    if (bytes.length() > m_traceBytesMax) {
        buf << "...";
    }
    buf << ", hex=" << std::hex << std::setfill('0');
    for (unsigned tbi = 0; tbi < maxLength; ++tbi) {
        buf << ' ' << std::setw(2) << (unsigned int) (unsigned char) bytes.bytes()[tbi];
    }            
    if (bytes.length() > m_traceBytesMax) {
        buf << "...";
    }
    buf << ", addr=0x" << std::setw(16) << (unsigned long long) (void *) bytes.bytes() << "]\n";    
    
    ScopedLock<Mutex> sl(lock);
    log(level, fname, lineno);
    std::cerr << buf.str();
}

