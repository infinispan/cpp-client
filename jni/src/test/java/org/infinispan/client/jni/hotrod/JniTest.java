package org.infinispan.client.jni.hotrod;

import static org.testng.Assert.assertEquals;

import org.infinispan.client.hotrod.BulkGetKeysDistTest;
import org.infinispan.client.hotrod.BulkGetKeysReplTest;
import org.infinispan.client.hotrod.BulkGetKeysSimpleTest;
import org.infinispan.client.hotrod.BulkGetReplTest;
import org.infinispan.client.hotrod.BulkGetSimpleTest;
import org.infinispan.client.hotrod.ClientAsymmetricClusterTest;
import org.infinispan.client.hotrod.CacheManagerStoppedTest;
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
            //HRCPP-120
//            ClientAsymmetricClusterTest.class,

            //Known to work
            BulkGetKeysDistTest.class, 
            BulkGetKeysReplTest.class, 
            BulkGetKeysSimpleTest.class, 
            BulkGetReplTest.class,
            BulkGetSimpleTest.class, 
            CacheManagerStoppedTest.class, 
            CrossLanguageHotRodTest.class,
            DefaultExpirationTest.class,
            ForceReturnValuesTest.class, 
            ForceReturnValueTest.class, 
            HotRodIntegrationTest.class,
            HotRodServerStartStopTest.class, 
            HotRodStatisticsTest.class, 
            RemoteCacheManagerTest.class,
            ServerErrorTest.class,
            ServerRestartTest.class,
            ServerShutdownTest.class,
            SocketTimeoutErrorTest.class, 
            
      });

      testng.addListener(tr);
      testng.run();

      String[] expectedTestFailures = { 
            // Async operations are not supported currently
            "HotRodIntegrationTest.testReplaceWithVersionWithLifespanAsync",
            "CacheManagerStoppedTest.testPutAllAsync",
            "CacheManagerStoppedTest.testPutAsync",
            "CacheManagerStoppedTest.testReplaceAsync",
            "CacheManagerStoppedTest.testVersionedRemoveAsync",
            // RemoteCacheManager doesn't support these tests
            "RemoteCacheManagerTest.testUrlAndBooleanConstructor",
            "RemoteCacheManagerTest.testMarshallerInstance",
            "RemoteCacheManagerTest.testGetUndefinedCache",
      };

      final int expectedFailures = expectedTestFailures.length;
      final int expectedSkips = 0;
      final int actualFailures = tr.getFailedTests().size();
      final int actualSkips = tr.getSkippedTests().size();

      int exitCode = 0;
      if (expectedFailures != actualFailures) {
          exitCode = 1;
          System.err.println(String.format("Failures expected: %d actual: %d",
                                           expectedFailures,
                                           actualFailures));
      }

      if (expectedSkips != actualSkips) {
          exitCode = 1;
          System.err.println(String.format("Skips expected: %d actual: %d",
                                           expectedSkips,
                                           actualSkips));
      }

      /* Force exit when tests pass also as some of the tests expected to fail
         might not properly clean-up and as a result the process will not terminate
         when main() returns. */
      System.exit(exitCode);
   }
}
