#ifndef ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H
#define ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H


#include <string>
#include <iostream>

namespace infinispan {
namespace hotrod {
namespace transport {

class InetSocketAddress
{
  public:
    InetSocketAddress(const std::string& addr, int p): address(addr), port(p) {}
    InetSocketAddress(const InetSocketAddress& other): address(other.address), port(other.port) {}

    const std::string& getAddress() const { return address; }
    int getPort() const { return port; }
    bool operator ==(const InetSocketAddress & rhs) {
        return getAddress().compare(rhs.getAddress()) == 0 && getPort() == rhs.getPort();
    }

    bool operator <(const InetSocketAddress & rhs) const {
        int comparator = getAddress().compare(rhs.getAddress());
        if (comparator < 0) {
            return true;
        } else if (comparator == 0) {
            return getPort() < rhs.getPort();
        }
        return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const InetSocketAddress& isa);

  private:
    std::string address;
    int port;
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H */
