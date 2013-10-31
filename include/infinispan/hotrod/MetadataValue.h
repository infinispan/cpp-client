#ifndef ISPN_HOTROD_METADATAVALUE_H
#define ISPN_HOTROD_METADATAVALUE_H

#include "infinispan/hotrod/types.h"
#include "infinispan/hotrod/VersionedValue.h"

namespace infinispan {
namespace hotrod {

class MetadataValue : public VersionedValue
{
  public:
    MetadataValue() : VersionedValue(), created(0), lifespan(-1), lastUsed(0), maxIdle(-1) {}

    int64_t created;
    int32_t lifespan;
    int64_t lastUsed;
    int32_t maxIdle;
};

}} // namespace infinispan::hotrod

#endif // ISPN_HOTROD_METADATAVALUE_H
