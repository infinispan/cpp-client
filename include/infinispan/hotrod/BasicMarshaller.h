#ifndef ISPN_HOTROD_BASICMARSHALLER_H
#define ISPN_HOTROD_BASICMARSHALLER_H


#include <string>
#include <iostream>
#include "infinispan/hotrod/Marshaller.h"

namespace infinispan {
namespace hotrod {

/*
 * A Marshaller for a few simple types.
 */


template <class T> class BasicMarshaller : public infinispan::hotrod::Marshaller<T>
{};

class BasicMarshallerHelper {
public:
    static void noRelease(ScopedBuffer*) { /* nothing allocated, nothing to release */ }
    static void release(ScopedBuffer *buf) {
        delete buf->getBytes();
    }
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

template <>
class BasicMarshaller<int> : public infinispan::hotrod::Marshaller<int> {
  public:
    void marshall(const int& s, ScopedBuffer& b) {
        char *buf = new char[4];
        for (int i = 0 ; i < 4 ; i++) {
            buf[3-i] = (char) ((s) >> (8*i));
        }
        b.set(buf, 4, &BasicMarshallerHelper::release);
    }
    int* unmarshall(const ScopedBuffer& b) {
    	int result = 0;
    	for (int i = 0; i < 4 ; i++) {
    	    result <<= 4;
    	    result ^= (int) *(b.getBytes()+i) & 0xFF;
    	}
        int* s = new int(result);
        return s;
    }
};

}} // namespace

#endif  /* ISPN_HOTROD_BASICMARSHALLER_H */
