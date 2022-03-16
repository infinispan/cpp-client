#ifndef ISPN_HOTROD_BASICTYPESPROTOSTREAMMARSHALLER_H
#define ISPN_HOTROD_BASICTYPESPROTOSTREAMMARSHALLER_H


#include <string>
#include <iostream>
#include "infinispan/hotrod/Marshaller.h"
#include "infinispan/hotrod/exceptions.h"
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable:4267 4244)
#endif
#include "infinispan/hotrod/base_types.pb.h"
#if _MSC_VER
#pragma warning(pop)
#endif
namespace infinispan {
namespace hotrod {

/*
 * A Marshaller for a few simple types that pretends to be compatible with JBoss Marshaller.
 * See below the Helper class for a list of the managed types.
 */
template <class T> class BasicTypesProtoStreamMarshaller : public infinispan::hotrod::Marshaller<T> {
};

class BasicTypesProtoStreamMarshallerHelper {
public:
    static void noRelease(std::vector<char>*) { /* nothing allocated, nothing to release */ }
    static void release(std::vector<char> *buf) {
        delete buf->data();
    }
    template <class T> static T unmarshall(char *, size_t size);
};

    template <> inline std::string BasicTypesProtoStreamMarshallerHelper::unmarshall(char *b, size_t size) {
        protobuf::base_types bt;
        bt.ParseFromArray(b,size);
        return bt.str();
    }

    template <> inline int BasicTypesProtoStreamMarshallerHelper::unmarshall(char *b, size_t size) {
        protobuf::base_types bt;
        bt.ParseFromArray(b,size);
        return bt.i32();
    }



// Specialization for std::string:

template <>
class BasicTypesProtoStreamMarshaller<std::string> : public infinispan::hotrod::Marshaller<std::string> {
  public:
    void marshall(const std::string& s, std::vector<char>& b) {
        protobuf::base_types bt;
        bt.set_str(s);
#if GOOGLE_PROTOBUF_VERSION < 3004001
        b.resize(bt.ByteSize());
        bt.SerializePartialToArray(b.data(),bt.ByteSize());
#else
        b.resize(bt.ByteSizeLong());
        bt.SerializePartialToArray(b.data(),bt.ByteSizeLong());
#endif
    }

    std::string* unmarshall(const std::vector<char>& b) {
        protobuf::base_types bt;
        bt.ParseFromArray(b.data(),b.size());
        return new std::string(bt.str());
    }



};

template <>
class BasicTypesProtoStreamMarshaller<int> : public infinispan::hotrod::Marshaller<int> {
  public:
    void marshall(const int& s, std::vector<char>& b) {
        protobuf::base_types bt;
        bt.set_i32(s);
#if GOOGLE_PROTOBUF_VERSION < 3004001
        b.resize(bt.ByteSize());
        bt.SerializePartialToArray(b.data(),bt.ByteSize());
#else
        b.resize(bt.ByteSizeLong());
        bt.SerializePartialToArray(b.data(),bt.ByteSizeLong());
#endif

    }

    int* unmarshall(const std::vector<char>& b) {
        protobuf::base_types bt;
        bt.ParseFromArray(b.data(),b.size());
        return new int(bt.i32());
    }
};
}} // namespace

#endif  /* ISPN_HOTROD_BasicTypesProtoStreamMarshaller_H */
