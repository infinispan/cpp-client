#ifndef ISPN_HOTROD_CONSISTENTHASHV2_H_
#define ISPN_HOTROD_CONSISTENTHASHV2_H_

#include "hotrod/impl/consistenthash/ConsistentHashV1.h"

namespace infinispan {
namespace hotrod {
namespace consistenthash {

class ConsistentHashV2: public ConsistentHashV1 {
public:

protected:
    ConsistentHashV2();
    friend class ConsistentHashFactory;

};
}}} // namespace infinispan::hotrod::consistenthash
#endif /* ISPN_HOTROD_CONSISTENTHASHV2_H_ */
