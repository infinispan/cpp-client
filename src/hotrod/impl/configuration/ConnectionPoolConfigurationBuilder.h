#ifndef CONNECTIONPOOLCONFIGURATIONBUILDER_H_
#define CONNECTIONPOOLCONFIGURATIONBUILDER_H_

#include "infinispan/hotrod/ImportExport.h"
#include "Builder.h"
#include "ConfigurationChildBuilder.h"

namespace infinispan {
namespace hotrod {


class HR_EXTERN ConnectionPoolConfigurationBuilder
  : public Builder<ConnectionPoolConfiguration>, public ConfigurationChildBuilder
{
  public:
    ConnectionPoolConfigurationBuilder(ConfigurationBuilder& builder);

    ConnectionPoolConfigurationBuilder& exhaustedAction(ExhaustedAction exhaustedAction_);
    ConnectionPoolConfigurationBuilder& lifo(bool lifo_);
    ConnectionPoolConfigurationBuilder& maxActive(int maxActive_);
    ConnectionPoolConfigurationBuilder& maxTotal(int maxTotal_);
    ConnectionPoolConfigurationBuilder& maxWait(long maxWait_);
    ConnectionPoolConfigurationBuilder& maxIdle(int maxIdle_);
    ConnectionPoolConfigurationBuilder& minIdle(int minIdle_);
    ConnectionPoolConfigurationBuilder& numTestsPerEvictionRun(int numTestsPerEvictionRun_);
    ConnectionPoolConfigurationBuilder& timeBetweenEvictionRuns(int timeBetweenEvictionRuns_);
    ConnectionPoolConfigurationBuilder& minEvictableIdleTime(int minEvictableIdleTime_);
    ConnectionPoolConfigurationBuilder& testOnBorrow(bool testOnBorrow_);
    ConnectionPoolConfigurationBuilder& testOnReturn(bool testOnReturn_);
    ConnectionPoolConfigurationBuilder& testWhileIdle(bool testWhileIdle_);

    virtual ConnectionPoolConfiguration create();
    virtual ConnectionPoolConfigurationBuilder& read(ConnectionPoolConfiguration& bean);

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
