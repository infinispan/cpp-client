#ifndef ISPN_HOTROD_VERSIONEDVALUEIMPL_H
#define ISPN_HOTROD_VERSIONEDVALUEIMPL_H

#include "infinispan/hotrod/VersionedValue.h"

namespace infinispan {
namespace hotrod {

template<class V> class VersionedValueImpl : public VersionedValue
{
  public:
    void setVersion(int64_t _version) {
        version = _version;
    }

    int64_t getVersion() {
        return version;
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

#endif // ISPN_HOTROD_VERSIONEDVALUEIMPL_H
