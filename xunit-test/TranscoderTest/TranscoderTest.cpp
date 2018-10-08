#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/JBasicMarshaller.h"
#include "TranscoderTest.h"

using ::infinispan::hotrod::ConfigurationBuilder;
using ::infinispan::hotrod::Configuration;

std::unique_ptr<infinispan::hotrod::RemoteCacheManager> TranscoderTest::remoteCacheManager;

TranscoderTest::TranscoderTest()
{
}

void TranscoderTest::SetUp() {
	if (TranscoderTest::remoteCacheManager == nullptr){
		ConfigurationBuilder builder;
		builder.addServer().host("127.0.0.1").port(11222);
		builder.protocolVersion(Configuration::PROTOCOL_VERSION_28);
		builder.balancingStrategyProducer(nullptr);
		TranscoderTest::remoteCacheManager.reset(new RemoteCacheManager(builder.build(), false));
	}
};

void TranscoderTest::TearDown() {
	TranscoderTest::remoteCacheManager->stop();
}

// Check transcoding between some media types


// Write in jboss format and read raw bytes. Cache is raw bytes
TEST_F(TranscoderTest, DefaultCacheWriteJbossReadRawBytes) {
    TranscoderTest::remoteCacheManager->start();

    JBasicMarshaller<std::string> *km = new JBasicMarshaller<std::string>();
    JBasicMarshaller<std::string> *vm = new JBasicMarshaller<std::string>();
    RemoteCache<std::string, std::string>& cache = TranscoderTest::remoteCacheManager->getCache<std::string, std::string>(km,
            &Marshaller<std::string>::destroy,
            vm,
            &Marshaller<std::string>::destroy,
            "default", true);

    // Define a cache with a raw media type for the value
    // and check that it returns value with the jboss marshaller preamble
    DataFormat<std::string, std::string> df1;
    df1.keyMediaType = MediaType(APPLICATION_JBOSS_MARSHALLING_TYPE);
    df1.valueMediaType = MediaType(APPLICATION_UNKNOWN_TYPE);
    df1.valueMarshaller.reset(new BasicMarshaller<std::string>());
    RemoteCache<std::string, std::string> rawCache = cache.withDataFormat(&df1);

    std::string k1("key13");
    std::string v1("boron");

    cache.clear();
    // Store data with jboss marshaller
    cache.put(k1,v1);

    std::shared_ptr<std::string> jv1(rawCache.get(k1));
    // Reading raw bytes the result contains the jboss preamble
    ASSERT_EQ(*jv1, "\003>\005boron");

    //Check that the initial cache works as usual
    std::shared_ptr<std::string> result_v1(cache.get(k1));
    ASSERT_EQ(*result_v1, "boron");
}

// Write in jboss format and read json. Cache is raw bytes
TEST_F(TranscoderTest, DefaultCacheWriteJbossReadJson) {
    TranscoderTest::remoteCacheManager->start();

    JBasicMarshaller<std::string> *km = new JBasicMarshaller<std::string>();
    JBasicMarshaller<std::string> *vm = new JBasicMarshaller<std::string>();
    RemoteCache<std::string, std::string>& cache = TranscoderTest::remoteCacheManager->getCache<std::string, std::string>(km,
            &Marshaller<std::string>::destroy,
            vm,
            &Marshaller<std::string>::destroy,
            "default", true);

    // Define a cache with a raw media type for the value
    // and check that it returns value with the jboss marshaller preamble
    DataFormat<std::string, std::string> df1;
    df1.keyMediaType = MediaType(APPLICATION_JBOSS_MARSHALLING_TYPE);
    df1.valueMediaType = MediaType(APPLICATION_JSON_TYPE);
    df1.valueMarshaller.reset(new BasicMarshaller<std::string>());
    RemoteCache<std::string, std::string> rawCache = cache.withDataFormat(&df1);

    std::string k1("key13");
    std::string v1("boron");

    cache.clear();
    // Store data with jboss marshaller
    cache.put(k1,v1);

    std::shared_ptr<std::string> jv1(rawCache.get(k1));
    // Data is stored in jboss marshaller, but json format is requested,
    // so the result is a json string containing the jboss preamble
    ASSERT_EQ(*jv1, "\"\\u0003>\\u0005boron\"");

    //Check that the initial cache works as usual
    std::shared_ptr<std::string> result_v1(cache.get(k1));
    ASSERT_EQ(*result_v1, "boron");

}

// Write in jboss format and read raw bytes. Cache is raw bytes
TEST_F(TranscoderTest, JBossMarshalledCacheWriteJbossReadRawBytes) {
    TranscoderTest::remoteCacheManager->start();

    JBasicMarshaller<std::string> *km = new JBasicMarshaller<std::string>();
    JBasicMarshaller<std::string> *vm = new JBasicMarshaller<std::string>();
    RemoteCache<std::string, std::string>& cache = TranscoderTest::remoteCacheManager->getCache<std::string, std::string>(km,
            &Marshaller<std::string>::destroy,
            vm,
            &Marshaller<std::string>::destroy,
            "transcodingCache", true);

    // Define a cache with a raw media type for the value
    // and check that it returns value with the jboss marshaller preamble
    DataFormat<std::string, std::string> df1;
    df1.keyMediaType = MediaType(APPLICATION_JBOSS_MARSHALLING_TYPE);
    df1.valueMediaType = MediaType(APPLICATION_UNKNOWN_TYPE);
    df1.valueMarshaller.reset(new BasicMarshaller<std::string>());
    RemoteCache<std::string, std::string> rawCache = cache.withDataFormat(&df1);

    std::string k1("key13");
    std::string v1("boron");

    cache.clear();
    cache.put(k1,v1);

    std::shared_ptr<std::string> jv1(rawCache.get(k1));
    // Check with jboss marshaller format
    ASSERT_EQ(*jv1, "\003>\005boron");

    //Check that the initial cache works as usual
    std::shared_ptr<std::string> result_v1(cache.get(k1));
    ASSERT_EQ(*result_v1, "boron");
}

