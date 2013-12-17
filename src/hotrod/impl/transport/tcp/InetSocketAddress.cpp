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
    
    std::set<std::string> rhs_addresses = rhs.getAddresses();

    for (std::set<std::string>::iterator it = addresses.begin(); it != addresses.end(); ++it) {
        if (rhs_addresses.find(*it) != rhs_addresses.end()) {
            return true;
        }
    }
    return false;
}

bool InetSocketAddress::operator<(const InetSocketAddress& rhs) const {
    std::set<std::string> rhs_addresses = rhs.getAddresses();;

    if (addresses < rhs_addresses) {
        return true;
    } else if (addresses == rhs_addresses) {
        return getPort() < rhs.getPort();
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
