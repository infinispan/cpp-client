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

Log::Log() {
    const char *level = getenv(ENV_HOTROD_LOG_LEVEL);        
    if (level == 0) {
        m_level = LEVEL_ERROR;
    } else if (!strcmp(level, LOG_LEVEL_TRACE)) {
        m_level = LEVEL_TRACE;
    } else if (!strcmp(level, LOG_LEVEL_DEBUG)) {
        m_level = LEVEL_DEBUG;
    } else if (!strcmp(level, LOG_LEVEL_INFO)) {
        m_level = LEVEL_INFO;
    } else if (!strcmp(level, LOG_LEVEL_WARN)) {
        m_level = LEVEL_WARN;
    } else if (!strcmp(level, LOG_LEVEL_ERROR)) {
        m_level = LEVEL_ERROR;
    } else {
        throw std::runtime_error("Invalid " ENV_HOTROD_LOG_LEVEL " environment variable");
    }
    m_logThread = (getenv(ENV_HOTROD_LOG_THREADS) != 0);
    m_logTime = (getenv(ENV_HOTROD_LOG_TIME) != 0);
    const char *tbm = getenv(ENV_HOTROD_LOG_TRACE_BYTES_MAX);
    m_traceBytesMax = (size_t) (tbm == 0 ? 32 : atoi(tbm));
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
    vsnprintf(buf, 2048, format, vl);
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

