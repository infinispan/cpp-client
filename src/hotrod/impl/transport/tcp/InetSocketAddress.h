#ifndef ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H
#define ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H

#include "infinispan/hotrod/ImportExport.h"

#include <set>
#include <string>
#include <iostream>

namespace infinispan {
namespace hotrod {
namespace transport {

class HR_EXTERN InetSocketAddress
{
  public:
    InetSocketAddress(const std::string& host, int p);
    InetSocketAddress(const InetSocketAddress& other);

    const std::string& getHostname() const;
    const std::set<std::string>& getAddresses() const;
    int getPort() const;

    bool operator ==(const InetSocketAddress& rhs) const;
    bool operator <(const InetSocketAddress& rhs) const;

    friend std::ostream& operator<<(std::ostream& os, const InetSocketAddress& isa);

  private:
    std::string hostname;
    std::set<std::string> addresses;
    int port;

    bool isSameHost(const std::set<std::string>& lhs, const std::set<std::string>& rhs) const;
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H */
