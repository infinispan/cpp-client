#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/Version.h"

#include "infinispan/hotrod/JBasicMarshaller.h"
#include "infinispan/hotrod/JBossMarshaller.h"
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <thread>
#include <future>
#include <chrono>

// For CTest: return 0 if all tests pass, non-zero otherwise.

using namespace infinispan::hotrod;

// Some useful global variables
std::string keyName("keyName"), keyVal("keyValue");
std::string expected[50];

// Install scripts and populate the cache
void prepareCache(RemoteCache<std::string, std::string> cache0) {
    std::string script(
            "// mode=local, language=javascript, parameters=[keyName]\nvar now = new Date().getTime();\n  while(new Date().getTime() < now + (250)){}\nvar cache = cacheManager.getCache(\"namedCache\");\ncache.get(keyName)\n");
    auto execution = cache0.getRemoteExecution<>();
    execution.putScript("get.js", script);
    std::string putStr(
            "// mode=local, language=javascript, parameters=[keyValue, keyName, num]\nvar cache = cacheManager.getCache(\"namedCache\");\ncache.put(keyName, keyValue);\n");
    execution.putScript("put.js", putStr);
    for (int i = 0; i < 50; i++) {
        std::string k = "k" + std::to_string(i);
        expected[i] = "v" + std::to_string(i);
        execution.addArg(keyName, k);
        execution.addArg(keyVal, expected[i]);
        delete execution.template execute<std::string*>("put.js");
    }
}

std::string* doSlowGet(RemoteCache<std::string, std::string> cache0, int i) {
    auto execution = cache0.getRemoteExecution<>();
    std::string keyVal("k" + std::to_string(i));
    execution.addArg(keyName, keyVal);
    return execution.template execute<std::string*>("get.js");
}

int main(int argc, char **argv) {
    int result = 0;
    {
        std::cout << "Test pool with WAIT condition" << std::endl;
        // Cache configuration
        ConfigurationBuilder builder;
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_28);
        builder.addServer().host("127.0.0.1").port(11222);
        builder.balancingStrategyProducer(nullptr);
        RemoteCacheManager cacheManager(builder.build(), false);
        cacheManager.start();

        BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
        BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
        RemoteCache<std::string, std::string> cache0 = cacheManager.getCache<std::string, std::string>(km,
                &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy, "namedCache", false);

        DataFormat<std::string, std::string> df;
        df.keyMediaType.typeSubtype = std::string("application/x-jboss-marshalling");
        df.valueMediaType.typeSubtype = std::string("application/x-jboss-marshalling");
        RemoteCache<std::string, std::string> cache = cache0.withDataFormat(&df);

        prepareCache(cache);

        // Start 50 threads and see if everything works.
        // With the WAIT setup the threads have to wait if the
        // pool is exhausted
        std::unique_ptr<std::string> res[50];
        std::thread t[50];
        std::promise<void> p[50];
        for (int i = 0; i < 50; i++) {
            t[i] = std::thread([&res, &p, &cache, i]() {
                res[i] = std::unique_ptr<std::string>(doSlowGet(cache, i));
                p[i].set_value();
            });
        }

        for (int i = 0; i < 50; i++) {
            p[i].get_future().wait();
            if (*res[i] != expected[i]) {
                std::cout << "FAIL: value doesn't match for index " << i << std::endl;
                return 1;
            }
        }
        std::cout << "PASS: test pool with WAIT condition" << std::endl;
        for (int i = 0; i < 50; i++) {
            t[i].join();
        }
        cache.clear();
        cacheManager.stop();
    }
    if (result != 0)
        return result;
    {
        std::cout << "Test pool with EXCEPTION condition" << std::endl;
        // Cache configuration
        ConfigurationBuilder builder;
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_28);
        builder.addServer().host("127.0.0.1").port(11222);
        builder.balancingStrategyProducer(nullptr);
        builder.connectionPool().exhaustedAction(EXCEPTION);
        RemoteCacheManager cacheManager(builder.build(), false);
        cacheManager.start();

        BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
        BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
        RemoteCache<std::string, std::string> cache0 = cacheManager.getCache<std::string, std::string>(km,
                &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy, "namedCache", false);

        DataFormat<std::string, std::string> df;
        df.keyMediaType.typeSubtype = std::string("application/x-jboss-marshalling");
        df.valueMediaType.typeSubtype = std::string("application/x-jboss-marshalling");
        RemoteCache<std::string, std::string> cache = cache0.withDataFormat(&df);

        prepareCache(cache);

        for (int i = 0; i < 50000; i++) {
            cache0.put("k" + std::to_string(i), "v" + std::to_string(i));
        }

        // Start 50 threads and see if everything works.
        // With the EXCEPTION setup the user code must implement
        // a recover policy if pool is exhausted, this test just waits
        // for 1 sec and retry
        std::unique_ptr<std::string> res[50];
        std::thread t[50];
        std::promise<void> p[50];
        int exceptionsCaught = 0;
        for (int i = 0; i < 50; i++) {
            t[i] = std::thread([&res, &p, &cache, i, &exceptionsCaught]() {
                bool done = false;
                while (!done) {
                    try {
                        res[i] = std::unique_ptr<std::string>(doSlowGet(cache, i))  ;
                        p[i].set_value();
                        done = true;
                    } catch (NoSuchElementException &ex) {
                        exceptionsCaught++;
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }
                }
            });
        }
        for (int i = 0; i < 50; i++) {
            auto status = p[i].get_future().wait_for(std::chrono::seconds(30));
            if (status != std::future_status::ready) {
                std::cerr << "FAIL: Timeout waiting for results " << std::endl;
                exit(1); // Test failed just stop immediately
            }
            if (*res[i] != expected[i]) {
                std::cerr << "FAIL: value doesn't match for index: " << i << std::endl;
                exit(1); // Test failed just stop immediately
            }
        }
        std::cout << "Exceptions caught by the user: " << exceptionsCaught << std::endl;
        auto slowGetRet = doSlowGet(cache, 20);
        if (*res[20] != *slowGetRet) {
            std::cerr << "FAIL: value doesn't match for last get" << std::endl;
            delete slowGetRet;
            return 1;
        }
        delete slowGetRet;
        std::cout << "PASS: test pool with EXCEPTION condition" << std::endl;
        for (int i = 0; i < 50; i++) {
            t[i].join();
        }
        cacheManager.stop();
    }
    return 0;
}

