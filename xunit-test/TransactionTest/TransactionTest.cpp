#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "TransactionTest.h"

using ::infinispan::hotrod::ConfigurationBuilder;
using ::infinispan::hotrod::Configuration;

std::unique_ptr<infinispan::hotrod::RemoteCacheManager> TransactionTest::remoteCacheManager;
std::unique_ptr<infinispan::hotrod::RemoteCacheManager> TransactionTest::nontxRemoteCacheManager;


TransactionTest::TransactionTest()
{
}

void TransactionTest::SetUp() {
	if (TransactionTest::remoteCacheManager == nullptr){
		ConfigurationBuilder builder;
		builder.addServer().host("127.0.0.1").port(11222);
		builder.protocolVersion(Configuration::PROTOCOL_VERSION_27);
		builder.balancingStrategyProducer(nullptr);
		builder.setTransactional(true);
		TransactionTest::remoteCacheManager.reset(new RemoteCacheManager(builder.build(), false));
	}
    if (TransactionTest::nontxRemoteCacheManager == nullptr){
        ConfigurationBuilder builder;
        builder.addServer().host("127.0.0.1").port(11222);
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_27);
        builder.balancingStrategyProducer(nullptr);
        TransactionTest::nontxRemoteCacheManager.reset(new RemoteCacheManager(builder.build(), false));
    }
};

void TransactionTest::TearDown() {
	TransactionTest::remoteCacheManager->stop();
    TransactionTest::nontxRemoteCacheManager->stop();
}

RemoteCache<std::string, std::string>& utilGetCache(std::unique_ptr<infinispan::hotrod::RemoteCacheManager> & rcm, const std::string & name = "non_xa") {
    BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
    BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
    RemoteCache<std::string, std::string>& cache = rcm->getCache<std::string, std::string>(km,
            &Marshaller<std::string>::destroy,
            vm,
            &Marshaller<std::string>::destroy,
            name, true);
    return cache;
}

