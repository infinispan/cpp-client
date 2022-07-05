/*
 * WeakCounterTest.cpp
 *
 *  Created on: May 8, 2018
 *      Author: rigazilla
 */#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "WeakCounterTest.h"

#include <future>

using ::infinispan::hotrod::ConfigurationBuilder;
using ::infinispan::hotrod::Configuration;

std::unique_ptr<infinispan::hotrod::RemoteCacheManager> WeakCounterTest::remoteCacheManager;


WeakCounterTest::WeakCounterTest()
{
}

void WeakCounterTest::SetUp() {
	if (WeakCounterTest::remoteCacheManager == nullptr){
		ConfigurationBuilder builder;
		builder.addServer().host("127.0.0.1").port(11222);
		builder.protocolVersion(Configuration::PROTOCOL_VERSION_27);
		builder.balancingStrategyProducer(nullptr);
		WeakCounterTest::remoteCacheManager.reset(new RemoteCacheManager(builder.build(), false));
	}
    WeakCounterTest::remoteCacheManager->start();
};

void WeakCounterTest::TearDown() {
    WeakCounterTest::remoteCacheManager->stop();
}


TEST_F(WeakCounterTest, testAdd) {
    const std::string counterName("weakTestAdd");
    const long initialValue = 10;
    auto& counterManager = WeakCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(initialValue,0,0,8,CounterType::WEAK,Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getWeakCounter(counterName);
    EXPECT_EQ(counter->getValue(),10);
    counter->add(10);
    EXPECT_EQ(counter->getValue(),20);
    counter->add(-20);
    EXPECT_EQ(counter->getValue(),0);
    counter->add(-20);
    EXPECT_EQ(counter->getValue(),-20);
    counter->remove();
}

TEST_F(WeakCounterTest, testReset) {
    const std::string counterName("weakTestReset");
    const long initialValue = 5;
    auto& counterManager = WeakCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(initialValue, 0, 0, 8, CounterType::WEAK, Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getWeakCounter(counterName);
    EXPECT_EQ(counter->getValue(),5);
    counter->add(100);
    EXPECT_EQ(counter->getValue(),105);
    counter->reset();
    EXPECT_EQ(counter->getValue(),initialValue);
    counter->remove();
}

TEST_F(WeakCounterTest, testConfiguration) {
    const std::string counterName("weakTestConfiguration");
    auto& counterManager = WeakCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(0, 0, 0, 64, CounterType::WEAK, Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getWeakCounter(counterName);
    CounterConfiguration retCc = counter->getConfiguration();
    EXPECT_EQ(cc.getConcurrencyLevel(), retCc.getConcurrencyLevel());
    EXPECT_EQ(cc.getType(), retCc.getType());
    EXPECT_EQ(cc.getStorage(), retCc.getStorage());
    EXPECT_EQ(counter->getName(), counterName);
    counterManager.remove(counterName);
    counter->remove();

    const std::string counterName1("weakTestConfiguration1");
    CounterConfiguration cc1 = CounterConfiguration(0, 0, 0, 1, CounterType::WEAK, Storage::PERSISTENT);
    counterManager.defineCounter(counterName1, cc1);
    counter = counterManager.getWeakCounter(counterName1);
    CounterConfiguration retCc1 = counter->getConfiguration();
    EXPECT_EQ(cc1.getConcurrencyLevel(), retCc1.getConcurrencyLevel());
    EXPECT_EQ(cc1.getType(), retCc1.getType());
    EXPECT_EQ(cc1.getStorage(), retCc1.getStorage());
    EXPECT_EQ(counter->getName(), counterName1);
    counterManager.remove(counterName1);
    counter->remove();
}

TEST_F(WeakCounterTest, testRemove) {
    const std::string counterName("weakTestRemove");
    const long initialValue = 5;
    auto& counterManager = WeakCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(initialValue, 0, 0, 8, CounterType::WEAK, Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getWeakCounter(counterName);
    counter->add(100);
    EXPECT_EQ(counter->getValue(),105);
    counter->remove();
    EXPECT_EQ(counter->getValue(),5);
    counter->add(-100);
    EXPECT_EQ(counter->getValue(),-95);
    counterManager.remove(counterName);
    EXPECT_EQ(counter->getValue(),5);
    counterManager.remove(counterName);
    counter->remove();
}

static std::promise<void> valueChangedPromise;
static void action(const event::CounterEvent e) {
    EXPECT_EQ(5, e.oldValue);
    EXPECT_EQ(15, e.newValue);
    EXPECT_EQ(VALID, e.newState);
    EXPECT_EQ(VALID, e.oldState);
    valueChangedPromise.set_value();
}

static void action1(const event::CounterEvent e) {
    EXPECT_EQ(6, e.oldValue);
    EXPECT_EQ(16, e.newValue);
    EXPECT_EQ(VALID, e.newState);
    EXPECT_EQ(VALID, e.oldState);
    valueChangedPromise.set_value();
}

TEST_F(WeakCounterTest, testAddRemoveListener) {
    const std::string counterName("testAddRemoveListener");
    event::CounterListener c(counterName, action);
    event::CounterListener c1(counterName, action1);
    const long initialValue = 5;
    valueChangedPromise = std::promise<void>();
    auto& counterManager = WeakCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(initialValue, 0, 20, 8, CounterType::WEAK, Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getWeakCounter(counterName);
    counter->reset();
    auto handler = counter->addListener(&c);
    counter->add(10);
    auto status = valueChangedPromise.get_future().wait_for(std::chrono::seconds(10));
    EXPECT_EQ(status, std::future_status::ready);
    valueChangedPromise = std::promise<void>();
    counter->removeListener(handler);
    counter->add(-9);
    status = valueChangedPromise.get_future().wait_for(std::chrono::seconds(10));
    EXPECT_EQ(status, std::future_status::timeout);
    handler = counter->addListener(&c1);
    valueChangedPromise = std::promise<void>();
    counter->add(10);
    status = valueChangedPromise.get_future().wait_for(std::chrono::seconds(10));
    EXPECT_EQ(status, std::future_status::ready);
}

/*

   @Override
   public void testListenerAddAndRemove(Method method) throws InterruptedException {
      strategy.testListenerAddAndRemove(method);
   }

*/

