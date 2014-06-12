#include "infinispan/hotrod/ImportExport.h"

/* The tests using internal classes are compiled directly into the shared library/DLL,
   this binary just runs them */

HR_EXTERN void threadTest();
HR_EXTERN void syncTest();
HR_EXTERN void runOnceTest();
HR_EXTERN void testTopologyChangeResponse();
HR_EXTERN void updateServersTest();
HR_EXTERN void consistentHashFactoryTests();
HR_EXTERN bool murmurHash2StringTest();
HR_EXTERN bool murmurHash3StringTest();
HR_EXTERN bool murmurHash2IntTest();
HR_EXTERN bool murmurHash3IntTest();

int main(int, char**) {
    threadTest();
    syncTest();
    runOnceTest();
    testTopologyChangeResponse();
    updateServersTest();
    consistentHashFactoryTests();
    murmurHash2StringTest();
    murmurHash3StringTest();
    murmurHash2IntTest();
    murmurHash3IntTest();
    return 0;
}
