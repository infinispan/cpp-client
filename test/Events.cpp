#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/Version.h"

#include "infinispan/hotrod/JBasicMarshaller.h"
#include "infinispan/hotrod/CacheClientListener.h"
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <thread>
#include <future>
#include <chrono>
#include <functional>

// For CTest: return 0 if all tests pass, non-zero otherwise.

#define ASSERT(VALUE, COND, RES, MSG) \
	do { if (!(COND)){ \
		std::cout << "FAIL :(" << #COND << "):(" << VALUE << "):" << #MSG << std::endl; \
		RES=-1; }\
		} while(0)

using namespace infinispan::hotrod;

int main(int argc, char** argv) {

    int result=0;
    std::cout << "Events and Listeners" << std::endl;
    {
        ConfigurationBuilder builder;
                builder.balancingStrategyProducer(nullptr);
        builder.addServer().host(argc > 1 ? argv[1] : "127.0.0.1").port(argc > 2 ? atoi(argv[2]) : 11222);
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
        RemoteCacheManager cacheManager(builder.build(), false);
        cacheManager.start();
        JBasicMarshaller<int> *km = new JBasicMarshaller<int>();
        JBasicMarshaller<std::string> *vm = new JBasicMarshaller<std::string>();
        RemoteCache<int, std::string> cache = cacheManager.getCache<int, std::string>(km,
                &Marshaller<int>::destroy,
                vm,
                &Marshaller<std::string>::destroy);
        cache.clear();
        std::vector<std::vector<char> > filterFactoryParams;
        std::vector<std::vector<char> > converterFactoryParams;
        CacheClientListener<int, std::string> cl(cache);
        int createdCount=0, modifiedCount=0, removedCount=0;
        std::function<void(ClientCacheEntryCreatedEvent<int>)> listenerCreated = [&createdCount](ClientCacheEntryCreatedEvent<int> e) { createdCount++; };
        std::function<void(ClientCacheEntryModifiedEvent<int>)> listenerModified = [&modifiedCount](ClientCacheEntryModifiedEvent <int> e) { modifiedCount++; };
        std::function<void(ClientCacheEntryRemovedEvent<int>)> listenerRemoved = [&removedCount](ClientCacheEntryRemovedEvent <int> e) { removedCount++; };

        cl.add_listener(listenerCreated);
        cl.add_listener(listenerModified);
        cl.add_listener(listenerRemoved);

        cache.put(1,"v1");
        cache.put(2,"v2");
        cache.put(3,"v3");

        cache.addClientListener(cl, filterFactoryParams, converterFactoryParams);

        cache.put(4,"v4");
        cache.put(5,"v5");
        cache.put(6,"v6");

        // Give some time to listener thread to update the counters
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        ASSERT(createdCount, createdCount == 3, result, "created count mismatch");

        cache.put(1,"v1a");
        cache.put(2,"v2a");
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        ASSERT(modifiedCount, modifiedCount == 2, result, "modified count mismatch");

        cache.remove(3);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        ASSERT(removedCount, removedCount == 1, result, "removed count mismatch");

        cache.removeClientListener(cl);

        // Now add a second listener to check includeCurrentState CS feature
        CacheClientListener<int, std::string> clCS(cache);
        clCS.includeCurrentState=true;
        int createdCountCS=0, modifiedCountCS=0, removedCountCS=0;
        std::function<void(ClientCacheEntryCreatedEvent<int>)> listenerCreatedCS = [&createdCountCS](ClientCacheEntryCreatedEvent<int> e) { createdCountCS++; };
        std::function<void(ClientCacheEntryModifiedEvent<int>)> listenerModifiedCS = [&modifiedCountCS](ClientCacheEntryModifiedEvent <int> e) { modifiedCountCS++; };
        std::function<void(ClientCacheEntryRemovedEvent<int>)> listenerRemovedCS = [&removedCountCS](ClientCacheEntryRemovedEvent <int> e) { removedCountCS++; };

        clCS.add_listener(listenerCreatedCS);
        clCS.add_listener(listenerModifiedCS);
        clCS.add_listener(listenerRemovedCS);

        cache.addClientListener(clCS, filterFactoryParams, converterFactoryParams);

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        ASSERT(createdCountCS, createdCountCS == 5, result, "created with current state");

        cache.put(1,"v1a");
        cache.put(2,"v2a");

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        ASSERT(modifiedCountCS, modifiedCountCS == 2, result, "modified count mismatch");

        cache.remove(3);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        ASSERT(removedCountCS, removedCountCS == 0, result, "removed count mismatch");

        cacheManager.stop();
    }
    if (result==0)
    {
      std::cout << "PASS: Events and Listeners" << std::endl;
    }
    else
    {
      std::cout << "FAIL: Events and Listeners" << std::endl;
    }
    return result;
}

