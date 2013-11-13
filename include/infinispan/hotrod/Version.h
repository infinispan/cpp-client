#ifndef ISPN_HOTROD_VERSION_H
#define ISPN_HOTROD_VERSION_H

#include "infinispan/hotrod/ImportExport.h"
#include <string>

namespace infinispan {
namespace hotrod {

class Version {
private:
    HR_EXTERN static const std::string PROTOCOL_VERSION;
    HR_EXTERN static const std::string VERSION;

public:
    static const std::string& getProtocolVersion() { return PROTOCOL_VERSION; }
    static const std::string& getVersion() { return VERSION; }
};

}}

#endif // ISPN_HOTROD_VERSIONEDVALUE_H
