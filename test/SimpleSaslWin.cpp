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
#include <cstdlib>

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

int kinit();
void kdestroy();

static char *simple_data; // plain
static char realm_data[] = "ApplicationRealm";
static char path_data[] = "/usr/lib64/sasl2";

static int simple(void *context, int id, const char **result, unsigned *len) {
    *result = simple_data;
    if (len)
        *len = strlen(simple_data);
    return SASL_OK;
}

static int getrealm(void *context, int id, const char **result, unsigned *len) {
    *result = realm_data;
    if (len)
        *len = strlen(realm_data);
    return SASL_OK;
}

#define PLUGINDIR "/usr/lib64/sasl2"

static int getpath(void *context, const char ** path) {
    const char *searchpath = (const char *)context;

    if (!path)
        return SASL_BADPARAM;
    *path = PLUGINDIR;
    return SASL_OK;
}

static char *secret_data;
static int getsecret(void *context, int id, const char **psecret, unsigned *retLen) {
    *psecret = secret_data;
    if (retLen)
        *retLen = strlen(secret_data);
    return SASL_OK;
}

static std::vector<sasl_callback_t> callbackHandler{ { HOTROD_SASL_CB_USER, (sasl_callback_ft)&simple, NULL },{
    HOTROD_SASL_CB_AUTHNAME, (sasl_callback_ft)&simple, NULL },{ HOTROD_SASL_CB_PASS, (sasl_callback_ft)&getsecret, NULL },{
        HOTROD_SASL_CB_GETREALM, (sasl_callback_ft)&getrealm, NULL },{ HOTROD_SASL_CB_GETPATH, (sasl_callback_ft)&getpath, NULL },{
            HOTROD_SASL_CB_LIST_END, NULL, NULL } };

int main(int argc, char** argv) {

    int result = 0;
    {
        ConfigurationBuilder builder;
        simple_data = "writer";
        secret_data = "somePassword";
        builder.addServer().host(argc > 1 ? argv[1] : "127.0.0.1").port(argc > 2 ? atoi(argv[2]) : 11222);
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
        builder.getSecurityConfigurationBuilder().getAuthConfigurationBuilder().saslMechanism("PLAIN").serverFQDN(
                "node0").callbackHandler(callbackHandler).enable();
        builder.balancingStrategyProducer(nullptr);
        RemoteCacheManager cacheManager(builder.build(), false);
        BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
        BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
        RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>(km,
                &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy, std::string("authCache"));
        cacheManager.start();
        cache.put("key", "value");
        try {
            std::shared_ptr<std::string> ret(cache.get("key"));
            std::cerr << "FAIL: 'writer' should not read" << std::endl;
            return -1;
        } catch (Exception& ex) {

        }
        std::cout << "PASS: 'PLAIN' sasl authorization" << std::endl;
        cacheManager.stop();
    }

    {
        ConfigurationBuilder builder;
        simple_data = "reader";
        secret_data = "password";
        builder.addServer().host(argc > 1 ? argv[1] : "127.0.0.1").port(argc > 2 ? atoi(argv[2]) : 11222);
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
        builder.getSecurityConfigurationBuilder().getAuthConfigurationBuilder().saslMechanism("DIGEST-MD5").serverFQDN(
                "node0").callbackHandler(callbackHandler).enable();
        builder.balancingStrategyProducer(nullptr);
        RemoteCacheManager cacheManager(builder.build(), false);
        BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
        BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
        RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>(km,
                &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy, std::string("authCache"));
        cacheManager.start();
        std::shared_ptr<std::string> ret(cache.get("key"));
        try {
            cache.put("key", "value");
            std::cerr << "FAIL: 'reader' should not write" << std::endl;
            return -1;
        } catch (Exception& ex) {

        }
        std::cout << "PASS: 'DIGEST-MD5' sasl authorization" << std::endl;
        cacheManager.stop();
    }
/*
    {
        kinit();
        ConfigurationBuilder builder;
        simple_data = "supervisor@INFINISPAN.ORG";
        secret_data = "lessStrongPassword";
        builder.addServer().host(argc > 1 ? argv[1] : "127.0.0.1").port(argc > 2 ? atoi(argv[2]) : 11222);
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
        builder.getSecurityConfigurationBuilder().getAuthConfigurationBuilder().saslMechanism("GSSAPI").serverFQDN(
                "node0").callbackHandler(callbackHandler).enable();
        builder.balancingStrategyProducer(nullptr);
        RemoteCacheManager cacheManager(builder.build(), false);
        BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
        BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
        RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>(km,
                &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy, std::string("authCache"));
        cacheManager.start();
        try {
            cache.put("key", "value");
            std::shared_ptr<std::string> ret(cache.get("key"));
            result = 0;
        } catch (Exception& ex) {
            std::cerr << "FAIL: 'supervisor' should read and write" << std::endl;
            result = -1;
        }
        cacheManager.stop();
        std::cout << "PASS: 'GSSAPI' sasl authorization" << std::endl;
        kdestroy();
    }
*/
    return result;
}

int kinit() {
    // Delegate Kerberos setup to the system
    //setenv("KRB5CCNAME", "krb5cc_hotrod", 1);
    //setenv("KRB5_CONFIG", "krb5.conf", 1);
    std::system("echo lessStrongPassword | kinit -c krb5cc_hotrod supervisor@INFINISPAN.ORG");
    return 1;
}

void kdestroy() {
    std::system("kdestroy");
}

