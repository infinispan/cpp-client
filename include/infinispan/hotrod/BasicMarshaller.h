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

template <>
class BasicMarshaller<int> : public infinispan::hotrod::Marshaller<int> {
  public:
    void marshall(const int& s, std::vector<char>& b) {
        char buf[4];
        for (int i = 0 ; i < 4 ; i++) {
            buf[3-i] = (char) ((s) >> (8*i));
        }
        b.assign(buf, buf+4);
    }
    int* unmarshall(const std::vector<char>& b) {
    	int result = 0;
    	for (int i = 0; i < 4 ; i++) {
    	    result <<= 8;
    	    result ^= (int) *(b.data()+i) & 0xFF;
    	}
        int* s = new int(result);
        return s;
    }
};

}} // namespace

#endif  /* ISPN_HOTROD_BASICMARSHALLER_H */