// Client must read last value both during the tx (from the context)
// and after the commit (from the cache)
TEST_F(TransactionTest, ReadCommitted) {
    TransactionTest::remoteCacheManager->start();
    RemoteCache<std::string, std::string>& cache = utilGetCache(remoteCacheManager);
    auto & txManager = remoteCacheManager->getTransactionManager();

    std::string k1("key13");
    std::string v1("boron");

    cache.clear();

    txManager.begin();
    cache.put(k1,v1);
    // Check the correct value from the tx context
    std::unique_ptr<std::string> rv1(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
    txManager.commit();
    // Check the correct value from remote cache
    rv1.reset(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
}

// Client must read last value both during the tx (from the context)
// and after the commit (from the cache)
TEST_F(TransactionTest, ChangeExistentAndCommit) {
    TransactionTest::remoteCacheManager->start();
    RemoteCache<std::string, std::string>& cache = utilGetCache(remoteCacheManager);
    auto & txManager = remoteCacheManager->getTransactionManager();

    std::string k1("key13");
    std::string v0("carbon");
    std::string v1("boron");
    cache.clear();
    cache.put(k1,v0);
    txManager.begin();
    cache.put(k1,v1);
    // Check the correct value from the tx context
    std::unique_ptr<std::string> rv1(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
    txManager.commit();
    // Check the correct value from remote cache
    rv1.reset(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
}


// Client must read last value during the tx (from the context)
// and the old value (null) from the cache after the rollback
TEST_F(TransactionTest, ReadRollbackOnNotExistent) {
    TransactionTest::remoteCacheManager->start();
    RemoteCache<std::string, std::string>& cache = utilGetCache(remoteCacheManager);
    auto & txManager = remoteCacheManager->getTransactionManager();

    std::string k1("key13");
    std::string v1("boron");

    cache.clear();

    txManager.begin();
    std::unique_ptr<std::string> oldv1(cache.put(k1,v1));
    // Check the correct value from the tx context
    std::unique_ptr<std::string> rv1(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
    txManager.rollback();
    // Check the correct value from remote cache
    rv1.reset(cache.get(k1));
    EXPECT_EQ(rv1, nullptr);
}

// Client must read last value during the tx (from the context)
// and the old value from the cache after the rollback
TEST_F(TransactionTest, ReadRollbackOnExistent) {
    TransactionTest::remoteCacheManager->start();
    RemoteCache<std::string, std::string>& cache = utilGetCache(remoteCacheManager);
    auto & txManager = remoteCacheManager->getTransactionManager();

    std::string k1("key13");
    std::string v1("boron");
    std::string oldv("oxygen");

    cache.clear();

    std::unique_ptr<std::string> oldrv1(cache.put(k1,oldv));
    EXPECT_EQ(oldrv1, nullptr);

    txManager.begin();
    oldrv1.reset(cache.put(k1,v1));
    EXPECT_NE(oldrv1, nullptr);
    EXPECT_EQ(*oldrv1, oldv);
    // Check the correct value from the tx context
    std::unique_ptr<std::string> rv1(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
    txManager.rollback();
    // Check the correct value from remote cache
    rv1.reset(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*oldrv1, oldv);
}

// Client must read last value during the tx (from the context)
TEST_F(TransactionTest, ReadInTransactionContext) {
    TransactionTest::remoteCacheManager->start();
    RemoteCache<std::string, std::string>& cache = utilGetCache(remoteCacheManager);
    auto & txManager = remoteCacheManager->getTransactionManager();

    std::string k1("key13");
    std::string v1("boron");

    cache.clear();

    txManager.begin();
    std::unique_ptr<std::string> oldv1(cache.put(k1,v1));
    // Check the correct value from the tx context
    std::unique_ptr<std::string> rv1(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
    rv1.reset(cache.remove(k1));
    rv1.reset(cache.get(k1));
    EXPECT_EQ(rv1, nullptr);
    txManager.rollback();
}

// TX Client must read last value during the tx (from the context)
// NONTX client must read old value from the cache
TEST_F(TransactionTest, ReadCommittedAndNonTxRead) {
    TransactionTest::remoteCacheManager->start();
    TransactionTest::nontxRemoteCacheManager->start();
    RemoteCache<std::string, std::string>& cache = utilGetCache(remoteCacheManager);
    RemoteCache<std::string, std::string>& nontxCache = utilGetCache(nontxRemoteCacheManager);

    auto & txManager = remoteCacheManager->getTransactionManager();

    std::string k1("key13");
    std::string v1("boron");

    cache.clear();

    txManager.begin();
    cache.put(k1,v1);
    // Check the correct value from the tx context
    std::unique_ptr<std::string> rv1(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);

    std::unique_ptr<std::string> nontxrv1(nontxCache.get(k1));
    EXPECT_EQ(nontxrv1, nullptr);
    txManager.commit();
    // Check the correct value from remote cache
    rv1.reset(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
    nontxrv1.reset(nontxCache.get(k1));
    EXPECT_NE(nontxrv1, nullptr);
    EXPECT_EQ(*nontxrv1, v1);
}

// NONTX client must put/get values from the cache
TEST_F(TransactionTest, PutAndReadWithNonTxCache) {
    TransactionTest::remoteCacheManager->start();
    TransactionTest::nontxRemoteCacheManager->start();
    RemoteCache<std::string, std::string>& cache = utilGetCache(remoteCacheManager);
    RemoteCache<std::string, std::string>& nontxCache = utilGetCache(nontxRemoteCacheManager);
    auto & txManager = remoteCacheManager->getTransactionManager();

    std::string k1("key13");
    std::string v1("boron");

    cache.clear();

    txManager.begin();
    std::unique_ptr<std::string> oldv1(nontxCache.put(k1,v1));
    // Check the correct value from the tx context
    std::unique_ptr<std::string> rv1(nontxCache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
    rv1.reset(cache.remove(k1));
    rv1.reset(cache.get(k1));
    EXPECT_EQ(rv1, nullptr);
    txManager.rollback();

    rv1.reset(nontxCache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
}

// NONTX client must put/get values from the cache
TEST_F(TransactionTest, RepeatableGetForTxClient) {
    TransactionTest::remoteCacheManager->start();
    TransactionTest::nontxRemoteCacheManager->start();
    RemoteCache<std::string, std::string>& cache = utilGetCache(remoteCacheManager);
    RemoteCache<std::string, std::string>& nontxCache = utilGetCache(nontxRemoteCacheManager);
    auto & txManager = remoteCacheManager->getTransactionManager();

    std::string k1("key13");
    std::string v1("boron");
    std::string v2("helium");

    cache.clear();

    txManager.begin();
    std::unique_ptr<std::string> oldv1(nontxCache.put(k1,v1));
    // Check the correct value from the tx context
    std::unique_ptr<std::string> rv1(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);

    oldv1.reset(nontxCache.put(k1,v2));

    rv1.reset(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);


    rv1.reset(cache.remove(k1));
    rv1.reset(cache.get(k1));
    EXPECT_EQ(rv1, nullptr);
    txManager.rollback();

    rv1.reset(nontxCache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v2);
}

// Client must read last value both during the tx (from the context)
// and after the commit (from the cache)
TEST_F(TransactionTest, ReadCommittedMultiCache) {
    TransactionTest::remoteCacheManager->start();
    RemoteCache<std::string, std::string>& cache = utilGetCache(remoteCacheManager);
    RemoteCache<std::string, std::string>& cache2 = utilGetCache(remoteCacheManager, "non_xa2");
    auto & txManager = remoteCacheManager->getTransactionManager();

    std::string k1("key13");
    std::string v1("boron");

    std::string k1_2("key13_2");
    std::string v1_2("boron_2");

    cache.clear();
    cache2.clear();

    txManager.begin();
    cache.put(k1,v1);
    cache2.put(k1_2,v1_2);
    // Check the correct value from the tx context
    std::unique_ptr<std::string> rv1(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
    std::unique_ptr<std::string> rv1_2(cache2.get(k1_2));
    EXPECT_NE(rv1_2, nullptr);
    EXPECT_EQ(*rv1_2, v1_2);
    txManager.commit();
    // Check the correct value from remote cache
    rv1.reset(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
    rv1_2.reset(cache2.get(k1_2));
    EXPECT_NE(rv1_2, nullptr);
    EXPECT_EQ(*rv1_2, v1_2);
}

// Client must read last value during the tx (from the context)
// and the old value from the cache after the rollback
TEST_F(TransactionTest, ReadRollbackMultiCache) {
    TransactionTest::remoteCacheManager->start();
    RemoteCache<std::string, std::string>& cache = utilGetCache(remoteCacheManager);
    RemoteCache<std::string, std::string>& cache2 = utilGetCache(remoteCacheManager,"non_xa");
    auto & txManager = remoteCacheManager->getTransactionManager();

    std::string k1("key13");
    std::string v1("boron");
    std::string oldv("oxygen");

    std::string k1_2("key13_2");
    std::string v1_2("boron_2");
    std::string oldv_2("oxygen_2");

    cache.clear();

    std::unique_ptr<std::string> oldrv1(cache.put(k1,oldv));
    EXPECT_EQ(oldrv1, nullptr);
    std::unique_ptr<std::string> oldrv1_2(cache2.put(k1_2,oldv_2));
    EXPECT_EQ(oldrv1_2, nullptr);

    txManager.begin();
    oldrv1.reset(cache.put(k1,v1));
    EXPECT_NE(oldrv1, nullptr);
    EXPECT_EQ(*oldrv1, oldv);
    oldrv1_2.reset(cache2.put(k1_2,v1_2));
    EXPECT_NE(oldrv1_2, nullptr);
    EXPECT_EQ(*oldrv1_2, oldv_2);
    // Check the correct value from the tx context
    std::unique_ptr<std::string> rv1(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*rv1, v1);
    std::unique_ptr<std::string> rv1_2(cache.get(k1_2));
    EXPECT_NE(rv1_2, nullptr);
    EXPECT_EQ(*rv1_2, v1_2);
    txManager.rollback();
    // Check the correct value from remote cache
    rv1.reset(cache.get(k1));
    EXPECT_NE(rv1, nullptr);
    EXPECT_EQ(*oldrv1, oldv);
    rv1_2.reset(cache2.get(k1_2));
    EXPECT_NE(rv1_2, nullptr);
    EXPECT_EQ(*oldrv1_2, oldv_2);
}

// NONTX client must put/get values from the cache
TEST_F(TransactionTest, ConflictsAndFail) {
    TransactionTest::remoteCacheManager->start();
    TransactionTest::nontxRemoteCacheManager->start();
    RemoteCache<std::string, std::string>& cache = utilGetCache(remoteCacheManager);
    RemoteCache<std::string, std::string>& nontxCache = utilGetCache(nontxRemoteCacheManager);
    auto & txManager = remoteCacheManager->getTransactionManager();

    std::string k1("key13");
    std::string v1("boron1");
    std::string k2("key14");
    std::string v2("oxygen");

    std::string vx("calcium");

    cache.clear();

    txManager.begin();
    std::unique_ptr<std::string> oldv1(cache.put(k1,v1));
    std::unique_ptr<std::string> oldv2(cache.put(k2,v2));
    // Check the correct value from the tx context
    std::unique_ptr<std::string> rv1(nontxCache.put(k1,vx));
    EXPECT_EQ(rv1, nullptr);
    txManager.commit();
    EXPECT_EQ(*cache.get(k1), vx);
    EXPECT_EQ(cache.get(k2), nullptr);
}

