/*
 * JBossMarshaller.h
 *
 *  Created on: Dec 4, 2017
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_JBOSSMARSHALLER_H_
#define INCLUDE_INFINISPAN_HOTROD_JBOSSMARSHALLER_H_

#include "infinispan/hotrod/JBasicMarshaller.h"

namespace infinispan {
namespace hotrod {

class JBossMarshaller {
public:
    template<typename R> static R unmarshall(const std::vector<char>& b);
    template<typename A> static std::vector<char> marshall(A a);
    virtual ~JBossMarshaller() = 0;

    static void marshallSmall(const std::string& s, std::vector<char>& b) {
        b.resize(s.size() + 3);
        char* buf = b.data();
        // JBoss preamble
        buf[0] = JBasicMarshallerHelper::MARSHALL_VERSION;
        buf[1] = JBasicMarshallerHelper::SMALL_STRING;
        buf[2] = (char) s.size();
        memcpy(buf + 3, s.data(), s.size());
    }
    static void marshallMedium(const std::string& s, std::vector<char>& b) {
        b.resize(s.size() + 4);
        char* buf = b.data();
        // JBoss preamble
        buf[0] = JBasicMarshallerHelper::MARSHALL_VERSION;
        buf[1] = JBasicMarshallerHelper::MEDIUM_STRING;
        buf[2] = (char) (s.size() >> 8);
        buf[3] = s.size() & 0xff;

        memcpy(buf + 4, s.data(), s.size());
    }

};

template<> inline int* JBossMarshaller::unmarshall(const std::vector<char>& b) {
    int result = 0;
    for (int i = 0; i < 4; i++) {
        result <<= 8;
        result ^= (int) *(b.data() + i + 2) & 0xFF;
    }
    int* s = new int(result);
    return s;
}

template<> inline std::string* JBossMarshaller::unmarshall(const std::vector<char>& b) {
    // TODO: this works only for SMALL_STRING
    std::string* s = new std::string(b.begin() + 3, b.end());
    return s;
}

template<> inline std::vector<char> JBossMarshaller::marshall(std::string s) {
    std::vector<char> b;
    if (s.size() <= 0x100) {
        JBossMarshaller::marshallSmall(s, b);
    } else {
        JBossMarshaller::marshallMedium(s, b);
    }
    return b;
}

template<> inline std::vector<char> JBossMarshaller::marshall(int a) {
    char buf[6];
    std::vector<char> res;
    // JBoss preamble
    buf[0] = JBasicMarshallerHelper::MARSHALL_VERSION;
    buf[1] = JBasicMarshallerHelper::INTEGER;
    for (int i = 0; i < 4; i++) {
        buf[5 - i] = (char) ((a) >> (8 * i));
    }
    res.assign(buf, buf + 6);
    return res;
}

}
}

#endif /* INCLUDE_INFINISPAN_HOTROD_JBOSSMARSHALLER_H_ */
