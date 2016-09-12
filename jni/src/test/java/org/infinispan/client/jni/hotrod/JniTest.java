package org.infinispan.client.jni.hotrod;

import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

import org.infinispan.client.hotrod.*;
import org.infinispan.client.hotrod.xsite.SiteDownFailoverTest;
import org.testng.IMethodSelector;
import org.testng.IMethodSelectorContext;
import org.testng.ITestNGMethod;
import org.testng.ITestResult;
import org.testng.TestNG;
import org.testng.reporters.TextReporter;

public class JniTest implements IMethodSelector {
	private final static String [] passOverTestList = {
	"CacheManagerNotStartedTest.testPutAllAsync",
	"CacheManagerNotStartedTest.testPutAsync",
	"CacheManagerNotStartedTest.testReplaceAsync",
	"CacheManagerNotStartedTest.testVersionedRemoveAsync",
	"CacheManagerStoppedTest.testPutAllAsync",
	"CacheManagerStoppedTest.testPutAsync",
	"CacheManagerStoppedTest.testReplaceAsync",
	"CacheManagerStoppedTest.testVersionedRemoveAsync",
        "HotRodAsyncReplicationTest.testPutKeyValue"};
	
   private final static HashSet<String> passOverTestSet = new HashSet<String>(Arrays.asList(passOverTestList));
	
   public static void main(String[] args) {
      TestNG testng = new TestNG();
      testng.addMethodSelector("org.infinispan.client.jni.hotrod.JniTest", 1);
      TextReporter tr = new TextReporter("SWIG Tests", 2);

      testng.setTestClasses(new Class[] {
            /* package org.infinispan.client.jni.hotrod (this one) */
            CrossLanguageHotRodTest.class,
            /* package org.infinispan.client.hotrod */
            // ApacheCommonsPoolTest.class,          // omitting
            BulkGetKeysDistTest.class, 
            BulkGetKeysReplTest.class, 
            BulkGetKeysSimpleTest.class, 
            BulkGetReplTest.class,
            BulkGetSimpleTest.class,
            // CacheContainerTest.class,             // not relevant
            CacheManagerNotStartedTest.class, 
            CacheManagerStoppedTest.class, 
            ClientAsymmetricClusterTest.class,       // HRCPP-120
            // ClientConnectionPoolingTest.class,    // requires transport extraction
            ClientSocketReadTimeoutTest.class,
            // omitting ConsistentHash* tests
            // CSAIntegrationTest.class,             // requires transport extraction
            // DistTopologyChangeTest.class,         // requires transport extraction
            // DroppedConnectionsTest.class,         // requires transport extraction
            DefaultExpirationTest.class,
            // ExpiryTest.class,                     // unstable
            ForceReturnValuesTest.class, 
            // HeavyLoadConnectionPoolingTest.class, // requires transport extraction
            HitsAwareCacheManagersTest.class,
            // HotRod10CSAIntegrationTest.class,     // requires transport extraction
            HotRodAsyncReplicationTest.class,
            HotRodIntegrationTest.class,
            HotRodServerStartStopTest.class, 
            HotRodStatisticsTest.class, 
            // PingOnStartupTest.class,              // requires transport extraction
            // RemoteAsyncAPITest.class,             // async API not implemented
            RemoteCacheManagerTest.class,
            // ReplTopologyChangeTest.class,         // requires transport extraction
            // omitting RoundRobin* tests
            ServerErrorTest.class,
            ServerRestartTest.class,
            SizeTest.class,
            SocketTimeoutErrorTest.class,
            SegmentOwnershipLocalTest.class,
            SegmentOwnershipDistTest.class,
            ServerShutdownTest.class,
            SiteDownFailoverTest.class
            // SslTest.class,                        // SSL not implemented
            // TransportObjectFactoryTest.class,     // omitting
      });

      testng.addListener(tr);
      testng.setGroups("unit,functional");
      testng.run();

      Set<String> expectedTestFailures = new TreeSet<String>(Arrays.asList( 
            "BulkGetKeysDistTest.testBulkGetAfterLifespanExpire",                // unstable, ISPN-4017
            "HotRodIntegrationTest.testReplaceWithVersionWithLifespanAsync",     // async not implemented
            "ClientSocketReadTimeoutTest.testPutTimeout",                        // TODO: TransportException not marshalled correctly
            "RemoteCacheManagerTest.testMarshallerInstance",                     // setting marshaller through configuration builder not implemented
            "BulkGetKeysReplTest.testBulkGetAfterLifespanExpire",
            "BulkGetKeysSimpleTest.testBulkGetAfterLifespanExpire",
            "BulkGetReplTest.testBulkGetAfterLifespanExpire",
            "BulkGetSimpleTest.testBulkGetAfterLifespanExpire",
            "ForceReturnValuesTest.testDifferentInstancesForDifferentForceReturnValues",
            "ForceReturnValuesTest.testSameInstanceForSameForceReturnValues",
            "HotRodIntegrationTest.testGetWithMetadata",
            "RemoteCacheManagerTest.testGetUndefinedCache"
      ));
      Set<String> expectedSkips = Collections.emptySet();

      Set<String> failures = new TreeSet<String>();
      for (ITestResult failed : tr.getFailedTests()) {
         failures.add(failed.getTestClass().getRealClass().getSimpleName() + "." + failed.getMethod().getMethodName());
      }
      Set<String> skips = new TreeSet<String>();
      for (ITestResult skipped : tr.getSkippedTests()) {
         failures.add(skipped.getTestClass().getRealClass().getSimpleName() + "." + skipped.getMethod().getMethodName());
      }

      int exitCode = 0;
      
      Set<String> unexpectedFails = new TreeSet<String>(failures);
      unexpectedFails.removeAll(expectedTestFailures);
      if (!unexpectedFails.isEmpty()) {
         exitCode = 1;
         System.err.println("These test fail (but should not!):");
	 for (String testName : unexpectedFails) {
            System.err.println("\t" + testName);
         } 
      }
      Set<String> notFailing = new TreeSet<String>(expectedTestFailures);
      notFailing.removeAll(failures);
      if (!notFailing.isEmpty()) {
         exitCode = 1;
         System.err.println("These test should fail (but don't!):");
	 for (String testName : notFailing) {
            System.err.println("\t" + testName);
         }
      }
      Set<String> unexpectedSkips = new TreeSet<String>(skips);
      unexpectedSkips.removeAll(expectedSkips);
      if (!unexpectedSkips.isEmpty()) {
         exitCode = 1;
         System.err.println("These test have been skipped (but should not!):");
	 for (String testName : unexpectedSkips) {
            System.err.println("\t" + testName);
         } 
      }
      Set<String> notSkipped = new TreeSet<String>(expectedSkips);
      notSkipped.removeAll(skips);
      if (!notSkipped.isEmpty()) {
         exitCode = 1;
         System.err.println("These test should have been skipped (but haven't!):");
	 for (String testName : notSkipped) {
            System.err.println("\t" + testName);
         }
      }

      /* Force exit when tests pass also as some of the tests expected to fail
         might not properly clean-up and as a result the process will not terminate
         when main() returns. */
      System.exit(exitCode);
   }

@Override
public boolean includeMethod(IMethodSelectorContext context, ITestNGMethod method, boolean isTestMethod) {
	String testName = method.getRealClass().getSimpleName()+"."+method.getMethodName();
	if (passOverTestSet.contains(testName))
	{
		context.setStopped(true);
		return false;
	}
	return true;
}

@Override
public void setTestMethods(List<ITestNGMethod> testMethods) {
	// TODO Auto-generated method stub
	
}
}
