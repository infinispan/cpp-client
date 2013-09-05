#ifndef ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H
#define ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H



#include <string>

namespace infinispan {
namespace hotrod {
namespace transport {

class InetSocketAddress
{
  public:
    InetSocketAddress(const std::string& addr, int p) :
      address(addr), port(p) {}

    const std::string& getAddress() const { return address; }
    int getPort() const { return port; }
    bool operator ==(InetSocketAddress o1, InetSocketAddress o2) {
      if(!o1.address.compare(o2.address))
          return (o1.port == o2.port);
      return 0;
    };
  private:
    std::string address;
    int port;
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H */

namespace std {

template<class T> struct less;

template<> struct less<infinispan::hotrod::transport::InetSocketAddress>
{
    bool operator() (
        const infinispan::hotrod::transport::InetSocketAddress& o1,
        const infinispan::hotrod::transport::InetSocketAddress& o2) const
    {
        return &o1 < &o2;
    }
};

} // namespace std
