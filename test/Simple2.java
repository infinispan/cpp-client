import org.infinispan.client.hotrod.RemoteCacheManager;
import org.infinispan.client.hotrod.RemoteCache;
import org.infinispan.client.hotrod.TestHelper;
import org.infinispan.api.BasicCache;
import org.infinispan.client.hotrod.test.HotRodClientTestingUtil;
import org.infinispan.configuration.cache.ConfigurationBuilder;
import org.infinispan.manager.EmbeddedCacheManager;
import org.infinispan.server.hotrod.HotRodServer;
import org.infinispan.test.SingleCacheManagerTest;
import org.infinispan.test.fwk.TestCacheManagerFactory;
import org.testng.annotations.AfterTest;
import org.testng.annotations.Test;
import org.infinispan.client.hotrod.CacheContainerTest;
import org.infinispan.client.hotrod.HotRodIntegrationTest;
import java.lang.reflect.Method;
import static org.infinispan.server.hotrod.test.HotRodTestingUtil.hotRodCacheConfiguration;
import static org.infinispan.test.TestingUtil.*;
import static org.infinispan.client.hotrod.test.HotRodClientTestingUtil.*;

public class Simple2 {
	
	private static final String CACHE_NAME = "someName";
	protected static EmbeddedCacheManager cacheManager = null;
	protected static HotRodServer hotrodServer = null;
	protected static RemoteCacheManager remCacheManager = null;

    public static void main(String[] args) {
    
	boolean usingJNI = false;
        System.out.println("at work");
	

	try {
		System.out.println("Java + JNI Hot Rod Client");
		//starting the server and creating the cache managers - not for now, we need configuration before
	    //createCacheManager();
		
		
		
	    //first test, the cache container test
		System.out.println("=== Cache container test ====");
		CacheContainerTest cacheContainerTest = new CacheContainerTest();
		//invoking cacheContainerTest.createCacheManager();
	    Method createCacheManager = cacheContainerTest.getClass().getSuperclass().getDeclaredMethod("createCacheManager");
	    if(createCacheManager == null) {
	    	System.out.println("Method is null");
	    } else {
	    	System.out.println("Method is NOT null");
	    }
	    createCacheManager.setAccessible(true);
	    createCacheManager.invoke(cacheContainerTest);
	    
	    //cacheContainerTest.testObtainingSameInstanceMultipleTimes();
	    //cacheContainerTest.testObtainingSameInstanceMultipleTimes2();
	    cacheContainerTest.release();
	    System.out.println("=== Cache container test ==== PASSED");
	    
	    System.out.println("=== Hot Rod integration test ====");
	    
	    HotRodIntegrationTest hotRodIntegrationTest = new HotRodIntegrationTest();
	    Method hrIntcreateCacheManager = hotRodIntegrationTest.getClass().getSuperclass().getDeclaredMethod("createCacheManager");
	    hrIntcreateCacheManager.setAccessible(true);
	    Method setup = hotRodIntegrationTest.getClass().getSuperclass().getDeclaredMethod("setup");
	    setup.setAccessible(true);
	    //invoking methods in the right order
	    //hrIntcreateCacheManager.invoke(hotRodIntegrationTest);
	    setup.invoke(hotRodIntegrationTest);
	    //hotRodIntegrationTest.testContains();
	    hotRodIntegrationTest.testPut();
	    hotRodIntegrationTest.testDestroyRemoteCacheFactory();
	    
	    System.out.println("=== Hot Rod integration test ==== PASSED");
	    
	    
	    
	    
	    /* RemoteCache<String, String> ncache = remCacheManager.getNativeCache();
	    // method only exists on JNI version
	    usingJNI = true;
	    
	    System.out.println("C++ marshaller test (string only)");
	    ncache.put("car", "ferrari");
	    assert ncache.get("car").equals("ferrari");
	    System.out.println("=== Marshaller test === PAASED");*/
	    
	} catch (java.lang.NoSuchMethodError e) {
	    System.out.println("Test Error");
	    e.printStackTrace();
	    //killing the servers and the cache managers - - not for now, we need configuration before
	    //release();
	} catch (Exception e) {
		System.out.println("Test Error");
		e.printStackTrace();
	    //killing the servers and the cache managers - not for now, we need configuration before
	    //release();
	} finally {
		//release();
	}
	
	/*System.out.println("Java marshaller test");
        System.out.println("testing String/String");
	RemoteCache<String, String> cache = remCacheManager.getCache();
	cache.put("car", "ferrari");
	assert cache.get("car").equals("ferrari");
	System.out.println("pass");

        System.out.println("testing String/Double");
	RemoteCache<String, Double> cache2 = remCacheManager.getCache();
	Double double0 = new Double(12.98);
	cache2.put("dtest", double0);
	assert double0.equals(cache2.get("dtest"));
	System.out.println("pass");

        System.out.println("done");
      //killing the servers and the cache managers
      //release(); */
    }
    
    /*protected static void createCacheManager() throws Exception {
    	  cacheManager = TestCacheManagerFactory.createCacheManager(
                hotRodCacheConfiguration());
          cacheManager.defineConfiguration(CACHE_NAME, hotRodCacheConfiguration().build());
          hotrodServer = TestHelper.startHotRodServer(cacheManager);
          remCacheManager = new RemoteCacheManager("localhost:" + hotrodServer.getPort(), true);
     }
    
    public static void release() {
        killCacheManagers(cacheManager);
        killRemoteCacheManager(remCacheManager);
        killServers(hotrodServer);
     }*/
    
}
