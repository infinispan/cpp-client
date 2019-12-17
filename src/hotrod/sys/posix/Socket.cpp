#include "infinispan/hotrod/exceptions.h"
#include "hotrod/sys/Log.h"
#include "hotrod/sys/Inet.h"
#include "hotrod/sys/Socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <poll.h>

#include <errno.h>
#include <string.h>

#include <iostream>
#include <istream>
#include <sstream>
#include <stdexcept>

//For OSX portability
int sendFlag = 0;

#ifndef MSG_NOSIGNAL
#  define MSG_NOSIGNAL 0
#  ifdef SO_NOSIGPIPE
#    define USE_SO_NOSIGPIPE
#  else
#     error "Cannot block SIGPIPE!"
#  endif
#endif

namespace infinispan {
namespace hotrod {
namespace sys {

// Part of a straw man IO layer

namespace posix {

static int preferredIPStack = getPreferredIPStack();

class Socket: public infinispan::hotrod::sys::Socket {
  public:
    Socket();

    virtual ~Socket();
    virtual void connect(const std::string& host, int port, int timeout);
    virtual void close();
    virtual void setTcpNoDelay(bool tcpNoDelay);
    virtual void setTimeout(int timeout);
    virtual size_t read(char *p, size_t n);
    virtual void write(const char *p, size_t n);
    virtual int getSocket();
    std::string getHost() { return host; }
    int getPort() { return port; }
    void setFd(int fd) { this->fd=fd; }
  private:
    int fd;
    std::string host;
    int port;
};

namespace {

static void throwIOErr (const std::string& host, int port, const char *msg, int errnum) {
    std::ostringstream m;
    m << msg;
    m << " (host: " << host;
    m << " port: " << port << ")";

    if (errnum != 0) {
        char buf[200];
        if (strerror_r(errnum, buf, 200) == 0) {
            m << " " << buf;
        } else {
            m << " " << strerror(errnum);
        }
    }
    throw TransportException(host, port, m.str(), errnum);
}

} /* namespace */

Socket::Socket() : fd(-1), port(-1) {}

Socket::~Socket() { close(); }

/* Try a non blocking connect with a timeout */
static int attemptConnect(const char* ip, int port, int sock, int timeout,
		struct addrinfo* addr) {
	// Connect
	DEBUG("Attempting connection to %s:%d", ip, port);
#ifdef USE_SO_NOSIGPIPE
        int val = 1;
        int r = setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void*)&val, sizeof(val));
        if (r) {
                r = -errno;
		DEBUG("couldn't set SO_NOSIGPIPE. errno:%x %s:%d", r, ip, port);
        }
#endif
	int s = ::connect(sock, addr->ai_addr, addr->ai_addrlen);
	if (s < 0 && errno == EINPROGRESS) {
		pollfd fds[1];
		fds[0].fd = sock;
		fds[0].events = POLLOUT;
		auto evCount = poll(fds, 1, timeout);
		if (evCount > 0) {
			if ((POLLOUT ^ fds[0].revents) != 0) {
				DEBUG("Failed to connect to %s:%d", ip, port);
			} else {
				int opt;
				socklen_t optlen = sizeof(opt);
				s = getsockopt(sock, SOL_SOCKET, SO_ERROR, (void*) ((&opt)),
						&optlen);
			}
		} else if (evCount == 0) {
			DEBUG("Timed out connecting to %s:%d", ip, port);
		}
	}
	return s;
}

/* Connect to host using trying all the available addresses */
static bool connectToHost(Socket* usrSocket, bool preferred, int timeout, struct addrinfo* addr_list) {
	char ip[INET6_ADDRSTRLEN];
	int flags = 0, sock = 0;
	struct addrinfo *addr;
	for (addr = addr_list; addr != NULL; addr = addr->ai_next) {
		inet_ntop(addr->ai_family,
				get_in_addr((struct sockaddr*) (addr->ai_addr)), ip,
				sizeof(ip));
		if ((preferredIPStack == addr->ai_family) != preferred) {
			// Skip non-preferred addresses on the first run and preferred ones on the second one.
			continue;
		}
		TRACE("Trying to connect to %s (%s).", ip, usrSocket->getHost().c_str());
		sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (sock == -1) {
			DEBUG("Failed to obtain socket for address family %d",
					addr->ai_family);
			continue;
		}
		sendFlag = MSG_NOSIGNAL;
		// Make the socket non-blocking for the connection
		flags = fcntl(sock, F_GETFL, 0);
		fcntl(sock, F_SETFL, flags | O_NONBLOCK);
		// Connect
		int s = attemptConnect(ip, usrSocket->getPort(), sock, timeout, addr);
		if (s < 0) {
			::close(sock);
		} else {
			// We got a successful connection
			// Set to blocking mode again
			fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
			usrSocket->setFd(sock);
			return true;
		}
	}
	return false;
}

void Socket::connect(const std::string& h, int p, int timeout) {
	struct addrinfo *addr_list;

	host = h;
	port = (unsigned short)p;
	if (fd != -1)
		throwIOErr(host, port, "reconnect attempt", 0);

	int ec = getaddrinfo(host, port, &addr_list);
	if (ec) {
		std::ostringstream msg;
		msg << "Error while invoking getaddrinfo: " << gai_strerror(ec);
		throwIOErr(host, port, msg.str().c_str(), 0);
	}

	// Cycle through all returned addresses
	bool preferred = true;
	if (!connectToHost(this, preferred, timeout, addr_list)) {
		preferred = false;
		connectToHost(this, preferred, timeout, addr_list);
	}

	freeaddrinfo(addr_list);
	/* No address succeeded */
	if (fd == -1) {
		// TODO: the exception is build with the last errno. Maybe this need a fix
		throwIOErr(host, port, "Failed to connect", errno);
	}
	DEBUG("Connected to %s:%d", host.c_str(), port);
}

void Socket::close() {
    if (fd >= 0) {
        //::close(fd); prefer shutdown to avoid socket reuse
        ::shutdown(fd, SHUT_RDWR);
        fd = -1;
    }
}

void Socket::setTcpNoDelay(bool tcpNoDelay) {
    int flag = tcpNoDelay;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *) &flag, sizeof(flag)) < 0) {
        throwIOErr(host, port, "Failure setting TCP_NODELAY", errno);
    }
}

void Socket::setTimeout(int timeout) {
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv)) < 0) {
        throwIOErr(host, port, "Failure setting receive socket timeout", errno);
    }

    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv)) < 0) {
        throwIOErr(host, port, "Failure setting send socket timeout", errno);
    }
}

size_t Socket::read(char *p, size_t length) {
    ssize_t n =  recv(fd, p, length, 0);
    if (n > 0)
        return n;
    else if (n == 0)
        throwIOErr(host, port, "no read", 0);
    // On call interrupted try one more time
    if (errno == EINTR) {
        n =  recv(fd, p, length, 0);
        if (n > 0)
            return n;
        else if (n == 0)
            throwIOErr(host, port, "no read", 0);
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK)
        throwIOErr(host, port, "timeout", errno);
    else
        throwIOErr(host, port, "read", errno);
    return n;
}

void Socket::write(const char *p, size_t length) {
    ssize_t n = send(fd, p, length, sendFlag);
    if (n == -1) throwIOErr (host, port,"write", errno);
    if ((size_t) n != length) throwIOErr (host, port,"write error", 0);
}

int Socket::getSocket() {
    return fd;
}

} /* posix namespace */


Socket* Socket::create() {
    return new posix::Socket();
}


}}} /* namespace */
