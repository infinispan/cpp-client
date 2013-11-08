#include "hotrod/impl/transport/tcp/InetSocketAddress.h"

namespace infinispan {
namespace hotrod {
namespace transport {

std::ostream& operator<<(std::ostream& os, const InetSocketAddress& isa) {
    os << "InetSocketAddress[address=" << isa.address << ", port=" << isa.port << "]";
    return os;
}

}}} // namespace
