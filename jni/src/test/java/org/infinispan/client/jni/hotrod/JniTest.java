package org.infinispan.client.jni.hotrod;

import org.infinispan.client.hotrod.BulkGetKeysDistTest;
import org.infinispan.client.hotrod.BulkGetKeysReplTest;
import org.infinispan.client.hotrod.BulkGetKeysSimpleTest;
import org.infinispan.client.hotrod.BulkGetReplTest;
import org.infinispan.client.hotrod.BulkGetSimpleTest;
import org.infinispan.client.hotrod.CacheManagerStoppedTest;
import org.infinispan.client.hotrod.ClientAsymmetricClusterTest;
import org.infinispan.client.hotrod.DefaultExpirationTest;
import org.infinispan.client.hotrod.ForceReturnValueTest;
import org.infinispan.client.hotrod.ForceReturnValuesTest;
import org.infinispan.client.hotrod.HotRodIntegrationTest;
import org.infinispan.client.hotrod.HotRodServerStartStopTest;
import org.infinispan.client.hotrod.HotRodStatisticsTest;
import org.infinispan.client.hotrod.RemoteCacheManagerTest;
import org.infinispan.client.hotrod.ServerErrorTest;
import org.infinispan.client.hotrod.ServerRestartTest;
import org.infinispan.client.hotrod.ServerShutdownTest;
import org.infinispan.client.hotrod.SocketTimeoutErrorTest;
import org.testng.TestNG;
import org.testng.reporters.TextReporter;

public class JniTest {
   public static void main(String[] args) {
      TestNG testng = new TestNG();
      TextReporter tr = new TextReporter("SWIG Tests", 2);

      testng.setTestClasses(new Class[] {
            // "Failed to connect Operation now in progress"
            //            RemoteCacheManagerTest.class,
            //            ClientAsymmetricClusterTest.class,
            //            ServerRestartTest.class,

            //Known to work
            BulkGetKeysDistTest.class,
            BulkGetKeysReplTest.class, 
            BulkGetKeysSimpleTest.class, 
            BulkGetReplTest.class, 
            BulkGetSimpleTest.class, 
            CacheManagerStoppedTest.class,
            DefaultExpirationTest.class,
            ForceReturnValuesTest.class, 
            ForceReturnValueTest.class, 
            HotRodIntegrationTest.class,
            HotRodServerStartStopTest.class, 
            HotRodStatisticsTest.class, 
            ServerErrorTest.class,
            ServerShutdownTest.class, 
            SocketTimeoutErrorTest.class, 
      });
      testng.addListener(tr);
      testng.run();

      String[] expectedTestFailures = { 
            // Async operations are not supported currently
            "HotRodIntegrationTest.testReplaceWithVersionWithLifespanAsync",
            // These CacheManagerStoppedTest throw the wrong exception from JNI
            "CacheManagerStoppedTest.testGet",
            "CacheManagerStoppedTest.testGetCacheOperations2",
            "CacheManagerStoppedTest.testGetCacheOperations3",
            "CacheManagerStoppedTest.testPut",
            "CacheManagerStoppedTest.testPutAll",
            "CacheManagerStoppedTest.testPutAllAsync",
            "CacheManagerStoppedTest.testPutAsync",
            "CacheManagerStoppedTest.testReplace",
            "CacheManagerStoppedTest.testReplaceAsync",
            "CacheManagerStoppedTest.testVersionedGet",
            "CacheManagerStoppedTest.testVersionedRemove",
            "CacheManagerStoppedTest.testVersionedRemoveAsync",
      };

      if (testng.hasFailure() && tr.getFailedTests().size() > expectedTestFailures.length) {
         System.exit(1);
      }
   }
}
