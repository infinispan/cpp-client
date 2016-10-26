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
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " ca_path [client_ca_file]" << std::endl;
        return 1;
    }
    std::cout << "TLS Test" << std::endl;
    {
      ConfigurationBuilder builder;
      builder.addServer().host("127.0.0.1").port(11222);
      builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
      builder.ssl().serverCAFile(argv[1]).sniHostName("sni");
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
      std::string v1("boron");

      cache.put(k1, v1);
      std::unique_ptr<std::string> rv(cache.get(k1));
      if (rv->compare(v1)) {
          std::cerr << "get/put fail for " << k1 << " got " << *rv << " expected " << v1 << std::endl;
          return 1;
      }
      std::cout << "PASS: connected with correct SNI" << std::endl;
      cacheManager.stop();
      }
      catch (Exception e)
      {
          std::cout << "FAIL: not connected with correct SNI" << std::endl;
          return 1;
      }
    }
    std::cout << "TLS Test (wrong SNI part)" << std::endl;
    {
      ConfigurationBuilder builder;
      builder.addServer().host("127.0.0.1").port(11222);
      builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
      builder.ssl().serverCAFile(argv[1]).sniHostName("sni2");
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
               std::string v1("boron");

               cache.put(k1, v1);
               std::unique_ptr<std::string> rv(cache.get(k1));
               if (rv->compare(v1)) {
                  std::cerr << "get/put fail for " << k1 << " got " << *rv << " expected " << v1 << std::endl;
                  return 1;
               }
               std::cout << "FAIL: connected with wrong SNI" << std::endl;
               cacheManager.stop();
               return 1;
      }
      catch (Exception e)
      {
          std::cout << "PASS: not connected with wrong SNI" << std::endl;
      }
      return 0;
    }
}

