/*
 * StrongCounterTest.cpp
 *
 *  Created on: May 8, 2018
 *      Author: rigazilla
 */
#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "StrongCounterTest.h"

using ::infinispan::hotrod::ConfigurationBuilder;
using ::infinispan::hotrod::Configuration;

std::unique_ptr<infinispan::hotrod::RemoteCacheManager> StrongCounterTest::remoteCacheManager;

StrongCounterTest::StrongCounterTest()
{
}

void StrongCounterTest::SetUp() {
    if (StrongCounterTest::remoteCacheManager == nullptr) {
        ConfigurationBuilder builder;
        builder.addServer().host("127.0.0.1").port(11222);
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_27);
        builder.balancingStrategyProducer(nullptr);
        StrongCounterTest::remoteCacheManager.reset(new RemoteCacheManager(builder.build(), false));
    }
    StrongCounterTest::remoteCacheManager->start();
}
;

void StrongCounterTest::TearDown() {
    StrongCounterTest::remoteCacheManager->stop();
}

TEST_F(StrongCounterTest, testReset) {
    const std::string counterName("strongTestReset");
    const long initialValue = 5;
    auto& counterManager = StrongCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(initialValue, 0, 200, 8, CounterType::BOUNDED_STRONG, Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getStrongCounter(counterName);
    EXPECT_EQ(counter->getValue(), 5);
    EXPECT_EQ(counter->addAndGet(100), 105);
    counter->reset();
    EXPECT_EQ(counter->getValue(), initialValue);
    counter->remove();
}

TEST_F(StrongCounterTest, testConfiguration) {
    const std::string counterName("strongTestConfiguration");
    auto& counterManager = StrongCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(0, -256, 65536, 64, CounterType::BOUNDED_STRONG, Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getStrongCounter(counterName);
    CounterConfiguration retCc = counter->getConfiguration();
    EXPECT_EQ(cc.getLowerBound(), retCc.getLowerBound());
    EXPECT_EQ(cc.getUpperBound(), retCc.getUpperBound());
    EXPECT_EQ(0, retCc.getConcurrencyLevel());
    EXPECT_EQ(cc.getType(), retCc.getType());
    EXPECT_EQ(cc.getStorage(), retCc.getStorage());
    EXPECT_EQ(counter->getName(), counterName);
    counterManager.remove(counterName);
    counter->remove();

    const std::string counterName1("strongTestConfiguration1");
    CounterConfiguration cc1 = CounterConfiguration(0, -100, 100, 1, CounterType::UNBOUNDED_STRONG,
            Storage::PERSISTENT);
    counterManager.defineCounter(counterName1, cc1);
    counter = counterManager.getStrongCounter(counterName1);
    CounterConfiguration retCc1 = counter->getConfiguration();
    EXPECT_EQ(0, retCc1.getLowerBound());
    EXPECT_EQ(0, retCc1.getUpperBound());
    EXPECT_EQ(0, retCc1.getConcurrencyLevel());
    EXPECT_EQ(cc1.getType(), retCc1.getType());
    EXPECT_EQ(cc1.getStorage(), retCc1.getStorage());
    EXPECT_EQ(counter->getName(), counterName1);
    counterManager.remove(counterName1);
    counter->remove();
}

TEST_F(StrongCounterTest, testRemove) {
    const std::string counterName("strongTestRemove");
    const long initialValue = 5;
    auto& counterManager = StrongCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(initialValue, -100, 200, 8, CounterType::UNBOUNDED_STRONG, Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getStrongCounter(counterName);
    EXPECT_EQ(counter->addAndGet(100), 105);
    counter->remove();
    EXPECT_EQ(counter->getValue(), 5);
    EXPECT_EQ(counter->addAndGet(-100), -95);
    counterManager.remove(counterName);
    EXPECT_EQ(counter->getValue(), 5);
    counterManager.remove(counterName);
    counter->remove();
}

TEST_F(StrongCounterTest, strongTestAdd) {
    const std::string counterName("strongTestAdd");
    const long initialValue = 10;
    auto& counterManager = StrongCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(initialValue,0,0,8,CounterType::UNBOUNDED_STRONG,Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getStrongCounter(counterName);
    EXPECT_EQ(counter->getValue(),10);
    EXPECT_EQ(counter->addAndGet(10),20);
    EXPECT_EQ(counter->addAndGet(-20),0);
    EXPECT_EQ(counter->addAndGet(-20),-20);
    counterManager.remove(counterName);
    counter->remove();
}

TEST_F(StrongCounterTest, testCompareAndSet) {
    const std::string counterName("strongTestCompareAndSet");
    const long initialValue = 2;
    auto& counterManager = StrongCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(initialValue, 0, 0, 0, CounterType::UNBOUNDED_STRONG, Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getStrongCounter(counterName);
    EXPECT_FALSE(counter->compareAndSet(0, 1));
    EXPECT_TRUE(counter->compareAndSet(2, 3));
    EXPECT_TRUE(counter->compareAndSet(3, 4));
    counterManager.remove(counterName);
    counter->remove();
}

TEST_F(StrongCounterTest, testCompareAndSwap) {
    const std::string counterName("strongTestCompareAndSwap");
    const long initialValue = 3;
    auto& counterManager = StrongCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(initialValue, 0, 0, 0, CounterType::UNBOUNDED_STRONG, Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getStrongCounter(counterName);
    EXPECT_EQ(3, counter->compareAndSwap(1, 2));
    EXPECT_EQ(3, counter->compareAndSwap(3, 2));
    EXPECT_EQ(2, counter->compareAndSwap(3, 4));
    EXPECT_EQ(2, counter->compareAndSwap(2, 5));
    counterManager.remove(counterName);
    counter->remove();
}

TEST_F(StrongCounterTest, testBoundaries) {
    const std::string counterName("strongTestBoundaries");
    const long initialValue = 1;
    auto& counterManager = StrongCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(initialValue, 0, 20, 0, CounterType::BOUNDED_STRONG, Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getStrongCounter(counterName);
    EXPECT_EQ(1, counter->getValue());
    EXPECT_THROW(counter->addAndGet(-10), CounterLowerBoundException);
    EXPECT_THROW(counter->addAndGet(30), CounterUpperBoundException);
    EXPECT_EQ(20, counter->getValue());
    EXPECT_THROW(counter->compareAndSet(20, -1), CounterLowerBoundException);
    EXPECT_EQ(20, counter->getValue());
    EXPECT_THROW(counter->compareAndSet(20, 21), CounterUpperBoundException);
    counterManager.remove(counterName);
    counter->remove();
}

static std::promise<void> valueChangedPromise;
static int step;
static void action(const event::CounterEvent e) {
    switch (step) {
    case 0:
        EXPECT_EQ(5, e.oldValue);
        EXPECT_EQ(15, e.newValue);
        EXPECT_EQ(VALID, e.newState);
        EXPECT_EQ(VALID, e.oldState);
        valueChangedPromise.set_value();
        break;
    case 1:
        EXPECT_EQ(15, e.oldValue);
        EXPECT_EQ(0, e.newValue);
        EXPECT_EQ(LOWER_BOUND_REACHED, e.newState);
        EXPECT_EQ(VALID, e.oldState);
        valueChangedPromise.set_value();
        break;
    case 2:
        EXPECT_EQ(0, e.oldValue);
        EXPECT_EQ(20, e.newValue);
        EXPECT_EQ(UPPER_BOUND_REACHED, e.newState);
        EXPECT_EQ(LOWER_BOUND_REACHED, e.oldState);
        valueChangedPromise.set_value();
        break;
    default:
        EXPECT_TRUE(false);
    }
}

TEST_F(StrongCounterTest, testBasicListener) {
    const std::string counterName("strongTestBasicListener");
    event::CounterListener c(counterName, action);
    const long initialValue = 5;
    step = 0;
    valueChangedPromise = std::promise<void>();
    auto& counterManager = StrongCounterTest::remoteCacheManager->getCounterManager();
    auto cc = CounterConfiguration(initialValue, 0, 20, 8, CounterType::BOUNDED_STRONG, Storage::VOLATILE);
    counterManager.defineCounter(counterName, cc);
    auto counter = counterManager.getStrongCounter(counterName);
    counter->reset();
    counter->addListener(&c);
    counter->addAndGet(10);
    auto status = valueChangedPromise.get_future().wait_for(std::chrono::seconds(10));
    EXPECT_EQ(status, std::future_status::ready);
    valueChangedPromise = std::promise<void>();
    step = 1;
    try {
        counter->addAndGet(-20);
    }
    catch (const CounterLowerBoundException& ex) {
    }
    status = valueChangedPromise.get_future().wait_for(std::chrono::seconds(10));
    EXPECT_EQ(status, std::future_status::ready);
    valueChangedPromise = std::promise<void>();
    step = 2;
    try {
        counter->addAndGet(30);
    }
    catch (const CounterUpperBoundException& ex) {
    }
    status = valueChangedPromise.get_future().wait_for(std::chrono::seconds(10));
    EXPECT_EQ(status, std::future_status::ready);
}

