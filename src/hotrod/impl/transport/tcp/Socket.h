#ifndef ISPN_HOTROD_TRANSPORT_TCP_INPUTSTREAM_H
#define ISPN_HOTROD_TRANSPORT_TCP_INPUTSTREAM_H

#include "hotrod/sys/Socket.h"

#include <sstream>
#include <memory>

namespace infinispan {
namespace hotrod {
namespace transport {

class InputStream
{
  public:
    void read(char* buffer, size_t size);
    char read();
  private:
    static const size_t BufferSize = 1024;
    InputStream(sys::Socket& socket);
    sys::Socket& socket;
    char buffer[BufferSize];
    char* ptr;
    bool hasMore;
    size_t capacity;

  friend class Socket;
};

class OutputStream
{
  public:
    void write(const char *p, size_t n);
    void write(char c);
    void flush();
  private:
    OutputStream(sys::Socket& socket);
    sys::Socket& socket;
    std::ostringstream out;

  friend class Socket;
};

class Socket
{
  public:
    Socket(sys::Socket *_socket);
    Socket(const Socket& s);
    ~Socket() {};
    void connect(const std::string& host, int port, int timeout);
    void close();
    void setTcpNoDelay(bool tcpNoDelay);
    void setTimeout(int timeout);
    InputStream& getInputStream();
    OutputStream& getOutputStream();

	sys::Socket* getSocket() const {
		return socket.get();
	}
	void setSocket(sys::Socket* socket) {
		this->socket.reset(socket);
	}
	void setValid(bool valid)
	{
		socket->valid=valid;
		if (!valid)
			socket->valid=valid;
	}
	bool isValid()
	{
		return socket->valid;
	}
  private:
    std::shared_ptr<sys::Socket> socket;
    InputStream inputStream;
    OutputStream outputStream;

};

}}} // namespace infinispan::hotrod::transport::tcp

#endif  // ISPN_HOTROD_TRANSPORT_TCP_INPUTSTREAM_H

