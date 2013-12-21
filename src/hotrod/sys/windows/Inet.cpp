#include "hotrod/sys/Inet.h"
#include "infinispan/hotrod/exceptions.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <sstream>

#include <iostream>

namespace infinispan {
namespace hotrod {
namespace sys {

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int getPreferredIPStack() {
    const char *stack = getenv("HOTROD_IPSTACK");
    if (stack == 0) {
        return AF_UNSPEC;
    }  else if (!_stricmp(stack, "IPV4")) {
        return AF_INET;
    } else if (!_stricmp(stack, "IPV6")) {
        return AF_INET6;
    } else {
        throw std::runtime_error("Invalid HOTROD_IPSTACK environment variable");
    }
}

static int preferredIPStack = getPreferredIPStack();

int getaddrinfo(const std::string& host, int port, struct addrinfo **addr_list) {
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = preferredIPStack;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;

    std::ostringstream ostr;
    ostr << port;

    return getaddrinfo(host.c_str(), ostr.str().c_str(), &hints, addr_list);
}

std::set<std::string> resolve(const std::string& hostname, bool no_throw) {
    std::set<std::string> result;

    struct addrinfo *addr_list = 0;

    int ec = getaddrinfo(hostname, 0, &addr_list);
    if (ec) {
        if (addr_list) {
            freeaddrinfo(addr_list);
        }
        if (no_throw) {
            result.insert(hostname);
        } else {
            std::ostringstream msg;
            msg << "Failed to resolve: " << hostname << " error: " << gai_strerror(ec);
            throw Exception(msg.str());
        }
    } else {
        char ip[INET6_ADDRSTRLEN];
        struct addrinfo *addr;
        for (addr = addr_list; addr != NULL; addr = addr->ai_next) {
            inet_ntop(addr->ai_family, get_in_addr((struct sockaddr *)addr->ai_addr), ip, sizeof(ip));
            result.insert(ip);
        }
        freeaddrinfo(addr_list);
    }

    return result;
}

std::set<std::string> resolve(const std::string& hostname) {
    return resolve(hostname, false);
}

}}} // namespace
