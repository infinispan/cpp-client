#include "hotrod/impl/hash/MurmurHash2.h"

namespace infinispan {
namespace hotrod {

//-----------------------------------------------------------------------------
// MurmurHashNeutral2, by Austin Appleby

// Same as MurmurHash2, but endian- and alignment-neutral.
// Half the speed though, alas.

uint32_t MurmurHashNeutral2(const void * key, int len) {
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    uint32_t h = -1;

    const unsigned char * data = (const unsigned char *) key;

    while (len >= 4) {
        uint32_t k;

        k = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
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

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

MurmurHash2::~MurmurHash2() {}

uint32_t MurmurHash2::hash(const hrbytes& key) const {
    hrbytes& k = const_cast<hrbytes&>(key);
    return MurmurHashNeutral2(k.bytes(), k.length());
}

uint32_t MurmurHash2::hash(int32_t key) const {
    unsigned char buffer[4] = { 0 };
    read_int(buffer, key);
    return MurmurHashNeutral2(buffer, 4);
}

void MurmurHash2::read_int(unsigned char *results, int32_t num) const {
    int32_t count = 0;
    int32_t len = sizeof(int32_t);
    char *num_bytes = (char*) &num;
    for (count = 0; count <len; count++) {
        results[count] = num_bytes[count];
    }
}

}} // namespace

