#include "hotrod/impl/hash/MurmurHash2.h"

namespace infinispan {
namespace hotrod {

//-----------------------------------------------------------------------------
// MurmurHashNeutral2, by Austin Appleby

// Same as MurmurHash2, but endian- and alignment-neutral.
// Half the speed though, alas.

uint32_t MurmurHashNeutral2(const void * key, int len) {
    const int32_t m = 0x5bd1e995;
    const int r = 24;

    int32_t h = -1;

    const int8_t * data = (const int8_t *) key;

    while (len >= 4) {
        int32_t k;

        k = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= ((uint32_t) k) >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len) {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    };

    h ^= ((uint32_t) h) >> 13;
    h *= m;
    h ^= ((uint32_t) h) >> 15;

    return h;
}

MurmurHash2::~MurmurHash2() {}

uint32_t MurmurHash2::hash(const hrbytes& key) const {
	hrbytes& k = const_cast<hrbytes&>(key);
    return MurmurHashNeutral2(k.bytes(), k.length());
}

uint32_t MurmurHash2::hash(int32_t key) const {
	return MurmurHashNeutral2(&key, sizeof(int32_t));
}

}} // namespace

