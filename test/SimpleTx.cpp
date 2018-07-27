#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"

#include "infinispan/hotrod/JBasicMarshaller.h"
#include <iostream>
#include <memory>

// For CTest: return 0 if all tests pass, non-zero otherwise.

using namespace infinispan::hotrod;

template<class T>
void assert_not_null(int line, const std::unique_ptr<T>& pointer) {
    if (pointer.get() == 0) {
        std::cerr << "pointer is null:" << line << std::endl;
        std::cerr.flush();
        throw std::exception();
    }
}

template<class T>
void assert_null(int line, const std::unique_ptr<T>& pointer) {
    if (pointer.get() != 0) {
        std::cerr << "pointer not null:" << line << std::endl;
        std::cerr.flush();
        throw std::exception();
    }
}

template<class T>
void assert_eq(int line, const T& t1, const T& t2) {
    if (t1 != t2) {
        std::cerr << t1 << "==" << t2 << " : check failed" << std::endl;
        std::cerr.flush();
        throw std::exception();
    }
}

int main(int argc, char** argv) {
    int result = 0;
    // Create configuration, cache manager and cache
    ConfigurationBuilder builder;
    builder.balancingStrategyProducer(nullptr);
    builder.addServer().host(argc > 1 ? argv[1] : "127.0.0.1").port(argc > 2 ? atoi(argv[2]) : 11222);
    builder.protocolVersion(Configuration::PROTOCOL_VERSION_27);
    builder.setTransactional(true);
    RemoteCacheManager cacheManager(builder.build(), false);
    JBasicMarshaller<std::string> *km = new JBasicMarshaller<std::string>();
    JBasicMarshaller<std::string> *vm = new JBasicMarshaller<std::string>();
    RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>(km,
            &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy, "non_xa", true);
    cacheManager.start();

    // Example
    try {
        std::cout << "HotRod C++ Library version " << cache.getVersion() << std::endl;
        std::cout << "Protocol " << cache.getProtocolVersion() << std::endl;

        std::string k1("key13");
        std::string k2("key14");
        std::string v1("boron");
        std::string v2("chlorine");

        cache.clear();

        cache.put(k1, v1);
        std::unique_ptr<std::string> rv(cache.get(k1));
        assert_not_null(__LINE__, rv);
        assert_eq(__LINE__, *rv, v1);
        auto & txManager = cacheManager.getTransactionManager();

        // Example of transaction with rollback
        txManager.begin();
        cache.put(k2, v2);
        std::unique_ptr<std::string> rv2(cache.get(k2));
        assert_not_null(__LINE__, rv2);
        assert_eq(__LINE__, *rv2, v2);
        std::cout << "k2 is " << (rv2 ? *rv2 : "null") << std::endl;
        txManager.rollback();
        rv2.reset(cache.get(k2));
        assert_null(__LINE__, rv2);
        std::cout << "k2 is " << (rv2 ? *rv2 : "null") << std::endl;

        // Example of transaction with commit
        txManager.begin();
        cache.put(k2, v2);
        rv2.reset(cache.get(k2));
        assert_not_null(__LINE__, rv2);
        assert_eq(__LINE__, *rv2, v2);
        std::cout << "k2 is " << (rv2 ? *rv2 : "null") << std::endl;
        txManager.commit();
        rv2.reset(cache.get(k2));
        assert_not_null(__LINE__, rv2);
        std::cout << "k2 is " << (rv2 ? *rv2 : "null") << std::endl;

        return 0;
    } catch (const Exception& e) {
        cacheManager.stop();
        std::cout << "is: " << typeid(e).name() << '\n';
        std::cerr << "fail unexpected exception: " << e.what() << std::endl;
        return 1;
    }

    cacheManager.stop();
    return result;
}

