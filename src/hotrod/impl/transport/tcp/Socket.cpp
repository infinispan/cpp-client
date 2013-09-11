

#include "hotrod/impl/transport/tcp/Socket.h"
#include "hotrod/sys/Socket.h"

#include <iostream>
#include <cstring>
#include <istream>
#include <sstream>


namespace infinispan {
namespace hotrod {

namespace transport {

// TODO

Socket::Socket() :
    socket(*sys::Socket::create()), inputStream(socket), outputStream(socket)
{}

void Socket::connect(const std::string& host, int port) {
	std::cout << "host " << host << " port " << port << std::endl;
    socket.connect(host, port);
}

void Socket::close() {
    socket.close();
}

InputStream& Socket::getInputStream() {
    return inputStream;
}

OutputStream& Socket::getOutputStream() {
    return outputStream;
}

void InputStream::read(char* buf, size_t size) {
	char* tmp_buffer = buf;
	  while (capacity < size) {
	    if (capacity != 0) {
	      memcpy(tmp_buffer, ptr, capacity);
	      tmp_buffer += capacity;
	      size -= capacity;
	    }
	    //if (!hasMore)
	      //throw HotRodClientException("stream not complete");
	    capacity = socket.read(&buffer[0],BufferSize);
	    ptr = &buffer[0];
	    hasMore = capacity < BufferSize ? false : true;
	  }
	  memcpy(tmp_buffer, ptr, size);
	  ptr += size;
	  capacity -= size;
}

char InputStream::read() {
	  char c;
	  read(&c,1);
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
