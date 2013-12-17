package org.infinispan.client.jni.hotrod;

import org.infinispan.client.hotrod.BulkGetKeysDistTest;
import org.infinispan.client.hotrod.BulkGetKeysReplTest;
import org.infinispan.client.hotrod.BulkGetKeysSimpleTest;
import org.infinispan.client.hotrod.BulkGetReplTest;
import org.infinispan.client.hotrod.BulkGetSimpleTest;
import org.infinispan.client.hotrod.DefaultExpirationTest;
import org.infinispan.client.hotrod.ForceReturnValueTest;
import org.infinispan.client.hotrod.ForceReturnValuesTest;
import org.infinispan.client.hotrod.HotRodIntegrationTest;
import org.infinispan.client.hotrod.HotRodServerStartStopTest;
import org.infinispan.client.hotrod.HotRodStatisticsTest;
import org.infinispan.client.hotrod.ServerErrorTest;
import org.infinispan.client.hotrod.ServerShutdownTest;
import org.infinispan.client.hotrod.SocketTimeoutErrorTest;
import org.testng.TestNG;
import org.testng.reporters.TextReporter;

public class JniTest {
   public static void main(String[] args) {
      TestNG testng = new TestNG();
      TextReporter tr = new TextReporter("SWIG Tests", 2);

      testng.setTestClasses(new Class[] {
            //Known to work
            BulkGetKeysDistTest.class,
            BulkGetKeysReplTest.class, 
            BulkGetReplTest.class, 
            BulkGetKeysSimpleTest.class, 
            BulkGetSimpleTest.class, 
            DefaultExpirationTest.class,
            ForceReturnValueTest.class, 
            ForceReturnValuesTest.class, 
            HotRodIntegrationTest.class,
            HotRodServerStartStopTest.class, 
            HotRodStatisticsTest.class, 
            ServerErrorTest.class,
            ServerShutdownTest.class, 
            SocketTimeoutErrorTest.class, 
      });
      testng.addListener(tr);
      testng.run();
      /*
       * We expect one failure: HotRodIntegrationTest.testReplaceWithVersionWithLifespanAsync
       */

      String[] expectedTestFailures = { "HotRodIntegrationTest.testReplaceWithVersionWithLifespanAsync" };

      if (testng.hasFailure() && tr.getFailedTests().size() > expectedTestFailures.length) {
         System.exit(1);
      }
   }
}
