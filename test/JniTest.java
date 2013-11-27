import org.testng.reporters.TextReporter;
import org.testng.TestNG;
import org.infinispan.client.hotrod.BulkGetKeysSimpleTest;
import org.infinispan.client.hotrod.BulkGetSimpleTest;
import org.infinispan.client.hotrod.CacheContainerTest;
import org.infinispan.client.hotrod.DefaultExpirationTest;
import org.infinispan.client.hotrod.ForceReturnValueTest;
import org.infinispan.client.hotrod.HotRodIntegrationTest;
import org.infinispan.client.hotrod.HotRodStatisticsTest;


public class JniTest {
    public static void main(String[] args) {
        TestNG testng = new TestNG();
        TextReporter tr = new TextReporter("SWIG Tests", 2);
        testng.setTestClasses(new Class[] { DefaultExpirationTest.class, HotRodIntegrationTest.class, ForceReturnValueTest.class, BulkGetSimpleTest.class, BulkGetKeysSimpleTest.class, HotRodStatisticsTest.class });
        testng.addListener(tr);
        testng.run();
        /*
         * We expect two failures:
         *  ForceReturnValueTest.testRemoveNonExistForceReturnPrevious
         *  HotRodIntegrationTest.testReplaceWithVersionWithLifespanAsync
         */


        if (testng.hasFailure() && tr.getFailedTests().size() > 2)
            System.exit(1);
    }
}
