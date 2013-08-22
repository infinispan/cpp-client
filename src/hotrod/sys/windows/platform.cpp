#include <hotrod/sys/platform.h>
#include <string.h>
#include <string.h>
#include <windows.h>

namespace infinispan {
namespace hotrod {
namespace sys {

std::string strError(int err) {
    const size_t bufsize = 512;
    char buf[bufsize];
    buf[0] = 0;
    if (0 == FormatMessage (FORMAT_MESSAGE_MAX_WIDTH_MASK
                            | FORMAT_MESSAGE_FROM_SYSTEM,
                            0,
                            err,
                            0,        // Default language
                            buf,
                            bufsize,
                            0))
    {
#ifdef _MSC_VER
        strerror_s(buf, bufsize, err);
#else
        return std::string(strerror(err));
#endif
    }
    return std::string(buf);
}

}}}
