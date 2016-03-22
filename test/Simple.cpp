#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/Version.h"

#include "infinispan/hotrod/JBasicMarshaller.h"
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <thread>
#include <future>
#include <chrono>

// For CTest: return 0 if all tests pass, non-zero otherwise.

using namespace infinispan::hotrod;

namespace infinispan {
    namespace hotrod {
        namespace transport {

            class MyRoundRobinBalancingStrategy : public FailOverRequestBalancingStrategy
            {
            public:
                MyRoundRobinBalancingStrategy() : index(0) { }

                static FailOverRequestBalancingStrategy *newInstance() {
                    return new MyRoundRobinBalancingStrategy();
                }

                void setServers(const std::vector<ServerNameId> &s) {
                    servers = s;
                    // keep the old index if possible so that we don't produce more requests for the first server
                    if (index >= servers.size()) {
                        index = 0;
                    }
                }

                ~MyRoundRobinBalancingStrategy() { }

                const ServerNameId &getServerByIndex(size_t pos) {
                    const ServerNameId &server = servers[pos];
                    return server;
                }
            private:
                std::vector<ServerNameId> servers;
                size_t index;
                const ServerNameId &nextServer() {
                    const ServerNameId &server = getServerByIndex(index++);
                    if (index >= servers.size()) {
                        index = 0;
                    }
                    return server;
                }


            };

        }}}


template <class T>
void assert_not_null(const std::string& message, int line,  const std::unique_ptr<T>& pointer) {
  if (pointer.get() == 0) {
    std::cerr << message << ":" << line << std::endl;
    std::cerr.flush();
    throw std::exception();
  }
}

