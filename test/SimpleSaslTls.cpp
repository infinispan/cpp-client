#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/Version.h"

#include "infinispan/hotrod/JBasicMarshaller.h"
#if !defined _WIN32 && !defined _WIN64
#include <sasl/saslplug.h>
#endif

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

class MyRoundRobinBalancingStrategy: public FailOverRequestBalancingStrategy {
public:
    MyRoundRobinBalancingStrategy() :
            index(0) {
    }

    static FailOverRequestBalancingStrategy *newInstance() {
        return new MyRoundRobinBalancingStrategy();
    }

    void setServers(const std::vector<transport::InetSocketAddress> &s) {
        servers = s;
        // keep the old index if possible so that we don't produce more requests for the first server
        if (index >= servers.size()) {
            index = 0;
        }
    }

    ~MyRoundRobinBalancingStrategy() {
    }

    const transport::InetSocketAddress &getServerByIndex(size_t pos) {
        const transport::InetSocketAddress &server = servers[pos];
        return server;
    }
private:
    std::vector<transport::InetSocketAddress> servers;
    size_t index;
    const transport::InetSocketAddress &nextServer(const std::set<transport::InetSocketAddress>& failedServers) {
        for (unsigned int i = 0; i <= servers.size(); i++) {
            const transport::InetSocketAddress &server = getServerByIndex(index++);
            if (failedServers.empty() || failedServers.find(server) != failedServers.end()
                    || i > failedServers.size()) {
                if (index >= servers.size()) {
                    index = 0;
                }
            }
            return server;
        }

        throw Exception("Bad news, no server available.");
    }
};

}
}
}

template<class T>
void assert_not_null(const std::string& message, int line, const std::unique_ptr<T>& pointer) {
    if (pointer.get() == 0) {
        std::cerr << message << ":" << line << std::endl;
        std::cerr.flush();
        throw std::exception();
    }
}

static std::vector<sasl_callback_t> callbackHandler { 
	{ SASL_CB_LIST_END, NULL, NULL } };

int main(int argc, char** argv) {
    std::cout << "TLS Test" << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " ca_path [client_ca_file]" << std::endl;
        return 1;
    }
    try
    {
      ConfigurationBuilder builder;
      builder.addServer().host("127.0.0.1").port(11222);
      builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
      builder.ssl().enable().serverCAFile(argv[1]);
      if (argc > 2) {
          std::cout << "Using supplied client certificate" << std::endl;
          builder.ssl().clientCertificateFile(argv[2]);
      }
      builder.security().authentication().saslMechanism("EXTERNAL").serverFQDN("node0").callbackHandler(callbackHandler).enable();
      RemoteCacheManager cacheManager(builder.build(), false);
      BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
      BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
      RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>(km,
          &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy, std::string("authCache"));
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
    catch (Exception &ex)
      {
        std::cout << "Error: " << ex.what() << std::endl;
      }
}

