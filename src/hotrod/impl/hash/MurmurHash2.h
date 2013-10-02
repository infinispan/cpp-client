#ifndef ISPN_HOTROD_MURMURMHASH2_H
#define ISPN_HOTROD_MURMURMHASH2_H

#include "infinispan/hotrod/Hash.h"

namespace infinispan {
namespace hotrod {

class HR_EXTERN MurmurHash2: public Hash {
public:
    ~MurmurHash2();

    uint32_t hash(const hrbytes& key) const;
};

}} // namespace

#endif  /* ISPN_HOTROD_MURMURMHASH2_H */
