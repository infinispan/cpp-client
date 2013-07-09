import org.infinispan.client.hotrod.RemoteCacheManager;
import org.infinispan.client.hotrod.RemoteCache;

public class Simple2 {

    public static void main(String[] args) {
	boolean usingJNI = false;
        System.out.println("at work");
	RemoteCacheManager remoteCacheManager = new RemoteCacheManager();

	try {
	    RemoteCache<String, String> ncache = remoteCacheManager.getNativeCache();
	    // method only exists on JNI version
	    usingJNI = true;
	    System.out.println("Java + JNI Hot Rod Client");
	    System.out.println("C++ marshaller test (string only)");

	    ncache.put("car", "ferrari");
	    assert ncache.get("car").equals("ferrari");
	    System.out.println("pass");
	} catch (java.lang.NoSuchMethodError e) {
	    System.out.println("Pure Java Hot Rod Client");
	}

	System.out.println("Java marshaller test");
        System.out.println("testing String/String");
	RemoteCache<String, String> cache = remoteCacheManager.getCache();
	cache.put("car", "ferrari");
	assert cache.get("car").equals("ferrari");
	System.out.println("pass");

        System.out.println("testing String/Double");
	RemoteCache<String, Double> cache2 = remoteCacheManager.getCache();
	Double double0 = new Double(12.98);
	cache2.put("dtest", double0);
	assert double0.equals(cache2.get("dtest"));
	System.out.println("pass");

        System.out.println("done");
    }
}
