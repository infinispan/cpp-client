#ifndef ISPN_HOTROD_HASH_H
#define ISPN_HOTROD_HASH_H

#include "infinispan/hotrod/types.h"
#include "infinispan/hotrod/ImportExport.h"

namespace infinispan {
namespace hotrod {

class HR_EXTERN Hash {
public:
    virtual ~Hash() {};

    virtual uint32_t hash(const hrbytes& key) const = 0;
};

}} // namespace

#endif  /* ISPN_HOTROD_HASH_H */
