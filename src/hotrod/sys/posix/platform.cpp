#include <hotrod/sys/platform.h>
#include <string.h>

namespace infinispan {
namespace hotrod {
namespace sys {

std::string strError(int err) {
    char buf[512] = "Unknown error";
#ifdef _GNU_SOURCE
    // GNU strerror_r returns the message
    return ::strerror_r(err, buf, sizeof(buf));
#else
    // POSIX strerror_r doesn't return the buffer
    ::strerror_r(err, buf, sizeof(buf));
    return std::string(buf);
#endif
}

}}}
