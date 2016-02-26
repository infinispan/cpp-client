#ifndef ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H
#define ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H

#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/ServerNameId.h"

#include <set>
#include <string>
#include <iostream>

namespace infinispan {
namespace hotrod {
namespace transport {

class InetSocketAddress
{
  public:
	InetSocketAddress() : hostname(""), port(0) {};
    HR_EXPORT InetSocketAddress(const std::string& host, int p);
    InetSocketAddress(const InetSocketAddress& other): hostname(other.hostname), addresses(other.addresses), port(other.port) {
    }

    HR_EXPORT const std::string& getHostname() const;
    HR_EXPORT const std::set<std::string>& getAddresses() const;
    HR_EXPORT int getPort() const;

    HR_EXPORT bool operator ==(const InetSocketAddress& rhs) const;
    HR_EXPORT bool operator <(const InetSocketAddress& rhs) const;
    HR_EXPORT InetSocketAddress& operator=(infinispan::hotrod::transport::InetSocketAddress const&);
    HR_EXPORT friend std::ostream& operator<<(std::ostream& os, const InetSocketAddress& isa);
    bool isEmpty() { return port==0 && hostname.empty(); }

    operator ServerNameId () const
		{
    	  return std::string(getHostname()+":"+(std::to_string(getPort())));
		}
  private:
    std::string hostname;
    std::set<std::string> addresses;
    int port;

    HR_EXPORT bool isSameHost(const std::set<std::string>& lhs, const std::set<std::string>& rhs) const;
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H */
