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
#include <string>
// For CTest: return 0 if all tests pass, non-zero otherwise.

#define ASSERT(VALUE, COND, RES, MSG) \
	do { if (!(COND)){ \
		std::cout << "FAIL :(" << #COND << "):(" << VALUE << "):" << #MSG << std::endl; \
		RES=-1; }\
		} while(0)

using namespace infinispan::hotrod;

int main(int argc, char** argv) {

    int result=0;
    std::cout << "Cluster" << std::endl;
    {
        ConfigurationBuilder builder;
                builder.balancingStrategyProducer(nullptr);
        builder.addServer().host(argc > 1 ? argv[1] : "127.0.0.1").port(11322);
        builder.addCluster("B-CLUSTER").addClusterNode("127.0.0.1", 11822);
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
        int i = 0;
        std::vector<std::vector<char> > filterFactoryParams;
        std::vector<std::vector<char> > converterFactoryParams;
        CacheClientListener<int, std::string> cl(cache);

        int createdCount=0, modifiedCount=0, removedCount=0;
        std::function<void(ClientCacheEntryCreatedEvent<int>)> listenerCreated = [&createdCount](ClientCacheEntryCreatedEvent<int> e)
        		{ std::cout << "created entry with key: " << e.getKey() << std::endl; createdCount++; };
        std::function<void(ClientCacheEntryModifiedEvent<int>)> listenerModified = [&modifiedCount](ClientCacheEntryModifiedEvent <int> e) { modifiedCount++; };
        std::function<void(ClientCacheEntryRemovedEvent<int>)> listenerRemoved = [&removedCount](ClientCacheEntryRemovedEvent <int> e) { removedCount++; };
        std::function<void()> listenerRecovery = [&cache, &cl, &filterFactoryParams, &converterFactoryParams, &listenerRecovery]()
        		{ std::cout << "Event listener transport down!!" << std::endl;
        		cache.addClientListener(cl, filterFactoryParams, converterFactoryParams, listenerRecovery);};
        cl.add_listener(listenerCreated);
        cl.add_listener(listenerModified);
        cl.add_listener(listenerRemoved);

        cache.addClientListener(cl, filterFactoryParams, converterFactoryParams, listenerRecovery);

        while (true)
        {
        	std::string s = "key"+std::to_string(i);
        	std::cout << "cache.put("<<i<<","<<s<<")"<<std::endl;
        	cache.put(i, s);
        	std::this_thread::sleep_for(std::chrono::seconds(2));
        	std::string* res=cache.get(i);
        	std::cout << "cache.get(" << i << ")=" << (res ? *res : "null") << std::endl << std::endl;
        	std::this_thread::sleep_for(std::chrono::seconds(2));
        	ASSERT(res,res && *res==s,result,"get returns wrong value");
        	i++;
        }
        cacheManager.stop();
    }
    if (result==0)
    {
      std::cout << "PASS: Cluster" << std::endl;
    }
    else
    {
      std::cout << "FAIL: Cluster" << std::endl;
    }
    return result;
}

