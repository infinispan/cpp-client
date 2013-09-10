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
    bool operator ==(InetSocketAddress &o1) {
          if(!o1.address.compare(address))
              return (o1.port == port);
          return 0;
    };

  private:
    std::string address;
    int port;
};

}}} // namespace infinispan::hotrod::transport

namespace std {

template<class T> struct less;

template<> struct less<infinispan::hotrod::transport::InetSocketAddress>
{
    bool operator() (
        const infinispan::hotrod::transport::InetSocketAddress& o1,
        const infinispan::hotrod::transport::InetSocketAddress& o2) const
    {
        int comparator = o1.getAddress().compare(o2.getAddress());

        if(comparator < 0) {
            return true;
        } else if(comparator == 0) {
            return o1.getPort()<o2.getPort();
        }

        return false;
    }
};

} // namespace std

#endif  /* ISPN_HOTROD_TRANSPORT_INETSOCKETADDRESS_H */
