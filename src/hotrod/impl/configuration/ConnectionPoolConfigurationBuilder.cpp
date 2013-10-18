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