// Write jboss format and read raw json. Cache is jboss marshalled, so string are recognized and transcoded correctly
TEST_F(TranscoderTest, JBossMarshalledCacheWriteJbossReadJson) {
    TranscoderTest::remoteCacheManager->start();

    JBasicMarshaller<std::string> *km = new JBasicMarshaller<std::string>();
    JBasicMarshaller<std::string> *vm = new JBasicMarshaller<std::string>();
    RemoteCache<std::string, std::string>& cache = TranscoderTest::remoteCacheManager->getCache<std::string, std::string>(km,
            &Marshaller<std::string>::destroy,
            vm,
            &Marshaller<std::string>::destroy,
            "transcodingCache", true);

    // Define a cache with a raw media type for the value
    // and check that it returns value with the jboss marshaller preamble
    DataFormat<std::string, std::string> df1;
    df1.keyMediaType = MediaType(APPLICATION_JBOSS_MARSHALLING_TYPE);
    df1.valueMediaType = MediaType(APPLICATION_JSON_TYPE);
    df1.valueMarshaller.reset(new BasicMarshaller<std::string>());
    RemoteCache<std::string, std::string> rawCache = cache.withDataFormat(&df1);

    std::string k1("key13");
    std::string v1("boron");

    cache.clear();
    cache.put(k1,v1);

    std::shared_ptr<std::string> jv1(rawCache.get(k1));
    // We got a clean json string
    ASSERT_EQ(*jv1, "\"boron\"");

    //Check that the initial cache works as usual
    std::shared_ptr<std::string> result_v1(cache.get(k1));
    ASSERT_EQ(*result_v1, "boron");
}

// Write jboss format and read raw json. Cache is jboss marshalled, so integers are recognized and transcoded correctly
TEST_F(TranscoderTest, JBossMarshalledCacheWriteIntegerJbossReadJson) {
    TranscoderTest::remoteCacheManager->start();

    JBasicMarshaller<std::string> *km = new JBasicMarshaller<std::string>();
    JBasicMarshaller<int> *vm = new JBasicMarshaller<int>();
    RemoteCache<std::string, int>& cache = TranscoderTest::remoteCacheManager->getCache<std::string, int>(km,
            &Marshaller<std::string>::destroy,
            vm,
            &Marshaller<int>::destroy,
            "transcodingCache", true);

    // Define a cache with a raw media type for the value
    // and check that it returns value with the jboss marshaller preamble
    DataFormat<std::string, std::string> df1;
    df1.keyMediaType = MediaType(APPLICATION_JBOSS_MARSHALLING_TYPE);
    df1.valueMediaType = MediaType(APPLICATION_JSON_TYPE);
    df1.valueMarshaller.reset(new BasicMarshaller<std::string>());
    df1.keyMarshaller.reset(new JBasicMarshaller<std::string>());
    RemoteCache<std::string, std::string> rawCache = cache.withDataFormat(&df1);

    std::string k1("key13");
    int v1(42);

    cache.clear();
    cache.put(k1,v1);

    std::shared_ptr<std::string> jv1(rawCache.get(k1));
    // We got a clean json integer
    ASSERT_EQ(*jv1, "42");

    //Check that the initial cache works as usual
    std::shared_ptr<int> result_v1(cache.get(k1));
    ASSERT_EQ(*result_v1, 42);
}

TEST_F(TranscoderTest, JBossMarshalledCacheWriteIntegerJsonReadJBoss) {
    TranscoderTest::remoteCacheManager->start();

    JBasicMarshaller<std::string> *km = new JBasicMarshaller<std::string>();
    JBasicMarshaller<int> *vm = new JBasicMarshaller<int>();
    RemoteCache<std::string, int>& cache = TranscoderTest::remoteCacheManager->getCache<std::string, int>(km,
            &Marshaller<std::string>::destroy,
            vm,
            &Marshaller<int>::destroy,
            "transcodingCache", true);

    // Define a cache with a raw media type for the value
    // and check that it returns value with the jboss marshaller preamble
    DataFormat<std::string, std::string> df1;
    df1.keyMediaType = MediaType(APPLICATION_JSON_TYPE);
    df1.valueMediaType = MediaType(APPLICATION_JSON_TYPE);
    df1.valueMarshaller.reset(new BasicMarshaller<std::string>());
    df1.keyMarshaller.reset(new BasicMarshaller<std::string>());
    RemoteCache<std::string, std::string> rawCache = cache.withDataFormat(&df1);

    std::string k1("key13");
    std::string v1("42");

    cache.clear();
    std::string jsonk1("\"key13\"");
    // Writing key,value in json format
    rawCache.put(jsonk1,v1);

    std::shared_ptr<std::string> jv1(rawCache.get(jsonk1));
    // Check json value
    ASSERT_EQ(*jv1, "42");

    // Check value with in jboss marshaller format
    std::shared_ptr<int> result_v1(cache.get(k1));
    ASSERT_EQ(*result_v1, 42);
}
