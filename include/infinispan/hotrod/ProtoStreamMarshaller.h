#ifndef ISPN_HOTROD_PROTOSTREAMMARSHALLER_H
#define ISPN_HOTROD_PROTOSTREAMMARSHALLER_H


#include <string>
#include <iostream>
#include "infinispan/hotrod/Marshaller.h"
#include "infinispan/hotrod/exceptions.h"
#include "message-wrapping.pb.h"

using namespace org::infinispan::protostream;

namespace infinispan {
namespace hotrod {

/*
 * A Marshaller for a few simple types that pretends to be compatible with JBoss Marshaller.
 * See below the Helper class for a list of the managed types.
 */
template <class T> class ProtoStreamMarshaller : public infinispan::hotrod::Marshaller<T> {
    void marshall(const T& obj, std::vector<char>& b) {
    	std::vector<char> msg(obj.ByteSize());
    	obj.SerializeToArray(msg.data(),obj.ByteSize());
    	WrappedMessage wm;
    	wm.set_wrappedmessagebytes(msg.data(), msg.size());
    	wm.set_wrappeddescriptorid(42);
    	b.resize(wm.ByteSize());
    	wm.SerializeToArray(b.data(),wm.ByteSize());
    }

    T* unmarshall(const std::vector<char>& b) {
    	auto *bt = new T();
    	bt->ParseFromArray(b.data(),b.size());
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
        	T bt;
        	bt.ParseFromArray(b,size);
        	return bt;
    }};

}} // namespace

#endif  /* ISPN_HOTROD_ProtoStreamMarshaller_H */
