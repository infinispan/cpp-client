#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/Version.h"

#include "infinispan/hotrod/JBasicMarshaller.h"
#include "infinispan/hotrod/CacheClientListener.h"
#include "infinispan/hotrod/ClientEvent.h"

#include <chrono>
#include <thread>
#include <string>
#include <cstdlib>
using namespace infinispan::hotrod;
using namespace infinispan::hotrod::event;

// This balancing strategy always returns server 11322 if up
// then fallback on 11222
class StickyBalancingStrategy: public FailOverRequestBalancingStrategy {
public:
    StickyBalancingStrategy() :
            index(0) {
    }
    static FailOverRequestBalancingStrategy *newInstance() {
        return new StickyBalancingStrategy();
    }

    void setServers(const std::vector<transport::InetSocketAddress> &s) {
        servers = s;
        // keep the old index if possible so that we don't produce more requests for the first server
        if (index >= servers.size()) {
            index = 0;
        }
    }

    ~StickyBalancingStrategy() {
    }

    const transport::InetSocketAddress &getServerByIndex(size_t pos) {
        const transport::InetSocketAddress &server = servers[pos];
        return server;
    }
private:
    std::vector<transport::InetSocketAddress> servers;
    size_t index;
    const transport::InetSocketAddress &nextServer(
            const std::set<transport::InetSocketAddress>& failedServers) {
        if (servers.size() == 1)
            return servers[0];
        if (failedServers.size() == 0)
                {
            return (servers[0].getPort() == 11322) ? servers[0] : servers[1];
        }
        else
        {
            return (servers[0].getPort() == 11322) ? servers[1] : servers[0];
        }
    }
};

// Some utility functions
RemoteCacheManager* getNewRemoteCacheManager()
{
    ConfigurationBuilder nearCacheBuilder;
    nearCacheBuilder.addServer().host("127.0.0.1").port(11222);
    nearCacheBuilder.addServer().host("127.0.0.1").port(11322);

    nearCacheBuilder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
    nearCacheBuilder.balancingStrategyProducer(StickyBalancingStrategy::newInstance);
    nearCacheBuilder.nearCache().mode(NearCacheMode::INVALIDATED).maxEntries(10);
    auto *manager = new RemoteCacheManager(nearCacheBuilder.build(), false);
    return manager;
}

RemoteCache<std::string, std::string> &getNewCache(RemoteCacheManager& m) {
    JBasicMarshaller<std::string> *km = new JBasicMarshaller<std::string>();
    JBasicMarshaller<std::string> *vm = new JBasicMarshaller<std::string>();
    m.start();
    return m.getCache<std::string, std::string>(km, &Marshaller<std::string>::destroy,
            vm, &Marshaller<std::string>::destroy);
}

bool areWrong(int value, int expect, std::string* rest, int line) {
    if (value != expect) {
        std::cerr << "Line " << line << " Awaiting " << expect << " got " << value << std::endl;
        if (rest)
            delete rest;
        return true;
    }
    if (rest->compare("value1")) {
        std::cerr << "Line " << line << " Awaiting value1 got " << (rest != nullptr ? rest->c_str() : "null") << std::endl;
        if (rest)
            delete rest;
        return true;
    }
    return false;
}

void releaseRemoteCacheManager(RemoteCacheManager * rcm)
{
    rcm->stop();
    delete rcm;
}

int main(int argc, char** argv) {
    std::string python_exec(argv[1]);
    std::string server_ctl(argv[2]);
    std::string probe_port(argv[3]);
    std::string server_pid_filename(argv[4]);

    std::unique_ptr<RemoteCacheManager, decltype(&releaseRemoteCacheManager)> nearCacheManager(getNewRemoteCacheManager(), &releaseRemoteCacheManager);
    std::unique_ptr<RemoteCacheManager, decltype(&releaseRemoteCacheManager)> nearCacheManager2(getNewRemoteCacheManager(), &releaseRemoteCacheManager);

    try
    {
        RemoteCache<std::string, std::string> nearCache = getNewCache(*nearCacheManager);
        RemoteCache<std::string, std::string> nearCache2 = getNewCache(*nearCacheManager2);

        // Cleanup the cache
        nearCache.clear();

        int pre_hits, post_hits;
        std::string *rest;
        // nearHits is added by the near cache framework
        pre_hits = std::stoi(nearCache.stats()["nearHits"]);
        nearCache.put("key1", "value1");
        // After put, this get goes remote
        rest = nearCache.get("key1");
        post_hits = std::stoi(nearCache.stats()["nearHits"]);
        if (areWrong(post_hits, pre_hits, rest, __LINE__)) {
            return -1;
        }
        delete rest;

        // This call goes remote
        pre_hits = post_hits;
        nearCache2.put("key1", "value1");
        // After put via different cache manager, this get goes remote
        rest = nearCache.get("key1");
        post_hits = std::stoi(nearCache.stats()["nearHits"]);
        if (areWrong(post_hits, pre_hits, rest, __LINE__)) {
            return -1;
        }
        delete rest;

        pre_hits = post_hits;
        // this get goes near
        rest = nearCache.get("key1");
        post_hits = std::stoi(nearCache.stats()["nearHits"]);
        if (areWrong(post_hits, pre_hits+1, rest, __LINE__)) {
            return -1;
        }
        delete rest;

        std::string stop_server = python_exec + " " + server_ctl + " stop " + server_pid_filename;
        std::cout << "Stopping with command: " << stop_server << std::endl;
        std::system(stop_server.c_str());
        std::string server_down = python_exec + " " + probe_port + " localhost 11322 60 down";
        std::cout << "Probing with command: " << server_down << std::endl;
        std::system(server_down.c_str());

        pre_hits = std::stoi(nearCache.stats()["nearHits"]);
        // After failover, this get goes remote
        rest = nearCache.get("key1");
        post_hits = std::stoi(nearCache.stats()["nearHits"]);
        if (areWrong(post_hits, pre_hits, rest, __LINE__)) {
            return -1;
        }

        pre_hits = post_hits;
        // this get goes near
        rest = nearCache.get("key1");
        post_hits = std::stoi(nearCache.stats()["nearHits"]);
        if (areWrong(post_hits, pre_hits+1, rest, __LINE__)) {
            return -1;
        }
    }
    catch (Exception &e) {
    }
}
