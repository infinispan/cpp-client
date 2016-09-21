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

        cache.addClientListener(cl, filterFactoryParams, converterFactoryParams);

        cache.put(1,"key1");
        cache.put(2,"key2");
        cache.put(3,"key3");

        cache.put(1,"key1a");
        cache.put(2,"key2a");

        cache.remove(3);

        if (createdCount!=3)
        {
    		std::cerr << "fail: listenerCreated expected 3 got " << createdCount << std::endl;
    		result=-1;
        }

        if (modifiedCount!=2)
        {
    		std::cerr << "fail: listenerModified expected 2 got " << modifiedCount << std::endl;
    		result=-1;
        }

        if (removedCount!=1)
        {
    		std::cerr << "fail: listenerRemoved expected 1 got " << removedCount << std::endl;
    		result=-1;
        }


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

