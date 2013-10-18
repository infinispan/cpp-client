#include "infinispan/hotrod/ConnectionPoolConfiguration.h"

namespace infinispan {
namespace hotrod {

ConnectionPoolConfiguration::ConnectionPoolConfiguration(ExhaustedAction exAction,
        bool lifoPar,
        int maxActivePar,
        int maxTotalPar,
        long maxWaitPar,
        int maxIdlePar,
        int minIdlePar,
        int numTestsPerEvictionRunPar,
        int timeBetweenEvictionRunsPar,
        int minEvictableIdleTimePar,
        bool testOnBorrowPar,
        bool testOnReturnPar,
        bool testWhileIdlePar) : exhaustedAction(exAction), lifo(lifoPar), maxActive(maxActivePar), maxTotal(maxTotalPar), maxWait(maxWaitPar),
            maxIdle(maxIdlePar), minIdle(minIdlePar), numTestsPerEvictionRun(numTestsPerEvictionRunPar), timeBetweenEvictionRuns(timeBetweenEvictionRunsPar),
            minEvictableIdleTime(minEvictableIdleTimePar), testOnBorrow(testOnBorrowPar), testOnReturn(testOnReturnPar), testWhileIdle(testWhileIdlePar)
{

}

const ExhaustedAction& ConnectionPoolConfiguration::getExhaustedAction() const
{
  return ConnectionPoolConfiguration::exhaustedAction;
}

const bool& ConnectionPoolConfiguration::isLifo() const
{
  return ConnectionPoolConfiguration::lifo;
}

const int& ConnectionPoolConfiguration::getMaxActive() const
{
  return ConnectionPoolConfiguration::maxActive;
}

const int& ConnectionPoolConfiguration::getMaxTotal() const
{
  return ConnectionPoolConfiguration::maxTotal;
}

const long& ConnectionPoolConfiguration::getMaxWait() const
{
  return ConnectionPoolConfiguration::maxWait;
}

const int& ConnectionPoolConfiguration::getMaxIdle() const
{
  return ConnectionPoolConfiguration::maxIdle;
}

const int& ConnectionPoolConfiguration::getMinIdle() const
{
  return ConnectionPoolConfiguration::minIdle;
}

const int& ConnectionPoolConfiguration::getNumTestsPerEvictionRun() const
{
  return ConnectionPoolConfiguration::numTestsPerEvictionRun;
}

const int& ConnectionPoolConfiguration::getTimeBetweenEvictionRuns() const
{
  return ConnectionPoolConfiguration::timeBetweenEvictionRuns;
}

const int& ConnectionPoolConfiguration::getMinEvictableIdleTime() const
{
  return ConnectionPoolConfiguration::minEvictableIdleTime;
}

const bool& ConnectionPoolConfiguration::isTestOnBorrow() const
{
  return ConnectionPoolConfiguration::testOnBorrow;
}

const bool& ConnectionPoolConfiguration::isTestOnReturn() const
{
  return ConnectionPoolConfiguration::testOnReturn;
}

const bool& ConnectionPoolConfiguration::isTestWhileIdle() const
{
  return ConnectionPoolConfiguration::testWhileIdle;
}

}
}




