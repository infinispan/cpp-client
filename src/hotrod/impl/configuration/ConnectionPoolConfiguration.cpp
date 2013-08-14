/*
 * ConnectionPoolconfiguration.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: samuele
 */

#include "hotrod/impl/configuration/ConnectionPoolConfiguration.h"

namespace infinispan {
namespace hotrod {

ConnectionPoolConfiguration::ConnectionPoolConfiguration(ExhaustedAction exhaustedAction,
        bool lifo,
        int maxActive,
        int maxTotal,
        long maxWait,
        int maxIdle,
        int minIdle,
        int numTestsPerEvictionRun,
        int timeBetweenEvictionRuns,
        int minEvictableIdleTime,
        bool testOnBorrow,
        bool testOnReturn,
        bool testWhileIdle)
{
  ConnectionPoolConfiguration::exhaustedAction = exhaustedAction;
  ConnectionPoolConfiguration::lifo = lifo;
  ConnectionPoolConfiguration::maxActive = maxActive;
  ConnectionPoolConfiguration::maxTotal = maxTotal;
  ConnectionPoolConfiguration::maxWait = maxWait;
  ConnectionPoolConfiguration::maxIdle = maxIdle;
  ConnectionPoolConfiguration::minIdle = minIdle;
  ConnectionPoolConfiguration::numTestsPerEvictionRun = numTestsPerEvictionRun;
  ConnectionPoolConfiguration::timeBetweenEvictionRuns = timeBetweenEvictionRuns;
  ConnectionPoolConfiguration::minEvictableIdleTime = minEvictableIdleTime;
  ConnectionPoolConfiguration::testOnBorrow = testOnBorrow;
  ConnectionPoolConfiguration::testOnReturn = testOnReturn;
  ConnectionPoolConfiguration::testWhileIdle = testWhileIdle;
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




