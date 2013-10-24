#ifndef ISPN_HOTROD_VERSIONEDVALUE_H
#define ISPN_HOTROD_VERSIONEDVALUE_H

#include "infinispan/hotrod/types.h"

namespace infinispan {
namespace hotrod {

class VersionedValue
{
  public:
    VersionedValue() : version(0) {}

    int64_t version;
};

}} // namespace infinispan::hotrod

#endif // ISPN_HOTROD_VERSIONEDVALUE_H
