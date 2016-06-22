#include "infinispan/hotrod/exceptions.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"
#include "hotrod/sys/Socket.h"

#include <iostream>
#include <cstring>
#include <istream>
#include <sstream>


namespace infinispan {
namespace hotrod {
namespace transport {

TcpTransport::TcpTransport(
    const InetSocketAddress& a, TransportFactory& factory)
: AbstractTransport(factory), socket(sys::Socket::create()), invalid(false){
    serverAddress = new InetSocketAddress(a);
    socket.connect(a.getHostname(),a.getPort(), factory.getConnectTimeout());
    socket.setTimeout(factory.getSoTimeout());
    socket.setTcpNoDelay(factory.isTcpNoDelay());
}

TcpTransport::TcpTransport(
    const InetSocketAddress& a, TransportFactory& factory, sys::Socket *sock)
: AbstractTransport(factory), socket(sock), invalid(false){
    serverAddress = new InetSocketAddress(a);
    socket.connect(a.getHostname(),a.getPort(), factory.getConnectTimeout());
    socket.setTimeout(factory.getSoTimeout());
    socket.setTcpNoDelay(factory.isTcpNoDelay());
}

//Testing purpose only!
TcpTransport::TcpTransport()
: AbstractTransport(*(TransportFactory*)NULL), socket(sys::Socket::create()), invalid(false), serverAddress(){}

void TcpTransport::flush() {
    socket.getOutputStream().flush();
}

void TcpTransport::writeByte(uint8_t uchar) {
    socket.getOutputStream().write((char)uchar);
}

void TcpTransport::writeVInt(uint32_t uint) {
    while ((uint & ~0x7F) != 0) {
        char b = (uint & 0x7f) | 0x80;
        socket.getOutputStream().write(b);
        uint >>= 7;
    }
    socket.getOutputStream().write((char) uint);
}

void TcpTransport::writeVLong(uint64_t ulong) {
    while ((ulong & ~0x7F) != 0) {
        char b = (ulong & 0x7f) | 0x80;
        socket.getOutputStream().write(b);
        ulong >>= 7;
    }
    socket.getOutputStream().write((char) ulong);
}

void TcpTransport::writeBytes(const std::vector<char>& bytes) {
    std::vector<char>& not_const_bytes = const_cast<std::vector<char>&>(bytes);
    socket.getOutputStream().write(not_const_bytes.data(), not_const_bytes.size());
}

uint8_t TcpTransport::readByte() {
    char r = socket.getInputStream().read();
    return r;
}

uint32_t TcpTransport::readVInt() {
    uint8_t b = socket.getInputStream().read();
    int32_t i = b & 0x7F;
    for (int shift = 7; (b & 0x80) != 0; shift += 7) {
        b = socket.getInputStream().read();
        i |= (b & 0x7FL) << shift;
    }
    return i;
}

uint64_t TcpTransport::readVLong() {
    char b = socket.getInputStream().read();
    int64_t i = b & 0x7F;
    for (int shift = 7; (b & 0x80) != 0; shift += 7) {
        b = socket.getInputStream().read();
        i |= (b & 0x7FL) << shift;
    }
    return i;
}

std::vector<char> TcpTransport::readBytes(uint32_t size) {
    if (size) {
        std::vector<char> hrb(size);
        socket.getInputStream().read(hrb.data(), size);
        return hrb;
    }
    return std::vector<char>();
}

void TcpTransport::release() {
    try {
        socket.close();
    } catch (const Exception& e) {
        invalid = true;
        TRACE("Caught exception when closing socket: %s", e.what());
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
