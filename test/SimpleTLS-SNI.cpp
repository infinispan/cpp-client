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

int test(const std::string &test_desc, int argc, char** argv, const std::string &sni_name) {
    std::cout << test_desc << std::endl;
    {
        ConfigurationBuilder builder;
        builder.addServer().host("127.0.0.1").port(11222);
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
        builder.ssl().serverCAFile(argv[1]).sniHostName(sni_name);
        if (argc > 2) {
            std::cout << "Using supplied client certificate" << std::endl;
            builder.ssl().clientCertificateFile(argv[2]);
        }
        builder.ssl().enable();
        RemoteCacheManager cacheManager(builder.build(), false);
        BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
        BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
        RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>(km,
                &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy);
        try {
            cacheManager.start();
            cache.clear();
            std::string k1("key13");
            std::string v1(sni_name);

            cache.put(k1, v1);
            std::unique_ptr < std::string > rv(cache.get(k1));
            if (rv->compare(v1)) {
                std::cerr << "get/put fail for " << k1 << " got " << *rv << " expected " << v1 << std::endl;
                cacheManager.stop();
                return 1;
            }
            cacheManager.stop();
        } catch (Exception e) {
            cacheManager.stop();
            return 1;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " ca_path [client_ca_file]" << std::endl;
        return 1;
    }

    if (test("TLS-SNI Test (First correct name and cert)", argc, argv, "sni") == 0) {
        std::cout << "PASS: connected with correct SNI" << std::endl;
    } else {
        std::cout << "FAIL: not connected with correct SNI" << std::endl;
        return 1;
    }

    if (test("TLS Test (Correct name and wrong cert)", argc, argv, "sni2") == 0) {
        std::cout << "FAIL: connected with wrong SNI" << std::endl;
        return 1;
    } else {
        std::cout << "PASS: not connected with wrong SNI" << std::endl;
    }

    if (test("TLS-SNI Test (Second correct name and cert)", argc, argv, "sni3") == 0) {
        std::cout << "PASS: connected with correct SNI" << std::endl;
    } else {
        std::cout << "FAIL: not connected with correct SNI" << std::endl;
        return 1;
    }

    if (test("TLS-SNI Test (Invalid name)", argc, argv, "sni4") == 0) {
        std::cout << "FAIL: connected with wrong SNI" << std::endl;
        return 1;
    } else {
        std::cout << "PASS: not connected with wrong SNI" << std::endl;
    }

    return 0;
}

