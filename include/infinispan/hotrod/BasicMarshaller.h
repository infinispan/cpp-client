#ifndef ISPN_HOTROD_BASICMARSHALLER_H
#define ISPN_HOTROD_BASICMARSHALLER_H


#include <string>
#include <cstring>
#include <iostream>
#include <type_traits>
#include "infinispan/hotrod/Marshaller.h"

namespace infinispan {
namespace hotrod {

/*
 * A Marshaller for a few simple types.
 */


template <class T> class BasicMarshaller : public infinispan::hotrod::Marshaller<T>
{
      public:
        void marshall(const T& s, std::vector<char>& b) {
#if __GNUG__ && __GNUC__ < 5
            static_assert(std::is_fundamental<T>::value, "Type is not fundamental. A marshaller specialization is needed");
#else
            static_assert(std::is_trivially_copyable<T>::value, "Type is not trivially_copyable. A marshaller specialization is needed");
#endif
            b.resize(sizeof(s));
            std::memcpy(b.data(), &s, sizeof(s));
        }
        T* unmarshall(const std::vector<char>& b) {
#if __GNUG__ && __GNUC__ < 5
            static_assert(std::is_fundamental<T>::value, "Type is not trivially_copyable. A marshaller specialization is needed");
#else
            static_assert(std::is_trivially_copyable<T>::value, "Type is not trivially_copyable. A marshaller specialization is needed");
#endif
            T* s = new T();
            std::memcpy(s, b.data(), sizeof(*s));
            return s;
        }
};

class BasicMarshallerHelper {
public:
    static void noRelease(std::vector<char>*) { /* nothing allocated, nothing to release */ }
    static void release(std::vector<char> *buf) {
        delete buf->data();
    }
};


// Specialization for std::string:

template <>
class BasicMarshaller<std::string> : public infinispan::hotrod::Marshaller<std::string> {
  public:
    void marshall(const std::string& s, std::vector<char>& b) {
        b.assign(s.data(), s.data()+s.size());
    }
    std::string* unmarshall(const std::vector<char>& b) {
        std::string* s = new std::string(b.data(), b.size());
        return s;
    }
};

}} // namespace

#endif  /* ISPN_HOTROD_BASICMARSHALLER_H */
