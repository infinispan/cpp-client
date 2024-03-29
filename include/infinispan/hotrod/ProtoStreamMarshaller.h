#ifndef ISPN_HOTROD_PROTOSTREAMMARSHALLER_H
#define ISPN_HOTROD_PROTOSTREAMMARSHALLER_H


#include <string>
#include <iostream>
#include "infinispan/hotrod/Marshaller.h"
#include "infinispan/hotrod/exceptions.h"
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable:4267 4244)
#endif
#include "infinispan/hotrod/message-wrapping.pb.h"
#if _MSC_VER
#pragma warning(pop)
#endif

using namespace org::infinispan::protostream;

namespace infinispan {
namespace hotrod {

/**
 * A Marshaller based on Google Protobuf and on the WrappedMessage .proto specification
 */
template <class T, unsigned int TypeId = 1000042> class ProtoStreamMarshaller : public infinispan::hotrod::Marshaller<T> {
    void marshall(const T& obj, std::vector<char>& b) {
#if GOOGLE_PROTOBUF_VERSION < 3004001
        std::vector<char> msg(obj.ByteSize());
        obj.SerializeToArray(msg.data(),obj.ByteSize());
#else
        std::vector<char> msg(obj.ByteSizeLong());
        obj.SerializeToArray(msg.data(),obj.ByteSizeLong());
#endif
        WrappedMessage wm;
        wm.set_wrappedmessagebytes(msg.data(), msg.size());
        wm.set_wrappeddescriptorid(TypeId);
#if GOOGLE_PROTOBUF_VERSION < 3004001
        b.resize(wm.ByteSize());
        wm.SerializeToArray(b.data(),wm.ByteSize());
#else
        b.resize(wm.ByteSizeLong());
        wm.SerializeToArray(b.data(),wm.ByteSizeLong());
#endif
    }

    T* unmarshall(const std::vector<char>& b) {
        WrappedMessage wm;
        wm.ParseFromArray(b.data(),b.size());
        const std::string &wmb=wm.wrappedmessagebytes();
        auto *bt = new T();
        bt->ParseFromArray(wmb.data(),wmb.size());
        return bt;
    }

};

template <class T> class ProtoStreamMarshallerHelper {
public:
    static void noRelease(std::vector<char>*) { /* nothing allocated, nothing to release */ }

    static void release(std::vector<char> *buf) {
        delete buf->data();
    }
    static T unmarshall(char *b, size_t size) {
        WrappedMessage wm;
        wm.ParseFromArray(b, size);
        const std::string &wmb=wm.wrappedmessagebytes();
        T bt;
        bt.ParseFromArray(wmb.data(),wmb.size());
        return bt;
    }};

}} // namespace

#endif  /* ISPN_HOTROD_ProtoStreamMarshaller_H */
