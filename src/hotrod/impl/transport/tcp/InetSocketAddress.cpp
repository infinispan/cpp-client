#include "hotrod/impl/transport/tcp/InetSocketAddress.h"

#include "hotrod/sys/Inet.h"

using namespace infinispan::hotrod::sys;

namespace infinispan {
namespace hotrod {
namespace transport {

InetSocketAddress::InetSocketAddress(const std::string& host, int p): hostname(host), port(p) {
    addresses = resolve(host, true);
}

InetSocketAddress::InetSocketAddress(const InetSocketAddress& other): hostname(other.hostname), addresses(other.addresses), port(other.port) {
}

const std::string& InetSocketAddress::getHostname() const {
    return hostname;
}

const std::set<std::string>& InetSocketAddress::getAddresses() const {
    return addresses;
}

int InetSocketAddress::getPort() const {
    return port;
}

bool InetSocketAddress::operator==(const InetSocketAddress& rhs) const {
    if (port != rhs.getPort()) {
        return false;
    }
    return isSameHost(this->getAddresses(), rhs.getAddresses());
}

bool InetSocketAddress::operator<(const InetSocketAddress& rhs) const {
    if (isSameHost(this->getAddresses(), rhs.getAddresses())) {
        return getPort() < rhs.getPort();
    }
    return addresses < rhs.getAddresses();
}

bool InetSocketAddress::isSameHost(const std::set<std::string>& lhs, const std::set<std::string>& rhs) const {
    for (std::set<std::string>::iterator it = lhs.begin(); it != lhs.end(); ++it) {
        if (rhs.find(*it) != rhs.end()) {
            return true;
        }
    }
    return false;
}

std::ostream& operator<<(std::ostream& os, const InetSocketAddress& isa) {
    os << "InetSocketAddress[hostname=" << isa.hostname << ", port=" << isa.port << ", addresses=";

    bool first = true;
    for (std::set<std::string>::const_iterator it = isa.addresses.begin(); it != isa.addresses.end(); ++it) {
        if (first) {
           first = false;
        } else {
           os << ',';
        }
        os << *it;
    }
    os << "]";
    return os;
}

}}} // namespace
