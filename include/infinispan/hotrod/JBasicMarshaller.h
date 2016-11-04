#ifndef ISPN_HOTROD_JBASICMARSHALLER_H
#define ISPN_HOTROD_JBASICMARSHALLER_H


#include <string>
#include <iostream>
#include "infinispan/hotrod/Marshaller.h"
#include "infinispan/hotrod/exceptions.h"

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
    enum  {MARSHALL_VERSION = 0x03, SMALL_STRING = 0x3e, MEDIUM_STRING = 0x3f, INTEGER=0x4b};
    static void noRelease(std::vector<char>*) { /* nothing allocated, nothing to release */ }
    static void release(std::vector<char> *buf) {
        delete buf->data();
    }
    template <class T> static T unmarshall(char *);
};

    template <> std::string JBasicMarshallerHelper::unmarshall(char *b) {
        if (b[0]!=JBasicMarshallerHelper::MARSHALL_VERSION)
            throw Exception("JBasicMarshallerHelper: bad version");
        if ( (b[1]!=JBasicMarshallerHelper::SMALL_STRING) && (b[1]!=JBasicMarshallerHelper::MEDIUM_STRING) )
            throw Exception("JBasicMarshallerHelper: not a string");
        return std::string(b+3,b[2]);
    }

    template <> int JBasicMarshallerHelper::unmarshall(char *b) {
        if (b[0]!=JBasicMarshallerHelper::MARSHALL_VERSION)
            throw Exception("JBasicMarshallerHelper: bad version");
        if (b[1]!=JBasicMarshallerHelper::INTEGER)
            throw Exception("JBasicMarshallerHelper: not a integer");
        int result = 0;
        for (int i = 0; i < 4 ; i++) {
            result <<= 8;
            result ^= (int) *(b+i+2) & 0xFF;
        }
        return result;
    }



// Specialization for std::string:

template <>
class JBasicMarshaller<std::string> : public infinispan::hotrod::Marshaller<std::string> {
  public:
    void marshall(const std::string& s, std::vector<char>& b) {
		if (s.size() <= 0x100) {
		marshallSmall(s, b);
		}
		else
			marshallMedium(s, b);
		}

    std::string* unmarshall(const std::vector<char>& b) {
        std::string* s = new std::string(b.data()+3, b.size()-3);
        return s;
    }

static std::string addPreamble(std::string &s) {
    	std::string res;
    	if (s.size()<0x100)
    	{
		  res = addPreambleSmall(s);
    	}
    	else
    	{
		  res = addPreambleMedium(s);
    	}
        return res;
    }

private:
	static std::string addPreambleSmall(std::string& s) {
		std::string res("\x03\x3e");
		res.append(1, (char)s.size());
		res.append(s);
		return res;
	}
	static std::string addPreambleMedium(std::string& s) {
		std::string res("\x03\x3f");
		res.append(1, (char)(s.size()>>8));
		res.append(1, (char)(s.size()&& 0xff));
		res.append(s);
		return res;
	}

	void marshallSmall(const std::string& s, std::vector<char>& b) {
		char buf[s.size() + 3];
		// JBoss preamble
		buf[0] = JBasicMarshallerHelper::MARSHALL_VERSION;
			buf[1] = JBasicMarshallerHelper::SMALL_STRING;
		buf[2] = (char)s.size();
		memcpy(buf + 3, s.data(), s.size());
		b.assign(buf, buf + s.size() + 3);
	}

	void marshallMedium(const std::string& s, std::vector<char>& b) {
		char buf[s.size() + 4];
		// JBoss preamble
		buf[1] = JBasicMarshallerHelper::MEDIUM_STRING;
		buf[2] = (char)(s.size() >> 8);
		buf[3] = s.size() & 0xff;

		memcpy(buf + 4, s.data(), s.size());
		b.assign(buf, buf + s.size() + 4);
	}
};

template <>
class JBasicMarshaller<int> : public infinispan::hotrod::Marshaller<int> {
  public:
    void marshall(const int& s, std::vector<char>& b) {
        char buf[6];
        // JBoss preamble
        buf[0] = JBasicMarshallerHelper::MARSHALL_VERSION;
        buf[1] = JBasicMarshallerHelper::INTEGER;
        for (int i = 0 ; i < 4 ; i++) {
            buf[5-i] = (char) ((s) >> (8*i));
        }
        b.assign(buf, buf+6);
    }
    int* unmarshall(const std::vector<char>& b) {
      int result = 0;
      for (int i = 0; i < 4 ; i++) {
        result <<= 8;
        result ^= (int) *(b.data()+i+2) & 0xFF;
      }
      int* s = new int(result);
      return s;
    }
};
}} // namespace

#endif  /* ISPN_HOTROD_JBasicMarshaller_H */
