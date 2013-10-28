#include "infinispan/hotrod/ConnectionPoolConfigurationBuilder.h"

namespace infinispan {
namespace hotrod {

ConnectionPoolConfigurationBuilder::ConnectionPoolConfigurationBuilder(ConfigurationBuilder& builder) :
        ConfigurationChildBuilder(builder),
        m_exhaustedAction(WAIT),
        m_lifo(true),
        m_maxActive(8),
        m_maxTotal(-1),
        m_maxWait(-1),
        m_maxIdle(8),
        m_minIdle(0),
        m_numTestsPerEvictionRun(3),
        m_timeBetweenEvictionRuns(-1),
        m_minEvictableIdleTime(1000l * 60l * 30l),
        m_testOnBorrow(false),
        m_testOnReturn(false),
        m_testWhileIdle(true)
      {}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::exhaustedAction(ExhaustedAction exhaustedAction_) {
    m_exhaustedAction = exhaustedAction_;
    return *this;
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::lifo(bool lifo_) {
    m_lifo = lifo_;
    return *this;
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::maxActive(int maxActive_) {
    m_maxActive = maxActive_;
    return *this;
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::maxTotal(int maxTotal_)  {
    m_maxTotal = maxTotal_;
    return *this;
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::maxWait(long maxWait_) {
    m_maxWait = maxWait_;
    return *this;
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::maxIdle(int maxIdle_) {
    m_maxIdle = maxIdle_;
    return *this;
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::minIdle(int minIdle_) {
    m_minIdle = minIdle_;
    return *this;
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::numTestsPerEvictionRun(int numTestsPerEvictionRun_) {
    m_numTestsPerEvictionRun = numTestsPerEvictionRun_;
    return *this;
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::timeBetweenEvictionRuns(int timeBetweenEvictionRuns_) {
    m_timeBetweenEvictionRuns = timeBetweenEvictionRuns_;
    return *this;
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::minEvictableIdleTime(int minEvictableIdleTime_) {
    m_minEvictableIdleTime = minEvictableIdleTime_;
    return *this;
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::testOnBorrow(bool testOnBorrow_) {
    m_testOnBorrow = testOnBorrow_;
    return *this;
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::testOnReturn(bool testOnReturn_) {
    m_testOnReturn = testOnReturn_;
    return *this;
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::testWhileIdle(bool testWhileIdle_) {
    m_testWhileIdle = testWhileIdle_;
    return *this;
}

ConnectionPoolConfiguration ConnectionPoolConfigurationBuilder::create()
{
    return ConnectionPoolConfiguration(
            m_exhaustedAction,
            m_lifo,
            m_maxActive,
            m_maxTotal,
            m_maxWait,
            m_maxIdle,
            m_minIdle,
            m_numTestsPerEvictionRun,
            m_timeBetweenEvictionRuns,
            m_minEvictableIdleTime,
            m_testOnBorrow,
            m_testOnReturn,
            m_testWhileIdle
    );
}

ConnectionPoolConfigurationBuilder& ConnectionPoolConfigurationBuilder::read(ConnectionPoolConfiguration& /*bean*/) {
    return *this;
}


}}
