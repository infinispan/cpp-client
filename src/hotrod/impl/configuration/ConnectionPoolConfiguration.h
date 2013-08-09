/*
 * ConnectionPoolConfigguration.h
 *
 *  Created on: Jul 10, 2013
 *      Author: samuele
 */

#ifndef CONNECTIONPOOLCONFIGURATION_H_
#define CONNECTIONPOOLCONFIGURATION_H_

#include "infinispan/hotrod/ImportExport.h"

namespace infinispan {
namespace hotrod {

enum ExhaustedAction { EXCEPTION, WAIT, CREATE_NEW};

class HR_EXTERN ConnectionPoolConfiguration
{
  public:
    //TODO: remove the default constructor
    ConnectionPoolConfiguration(ExhaustedAction exhaustedAction,
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
        bool testWhileIdle);

    const ExhaustedAction& getExhaustedAction() const;
    const bool& isLifo() const;
    const int& getMaxActive() const;
    const int& getMaxTotal() const;
    const long& getMaxWait() const;
    const int& getMaxIdle() const;
    const int& getMinIdle() const;
    const int& getNumTestsPerEvictionRun() const;
    const int& getTimeBetweenEvictionRuns() const;
    const int& getMinEvictableIdleTime() const;
    const bool& isTestOnBorrow() const;
    const bool& isTestOnReturn() const;
    const bool& isTestWhileIdle() const;

  private:
    ExhaustedAction exhaustedAction;
    bool lifo;
    int maxActive;
    int maxTotal;
    long maxWait;
    int maxIdle;
    int minIdle;
    int numTestsPerEvictionRun;
    int timeBetweenEvictionRuns;
    int minEvictableIdleTime;
    bool testOnBorrow;
    bool testOnReturn;
    bool testWhileIdle;
};

}
}

#endif /* CONNECTIONPOOLCONFIGURATION_H_ */
