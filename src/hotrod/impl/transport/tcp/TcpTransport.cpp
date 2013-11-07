

#include "infinispan/hotrod/exceptions.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"

#include <iostream>
#include <cstring>
#include <istream>
#include <sstream>


namespace infinispan {
namespace hotrod {
namespace transport {

TcpTransport::TcpTransport(
    const InetSocketAddress& a, TransportFactory& factory)
: AbstractTransport(factory), socket(), /*inStr(*socket),*/ invalid(false){
    serverAddress = new InetSocketAddress(a.getAddress(), a.getPort());
    socket.connect(a.getAddress(),a.getPort(), factory.getConnectTimeout());
    socket.setTimeout(factory.getSoTimeout());
    socket.setTcpNoDelay(factory.isTcpNoDelay());
}

void TcpTransport::flush() {
    socket.getOutputStream().flush();

    //std::string outstr(out.str());
    //socket->write(outstr.data(), outstr.size());
}

void TcpTransport::writeByte(uint8_t uchar) {
    //out.put((char) uchar);
	socket.getOutputStream().write((char)uchar);
}

void TcpTransport::writeVInt(uint32_t uint) {
	while ((uint & ~0x7F) != 0) {
        char b = (uint & 0x7f) | 0x80;
        //out.put(b);
        socket.getOutputStream().write(b);
        uint >>= 7;
    }
    //out.put((char) uint);
	socket.getOutputStream().write((char) uint);
}

void TcpTransport::writeVLong(uint64_t ulong) {
	while ((ulong & ~0x7F) != 0) {
        char b = (ulong & 0x7f) | 0x80;
        //out.put(b);
        socket.getOutputStream().write(b);
        ulong >>= 7;
    }
    //out.put((char) ulong);
	socket.getOutputStream().write((char) ulong);
}

void TcpTransport::writeBytes(const hrbytes& bytes) {
	hrbytes& not_const_bytes = const_cast<hrbytes&>(bytes);
    //out.write(not_const_bytes.bytes(),not_const_bytes.length());
	socket.getOutputStream().write(not_const_bytes.bytes(),not_const_bytes.length());
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
	//char r;
	//inStr >> r;
	char r = socket.getInputStream().read();
	return r;
}

uint32_t TcpTransport::readVInt() {
	//uint8_t b = inStr.read();
	uint8_t b = socket.getInputStream().read();
    //char b = *in++;
    int32_t i = b & 0x7F;
    for (int shift = 7; (b & 0x80) != 0; shift += 7) {
        //b = inStr.read();
        b = socket.getInputStream().read();
        i |= (b & 0x7FL) << shift;
    }
    return i;
}

uint64_t TcpTransport::readVLong() {
    //char b = inStr.read();
    char b = socket.getInputStream().read();
    int64_t i = b & 0x7F;
    for (int shift = 7; (b & 0x80) != 0; shift += 7) {
        //b = inStr.read();
        b = socket.getInputStream().read();
        i |= (b & 0x7FL) << shift;
    }
    return i;
}

void TcpTransport::readBytes(hrbytes& bytes, uint32_t size) {
	if (size) {
	    bytes.reserve(size);
	    //inStr.read(bytes.bytes(), size);
	    socket.getInputStream().read(bytes.bytes(), size);
	}

    return;
}

void TcpTransport::release() {
    try {
        socket.close();
    } catch(Exception& e) {
        invalid = true;
        // log
    }
}

void TcpTransport::invalidate() {
    invalid = true;
}

void TcpTransport::destroy() {
    socket.close();
    delete serverAddress;
}

bool TcpTransport::isValid(){
    return !invalid;
}

const InetSocketAddress& TcpTransport::getServerAddress() {
   return *serverAddress;
}

}}} /* namespace */
