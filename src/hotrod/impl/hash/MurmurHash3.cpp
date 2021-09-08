#include "hotrod/impl/hash/MurmurHash3.h"

namespace infinispan {
namespace hotrod {

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER)

#define FORCE_INLINE    __forceinline

#include <stdlib.h>

#define ROTL32(x,y)     _rotl(x,y)
#define ROTL64(x,y)     _rotl64(x,y)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else   // defined(_MSC_VER)

#define FORCE_INLINE inline __attribute__((always_inline))

inline uint32_t rotl32(uint32_t x, int8_t r) {
    return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64(uint64_t x, int8_t r) {
    return (x << r) | (x >> (64 - r));
}

#define ROTL32(x,y)     rotl32(x,y)
#define ROTL64(x,y)     rotl64(x,y)

#define BIG_CONSTANT(x) (x##LL)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

FORCE_INLINE int64_t getblock64(const int64_t * p, int i) {
    return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

FORCE_INLINE uint64_t fmix64(uint64_t k) {
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;

    return k;
}

//-----------------------------------------------------------------------------


uint64_t MurmurHash3_x64_64(const void * key, const int32_t len, const int32_t seed) {
    // Exactly the same as MurmurHash3_x64_128, except it only returns state.h1
    const int8_t * data = (const int8_t*) key;
    const int nblocks = len / 16;

    int64_t h1 = BIG_CONSTANT(0x9368e53c2f6af274) ^ seed;
    int64_t h2 = BIG_CONSTANT(0x586dcd208f7cd3fd) ^ seed;

    int64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
    int64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

    const int64_t * blocks = (const int64_t *) (data);

    for (int i = 0; i < nblocks; i++) {
        int64_t k1 = getblock64(blocks, i * 2 + 0);
        int64_t k2 = getblock64(blocks, i * 2 + 1);

        // bmix

        k1 *= c1;
        k1 = ROTL64(k1, 23);
        k1 *= c2;
        h1 ^= k1;
        h1 += h2;

        h2 = ROTL64(h2, 41);


        k2 *= c2;
        k2 = ROTL64(k2, 23);
        k2 *= c1;
        h2 ^= k2;
        h2 += h1;

        h1 = h1 * 3 + 0x52dce729;
        h2 = h2 * 3 + 0x38495ab5;

        c1 = c1 * 5 + 0x7b7d159c;
        c2 = c2 * 5 + 0x6bce6396;
    }

    //----------
    // tail

    const int8_t * tail = (const int8_t*) (data + nblocks * 16);

    int64_t k1 = 0;
    int64_t k2 = 0;

    switch (len & 15) {
    case 15:
        k2 ^= ((int64_t) tail[14]) << 48;
	// fall through
    case 14:
        k2 ^= ((int64_t) tail[13]) << 40;
	// fall through
    case 13:
        k2 ^= ((int64_t) tail[12]) << 32;
	// fall through
    case 12:
        k2 ^= ((int64_t) tail[11]) << 24;
	// fall through
    case 11:
        k2 ^= ((int64_t) tail[10]) << 16;
	// fall through
    case 10:
        k2 ^= ((int64_t) tail[9]) << 8;
	// fall through
    case 9:
        k2 ^= ((int64_t) tail[8]) << 0;
	// fall through
    case 8:
        k1 ^= ((int64_t) tail[7]) << 56;
	// fall through
    case 7:
        k1 ^= ((int64_t) tail[6]) << 48;
	// fall through
    case 6:
        k1 ^= ((int64_t) tail[5]) << 40;
	// fall through
    case 5:
        k1 ^= ((int64_t) tail[4]) << 32;
	// fall through
    case 4:
        k1 ^= ((int64_t) tail[3]) << 24;
	// fall through
    case 3:
        k1 ^= ((int64_t) tail[2]) << 16;
	// fall through
    case 2:
        k1 ^= ((int64_t) tail[1]) << 8;
	// fall through
    case 1:
        k1 ^= ((int64_t) tail[0]) << 0;
    };

    if ((len & 15) != 0) {
        // bmix
        k1 *= c1;
        k1 = ROTL64(k1, 23);
        k1 *= c2;
        h1 ^= k1;
        h1 += h2;

        h2 = ROTL64(h2, 41);

        k2 *= c2;
        k2 = ROTL64(k2, 23);
        k2 *= c1;
        h2 ^= k2;
        h2 += h1;

        h1 = h1 * 3 + 0x52dce729;
        h2 = h2 * 3 + 0x38495ab5;

        c1 = c1 * 5 + 0x7b7d159c;
        c2 = c2 * 5 + 0x6bce6396;
    }

    //----------
    // finalization
    h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 = fmix64(h1);
    h2 = fmix64(h2);

    h1 += h2;
    h2 += h1;

    return h1;
}

int32_t MurmurHash3_x64_32(const void * key, const int32_t len, const int32_t seed) {
    return (int32_t) ((uint64_t) MurmurHash3_x64_64(key, len, seed) >> 32);
}

// Method declarations

uint32_t MurmurHash3::hash(const void *key, size_t size) {
    return (uint32_t) MurmurHash3_x64_32(key, (int32_t)size, 9001);
}

uint32_t MurmurHash3::hash(int32_t key) {
	// Obtained by inlining MurmurHash3_x64_32(byte[], 9001) and removing all the unused code
	// (since we know the input is always 4 bytes and we only need 4 bytes of output)
	int8_t b0 = (int8_t) key;
	int8_t b1 = (int8_t) ((uint32_t) key >> 8);
	int8_t b2 = (int8_t) ((uint32_t) key >> 16);
	int8_t b3 = (int8_t) ((uint32_t) key >> 24);

	int64_t h1 = BIG_CONSTANT(0x9368e53c2f6af274) ^ 9001;
	int64_t h2 = BIG_CONSTANT(0x586dcd208f7cd3fd) ^ 9001;

	int64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
	int64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

	int64_t k1 = 0;
	int64_t k2 = 0;

	k1 ^= (int64_t) b3 << 24;
	k1 ^= (int64_t) b2 << 16;
	k1 ^= (int64_t) b1 << 8;
	k1 ^= b0;

	// bmix
	k1 *= c1;
	k1 = ROTL64(k1, 23);
	k1 *= c2;
	h1 ^= k1;
	h1 += h2;

	h2 = ROTL64(h2, 41);

	k2 *= c2;
	k2 = ROTL64(k2, 23);
	k2 *= c1;
	h2 ^= k2;
	h2 += h1;

	h1 = h1 * 3 + 0x52dce729;
	h2 = h2 * 3 + 0x38495ab5;

	c1 = c1 * 5 + 0x7b7d159c;
	c2 = c2 * 5 + 0x6bce6396;

	h2 ^= 4;

	h1 += h2;
	h2 += h1;

	h1 = fmix64(h1);
	h2 = fmix64(h2);

	h1 += h2;
	h2 += h1;

	return (int32_t) ((uint64_t) h1 >> 32);
}

}} // namespace
