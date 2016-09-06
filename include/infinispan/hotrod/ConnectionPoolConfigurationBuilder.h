#ifndef CONNECTIONPOOLCONFIGURATIONBUILDER_H_
#define CONNECTIONPOOLCONFIGURATIONBUILDER_H_

#include "infinispan/hotrod/ImportExport.h"

namespace infinispan {
namespace hotrod {

/**
 *
 * ConnectionPoolConfigurationBuilder is used to specify connection pooling properties for the HotRod client
 *
 */
class ConnectionPoolConfigurationBuilder
{
  public:
    ConnectionPoolConfigurationBuilder():
        m_exhaustedAction(WAIT),
        m_lifo(true),
        m_maxActive(8),
        m_maxTotal(-1),
        m_maxWait(-1),
        m_maxIdle(8),
        m_minIdle(0),
        m_numTestsPerEvictionRun(3),
        m_timeBetweenEvictionRuns(120000),
        m_minEvictableIdleTime(1000l * 60l * 30l),
        m_testOnBorrow(false),
        m_testOnReturn(false),
        m_testWhileIdle(true) {}

    /**
       * Specifies what happens when asking for a connection from a server's pool, and that pool is
       * exhausted.
       *
       * \return  ConnectionPoolConfigurationBuilder for further configuration
       */
    ConnectionPoolConfigurationBuilder& exhaustedAction(ExhaustedAction exhaustedAction_) {
        m_exhaustedAction = exhaustedAction_;
        return *this;
    }

    /**
       * Sets the LIFO status. True means that borrowObject returns the most recently used ("last in")
       * idle object in a pool (if there are idle instances available). False means that pools behave
       * as FIFO queues - objects are taken from idle object pools in the order that they are returned.
       * The default setting is true
       *
       * \return  ConnectionPoolConfigurationBuilder for further configuration
       */
    ConnectionPoolConfigurationBuilder& lifo(bool lifo_) {
        m_lifo = lifo_;
        return *this;
    }

    /**
     * Controls the maximum number of connections per server that are allocated (checked out to
     * client threads, or idle in the pool) at one time. When non-positive, there is no limit to the
     * number of connections per server. When maxActive is reached, the connection pool for that
     * server is said to be exhausted. The default setting for this parameter is -1, i.e. there is no
     * limit.
     *
     * \return  ConnectionPoolConfigurationBuilder for further configuration
     */
    ConnectionPoolConfigurationBuilder& maxActive(int maxActive_) {
        m_maxActive = maxActive_;
        return *this;
    }

    /**
     * Sets a global limit on the number persistent connections that can be in circulation within the
     * combined set of servers. When non-positive, there is no limit to the total number of
     * persistent connections in circulation. When maxTotal is exceeded, all connections pools are
     * exhausted. The default setting for this parameter is -1 (no limit).
     *
     * \return  ConnectionPoolConfigurationBuilder for further configuration
     */
    ConnectionPoolConfigurationBuilder& maxTotal(int maxTotal_) {
        m_maxTotal = maxTotal_;
        return *this;
    }

    /**
     * The amount of time in milliseconds to wait for a connection to become available when the
     * exhausted action is ExhaustedAction::WAIT, after which an will be thrown. If a negative
     * value is supplied, the pool will block indefinitely.
     *
     * \return  ConnectionPoolConfigurationBuilder for further configuration
     */
    ConnectionPoolConfigurationBuilder& maxWait(long maxWait_) {
        m_maxWait = maxWait_;
        return *this;
    }

    /**
     * Controls the maximum number of idle persistent connections, per server, at any time. When
     * negative, there is no limit to the number of connections that may be idle per server. The
     * default setting for this parameter is -1.
     *
     * \return  ConnectionPoolConfigurationBuilder for further configuration
     */
    ConnectionPoolConfigurationBuilder& maxIdle(int maxIdle_) {
        m_maxIdle = maxIdle_;
        return *this;
    }

