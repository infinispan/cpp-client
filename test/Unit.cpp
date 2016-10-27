#include "infinispan/hotrod/ImportExport.h"
#include <stdio.h>

/* The tests using internal classes are compiled directly into the shared library/DLL,
   this binary just runs them */

HR_EXTERN void threadTest();
HR_EXTERN void syncTest();
HR_EXTERN void runOnceTest();
HR_EXTERN void testTopologyChangeResponse();
HR_EXTERN void updateServersTest();
HR_EXTERN void consistentHashFactoryTests();
HR_EXTERN bool murmurHash3StringTest();
HR_EXTERN bool murmurHash3IntTest();
HR_EXTERN void runConcurrentCodecWritesTest();
HR_EXTERN void testMinIdle();
HR_EXTERN void testMaxActive();
HR_EXTERN void testMaxTotal();
HR_EXTERN void testMaxTotal2();
HR_EXTERN void testMaxTotal3();
HR_EXTERN void testMaxTotal4();

int main(int, char**) {
/*    runConcurrentCodecWritesTest();
    threadTest();
    syncTest();
    runOnceTest();
    testTopologyChangeResponse();
    updateServersTest();
    consistentHashFactoryTests();
    murmurHash3StringTest();
    murmurHash3IntTest();*/
    //ConnectionPool unit tests
    testMinIdle();
    testMaxActive();
    testMaxTotal();
    testMaxTotal2();
    testMaxTotal3();
    testMaxTotal4();
    return 0;
}
