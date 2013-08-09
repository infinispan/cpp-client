


#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/ScopedBuffer.h"

#include <iostream>
#include <memory>

using namespace infinispan::hotrod;

	int main(int, char**) {
	    RemoteCacheManager cacheManager(false);
	    RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>();
	    cacheManager.start();

	    std::string k("abcdef");
	    std::string v("The rain in Spain");
	    std::string v2("falls mainly on the plain");

	    cache.put(k, v);
	    std::auto_ptr<std::string> rv(cache.get(k));
	    std::cout << "put and get test: " << *rv << std::endl;

	    cache.put(k, v2);
	    std::auto_ptr<std::string> rv2(cache.get(k));
	    std::cout << "put and get test 2: " << *rv2 << std::endl;

	    std::auto_ptr<std::string> badValue(cache.get(std::string("no such key in the cache")));
	    if (badValue.get())
		std::cout << "non-existent key failure, got " << *badValue << "\n";
	    else
	        std::cout << "non-existent key test passed\n";

	    std::string lor_k("lorenzo");
	    std::string lor_v("fili");
	    std::string lor_v2("verona");

	    cache.put(lor_k, lor_v, 100, 100);
	    std::auto_ptr<std::string> lor_r(cache.get(lor_k));
	    std::cout << "put if absent test: " << *lor_r << std::endl;


	    cache.putIfAbsent(lor_k, lor_v2);
	    std::auto_ptr<std::string> lor_r2(cache.get(lor_k));
	    std::cout << "put if absent test 2: " << *lor_r2 << std::endl;


	    // GetWithMetadata Test
	    std::pair<HR_SHARED_PTR<std::string>,MetadataValue> gwm = cache.getWithMetadata(lor_k);
	    if (gwm.first.get()) {
	        std::cout << "get with metadata: " << std::endl;
	        std::cout << "  value: " << *gwm.first.get() << std::endl;
	        std::cout << "  version: " << gwm.second.version << std::endl;
	        std::cout << "  created: " << gwm.second.created << std::endl;
	        std::cout << "  lifespan: " << gwm.second.lifespan << std::endl;
	        std::cout << "  lastUsed: " << gwm.second.lastUsed << std::endl;
	        std::cout << "  maxIdle: " << gwm.second.maxIdle << std::endl;
	    }
	    else
	    	std::cout << "get with metadata key not found" << std::endl;

	    // replace if unmodified
	    cache.replaceWithVersion(lor_k,lor_v2,gwm.second.version);
	    cache.replaceWithVersion(lor_k,lor_v2,gwm.second.version);

	/*
	    // GetWithMetadata Test
	    std::auto_ptr<std::string> gwm2(cache.getWithMetadata(lor_k));
	    if (gwm2.get())
	    std::cout << "get with metadata: " << *gwm2 << std::endl;
	    else
	    	std::cout << "get with metadata key not found" << std::endl;
	*/



	    std::pair<HR_SHARED_PTR<std::string>,MetadataValue> gwm2 = cache.getWithMetadata(lor_k);
	    if (gwm2.first.get()) {
	        std::cout << "get with metadata: " << std::endl;
	        std::cout << "  value: " << *gwm2.first.get() << std::endl;
	        std::cout << "  version: " << gwm2.second.version << std::endl;
	        std::cout << "  created: " << gwm2.second.created << std::endl;
	        std::cout << "  lifespan: " << gwm2.second.lifespan << std::endl;
	        std::cout << "  lastUsed: " << gwm2.second.lastUsed << std::endl;
	        std::cout << "  maxIdle: " << gwm2.second.maxIdle << std::endl;
	    }
	    else
	    	std::cout << "get with metadata key not found" << std::endl;
/*
	    std::map<std::string,std::string> res = cache.getBulk(10);
	    for(std::map<std::string,std::string>::iterator i = res.begin();
	    		i != res.end();i++) {
	        std::cout << "Key: " << i->first << ", value: " << i->second << std::endl;
	    }

	    std::cout << "GET BULK 2" << std::endl;
	    std::map<HR_SHARED_PTR<std::string>,HR_SHARED_PTR<std::string> > resB2 = cache.getBulk2(10);
	    for(std::map<HR_SHARED_PTR<std::string>,HR_SHARED_PTR<std::string> >::iterator i = resB2.begin();
	    		i != resB2.end();i++) {
	        std::cout << "Key: " << *(i->first).get() << ", value: " << *(i->second).get() << std::endl;
	    }


	    std::set<std::string> res2 = cache.keySet();
	    for(std::set<std::string>::iterator i = res2.begin();
	    		i != res2.end();i++) {
	        std::cout << "Key: " << *i << std::endl;
	    }
*/
	    int32_t size = cache.size();
	    std::cout << "Size:" << size << std::endl;

	    const std::map<std::string,std::string>& stats = cache.stats();
	    for(std::map<std::string,std::string>::const_iterator i=stats.begin();
	    		i!=stats.end();i++) {
	        std::cout << "key: " << i->first << ", value: " << i->second << std::endl;
	    }

	    cache.clear();

	    const std::map<std::string,std::string>& stats2 = cache.stats();
	    for(std::map<std::string,std::string>::const_iterator i2=stats2.begin();
	    		i2!=stats2.end();i2++) {
	        std::cout << "key: " << i2->first << ", value: " << i2->second << std::endl;
	    }

	    std::string* v5 = cache.withFlags(FORCE_RETURN_VALUE).put("k1","v1");
	    if (v5==0)
	    	std::cout << "OK" << std::endl;
	    else
	    	std::cout << "KO" << std::endl;

	    std::set<HR_SHARED_PTR<std::string> > keySet = cache.keySet();
	    for(std::set<HR_SHARED_PTR<std::string> >::const_iterator i=keySet.begin();	i!=keySet.end(); i++) {
	        std::cout << "key: " << *i->get() << std::endl;
	    }

	    std::map<HR_SHARED_PTR<std::string>,HR_SHARED_PTR<std::string> > map = cache.getBulk(0);
	    for(std::map<HR_SHARED_PTR<std::string>,HR_SHARED_PTR<std::string> >::const_iterator i=map.begin(); i!=map.end(); i++) {
	        std::cout << "key: " << *i->first.get() << ", value: " << *i->second.get() << std::endl;
	    }

	    cache.replace("k1","v2");

	    std::map<HR_SHARED_PTR<std::string>,HR_SHARED_PTR<std::string> > map2 = cache.getBulk(0);
	    for(std::map<HR_SHARED_PTR<std::string>,HR_SHARED_PTR<std::string> >::const_iterator i=map2.begin(); i!=map2.end(); i++) {
	        std::cout << "key: " << *i->first.get() << ", value: " << *i->second.get() << std::endl;
	    }

	    /*
	    RemoteCache<int, std::string> cache2 = cacheManager.getCache<int, std::string>();

	    int my_int = 15;
	    std::string v("The rain in Spain");
	    std::string v2("falls mainly on the plain");

	    cache2.put(my_int, v);
	    std::auto_ptr<std::string> rv(cache2.get(my_int));
	    std::cout << "put and get test: " << *rv << std::endl;

	    RemoteCache<std::string, std::string>& cache = (RemoteCache<std::string, std::string>&) cache2;
	    std::string k("LORENZO");
	    cache.put(k,v2);
	    std::auto_ptr<std::string> rv2(cache.get(k));
	    std::cout << "put and get test: " << *rv2 << std::endl;

	*/
	    // CACHE NON DEFAULT
	    std::string cache_name_nd("___defaultcache");
	    RemoteCache<std::string, std::string> cache_nd = cacheManager.getCache<std::string, std::string>(cache_name_nd);

	    std::string nd_k("abcdef");
	    std::string nd_v("The rain in Spain");
	    std::string nd_v2("falls mainly on the plain");

	    cache_nd.put(nd_k, nd_v);
	    std::auto_ptr<std::string> nd_rv(cache_nd.get(nd_k));
	    std::cout << "put and get test: " << *nd_rv << std::endl;

	    return 0;

}
