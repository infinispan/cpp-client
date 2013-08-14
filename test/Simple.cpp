#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/ScopedBuffer.h"

#include <iostream>
#include <memory>

// For CTest: return 0 if all tests pass, non-zero otherwise.

using namespace infinispan::hotrod;

int main(int, char**) {
    RemoteCacheManager cacheManager(false);
    RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>();
    cacheManager.start();

    std::string k1("key13");
    std::string k2("key14");
    std::string v1("boron");
    std::string v2("chlorine");

    cache.put(k1, v1);
    std::auto_ptr<std::string> rv(cache.get(k1));
    if (rv->compare(v1)) {
        std::cerr << "get/put fail for " << k1 << " got " << *rv << " expected " << v1 << std::endl;
        return 1;
    }

    cache.put(k2, v2);
    std::auto_ptr<std::string> rv2(cache.get(k2));
    if (rv2->compare(v2)) {
        std::cerr << "get/put fail for " << k2 << " got " << *rv2 << " expected " << v2 << std::endl;
        return 1;
    }
 
    std::auto_ptr<std::string> badValue(cache.get(std::string("no such key in the cache")));
    if (badValue.get()) {
        std::cout << "non-existent key failure, got " << *badValue << std::endl;
        return 1;
    }

    std::cout << "PASS: simple get/put" << std::endl;


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
        std::cout << "put with flags and not existing return value OK" << std::endl;
    else
        std::cout << "put with flags and not existing return value KO" << std::endl;

    std::set<HR_SHARED_PTR<std::string> > keySet = cache.keySet();
    for(std::set<HR_SHARED_PTR<std::string> >::const_iterator i=keySet.begin(); i!=keySet.end(); i++) {
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

    return 0;
}
