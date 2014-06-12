#ifndef ISPN_HOTROD_MURMURMHASH2_H
#define ISPN_HOTROD_MURMURMHASH2_H

#include "hotrod/impl/hash/Hash.h"

namespace infinispan {
namespace hotrod {

/**
 * Note that the actual implementation is not "THE MurmurHash2" - differences
 * were introduced in Java implementation on the server, and this class
 * mirrors the Java behavior for backwards compatibility.
 */
class MurmurHash2: public Hash {
public:
    ~MurmurHash2();

    uint32_t hash(const void *key, size_t size) const;
    uint32_t hash(int32_t key) const;

private:
    void read_int(unsigned char *results, int32_t num) const;
};

}} // namespace

#endif  /* ISPN_HOTROD_MURMURMHASH2_H */
