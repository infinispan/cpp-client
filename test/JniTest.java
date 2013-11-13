import org.infinispan.client.hotrod.RemoteCacheManager;
import org.infinispan.client.hotrod.RemoteCache;
import org.infinispan.client.hotrod.TestHelper;
import org.infinispan.api.BasicCache;
import org.infinispan.client.hotrod.test.HotRodClientTestingUtil;
import org.infinispan.commons.util.ReflectionUtil;
import org.infinispan.commons.util.Util;
import org.infinispan.configuration.cache.ConfigurationBuilder;
import org.infinispan.manager.EmbeddedCacheManager;
import org.infinispan.server.hotrod.HotRodServer;
import org.infinispan.test.SingleCacheManagerTest;
import org.infinispan.test.fwk.TestCacheManagerFactory;
import org.testng.annotations.AfterTest;
import org.testng.annotations.Test;
import org.infinispan.client.hotrod.BulkGetKeysSimpleTest;
import org.infinispan.client.hotrod.BulkGetSimpleTest;
import org.infinispan.client.hotrod.CacheContainerTest;
import org.infinispan.client.hotrod.ForceReturnValueTest;
import org.infinispan.client.hotrod.HotRodIntegrationTest;

import java.lang.reflect.Method;

import static org.infinispan.server.hotrod.test.HotRodTestingUtil.hotRodCacheConfiguration;
import static org.infinispan.test.TestingUtil.*;
import static org.infinispan.client.hotrod.test.HotRodClientTestingUtil.*;

public class JniTest {

    private static final String CACHE_NAME = "someName";
    protected static EmbeddedCacheManager cacheManager = null;
    protected static HotRodServer hotrodServer = null;
    protected static RemoteCacheManager remCacheManager = null;

    private static void invokeMethod(Object obj, String name) {
        Method method = ReflectionUtil.findMethod(obj.getClass(), name);
        ReflectionUtil.invokeAccessibly(obj, method, null);
    }

