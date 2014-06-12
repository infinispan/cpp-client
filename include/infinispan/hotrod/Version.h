#ifndef ISPN_HOTROD_VERSION_H
#define ISPN_HOTROD_VERSION_H

#include "infinispan/hotrod/portable.h"
#include "infinispan/hotrod/ImportExport.h"
#include <string>

namespace infinispan {
namespace hotrod {

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
    static const char *getProtocolVersion() { return PROTOCOL_VERSION; }

    /**
  	 * Returns version of the client.
  	 *
  	 *\sa RemoteCache::getVersion
  	 */
    static const char *getVersion() { return VERSION; }
};

}}

#endif // ISPN_HOTROD_VERSIONEDVALUE_H
