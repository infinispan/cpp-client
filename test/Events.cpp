#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/Version.h"

#include "infinispan/hotrod/BasicMarshaller.h"
#include "infinispan/hotrod/CacheClientListener.h"
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <thread>
#include <future>
#include <chrono>
#include <functional>
#include <mutex>

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
        builder.protocolVersion(argc > 1 ? argv[1] : Configuration::PROTOCOL_VERSION_24);
        builder.addServer().host(argc > 2 ? argv[2] : "127.0.0.1").port(argc > 3 ? atoi(argv[3]) : 11222);
        RemoteCacheManager cacheManager(builder.build(), false);
        cacheManager.start();
        JBasicMarshaller<int> *km = new JBasicMarshaller<int>();
        JBasicMarshaller<std::string> *vm = new JBasicMarshaller<std::string>();
        RemoteCache<int, std::string> cache = cacheManager.getCache<int, std::string>(km,
                &Marshaller<int>::destroy,
                vm,
                &Marshaller<std::string>::destroy, "Default", true);
        cache.clear();
        std::vector<std::vector<char> > filterFactoryParams;
        std::vector<std::vector<char> > converterFactoryParams;
        CacheClientListener<int, std::string> cl(cache);
        int createdCount=0, modifiedCount=0, removedCount=0, expiredCount=0;
        // We're using future and promise to have a basic listeners/main thread synch
        int setFutureEventKey=0;
        std::promise<void> promise;
        // Listeners definitions
        std::function<void(ClientCacheEntryCreatedEvent<int>)> listenerCreated
            = [&createdCount, &setFutureEventKey, &promise](ClientCacheEntryCreatedEvent<int> e) {
                createdCount++;
                if (setFutureEventKey==e.getKey())
                {
                    promise.set_value();
                };
            };
        std::function<void(ClientCacheEntryModifiedEvent<int>)> listenerModified
            = [&modifiedCount, &setFutureEventKey, &promise](ClientCacheEntryModifiedEvent <int> e) {
                modifiedCount++;
                if (setFutureEventKey==e.getKey())
                {
                    promise.set_value();
                }
            };
        std::function<void(ClientCacheEntryRemovedEvent<int>)> listenerRemoved
            = [&removedCount, &setFutureEventKey, &promise](ClientCacheEntryRemovedEvent <int> e) {
                removedCount++;
                if (setFutureEventKey==e.getKey())
                {
                    promise.set_value();
                }
            };
        std::function<void(ClientCacheEntryExpiredEvent<int>)> listenerExpired
            = [&expiredCount, &setFutureEventKey, &promise](ClientCacheEntryExpiredEvent <int> e) {
                expiredCount++;
                if (setFutureEventKey==e.getKey())
                {
                    promise.set_value();
                }
            };

        cl.add_listener(listenerCreated);
        cl.add_listener(listenerModified);
        cl.add_listener(listenerRemoved);
        cl.add_listener(listenerExpired);


        cache.put(1,"v1");
        cache.put(2,"v2");
        cache.put(3,"v3");

        cache.addClientListener(cl, filterFactoryParams, converterFactoryParams);
        // This tells the listener to fill the promise on event with key = 6
        setFutureEventKey=6;
        cache.put(4,"v4");
        cache.put(5,"v5");
        cache.put(6,"v6");
        if (std::future_status::timeout==promise.get_future().wait_for(std::chrono::seconds(30)))
        { 
          std::cout << "FAIL: Events and Listeners on Create (Timeout)" << std::endl;
          return -1;
        }
        ASSERT(createdCount, createdCount == 3, result, "created count mismatch");

        setFutureEventKey=2;
        promise= std::promise<void>();
        cache.put(1,"v1a");
        cache.put(2,"v2a");
        if (std::future_status::timeout==promise.get_future().wait_for(std::chrono::seconds(30)))
        { 
          std::cout << "FAIL: Events and Listeners on Modify (Timeout)" << std::endl;
          return -1;
        }
        ASSERT(modifiedCount, modifiedCount == 2, result, "modified count mismatch");

        setFutureEventKey=3;
        promise= std::promise<void>();
        cache.remove(3);
        if (std::future_status::timeout==promise.get_future().wait_for(std::chrono::seconds(30)))
        { 
          std::cout << "FAIL: Events and Listeners on Remove (Timeout)" << std::endl;
          return -1;
        }
        ASSERT(removedCount, removedCount == 1, result, "removed count mismatch");

        // Now test expired events
        setFutureEventKey=10;
        promise= std::promise<void>();
        cache.put(11,"nexp");
        cache.put(10,"exp1",3);

        if (std::future_status::timeout==promise.get_future().wait_for(std::chrono::seconds(30)))
        {
              std::cout << "FAIL: Events and Listeners (Timeout)" << std::endl;
              return -1;
        }

        promise= std::promise<void>();
        if (std::future_status::timeout==promise.get_future().wait_for(std::chrono::seconds(30)))
        {
              std::cout << "FAIL: Events and Listeners (Timeout)" << std::endl;
              return -1;
        }
        ASSERT(expiredCount, expiredCount == 1, result, "removed count mismatch");



        cache.removeClientListener(cl);

        // Now add a second listener to check includeCurrentState CS feature
        CacheClientListener<int, std::string> clCS(cache);
        clCS.includeCurrentState=true;
        int createdCountCS=0, modifiedCountCS=0, removedCountCS=0;

        std::function<void(ClientCacheEntryCreatedEvent<int>)> listenerCreatedCS
            = [&createdCountCS, &setFutureEventKey, &promise](ClientCacheEntryCreatedEvent<int> e) {
                createdCountCS++;
                if (setFutureEventKey==e.getKey())
                {
                    promise.set_value();
                }
            };
        std::function<void(ClientCacheEntryModifiedEvent<int>)> listenerModifiedCS
            = [&modifiedCountCS, &setFutureEventKey, &promise](ClientCacheEntryModifiedEvent <int> e) {
                 modifiedCountCS++;
                 if (setFutureEventKey==e.getKey())
                 {
                     promise.set_value();
                 }
            };
        std::function<void(ClientCacheEntryRemovedEvent<int>)> listenerRemovedCS
            = [&removedCountCS, &setFutureEventKey, &promise](ClientCacheEntryRemovedEvent <int> e) {
                removedCountCS++;
                if (setFutureEventKey==e.getKey())
                {
                    promise.set_value();
                }
            };

        clCS.add_listener(listenerCreatedCS);
        clCS.add_listener(listenerModifiedCS);
        clCS.add_listener(listenerRemovedCS);

        setFutureEventKey=8;
        promise=std::promise<void>();
        cache.addClientListener(clCS, filterFactoryParams, converterFactoryParams);
        cache.put(8,"v8");
        if (std::future_status::timeout==promise.get_future().wait_for(std::chrono::seconds(30)))
        { 
          std::cout << "FAIL: Events and Listeners (Timeout)" << std::endl;
          return -1;
        }
        ASSERT(createdCountCS, createdCountCS == 7, result, "created with current state");
        std::cout << "9 Events and Listeners" << std::endl;

        setFutureEventKey=2;
        promise=std::promise<void>();
        cache.put(1,"v1a");
        cache.put(2,"v2a");
        if (std::future_status::timeout==promise.get_future().wait_for(std::chrono::seconds(30)))
        { 
          std::cout << "FAIL: Events and Listeners (Timeout)" << std::endl;
          return -1;
        }
        ASSERT(modifiedCountCS, modifiedCountCS == 2, result, "modified count mismatch");

        setFutureEventKey=8;
        promise=std::promise<void>();
        cache.remove(8);
        if (std::future_status::timeout==promise.get_future().wait_for(std::chrono::seconds(30)))
        { 
          std::cout << "FAIL: Events and Listeners (Timeout)" << std::endl;
          return -1;
        }
        ASSERT(removedCountCS, removedCountCS == 1, result, "removed count mismatch");

        cache.removeClientListener(clCS);

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

