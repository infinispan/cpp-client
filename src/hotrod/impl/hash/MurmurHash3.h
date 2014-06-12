#ifndef ISPN_HOTROD_MURMURMHASH3_H
#define ISPN_HOTROD_MURMURMHASH3_H

#include "hotrod/impl/hash/Hash.h"

namespace infinispan {
namespace hotrod {

class MurmurHash3: public Hash {
public:
    ~MurmurHash3();

    uint32_t hash(const void *key, size_t size) const;
    uint32_t hash(int32_t key) const;

private:
    void read_int(unsigned char *results, int32_t num) const;
};

}} // namespace

#endif  /* ISPN_HOTROD_MURMURMHASH3_H */