template <typename K, typename V>
int basicTest(RemoteCacheManager &cacheManager, RemoteCache<K,V> &cache) {

    std::cout << "HotRod C++ Library version " << cache.getVersion() << std::endl;
    std::cout << "Protocol " << cache.getProtocolVersion() << std::endl;

    std::string k1("key13");
    std::string k2("key14");
    std::string v1("boron");
    std::string v2("chlorine");

    cache.clear();

    // put
    cache.put(k1, v1);
    std::unique_ptr<std::string> rv(cache.get(k1));
    assert_not_null("get returned null!", __LINE__, rv);
    if (rv->compare(v1)) {
        std::cerr << "get/put fail for " << k1 << " got " << *rv << " expected " << v1 << std::endl;
        return 1;
    }

    cache.put(k2, v2);
    std::unique_ptr<std::string> rv2(cache.get(k2));
    assert_not_null("get returned null!", __LINE__, rv2);
    if (rv2->compare(v2)) {
        std::cerr << "get/put fail for " << k2 << " got " << *rv2 << " expected " << v2 << std::endl;
        return 1;
    }
 
    std::unique_ptr<std::string> badValue(cache.get(std::string("no such key in the cache")));
    if (badValue.get()) {
        std::cout << "non-existent key failure, got " << *badValue << std::endl;
        return 1;
    }

    std::cout << "PASS: simple get/put" << std::endl;

    {
      std::string k3("rolling");
      std::string v3("stones");
      std::string v4("beatles");

      // putIfAbsent
      cache.putIfAbsent(k3, v3);
      std::unique_ptr<std::string> rv3(cache.get(k3));
      assert_not_null("get returned null!", __LINE__, rv3);
      if (rv3->compare(v3)) {
          std::cerr << "putIfAbsent fail for " << k3 << " got " << *rv3 << " expected " << v3 << std::endl;
          return 1;
      }

      cache.putIfAbsent(k3, v4);
      std::unique_ptr<std::string> rv4(cache.get(k3));
      assert_not_null("get returned null!", __LINE__, rv4);
      if (rv4->compare(v3)) {
          std::cerr << "putIfAbsent fail for " << k3 << " got " << *rv4 << " expected " << v3 << std::endl;
          return 1;
      }

      std::cout << "PASS: simple putIfAbsent"  << std::endl;
    }
    {
      std::string k3("rolling");
      std::string v3("stones");
      std::string v4("beatles");

      // putIfAbsent
      cache.putIfAbsent(k3, v3);
      std::unique_ptr<std::string> rv3(cache.get(k3));
      assert_not_null("get returned null!", __LINE__, rv3);
      if (rv3->compare(v3)) {
          std::cerr << "putIfAbsent fail for " << k3 << " got " << *rv3 << " expected " << v3 << std::endl;
          return 1;
      }

      std::unique_ptr<std::string> rv4(cache.withFlags(FORCE_RETURN_VALUE).putIfAbsent(k3, v4));
      assert_not_null("get returned null!", __LINE__, rv4);
      if (rv4->compare(v3)) {
          std::cerr << "putIfAbsent fail for " << k3 << " got " << *rv4 << " expected " << v3 << std::endl;
          return 1;
      }

      std::cout << "PASS: simple putIfAbsent with force return value"  << std::endl;
    }

    std::string k3("rolling");
    std::string v3("stones");
    std::string v4("beatles");

    cache.put(k3, v3, 10, SECONDS);
    // getWithMetadata
    std::pair<std::shared_ptr<std::string>, MetadataValue> rv5 = cache.getWithMetadata(k3);
    if (!rv5.first.get() || rv5.second.lifespan != 10) {
        std::cerr << "getWithMetadata with mortal entry fail for " << k3 << " not found" << std::endl;
        return 1;
    }
    std::cout << "PASS: simple getWithMetadata with mortal entry"  << std::endl;

    cache.put(k3, v3);
    // getWithMetadata
    rv5 = cache.getWithMetadata(k3);
    if (!rv5.first.get()
        || rv5.second.lifespan >= 0 || rv5.second.created >= 0
        || rv5.second.maxIdle >= 0 || rv5.second.lastUsed >= 0) {
        std::cerr << "getWithMetadata with immortal entry fail for " << k3 << std::endl;
        return 1;
    }
    std::cout << "PASS: simple getWithMetadata with immortal entry"  << std::endl;

    // getWithVersion
    std::pair<std::shared_ptr<std::string>, VersionedValue> rv5a = cache.getWithVersion(k3);
    if (!rv5a.first.get()) {
        std::cerr << "getWithVersion fail for " << k3 << " not found" << std::endl;
        return 1;
    }

    std::cout << "PASS: simple getWithVersion"  << std::endl;

    // replaceWithVersion
    cache.replaceWithVersion(k3, v4, rv5.second.version);
    std::unique_ptr<std::string> rv6(cache.get(k3));
    assert_not_null("get returned null!", __LINE__, rv6);
    if (rv6->compare(v4)) {
        std::cerr << "replaceWithVersion fail for " << k3 << " got " << *rv6 << " expected " << v4 << std::endl;
        return 1;
    }

    cache.replaceWithVersion(k3, v3, rv5.second.version);
    std::unique_ptr<std::string> rv7(cache.get(k3));
    assert_not_null("get returned null!", __LINE__, rv7);
    if (rv7->compare(v4)) {
        std::cerr << "replaceWithVersion fail for " << k3 << " got " << *rv7 << " expected " << v4 << std::endl;
        return 1;
    }

    std::cout << "PASS: simple replaceWithVersion" << std::endl;

    // size
    uint64_t size = cache.size();
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
    uint64_t size2 = cache.size();
    if (size2 != 0) {
        std::cerr << "clear fail cache has size " << size2 << " expected 0 " << std::endl;
        return 1;
    }

    std::cout << "PASS: simple clear" << std::endl;

    std::string k4("real");
    std::string v5("madrid");
    std::string v6("barcelona");

    // put with FORCE_RETURN_VALUE flag
    std::unique_ptr<std::string> rv8(cache.withFlags(FORCE_RETURN_VALUE).put(k4,v5));
    if (rv8.get()) {
        std::cerr << "put with FORCE_RETURN_FLAG fail for " << k4 << " got " << *rv8 << " expected null pointer" << std::endl;
        return 1;
    }

    std::unique_ptr<std::string> rv9(cache.withFlags(FORCE_RETURN_VALUE).put(k4,v6));
    assert_not_null("get returned null!", __LINE__, rv9);
    if (rv9->compare(v5)) {
        std::cerr << "put with FORCE_RETURN_FLAG fail for " << k4 << " got " << *rv9 << " expected " << v5 << std::endl;
        return 1;
    }

    std::cout << "PASS: simple put with FORCE_RETURN_FLAG" << std::endl;

    // keySet
    std::set<std::shared_ptr<std::string> > keySet = cache.keySet();
    if (keySet.size()!=1) {
        std::cerr << "keySet fail got " << keySet.size() << " entries expected 1" << std::endl;
        return 1;
    }

    std::cout << "  keySet result is:" << std::endl;
    for(std::set<std::shared_ptr<std::string> >::const_iterator i=keySet.begin(); i!=keySet.end(); i++) {
        std::cout << "    key: " << *i->get() << std::endl;
    }

    std::cout << "PASS: simple keySet" << std::endl;

    // getBulk
    std::map<std::shared_ptr<std::string>,std::shared_ptr<std::string> > map = cache.getBulk();
    if (map.size()!=1) {
        std::cerr << "getBulk fail got" << map.size() << " entries expected 1" << std::endl;
        return 1;
    }

    std::cout << "  getBulk result is:" << std::endl;
    for(std::map<std::shared_ptr<std::string>,std::shared_ptr<std::string> >::const_iterator i=map.begin(); i!=map.end(); i++) {
        std::cout << "    key: " << *i->first.get() << ", value: " << *i->second.get() << std::endl;
    }

    std::cout << "PASS: simple getBulk" << std::endl;

    // replace
    cache.replace(k4,v5);
    std::unique_ptr<std::string> rv10(cache.get(k4));
    assert_not_null("get returned null!", __LINE__, rv10);
    if (rv10->compare(v5)) {
        std::cerr << "replace fail for " << k4 << " got " << *rv10 << " expected " << v5 << std::endl;
        return 1;
    }

    std::cout << "PASS: simple replace" << std::endl;
    try {
        RemoteCache<std::string, std::string> namedCache =
                cacheManager.getCache<std::string, std::string>("namedCache", false);
        std::unique_ptr<std::string> namedCacheV1(namedCache.get("k1"));
    }
    catch (...)
    {
        return 1;
    }
    std::cout << "PASS: get for namedCache" << std::endl;

    // get non-existing cache
        try {
            RemoteCache<std::string, std::string> non_existing =
                    cacheManager.getCache<std::string, std::string>("non-existing", false);
            std::cerr << "fail getCache for non-existing cache didn't throw exception" << std::endl;
            return 1;
        } catch (const HotRodClientException &ex) {
            std::cout << "PASS: get non-existing cache" << std::endl;
        } catch (const Exception &e) {
            std::cout << "is: " << typeid(e).name() << '\n';
            std::cerr << "fail unexpected exception: " << e.what() << std::endl;
            return 1;
        }

    // Async test
    std::string ak1("asyncK1");
    std::string av1("sun");

    std::string ak2("asyncK2");
    std::string av2("mercury");

    cache.clear();

/*    // Put ak1,av1 in async thread A
    std::future<std::string*> future_put= cache.putAsync(ak1,av1);
    // Get the value in this thread
    std::string* arv1= cache.get(ak1);
    if (future_put.wait_for(std::chrono::seconds(0))!=std::future_status::ready)
    {
    	// get happens before put completes, arv1 must be null
    	if (arv1)
    	{
    		std::cerr << "fail: expected null got value" << std::endl;
    		return 1;
    	}
    }
    // Now wait for put completion
    future_put.wait();

    // All is synch now
    std::string* arv11= cache.get(ak1);
    if (!arv11 || arv11->compare(av1))
    {
    	std::cout << "fail: expected " << av1 << "got " << (arv11 ? *arv11 : "null") << std::endl;
    	return 1;
    }

    // Read ak1 again, but in async way and test that the result is the same
    std::future<std::string*> future_ga= cache.getAsync(ak1);
    std::string* arv2= future_ga.get();
    if (!arv2 || arv2->compare(av1))
    {
    	std::cerr << "fail: expected " << av1 << " got " << (arv2 ? *arv2 : "null") << std::endl;
    	return 1;
    }
*/
    bool flag=false;
    // Now user pass a lambda func that will be executed in the async thread after the put completion
    std::future<std::string*> future_put1= cache.putAsync(ak2,av2,0,0,[&] (std::string *v){flag=true; return v;});
    {
    	// If the async put is not completed flag must be false
    	if (future_put1.wait_for(std::chrono::seconds(0))!=std::future_status::ready)
    	{
    		if (flag)
    		{
    			std::cerr << "fail: expected false got true" << std::endl;
    			return 1;
    		}
    	}
    }
    // Now wait for put completion
    future_put1.wait();
    {
    	// The user lambda must be executed so flag must be true
    	if (!flag)
    	{
    		std::cerr << "fail: expected true got false" << std::endl;
    		return 1;
    	}
    }
    // Same test for get
    flag=false;
    std::future<std::string*> future_get1= cache.getAsync(ak2,[&] (std::string *v){flag=true; return v;});

    if (future_get1.wait_for(std::chrono::seconds(0))!=std::future_status::ready)
    {
    	if (flag)
    	{
    		std::cerr << "fail: expected false got true" << std::endl;
    		return 1;
    	}
    }
    future_get1.wait();
    {
    	if (!flag)
    	{
    		std::cerr << "fail: expected true got false" << std::endl;
    		return 1;
    	}
    }
    std::string* arv3= future_get1.get();

    if (!arv3 || arv3->compare(av2))
    {
    	std::cerr << "fail: expected " << av2 << " got " << (arv3 ? *arv3 : "null") << std::endl;
    	return 1;
    }
    std::cout << "PASS: Async test" << std::endl;
    return 0;
}

