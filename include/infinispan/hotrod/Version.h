#ifndef ISPN_HOTROD_VERSION_H
#define ISPN_HOTROD_VERSION_H

#include "infinispan/hotrod/ImportExport.h"
#include <string>

namespace infinispan {
namespace hotrod {

// The memory won't be ever released. It's even worse - the pointer is static,
// therefore, it will be allocated anew for each compilation unit using this.
// However, if we placed the pointers to the library, two differently compiled
// modules could access the same data, causing memory corruption.
static std::string *__protocolVersionPtr = NULL;
static std::string *__versionPtr = NULL;

/**
 * Version instance represents HotRod client and protocol version
 *
 */
class Version {
private:
    HR_EXTERN static const char *PROTOCOL_VERSION;
    HR_EXTERN static const char *VERSION;

public:
    /**
     * Returns protocol version of the client.
     *
     *\sa RemoteCache::getProtocolVersion
     */
    static const char *getProtocolVersionCString() {
        return PROTOCOL_VERSION;
    }

    /**
     * Returns version of the client.
     *
     *\sa RemoteCache::getVersion
     */
    static const char *getVersionCString() {
        return VERSION;
    }

    /**
     * DEPRECATED: Use Version::getProtocolVersionCString();
	 */
    static const std::string &getProtocolVersion() {
        if (__protocolVersionPtr == NULL) {
            __protocolVersionPtr = new std::string(PROTOCOL_VERSION);
        }
        return *__protocolVersionPtr;
    }

    /**
     * DEPRECATED: Use Version::getVersionCString()
  	 */
    static const std::string &getVersion() {
        if (__versionPtr == NULL) {
            __versionPtr = new std::string(VERSION);
        }
        return *__versionPtr;
    }
};

}}

#endif // ISPN_HOTROD_VERSIONEDVALUE_H