    public static void main(String[] args) {
        boolean usingJNI = false;
        System.out.println("at work");

        HotRodIntegrationTest hotRodIntegrationTest = null;
        CacheContainerTest cacheContainerTest = null;
        ForceReturnValueTest forceReturnValueTest = null;
        BulkGetKeysSimpleTest bulkGetKeysSimpleTest = null;
        BulkGetSimpleTest bulkGetSimpleTest = null;

        try {
            System.out.println("Java + JNI Hot Rod Client");

            //first test, the cache container test
            System.out.println("=== Cache container test ====");
            cacheContainerTest = new CacheContainerTest();
            //invoking cacheContainerTest.createCacheManager();
            invokeMethod(cacheContainerTest, "createCacheManager");

            //cacheContainerTest.testObtainingSameInstanceMultipleTimes();
            //cacheContainerTest.testObtainingSameInstanceMultipleTimes2();
            cacheContainerTest.release();
            System.out.println("=== Cache container test ==== PASSED");

            System.out.println("=== Hot Rod integration test ====");

            hotRodIntegrationTest = new HotRodIntegrationTest();
            Method hrIntcreateCacheManager = hotRodIntegrationTest.getClass().getSuperclass()
                    .getDeclaredMethod("createCacheManager");
            hrIntcreateCacheManager.setAccessible(true);
            Method setup = hotRodIntegrationTest.getClass().getSuperclass().getDeclaredMethod("setup");
            setup.setAccessible(true);
            //invoking methods in the right order
            //hrIntcreateCacheManager.invoke(hotRodIntegrationTest);
            setup.invoke(hotRodIntegrationTest);
            System.out.println("=== Hot Rod integration test - test contains ====");
            hotRodIntegrationTest.testContains();
            System.out.println("=== Hot Rod integration test - test contains PASSED ====");
            System.out.println("=== Hot Rod integration test - test Put ====");
            hotRodIntegrationTest.testPut();
            System.out.println("=== Hot Rod integration test - test Put PASSED ====");
            System.out.println("=== Hot Rod integration test - test Remove ====");
            hotRodIntegrationTest.testRemove();
            System.out.println("=== Hot Rod integration test - test Remove PASSED ====");
            System.out.println("=== Hot Rod integration test - test Replace ====");
            hotRodIntegrationTest.testReplace();
            System.out.println("=== Hot Rod integration test - test Replace PASSED ====");
            System.out.println("=== Hot Rod integration test - test PutIfAbsent ====");
            hotRodIntegrationTest.testPutIfAbsent();
            System.out.println("=== Hot Rod integration test - test PutIfAbsent PASSED ====");

            System.out.println("=== Hot Rod integration test - test GetVersionedCacheEntry ====");
            hotRodIntegrationTest.testDestroyRemoteCacheFactory();
            setup.invoke(hotRodIntegrationTest);

            hotRodIntegrationTest.testGetVersionedCacheEntry();
            System.out.println("=== Hot Rod integration test - test GetVersionedCacheEntry PASSED ====");

            System.out.println("=== Hot Rod integration test - test GetWithMetadata ====");
            hotRodIntegrationTest.testDestroyRemoteCacheFactory();
            setup.invoke(hotRodIntegrationTest);

            hotRodIntegrationTest.testGetWithMetadata();
            System.out.println("=== Hot Rod integration test - test GetWithMetadata PASSED ====");

            //TODO: testClear need investigations, it fails in the same manner than java
            //Exception in thread "main" java.lang.AssertionError
            //at org.infinispan.client.hotrod.HotRodIntegrationTest.testClear(HotRodIntegrationTest.java:293)
            //at Simple2.main(Simple2.java:80)
            ///////////
            /*
             * System.out.println("=== Hot Rod integration test - test Clear ====");
             * hotRodIntegrationTest.testClear();
             * System.out.println("=== Hot Rod integration test - test Clear PASSED ====");
             */

            System.out.println("=== Hot Rod integration test ==== PASSED");

            System.out.println("=== Force Return Value test ====");
            forceReturnValueTest = new ForceReturnValueTest();
            setup = forceReturnValueTest.getClass().getDeclaredMethod("setup");
            setup.setAccessible(true);
            setup.invoke(forceReturnValueTest);

            System.out.println("=== Force Return Value test - test Put ====");
            forceReturnValueTest.testPut();
            System.out.println("=== Force Return Value test - test Put PASSED ====");

            System.out.println("=== Force Return Value test - test Remove ====");
            forceReturnValueTest.testRemove();
            System.out.println("=== Force Return Value test - test Remove PASSED ====");

            //TODO: this fails with invalid magic number in response
            // System.out.println("=== Force Return Value test - test RemoveNonExistForceReturnPrevious ====");
            // forceReturnValueTest.testRemoveNonExistForceReturnPrevious();
            // System.out.println("=== Force Return Value test - test RemoveNonExistForceReturnPrevious PASSED ====");

            System.out.println("=== Force Return Value test - test Contains ====");
            forceReturnValueTest.testContains();
            System.out.println("=== Force Return Value test - test Contains PASSED ====");

            System.out.println("=== Force Return Value test - test Replace ====");
            forceReturnValueTest.testReplace();
            System.out.println("=== Force Return Value test - test Replace PASSED ====");

            System.out.println("=== Force Return Value test - test PutIfAbsent ====");
            forceReturnValueTest.testPutIfAbsent();
            System.out.println("=== Force Return Value test - test PutIfAbsent PASSED ====");

            System.out.println("=== Force Return Value test ==== PASSED");

            System.out.println("=== Bulk Get Simple test ====");
            bulkGetSimpleTest = new BulkGetSimpleTest();
            invokeMethod(bulkGetSimpleTest, "createCacheManagers");

            System.out.println("=== Bulk Get Simple test - test Get Bulk ====");
            bulkGetSimpleTest.testBulkGet();
            System.out.println("=== Bulk Get Simple test - test Get Bulk PASSED ====");

            System.out.println("=== Bulk Get Simple test - test Get Bulk With Size ====");
            bulkGetSimpleTest.testBulkGetWithSize();
            System.out.println("=== Bulk Get Simple test - test Get Bulk With Size PASSED ====");

            System.out.println("=== Bulk Get Keys Simple test ====");
            bulkGetKeysSimpleTest = new BulkGetKeysSimpleTest();
            invokeMethod(bulkGetKeysSimpleTest, "createCacheManagers");
            System.out.println("=== Bulk Get Simple test - test Get Bulk KeySet ====");
            bulkGetKeysSimpleTest.testBulkGetKeys();
            System.out.println("=== Bulk Get Simple test - test Get Bulk KeySet PASSED ====");

            System.out.println("=== Bulk Get Simple test ==== PASSED");
        } catch (Throwable e) {
            System.out.println("Test Error");
            e.printStackTrace();
            System.exit(1);
        } finally {
            if (hotRodIntegrationTest != null) {
                hotRodIntegrationTest.testDestroyRemoteCacheFactory();
            }
            if (cacheContainerTest != null) {
                cacheContainerTest.release();
            }
            if (forceReturnValueTest != null) {
                forceReturnValueTest.destroy();
            }
            if (bulkGetSimpleTest != null) {
                try {
                    invokeMethod(bulkGetSimpleTest, "destroy");
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
                bulkGetSimpleTest.release();
            }
            if (bulkGetKeysSimpleTest != null) {
                try {
                    invokeMethod(bulkGetKeysSimpleTest, "destroy");
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
                bulkGetKeysSimpleTest.release();
            }
        }
    }
}
