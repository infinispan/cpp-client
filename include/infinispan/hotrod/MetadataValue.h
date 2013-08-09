#ifndef ISPN_HOTROD_METADATAVALUE_H
#define ISPN_HOTROD_METADATAVALUE_H



#include <stdint.h>

namespace infinispan {
namespace hotrod {

class MetadataValue
{
  public:
	MetadataValue() :
        created(0), lifespan(0), lastUsed(0), maxIdle(0), version(0) {}

    int64_t created;
    uint32_t lifespan;
    int64_t lastUsed;
    uint32_t maxIdle;
    int64_t version;
};

}} // namespace infinispan::hotrod

#endif // ISPN_HOTROD_METADATAVALUE_H
