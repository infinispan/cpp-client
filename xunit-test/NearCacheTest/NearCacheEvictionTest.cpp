#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "NearCacheEvictionTest.h"
#include <functional>
#include <chrono>
#include <thread>
using ::infinispan::hotrod::ConfigurationBuilder;
using ::infinispan::hotrod::Configuration;

std::unique_ptr<infinispan::hotrod::RemoteCacheManager> NearCacheEvictionTest::remoteCacheManager;

template<typename Lambda>
void waitFor(Lambda &lambda, long waitTime = 1000, int pollInterval = 100){
    {
    using namespace std::chrono;
    milliseconds end = duration_cast< milliseconds >(system_clock::now().time_since_epoch())+ milliseconds(waitTime);
        while(duration_cast< milliseconds >(system_clock::now().time_since_epoch())<end)
        {
            if (lambda()) return;
            std::cout << ".";
            std::this_thread::sleep_for(milliseconds(pollInterval));
        }
        ASSERT_TRUE(lambda()) << "Error timeout";
    }
}

NearCacheEvictionTest::NearCacheEvictionTest()
{
}

void NearCacheEvictionTest::SetUp() {
    if (NearCacheEvictionTest::remoteCacheManager == nullptr) {
        ConfigurationBuilder builder;
        builder.addServer().host("127.0.0.1").port(11222);
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
        builder.balancingStrategyProducer(nullptr);
        builder.nearCache().mode(NearCacheMode::INVALIDATED).maxEntries(10);
        NearCacheEvictionTest::remoteCacheManager.reset(new RemoteCacheManager(builder.build(), false));
    }
    remoteCacheManager->start();
}
;

void NearCacheEvictionTest::TearDown() {
    NearCacheEvictionTest::remoteCacheManager->stop();
}

TEST_F(NearCacheEvictionTest, EvictionOnFullNearCacheTest) {
    BasicMarshaller<std::string> *km1 = new BasicMarshaller<std::string>();
    BasicMarshaller<std::string> *vm1 = new BasicMarshaller<std::string>();
    RemoteCache<std::string, std::string> cache = NearCacheEvictionTest::remoteCacheManager->getCache<std::string,
            std::string>(km1,
            &Marshaller<std::string>::destroy,
            vm1,
            &Marshaller<std::string>::destroy, true);

    cache.clear();

    auto stats0 = cache.stats();
    cache.get("key1");
    cache.put("key1", "value1");
    cache.get("key1");
    // key1 is near now
    cache.get("key1");
    auto stats1 = cache.stats();
    // Retrieve stats form the server and do some checks
    // counters don't consider hit and miss on the near cache
    ASSERT_EQ(std::stoi(stats0["hits"]) + 1, std::stoi(stats1["hits"]));
    ASSERT_EQ(std::stoi(stats0["misses"]) + 1, std::stoi(stats1["misses"]));
    // now fill the near cache
    for (int i = 0; i < 10; i++)
    {
        cache.put(std::string("key") + std::to_string(i + 2), std::string("value") + std::to_string(i + 2));
        //call Get to populate the near cache
        cache.get(std::string("key") + std::to_string(i + 2));
    }
    // key1 is now far
    auto stats2 = cache.stats();
    auto oneMoreHit = [&cache, &stats2]()->bool{
        cache.get("key1");
        auto tmpStat = cache.stats();
        return (std::stoi(stats2["hits"]) + 1 == std::stoi(tmpStat["hits"]));
    };
    waitFor(oneMoreHit);
    // key1 push key2 out from near cache
    // key2 is now far
    auto threeMoreHit = [&cache, &stats2]()->bool{
        cache.get("key2");
        cache.get("key3");
        auto tmpStat = cache.stats();
        return (std::stoi(stats2["hits"]) + 3 == std::stoi(tmpStat["hits"]));
    };
    waitFor(threeMoreHit);
    auto stats3 = cache.stats();
    ASSERT_EQ(stats2["misses"], stats3["misses"]);
    cache.clear();
}
