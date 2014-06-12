#ifndef ISPN_HOTROD_HASH_H
#define ISPN_HOTROD_HASH_H

#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/defs.h"

namespace infinispan {
namespace hotrod {
/**
 * Consistent hashing interface. Two implementations are provided by default,
 * MurmurHash2 and MurmurHash3. Clients are not expected to provide
 * additional implementations.
 *
 *
 * See https://sites.google.com/site/murmurhash/ for more details.
 */
class Hash {
public:
    virtual ~Hash() {};

    virtual uint32_t hash(const void *key, size_t size) const = 0;
    virtual uint32_t hash(int32_t key) const = 0;
};

}} // namespace

#endif  /* ISPN_HOTROD_HASH_H */
