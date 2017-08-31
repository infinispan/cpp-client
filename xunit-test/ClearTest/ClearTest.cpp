#include "ClearTest.h"
#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"

using ::infinispan::hotrod::ConfigurationBuilder;
using ::infinispan::hotrod::Configuration;

std::unique_ptr<infinispan::hotrod::RemoteCacheManager> ClearTest::remoteCacheManager;


ClearTest::ClearTest()
{
}

void ClearTest::SetUp() {
	if (ClearTest::remoteCacheManager == nullptr){
		ConfigurationBuilder builder;
		builder.addServer().host("127.0.0.1").port(11222);
		builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
		builder.balancingStrategyProducer(nullptr);
		ClearTest::remoteCacheManager.reset(new RemoteCacheManager(builder.build(), false));
	}
};

void ClearTest::TearDown() {
	ClearTest::remoteCacheManager->stop();
}


TEST_F(ClearTest, ClearEmptiesTheCacheTest) {
	ClearTest::remoteCacheManager->start();
    BasicMarshaller<std::string> *km = new BasicMarshaller<std::string>();
    BasicMarshaller<std::string> *vm = new BasicMarshaller<std::string>();
    RemoteCache<std::string, std::string> cache = ClearTest::remoteCacheManager->getCache<std::string, std::string>(km,
            &Marshaller<std::string>::destroy,
            vm,
            &Marshaller<std::string>::destroy, true);
    cache.clear();
    EXPECT_EQ(0,cache.size());
}
