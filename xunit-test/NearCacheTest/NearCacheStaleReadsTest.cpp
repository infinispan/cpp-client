#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "NearCacheStaleReadsTest.h"
#include <functional>
#include <chrono>
#include <thread>
using ::infinispan::hotrod::ConfigurationBuilder;
using ::infinispan::hotrod::Configuration;

std::unique_ptr<infinispan::hotrod::RemoteCacheManager> NearCacheStaleReadsTest::remoteCacheManager;

void waitFor(const std::function<bool()> &func, long waitTime = 1000, int pollInterval = 100)
{
using namespace std::chrono;
milliseconds end = duration_cast< milliseconds >(system_clock::now().time_since_epoch())+ milliseconds(waitTime);
    while(duration_cast< milliseconds >(system_clock::now().time_since_epoch())<end)
    {
        if (func()) return;
        std::this_thread::sleep_for(milliseconds(pollInterval));
    }
    ASSERT_TRUE(func()) << "Error timeout";
}

void Repeat(std::function<void(int, RemoteCache<std::string, std::string>&)> f)
{
    BasicMarshaller<std::string> *km1 = new BasicMarshaller<std::string>();
    BasicMarshaller<std::string> *vm1 = new BasicMarshaller<std::string>();
    RemoteCache<std::string, std::string> cache = NearCacheStaleReadsTest::remoteCacheManager->getCache<std::string, std::string>(km1,
            &Marshaller<std::string>::destroy,
            vm1,
            &Marshaller<std::string>::destroy, "default", false);

    cache.clear();
    cache.putIfAbsent("k", "v0");
    for (int i = 1; i != 1000; i++)
    {
        f(i, cache);
    }
}

NearCacheStaleReadsTest::NearCacheStaleReadsTest()
{
}

void NearCacheStaleReadsTest::SetUp() {
    if (NearCacheStaleReadsTest::remoteCacheManager == nullptr){
        ConfigurationBuilder builder;
        builder.addServer().host("127.0.0.1").port(11222);
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
        builder.balancingStrategyProducer(nullptr);
        builder.nearCache().mode(NearCacheMode::INVALIDATED).maxEntries(0);
        NearCacheStaleReadsTest::remoteCacheManager.reset(new RemoteCacheManager(builder.build(), false));
    }
    remoteCacheManager->start();
};

void NearCacheStaleReadsTest::TearDown() {
    remoteCacheManager->stop();

}

TEST_F(NearCacheStaleReadsTest, AvoidStaleReadAfterPutRemoveTest) {
    std::function<void(int, RemoteCache<std::string, std::string>&)> f = [](int i, RemoteCache<std::string, std::string> &cache) {
    std::string value = std::string("v") + std::to_string(i);
    delete cache.put("k", value);
    std::unique_ptr<std::string> v(cache.get("k"));
    ASSERT_EQ(value, *v) << "Return value different from: " << value;
    delete cache.remove("k");
    v=std::unique_ptr<std::string>(cache.get("k"));
    ASSERT_EQ(nullptr, v) << "Return value is not null";
};
    Repeat(f);
}

TEST_F(NearCacheStaleReadsTest, AvoidStaleReadAfterPutAllTest) {
    std::function<void(int, RemoteCache<std::string, std::string>&)> f = [](int i, RemoteCache<std::string, std::string> &cache) {
        std::map<std::string, std::string> map;
        std::string value = std::string("v") + std::to_string(i);
        map["k"]=value;
        cache.putAll(map);
        std::unique_ptr<std::string> v(cache.get("k"));
        ASSERT_EQ(value, *v);
};
    Repeat(f);
}

TEST_F(NearCacheStaleReadsTest, AvoidStaleReadAfterReplaceTest) {
    std::function<void(int, RemoteCache<std::string, std::string>&)> f = [](int i, RemoteCache<std::string, std::string> &cache) {
        std::string value = std::string("v") + std::to_string(i);
        delete cache.replace("k", value);
        auto versioned = cache.getWithVersion("k");
        ASSERT_EQ(value, *versioned.first);
};
    Repeat(f);
}

TEST_F(NearCacheStaleReadsTest, AvoidStaleReadAfterReplaceWithVersionTest) {
    std::function<void(int, RemoteCache<std::string, std::string>&)> f = [](int i, RemoteCache<std::string, std::string> &cache) {
        std::string value = std::string("v") + std::to_string(i);
        auto versioned = cache.getWithVersion("k");
        ASSERT_TRUE(cache.replaceWithVersion("k", value, (uint64_t) versioned.second.version)) << "Not replaced!";
        std::unique_ptr<std::string> v(cache.get("k"));
        ASSERT_EQ(value, *v);
    };
    Repeat(f);
}

TEST_F(NearCacheStaleReadsTest, AvoidStaleReadAfterPutAsyncRemoveWithVersionTest) {
    std::function<void(int, RemoteCache<std::string, std::string>&)> f = [](int i, RemoteCache<std::string, std::string> &cache) {
        std::string value = std::string("v") + std::to_string(i);
        cache.putAsync("k", value).wait();
        auto versioned = cache.getWithVersion("k");
        ASSERT_EQ(value, *versioned.first);
        ASSERT_TRUE(cache.removeWithVersion("k", (uint64_t) versioned.second.version)) << "Not removed!";
        std::unique_ptr<std::string> v(cache.get("k"));
        ASSERT_EQ(nullptr, v);
    };
    Repeat(f);
}
