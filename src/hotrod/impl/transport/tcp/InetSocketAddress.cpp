#include <infinispan/hotrod/InetSocketAddress.h>
#include "hotrod/sys/Inet.h"

using namespace infinispan::hotrod::sys;

namespace infinispan {
namespace hotrod {
namespace transport {

HR_EXPORT InetSocketAddress::InetSocketAddress(const std::string& host, int p): hostname(host), port((unsigned short)p) {
    addresses = resolve(host, true);
}


HR_EXPORT const std::string& InetSocketAddress::getHostname() const {
    return hostname;
}

const std::set<std::string>& InetSocketAddress::getAddresses() const {
    return addresses;
}

HR_EXPORT int InetSocketAddress::getPort() const {
    return port;
}

bool InetSocketAddress::operator==(const InetSocketAddress& rhs) const {
    if (port != rhs.getPort()) {
        return false;
    }
    return hostname.compare(rhs.hostname) ? isSameHost(this->getAddresses(), rhs.getAddresses()) : true;
}

HR_EXPORT bool InetSocketAddress::operator<(const InetSocketAddress& rhs) const {
	if (hostname==rhs.hostname)
		return port< rhs.getPort();
	return hostname<rhs.hostname;
}

HR_EXPORT bool InetSocketAddress::isSameHost(const std::set<std::string>& lhs, const std::set<std::string>& rhs) const {
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

InetSocketAddress& InetSocketAddress::operator=(infinispan::hotrod::transport::InetSocketAddress const& r){
	//addresses=r.addresses;
	hostname=r.hostname;
	port=r.port;
	return *this;
}
}}} // namespace
