#include "hotrod/impl/consistenthash/ConsistentHashV2.h"
#include "hotrod/impl/hash/MurmurHash3.h"
#include "infinispan/hotrod/types.h"

namespace infinispan {
namespace hotrod {
namespace consistenthash {


ConsistentHashV2::ConsistentHashV2() {
    hash = new MurmurHash3();
}

}}} // namespace infinispan::hotrod::consistenthash

