#include "infinispan/hotrod/exceptions.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"

#include <iostream>
#include <cstring>
#include <istream>
#include <sstream>


namespace infinispan {
namespace hotrod {
namespace transport {

// TODO

TcpTransport::TcpTransport() : socket(sys::Socket::create()), /*in(0),*/ inStr(*socket) {
    socket->connect();
}

void TcpTransport::flush() {
    std::string outstr(out.str());
    socket->write(outstr.data(), outstr.size());
}

void TcpTransport::writeByte(uint8_t uchar) {
    out.put((char) uchar);
}

void TcpTransport::writeVInt(uint32_t uint) {
	while ((uint & ~0x7F) != 0) {
        char b = (uint & 0x7f) | 0x80;
        out.put(b);
        uint >>= 7;
    }
    out.put((char) uint);
}

void TcpTransport::writeVLong(uint64_t ulong) {
	while ((ulong & ~0x7F) != 0) {
        char b = (ulong & 0x7f) | 0x80;
        out.put(b);
        ulong >>= 7;
    }
    out.put((char) ulong);
}

void TcpTransport::writeBytes(const hrbytes& bytes) {
	hrbytes& not_const_bytes = const_cast<hrbytes&>(bytes);
    out.write(not_const_bytes.bytes(),not_const_bytes.length());
}

uint8_t TcpTransport::readByte() {
/*
	if (!in) {
        in = new char[1024];
        //unsigned int bufsize = sizeof(in);
//        size_t n = socket->read(in, bufsize);
        //size_t n = socket->read(in, 1024);
        socket->read(in, 1024);
    }
*/
//	uint8_t r = inStr.read();
	char r;
	inStr >> r;
	return r;
}

uint32_t TcpTransport::readVInt() {
	uint8_t b = inStr.read();
    //char b = *in++;
    int32_t i = b & 0x7F;
    for (int shift = 7; (b & 0x80) != 0; shift += 7) {
        b = inStr.read();
        i |= (b & 0x7FL) << shift;
    }
    return i;
}

uint64_t TcpTransport::readVLong() {
    char b = inStr.read();
    int64_t i = b & 0x7F;
    for (int shift = 7; (b & 0x80) != 0; shift += 7) {
        b = inStr.read();
        i |= (b & 0x7FL) << shift;
    }
    return i;
}

void TcpTransport::readBytes(hrbytes& bytes, uint32_t size) {
    bytes.reserve(size);
    inStr.read(bytes.bytes(), size);

    return;
}

}}} /* namespace */
