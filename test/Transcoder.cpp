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

template<class T>
void assert_not_null(const std::string& message, int line, const std::unique_ptr<T>& pointer) {
    if (pointer.get() == 0) {
        std::cerr << message << ":" << line << std::endl;
        std::cerr.flush();
        throw std::exception();
    }
}

// Hotrod 2.8 with mediatype allows to store and retrieve entries in different data format
// this can be done specifying the mediatype for the communication
int main(int argc, char** argv) {
    ConfigurationBuilder builder;
    builder.protocolVersion(Configuration::PROTOCOL_VERSION_28);
    builder.addServer().host("127.0.0.1").port(11222);
    builder.balancingStrategyProducer(nullptr);
    RemoteCacheManager cacheManager(builder.build(), false);
    cacheManager.start();

    std::cout << "Tests for CacheManager" << std::endl;
    Marshaller<std::string> *km = new JBasicMarshaller<std::string>();
    Marshaller<std::string> *vm = new JBasicMarshaller<std::string>();
    RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>(km,
            &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy, std::string("transcodingCache"));

    // Define a data format for json
    DataFormat<std::string, std::string> df;
    df.keyMediaType = MediaType(APPLICATION_UNKNOWN_TYPE);
    df.valueMediaType = MediaType(APPLICATION_JSON_TYPE);
    df.valueMarshaller.reset(new BasicMarshaller<std::string>());
    RemoteCache<std::string, std::string> cacheJson = cache.withDataFormat(&df);
    // Define a data forma for jboss marshaller
    DataFormat<std::string, std::string> df1;
    df1.keyMediaType = MediaType(APPLICATION_JBOSS_MARSHALLING_TYPE);
    df1.valueMediaType = MediaType(APPLICATION_JBOSS_MARSHALLING_TYPE);
    RemoteCache<std::string, std::string> cacheJBoss = cache.withDataFormat(&df1);

    std::string k1("key13");
    std::string k2("key14");
    std::string v1("boron");
    std::string v2("\"chlorine\"");

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
    std::unique_ptr<std::string> rv2Json(cacheJson.get(k2));
    assert_not_null("get returned null!", __LINE__, rv2Json);
    if (rv2Json->compare("\"chlorine\"")) {
        std::cerr << "get/put fail for " << k2 << " got " << *rv2 << " expected \"chlorine\"" << std::endl;
        return 1;
    }

    std::unique_ptr<std::string> rv2JBoss(cacheJBoss.get(k2));
    assert_not_null("get returned null!", __LINE__, rv2JBoss);
    if (rv2JBoss->compare(v2)) {
        std::cerr << "get/put fail for " << k2 << " got " << *rv2JBoss << " expected " << v2 << std::endl;
        return 1;
    }
    }
