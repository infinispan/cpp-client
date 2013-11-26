#include "hotrod/impl/hash/MurmurHash2.h"
#include "hotrod/impl/hash/MurmurHash3.h"

#include <iostream>
#include <string>
#include <assert.h>

using namespace infinispan::hotrod;

#define UNUSED(x) ((void)x)

void testHashConsistency(const Hash& hash) {
    hrbytes data(const_cast<char*>("acbde"), 5);
    uint32_t i1 = hash.hash(data);
    uint32_t i2 = hash.hash(data);
    uint32_t i3 = hash.hash(data);

    assert(i1 == i2);
    assert(i3 == i2);
    assert(i1 == i3);

    UNUSED(i1);
    UNUSED(i2);
    UNUSED(i3);
}

void murmurHash2Test() {
    MurmurHash2 murmur2;
    testHashConsistency(murmur2);
    std::cout << "MurmurHash2 passed consistency test" << std::endl;
}

void murmurHash3Test() {
    MurmurHash3 murmur3;
    testHashConsistency(murmur3);
    std::cout << "MurmurHash3 passed consistency test" << std::endl;
}

int main(int, char**) {
    murmurHash2Test();
    murmurHash3Test();
    return 0;
}