int main(int argc, char** argv) {
    // Call basic test for every marshaller and every codec

    int result;
    std::cout << "Basic Test with BasicMarshaller" << std::endl;
    {
        ConfigurationBuilder builder;
        builder.addServer().host(argc > 1 ? argv[1] : "127.0.0.1").port(argc > 2 ? atoi(argv[2]) : 11222).protocolVersion(Configuration::PROTOCOL_VERSION_24);
//        builder.balancingStrategyProducer(transport::MyRoundRobinBalancingStrategy::newInstance);
        builder.balancingStrategyProducer(nullptr);
        RemoteCacheManager cacheManager(builder.build(), false);
        BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
        BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
        RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>(km,
                &Marshaller<std::string>::destroy,
                vm,
                &Marshaller<std::string>::destroy);
        cacheManager.start();
        result = basicTest<std::string, std::string>(cacheManager, cache);

        try
        {
            std::map<std::string,std::string> s;
            std::string argName = std::string("a");
            std::string argValue = std::string("b");
            // execute() operation wants JBossMarshalling format sometimes
            s.insert(std::pair<std::string, std::string>(argName,JBasicMarshaller<std::string>::addPreamble(argValue)));
            std::string script ("// mode=local,language=javascript\n "
            "var cache = cacheManager.getCache();\n"
            "cache.put(\"a\", \"abc\");\n"
            "cache.put(\"b\", \"b\");\n"
            "cache.get(\"a\");\n");
            std::string script_name("script.js");
            std::string p_script_name=JBasicMarshaller<std::string>::addPreamble(script_name);
            std::string p_script=JBasicMarshaller<std::string>::addPreamble(script);
            RemoteCache<std::string, std::string> scriptCache=cacheManager.getCache<std::string,std::string>("___script_cache",false);
            scriptCache.put(p_script_name, p_script);
            char* execResult = cache.execute(script_name,s);


            // We know the remote script returns a string and
            // we use the helper to unmarshall
            std::string res(JBasicMarshallerHelper::unmarshall<std::string>(execResult));
            if (res.compare("abc")!=0)
            {
                std::cerr << "fail: cache.exec() returned unexpected result"<< std::endl;
                return 1;
            }
            delete(execResult);
        } catch (const Exception& e) {
            std::cout << "is: " << typeid(e).name() << '\n';
            std::cerr << "fail unexpected exception: " << e.what() << std::endl;
            return 1;
        }
        std::cout << "PASS: script execution on server" << std::endl;

        cacheManager.stop();
    }
    if (result!=0)
        return result;

    std::cout << "Basic Test with JBasicMarshaller" << std::endl;
    {
        ConfigurationBuilder builder;
        //        builder.balancingStrategyProducer(transport::MyRoundRobinBalancingStrategy::newInstance);
                builder.balancingStrategyProducer(nullptr);
        builder.addServer().host(argc > 1 ? argv[1] : "127.0.0.1").port(argc > 2 ? atoi(argv[2]) : 11222).protocolVersion(Configuration::PROTOCOL_VERSION_24);
        RemoteCacheManager cacheManager(builder.build(), false);
        JBasicMarshaller<std::string> *km = new JBasicMarshaller<std::string>();
        JBasicMarshaller<std::string> *vm = new JBasicMarshaller<std::string>();
        RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>(km,
                &Marshaller<std::string>::destroy,
                vm,
                &Marshaller<std::string>::destroy);
        cacheManager.start();
        result = basicTest<std::string, std::string>(cacheManager, cache);
        try
        {
            std::map<std::string,std::string> s;
            std::string argName = std::string("a");
            std::string argValue = std::string("b");
            s.insert(std::pair<std::string, std::string>(argName,argValue));
            std::string script ("// mode=local,language=javascript\n "
            "var cache = cacheManager.getCache();\n"
            "cache.put(\"a\", \"abc\");\n"
            "cache.put(\"b\", \"b\");\n"
            "cache.get(\"a\");\n");
            std::string script_name("script.js");
            cacheManager.getCache<std::string,std::string>(km,
                    &Marshaller<std::string>::destroy,
                    vm,
                    &Marshaller<std::string>::destroy,"___script_cache").put(script_name, script);
            char* execResult = cache.execute(script_name,s);

            // We know the remote script returns a string and
            // we use the helper to unmarshall
            std::string res(JBasicMarshallerHelper::unmarshall<std::string>(execResult));
            if (res.compare("abc")!=0)
            {
                std::cerr << "fail: cache.exec() returned unexpected result"<< std::endl;
                return 1;
            }
            delete(execResult);
        } catch (const Exception& e) {
            std::cout << "is: " << typeid(e).name() << '\n';
            std::cerr << "fail unexpected exception: " << e.what() << std::endl;
            return 1;
        }
        std::cout << "PASS: script execution on server" << std::endl;

        cacheManager.stop();
    }
    return result;
}