    /**
     * Sets a target value for the minimum number of idle connections (per server) that should always
     * be available. If this parameter is set to a positive number and timeBetweenEvictionRuns
     * > 0, each time the idle connection eviction thread runs, it will try to create enough idle
     * instances so that there will be minIdle idle instances available for each server. The default
     * setting for this parameter is 1.
     *
     * \return  ConnectionPoolConfigurationBuilder for further configuration
     */
    ConnectionPoolConfigurationBuilder& minIdle(int minIdle_) {
        m_minIdle = minIdle_;
        return *this;
    }

    /**
     * Indicates the maximum number of connections to test during idle eviction runs. The default
     * setting is 3.
     *
     * \return  ConnectionPoolConfigurationBuilder for further configuration
     */
    ConnectionPoolConfigurationBuilder& numTestsPerEvictionRun(int numTestsPerEvictionRun_) {
        m_numTestsPerEvictionRun = numTestsPerEvictionRun_;
        return *this;
    }

    /**
     * Indicates how long the eviction thread should sleep before "runs" of examining idle
     * connections. When non-positive, no eviction thread will be launched. The default setting for
     * this parameter is 2 minutes.
     *
     * \return  ConnectionPoolConfigurationBuilder for further configuration
     */
    ConnectionPoolConfigurationBuilder& timeBetweenEvictionRuns(int timeBetweenEvictionRuns_) {
        m_timeBetweenEvictionRuns = timeBetweenEvictionRuns_;
        return *this;
    }

    /**
     * Specifies the minimum amount of time that an connection may sit idle in the pool before it is
     * eligible for eviction due to idle time. When non-positive, no connection will be dropped from
     * the pool due to idle time alone. This setting has no effect unless
     * timeBetweenEvictionRuns > 0. The default setting for this parameter is 1800000ms (30
     * minutes).
     *
     * \return  ConnectionPoolConfigurationBuilder for further configuration
     */
    ConnectionPoolConfigurationBuilder& minEvictableIdleTime(int minEvictableIdleTime_) {
        m_minEvictableIdleTime = minEvictableIdleTime_;
        return *this;
    }

    /**
     * Indicates whether connections should be validated before being taken from the pool by sending
     * an TCP packet to the server. Connections that fail to validate will be dropped from the pool.
     * The default setting for this parameter is false.
     *
     * \return  ConnectionPoolConfigurationBuilder for further configuration
     */
    ConnectionPoolConfigurationBuilder& testOnBorrow(bool testOnBorrow_) {
        m_testOnBorrow = testOnBorrow_;
        return *this;
    }

    /**
     * Indicates whether connections should be validated when being returned to the pool sending an
     * TCP packet to the server. Connections that fail to validate will be dropped from the pool. The
     * default setting for this parameter is false.
     *
     * \return  ConnectionPoolConfigurationBuilder for further configuration
     */
    ConnectionPoolConfigurationBuilder& testOnReturn(bool testOnReturn_) {
        m_testOnReturn = testOnReturn_;
        return *this;
    }

    /**
     * Indicates whether or not idle connections should be validated by sending an TCP packet to the
     * server, during idle connection eviction runs. Connections that fail to validate will be
     * dropped from the pool. This setting has no effect unless timeBetweenEvictionRuns > 0.
     * The default setting for this parameter is true.
     *
     *\return  ConnectionPoolConfigurationBuilder for further configuration
     */
    ConnectionPoolConfigurationBuilder& testWhileIdle(bool testWhileIdle_) {
        m_testWhileIdle = testWhileIdle_;
        return *this;
    }

    ConnectionPoolConfiguration create() {
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
            m_testWhileIdle);
    }

  private:
    ExhaustedAction m_exhaustedAction;
    bool m_lifo;
    int m_maxActive;
    int m_maxTotal;
    long m_maxWait;
    int m_maxIdle;
    int m_minIdle;
    int m_numTestsPerEvictionRun;
    int m_timeBetweenEvictionRuns;
    int m_minEvictableIdleTime;
    bool m_testOnBorrow;
    bool m_testOnReturn;
    bool m_testWhileIdle;
};

}} //namespace
#endif /* CONNECTIONPOOLCONFIGURATIONBUILDER_H_ */
