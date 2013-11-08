#include "hotrod/impl/consistenthash/ConsistentHashV2.h"
#include "hotrod/impl/hash/MurmurHash3.h"
#include "infinispan/hotrod/types.h"

namespace infinispan {
namespace hotrod {
namespace consistenthash {

ConsistentHashV2::ConsistentHashV2(): ConsistentHashV1(new MurmurHash3()) {}

}}} // namespace infinispan::hotrod::consistenthash

