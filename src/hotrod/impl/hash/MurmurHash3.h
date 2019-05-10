#ifndef ISPN_HOTROD_MURMURMHASH3_H
#define ISPN_HOTROD_MURMURMHASH3_H

#include "infinispan/hotrod/defs.h"

namespace infinispan {
namespace hotrod {

class MurmurHash3 {
public:
    static uint32_t hash(const void *key, size_t size);
    static uint32_t hash(int32_t key);

private:
    static void read_int(unsigned char *results, int32_t num);
};

}} // namespace

#endif  /* ISPN_HOTROD_MURMURMHASH3_H */
