#include "hotrod/impl/transport/tcp/Socket.h"
#include "hotrod/sys/Socket.h"
#include "hotrod/sys/Log.h"

#include <iostream>
#include <cstring>
#include <istream>
#include <sstream>


namespace infinispan {
namespace hotrod {

namespace transport {

Socket::Socket(sys::Socket *_socket) :
    socket(_socket), inputStream(*socket), outputStream(*socket)
{}

Socket::Socket(const Socket& s) : socket(s.socket), inputStream(*socket), outputStream(*socket)
{
	std::cout << "COPYING: Socket::Socket(const Socket& s)" << std::endl;
}

void Socket::connect(const std::string& host, int port, int timeout) {
    socket->connect(host, port, timeout);
}

void Socket::close() {
    socket->close();
//    socket.reset();
}

void Socket::setTcpNoDelay(bool tcpNoDelay) {
    socket->setTcpNoDelay(tcpNoDelay);
}

void Socket::setTimeout(int timeout) {
    socket->setTimeout(timeout);
}

InputStream& Socket::getInputStream() {
    return inputStream;
}

OutputStream& Socket::getOutputStream() {
    return outputStream;
}

void InputStream::read(char *buf, size_t size) {
    char *tmp_buffer = buf;
    while (capacity < size) {
        if (capacity != 0) {
            memcpy(tmp_buffer, ptr, capacity);
            tmp_buffer += capacity;
            size -= capacity;
        }
        capacity = socket.read(&buffer[0], size < BufferSize ? size : BufferSize);
        ptr = &buffer[0];
        hasMore = capacity < BufferSize ? false : true;
    }
    memcpy(tmp_buffer, ptr, size);
    ptr += size;
    capacity -= size;
}

char InputStream::read() {
    char c;
    read(&c, 1);
    return c;
}

InputStream::InputStream(sys::Socket& s) :
    socket(s), ptr(&buffer[0]), hasMore(true), capacity(0)
{}

OutputStream::OutputStream(sys::Socket& s) :
    socket(s)
{}

void OutputStream::write(const char *p, size_t n) {
    out.write(p,n);
}

void OutputStream::write(char c) {
    out.put(c);
}

void OutputStream::flush() {
    const std::string& outstr(out.str());
    socket.write(outstr.data(), outstr.size());
    out.str("");
}


}}} // namespace infinispan::hotrod::transport::tcp
