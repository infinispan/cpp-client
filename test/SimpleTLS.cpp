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

int main(int argc, char** argv) {
    std::cout << "TLS Test" << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " ca_path [client_ca_file]" << std::endl;
        return 1;
    }
    ConfigurationBuilder builder;
    builder.addServer().host("127.0.0.1").port(11222);
    builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
    builder.ssl().enable().serverCAFile(argv[1]);
    if (argc > 2) {
        std::cout << "Using supplied client certificate" << std::endl;
        builder.ssl().clientCertificateFile(argv[2]);
    }
    RemoteCacheManager cacheManager(builder.build(), false);
    BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
    BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
    RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>(km,
        &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy);
    cacheManager.start();
    cache.clear();
    std::string k1("key13");
    std::string v1("boron");

    cache.put(k1, v1);
    std::unique_ptr<std::string> rv(cache.get(k1));
    if (rv->compare(v1)) {
        std::cerr << "get/put fail for " << k1 << " got " << *rv << " expected " << v1 << std::endl;
        return 1;
    }
    cacheManager.stop();

    return 0;
}

