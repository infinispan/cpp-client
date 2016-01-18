#ifndef ISPN_HOTROD_JBASICMARSHALLER_H
#define ISPN_HOTROD_JBASICMARSHALLER_H


#include <string>
#include <iostream>
#include "infinispan/hotrod/Marshaller.h"

namespace infinispan {
namespace hotrod {

/*
 * A Marshaller for a few simple types that pretends to be compatible with JBoss Marshaller.
 * See below the Helper class for a list of the managed types.
 */
template <class T> class JBasicMarshaller : public infinispan::hotrod::Marshaller<T> {
};

class JBasicMarshallerHelper {
public:
	// Type managed: SMALL_STRING, INTEGER
    enum  {MARSHALL_VERSION = 0x03, SMALL_STRING = 0x3e, INTEGER=0x4b};
    static void noRelease(ScopedBuffer*) { /* nothing allocated, nothing to release */ }
    static void release(ScopedBuffer *buf) {
        delete buf->getBytes();
    }
};


// Specialization for std::string:

template <>
class JBasicMarshaller<std::string> : public infinispan::hotrod::Marshaller<std::string> {
  public:
    void marshall(const std::string& s, ScopedBuffer& b) {
    	char* buf = new char[s.size()+3];
    	// JBoss preamble
    	buf[0] = JBasicMarshallerHelper::MARSHALL_VERSION;
    	buf[1] = JBasicMarshallerHelper::SMALL_STRING;
    	buf[2]=s.size();
    	memcpy(buf+3,s.data(),s.size());
        b.set(buf, s.size()+3, &JBasicMarshallerHelper::release);
    }

    std::string* unmarshall(const ScopedBuffer& b) {
        std::string* s = new std::string(b.getBytes()+3, b.getLength()-3);
        return s;
    }

    static std::string addPreamble(std::string &s) {
        std::string res("\x03\x3e");
        res.append(1,s.size());
        res.append(s);
        return res;
    }
};

template <>
class JBasicMarshaller<int> : public infinispan::hotrod::Marshaller<int> {
  public:
    void marshall(const int& s, ScopedBuffer& b) {
        char *buf = new char[6];
        // JBoss preamble
        buf[0] = JBasicMarshallerHelper::MARSHALL_VERSION;
        buf[1] = JBasicMarshallerHelper::INTEGER;
        for (int i = 0 ; i < 4 ; i++) {
            buf[5-i] = (char) ((s) >> (8*i));
        }
        b.set(buf, 6, &JBasicMarshallerHelper::release);
    }
    int* unmarshall(const ScopedBuffer& b) {
      int result = 0;
      for (int i = 0; i < 4 ; i++) {
        result <<= 8;
        result ^= (int) *(b.getBytes()+i+2) & 0xFF;
      }
      int* s = new int(result);
      return s;
    }
};
}} // namespace

#endif  /* ISPN_HOTROD_JBasicMarshaller_H */
