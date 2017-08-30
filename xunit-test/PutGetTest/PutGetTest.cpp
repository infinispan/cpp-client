#include "PutGetTest.h"
#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"

using ::infinispan::hotrod::ConfigurationBuilder;
using ::infinispan::hotrod::Configuration;

std::unique_ptr<infinispan::hotrod::RemoteCacheManager> PutGetTest::remoteCacheManager;


PutGetTest::PutGetTest()
{
}

void PutGetTest::SetUp() {
	if (PutGetTest::remoteCacheManager == nullptr){
		ConfigurationBuilder builder;
		builder.addServer().host("127.0.0.1").port(11222);
		builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
		builder.balancingStrategyProducer(nullptr);
		PutGetTest::remoteCacheManager.reset(new RemoteCacheManager(builder.build(), false));
	}
};

void PutGetTest::TearDown() {
	PutGetTest::remoteCacheManager->stop();
}

TEST_F(PutGetTest, OperationOnCacheManagerNotStarted) {
    BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
    BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
    RemoteCache<std::string, std::string> cache = PutGetTest::remoteCacheManager->getCache<std::string, std::string>(km,
            &Marshaller<std::string>::destroy,
            vm,
            &Marshaller<std::string>::destroy, true);
    EXPECT_ANY_THROW(cache.get("k1"));
}



TEST_F(PutGetTest, SimplePutForcedReturnReturnsPrevVal) {
	PutGetTest::remoteCacheManager->start();
    BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
    BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
    RemoteCache<std::string, std::string> cache = PutGetTest::remoteCacheManager->getCache<std::string, std::string>(km,
            &Marshaller<std::string>::destroy,
            vm,
            &Marshaller<std::string>::destroy, true);
    std::unique_ptr<std::string> prevVal(cache.get("k1"));
    std::unique_ptr<std::string> ret(cache.put("k1","v1"));
    EXPECT_TRUE(prevVal==ret || *prevVal==*ret);
}

TEST_F(PutGetTest, ATestThatFails) {
	PutGetTest::remoteCacheManager->start();
    BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
    BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
    RemoteCache<std::string, std::string> cache = PutGetTest::remoteCacheManager->getCache<std::string, std::string>(km,
            &Marshaller<std::string>::destroy,
            vm,
            &Marshaller<std::string>::destroy, true);
    cache.clear();
    EXPECT_LT(0,cache.size());
}
