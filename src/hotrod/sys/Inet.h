#ifndef ISPN_HOTROD_SYS_INET_H
#define ISPN_HOTROD_SYS_INET_H

#if defined(WIN32) || defined(_WIN32)

#include <Ws2tcpip.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#endif

#include <set>
#include <string>

namespace infinispan {
namespace hotrod {
namespace sys {

int getPreferredIPStack();
void *get_in_addr(struct sockaddr *sa);
int getaddrinfo(const std::string& host, int port, struct addrinfo **addr_list);

std::set<std::string> resolve(const std::string& hostname, bool no_throw);
std::set<std::string> resolve(const std::string& hostname);

}}} // namespace

#endif  /* ISPN_HOTROD_SYS_INET_H */
