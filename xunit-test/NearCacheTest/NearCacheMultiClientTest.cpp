#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "NearCacheMultiClientTest.h"
#include <functional>
#include <chrono>
#include <thread>
using ::infinispan::hotrod::ConfigurationBuilder;
using ::infinispan::hotrod::Configuration;

std::unique_ptr<infinispan::hotrod::RemoteCacheManager> NearCacheTest::remoteCacheManager;

void waitFor(const std::function<bool()> &func, long waitTime = 1000, int pollInterval = 100);
NearCacheTest::NearCacheTest()
{
}

void NearCacheTest::SetUp() {
    if (NearCacheTest::remoteCacheManager == nullptr){
        ConfigurationBuilder builder;
        builder.addServer().host("127.0.0.1").port(11222);
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
        builder.balancingStrategyProducer(nullptr);
        builder.nearCache().mode(NearCacheMode::INVALIDATED).maxEntries(10);
        NearCacheTest::remoteCacheManager.reset(new RemoteCacheManager(builder.build(), false));
    }
};

void NearCacheTest::TearDown() {
    NearCacheTest::remoteCacheManager->stop();
}

TEST_F(NearCacheTest, ClientsInvalidatedTest) {
    NearCacheTest::remoteCacheManager->start();
    BasicMarshaller<std::string> *km1 = new BasicMarshaller<std::string>();
    BasicMarshaller<std::string> *vm1 = new BasicMarshaller<std::string>();
    RemoteCache<std::string, std::string> cache1 = NearCacheTest::remoteCacheManager->getCache<std::string, std::string>(km1,
            &Marshaller<std::string>::destroy,
            vm1,
            &Marshaller<std::string>::destroy, true);

    BasicMarshaller<std::string> *km2 = new BasicMarshaller<std::string>();
    BasicMarshaller<std::string> *vm2 = new BasicMarshaller<std::string>();
    RemoteCache<std::string, std::string> cache2 = NearCacheTest::remoteCacheManager->getCache<std::string, std::string>(km2,
            &Marshaller<std::string>::destroy,
            vm2,
            &Marshaller<std::string>::destroy, true);

    cache1.clear();
    cache2.clear();
    std::string k("k");
    ASSERT_EQ(nullptr, cache1.get(k));
    ASSERT_EQ(nullptr, cache2.get(k));
    cache1.put(k, "v1");
    auto stats1 = cache2.stats();
    //Get needs to go remotely because this is first time cache2 client reads the value
    ASSERT_EQ("v1", *cache2.get(k));
    //Get is only local now
    ASSERT_EQ("v1", *cache2.get(k));
    auto stats2 = cache2.stats();
    ASSERT_EQ(std::stoi(stats1["hits"]) + 1, std::stoi(stats2["hits"])) << "Client 2 did not reach the server!";
    cache1.put(k, "v2");
    //Get needs to go remotely
    std::function<bool()> f = [&cache2, &k](){std::string* res=cache2.get(k); return (res!=nullptr) && *res== std::string("v2");};
    waitFor(f);

    auto stats3 = cache2.stats();
    ASSERT_EQ(std::stoi(stats2["hits"]) + 1, std::stoi(stats3["hits"])) << "Client 2 did not reach the server for new value!";
    auto cache1Stats1 = cache1.stats();
    cache2.put(k, "v3");

    //Get needs to go remotely
    std::function<bool()> f1 = [&cache1, &k](){ std::string* res=cache1.get(k); return (res!=nullptr) && *res== std::string("v3"); };
    waitFor(f1);

    auto cache1Stats2 = cache1.stats();
    ASSERT_EQ(std::stoi(cache1Stats1["hits"]) + 1, std::stoi(cache1Stats2["hits"])) << "Client 1 did not reach the server for new value!";

}

