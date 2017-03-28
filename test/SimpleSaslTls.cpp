#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/Version.h"

#include "infinispan/hotrod/JBasicMarshaller.h"
#include <sasl/saslplug.h>

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

static char *simple_data; // plain
static char realm_data[] = "applicationRealm";

static int simple(void *context __attribute__((unused)), int id, const char **result, unsigned *len) {
    *result = simple_data;
    if (len)
        *len = strlen(simple_data);
    return SASL_OK;
}

static int getrealm(void *context __attribute__((unused)), int id, const char **result, unsigned *len) {
    *result = realm_data;
    if (len)
        *len = strlen(simple_data);
    return SASL_OK;
}

#define PLUGINDIR "/usr/lib64/sasl2"

static int getpath(void *context, const char ** path) {
    const char *searchpath = (const char *) context;

    if (!path)
        return SASL_BADPARAM;
    *path = PLUGINDIR;
    return SASL_OK;
}

static char *secret_data;
static int getsecret(sasl_conn_t *conn, void *context __attribute__((unused)), int id, sasl_secret_t **psecret) {
    size_t len;
    static sasl_secret_t *x;

    /* paranoia check */
    if (!conn || !psecret || id != SASL_CB_PASS)
        return SASL_BADPARAM;

    len = strlen(secret_data);

    x = (sasl_secret_t *) realloc(x, sizeof(sasl_secret_t) + len);

    x->len = len;
    strcpy((char *) x->data, secret_data);

    *psecret = x;
    return SASL_OK;
}

static std::vector<sasl_callback_t> callbackHandler { { SASL_CB_USER, (sasl_callback_ft) &simple, NULL }, {
SASL_CB_AUTHNAME, (sasl_callback_ft) &simple, NULL }, { SASL_CB_PASS, (sasl_callback_ft) &getsecret, NULL }, {
SASL_CB_GETREALM, (sasl_callback_ft) &getrealm, NULL }, /*{ SASL_CB_GETPATH, (sasl_callback_ft) &getpath, NULL },*/ {
SASL_CB_LIST_END, NULL, NULL } };

int main(int argc, char** argv) {
    std::cout << "TLS Test" << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " ca_path [client_ca_file]" << std::endl;
        return 1;
    }
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
    }
    return 0;
}

