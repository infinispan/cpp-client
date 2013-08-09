#ifndef ISPN_HOTROD_METADATAVALUEIMPL_H
#define ISPN_HOTROD_METADATAVALUEIMPL_H



#include "infinispan/hotrod/MetadataValue.h"

namespace infinispan {
namespace hotrod {

template<class V> class MetadataValueImpl : public MetadataValue
{
  public:
    void setVersion(int64_t _version) {
        version = _version;
    }

    void setCreated(int64_t _created) {
        created = _created;
    }

    void setLifespan(uint32_t _lifespan) {
        lifespan = _lifespan;
    }

    void setLastUsed(int64_t _lastUsed) {
        lastUsed = _lastUsed;
    }

    void setMaxIdle(uint32_t _maxIdle) {
        maxIdle = _maxIdle;
    }

    V getValue() {
    	return value;
    }

    void setValue(const V& _value) {
    	value = _value;
    }

  private:
    V value;
};

}} // namespace infinispan::hotrod

#endif // ISPN_HOTROD_METADATAVALUEIMPL_H
