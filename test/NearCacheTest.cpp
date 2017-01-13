#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/Version.h"

#include "infinispan/hotrod/JBasicMarshaller.h"
#include "infinispan/hotrod/CacheClientListener.h"
#include "infinispan/hotrod/ClientEvent.h"

#include <chrono>
#include <thread>
#include <string>
using namespace infinispan::hotrod;
using namespace infinispan::hotrod::event;


int main(int argc, char** argv) {
    ConfigurationBuilder nearCacheBuilder;
    nearCacheBuilder.addServer().host(argc > 1 ? argv[1] : "127.0.0.1").port(argc > 2 ? atoi(argv[2]) : 11222);
    nearCacheBuilder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
    nearCacheBuilder.balancingStrategyProducer(nullptr);
    nearCacheBuilder.nearCache().mode(NearCacheMode::INVALIDATED).maxEntries(10);
    RemoteCacheManager nearCacheManager(nearCacheBuilder.build(), false);
    JBasicMarshaller<std::string> *km = new JBasicMarshaller<std::string>();
    JBasicMarshaller<std::string> *vm = new JBasicMarshaller<std::string>();
    try
    {
    nearCacheManager.start();
    RemoteCache<std::string, std::string> nearCache = nearCacheManager.getCache<std::string, std::string>(km,
            &Marshaller<std::string>::destroy,
            vm,
            &Marshaller<std::string>::destroy);
    nearCache.clear();
    // Read stats to do some checks on hits and miss counter
    std::map<std::string,std::string> statsBegin= nearCache.stats();
    auto hitsBegin = std::stoi(statsBegin["hits"]);
    auto missesBegin = std::stoi(statsBegin["misses"]);
    // Only the first get goes to the remote cache and miss the value
    // then all the gets are resolved nearly
    nearCache.get("key1");
    nearCache.put("key1", "value1");
    std::string *rest = nearCache.get("key1");
    std::cout << "Got result from near cache:" << ((rest) ? *rest : "null") << std::endl;
    delete rest;
    rest = nearCache.get("key1");
    delete rest;
    std::map<std::string,std::string> stats1= nearCache.stats();
    auto hits1 = std::stoi(stats1["hits"]);
    auto misses1 = std::stoi(stats1["misses"]);
    std::cout << "Remote misses is: " << misses1-missesBegin << "" << std::endl;
    std::cout << "Remote hits is: " << hits1-hitsBegin << "" << std::endl;
    for(int i=2; i <= 11; i++)
    {   // fill cache with 10 more entries (11 in total)
        nearCache.put("key"+std::to_string(i),std::to_string(i));
    }
    // now key1 one should not be near
    rest = nearCache.get("key1");  // remote get. Stored near (this delete key2 nearly)
    delete rest;
    rest = nearCache.get("key2");  // remote get. Stored near (this delete key3 nearly)
    delete rest;
    rest = nearCache.get("key1");  // near
    delete rest;
    std::map<std::string,std::string> statsEnd= nearCache.stats();
    auto hitsEnd = std::stoi(statsEnd["hits"]);
    auto missesEnd = std::stoi(statsEnd["misses"]);
    std::cout << "Remote misses is: " << missesEnd-missesBegin << "" << std::endl;
    std::cout << "Remote hits is: " << hitsEnd-hitsBegin << "" << std::endl;
    nearCacheManager.stop();
    }
    catch (Exception &e) {
        nearCacheManager.stop();
    }
}
