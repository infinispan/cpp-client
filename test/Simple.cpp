#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/ScopedBuffer.h"

#include <iostream>
#include <memory>
#include <unistd.h>

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

    // put
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

    std::string k3("rolling");
    std::string v3("stones");
    std::string v4("beatles");

    // putIfAbsent
    cache.putIfAbsent(k3, v3);
    std::auto_ptr<std::string> rv3(cache.get(k3));
    if (rv3->compare(v3)) {
        std::cerr << "putIfAbsent fail for " << k3 << " got " << *rv3 << " expected " << v3 << std::endl;
        return 1;
    }

    cache.putIfAbsent(k3, v4);
    std::auto_ptr<std::string> rv4(cache.get(k3));
    if (rv4->compare(v3)) {
        std::cerr << "putIfAbsent fail for " << k3 << " got " << *rv4 << " expected " << v3 << std::endl;
        return 1;
    }

    std::cout << "PASS: simple putIfAbsent"  << std::endl;

    // getWithMetadata
    std::pair<HR_SHARED_PTR<std::string>,MetadataValue> rv5 = cache.getWithMetadata(k3);
    if (!rv5.first.get()) {
        std::cerr << "getWithMetadata fail for " << k3 << " not found" << std::endl;
        return 1;
    }

    std::cout << "PASS: simple getWithMetadata"  << std::endl;

    // replaceWithVersion
    cache.replaceWithVersion(k3, v4, rv5.second.version);
    std::auto_ptr<std::string> rv6(cache.get(k3));
    if (rv6->compare(v4)) {
        std::cerr << "putIfAbsent fail for " << k3 << " got " << *rv6 << " expected " << v4 << std::endl;
        return 1;
    }

    cache.replaceWithVersion(k3, v3, rv5.second.version);
    std::auto_ptr<std::string> rv7(cache.get(k3));
    if (rv7->compare(v4)) {
        std::cerr << "putIfAbsent fail for " << k3 << " got " << *rv7 << " expected " << v4 << std::endl;
        return 1;
    }

    std::cout << "PASS: simple replaceWithVersion" << std::endl;

    // size
    int32_t size = cache.size();
    if (size != 3) {
        std::cerr << "size fail got " << size << " expected 3 " << std::endl;
        return 1;
    }

    std::cout << "PASS: simple size" << std::endl;

    // stats
    const std::map<std::string,std::string>& stats = cache.stats();
    if (stats.empty()) {
        std::cerr << "stats fail with empty map" << std::endl;
        return 1;
    }

    std::cout << "  stats result is:" << std::endl;
    for(std::map<std::string,std::string>::const_iterator i=stats.begin(); i!=stats.end(); i++) {
        std::cout << "    key: " << i->first << ", value: " << i->second << std::endl;
    }

    std::cout << "PASS: simple stats" << std::endl;

    // clear
    cache.clear();
    int32_t size2 = cache.size();
    if (size2 != 0) {
        std::cerr << "clear fail cache has size " << size2 << " expected 0 " << std::endl;
        return 1;
    }

    std::cout << "PASS: simple clear" << std::endl;

    std::string k4("real");
    std::string v5("madrid");
    std::string v6("barcelona");

    // put with FORCE_RETURN_VALUE flag
    std::auto_ptr<std::string> rv8(cache.withFlags(FORCE_RETURN_VALUE).put(k4,v5));
    if (rv8.get()) {
        std::cerr << "put with FORCE_RETURN_FLAG fail for " << k4 << " got " << *rv8 << " expected null pointer" << std::endl;
        return 1;
    }

    std::auto_ptr<std::string> rv9(cache.withFlags(FORCE_RETURN_VALUE).put(k4,v6));
    if (rv9->compare(v5)) {
        std::cerr << "put with FORCE_RETURN_FLAG fail for " << k4 << " got " << *rv9 << " expected " << v5 << std::endl;
        return 1;
    }

    std::cout << "PASS: simple put with FORCE_RETURN_FLAG" << std::endl;

    // keySet
    std::set<HR_SHARED_PTR<std::string> > keySet = cache.keySet();
    if (keySet.size()!=1) {
        std::cerr << "keySet fail got " << keySet.size() << " entries expected 1" << std::endl;
        return 1;
    }

    std::cout << "  keySet result is:" << std::endl;
    for(std::set<HR_SHARED_PTR<std::string> >::const_iterator i=keySet.begin(); i!=keySet.end(); i++) {
        std::cout << "    key: " << *i->get() << std::endl;
    }

    std::cout << "PASS: simple keySet" << std::endl;

    // getBulk
    std::map<HR_SHARED_PTR<std::string>,HR_SHARED_PTR<std::string> > map = cache.getBulk(0);
    if (map.size()!=1) {
        std::cerr << "getBulk fail got" << map.size() << " entries expected 1" << std::endl;
        return 1;
    }

    std::cout << "  getBulk result is:" << std::endl;
    for(std::map<HR_SHARED_PTR<std::string>,HR_SHARED_PTR<std::string> >::const_iterator i=map.begin(); i!=map.end(); i++) {
        std::cout << "    key: " << *i->first.get() << ", value: " << *i->second.get() << std::endl;
    }

    std::cout << "PASS: simple getBulk" << std::endl;

    // replace
    cache.replace(k4,v5);
    std::auto_ptr<std::string> rv10(cache.get(k4));
    if (rv10->compare(v5)) {
        std::cerr << "replace fail for " << k4 << " got " << *rv10 << " expected " << v5 << std::endl;
        return 1;
    }

    std::cout << "PASS: simple replace" << std::endl;

    cacheManager.stop();

    return 0;
}
