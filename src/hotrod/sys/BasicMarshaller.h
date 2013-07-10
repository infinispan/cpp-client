#ifndef ISPN_HOTROD_BASICMARSHALLER_H
#define ISPN_HOTROD_BASICMARSHALLER_H

#include <string>
#include "infinispan/hotrod/Marshaller.h"

namespace infinispan {
namespace hotrod {
namespace sys {

/*
 * A Marshaller for a few simple types.
 */


template <class T> class BasicMarshaller : public infinispan::hotrod::Marshaller<T>
{
};

class BasicMarshallerHelper {
  public:
    static void noRelease(ScopedBuffer*) { /* nothing allocated, nothing to release */ }
};


// Specialization for std::string:

template <>
class BasicMarshaller<std::string> : public infinispan::hotrod::Marshaller<std::string> {
  public:
    void marshall(const std::string& s, ScopedBuffer& b) {
        b.set(const_cast<char *> (s.data()), s.size(), &BasicMarshallerHelper::noRelease);
    }
    std::string* unmarshall(const ScopedBuffer& b) {
        std::string* s = new std::string(b.getBytes(), b.getLength());
        return s;
    }
};

}}} // namespace

#endif  /* ISPN_HOTROD_BASICMARSHALLER_H */
