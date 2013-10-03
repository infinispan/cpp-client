#ifndef ISPN_HOTROD_TRANSPORT_GENERICKEYEDOBJECTPOOLFACTORY_H
#define ISPN_HOTROD_TRANSPORT_GENERICKEYEDOBJECTPOOLFACTORY_H



#include "hotrod/impl/transport/tcp/KeyedPoolableObjectFactory.h"
#include "hotrod/impl/transport/tcp/GenericKeyedObjectPool.h"

namespace infinispan {
namespace hotrod {
namespace transport {

template <class K, class V> class GenericKeyedObjectPoolFactory
{
  public:
	GenericKeyedObjectPoolFactory(
      KeyedPoolableObjectFactory<K,V>* factory_,
      int maxActive_,
      WhenExhaustedAction whenExhaustedAction_,
      long maxWait_,
      int maxIdle_,
      int maxTotal_,
      int minIdle_,
      bool testOnBorrow_,
      bool testOnReturn_,
      long timeBetweenEvictionRunsMillis_,
      int numTestsPerEvictionRun_,
      long minEvictableIdleTimeMillis_,
      bool testWhileIdle_,
      bool lifo_)
      : maxIdle(maxIdle_), maxActive(maxActive_),
        maxTotal(maxTotal_), minIdle(minIdle_), maxWait(maxWait_),
        whenExhaustedAction(whenExhaustedAction_), testOnBorrow(testOnBorrow_),
        testOnReturn(testOnReturn_), testWhileIdle(testWhileIdle_),
        timeBetweenEvictionRunsMillis(timeBetweenEvictionRunsMillis_),
        numTestsPerEvictionRun(numTestsPerEvictionRun_),
        minEvictableIdleTimeMillis(minEvictableIdleTimeMillis_), lifo(lifo_)
    {
	    factory.reset(factory_);
    }

    int getMaxIdle() { return maxIdle; }
    int getMaxActive() { return maxActive; }
    int getMaxTotal() { return maxTotal; }
    int getMinIdle() { return minIdle; }
    long getMaxWait() { return maxWait; }
    WhenExhaustedAction getWhenExhaustedAction() { return whenExhaustedAction; }
    bool hasTestOnBorrow() { return testOnBorrow; }
    bool hasTestOnReturn() { return testOnReturn; }
    bool hasTestWhileIdle() { return testWhileIdle; }
    int getTimeBetweenEvictionRunsMillis() { return timeBetweenEvictionRunsMillis; }
    int getNumTestsPerEvictionRun() { return numTestsPerEvictionRun; }
    int getMinEvictableIdleTimeMillis() { return minEvictableIdleTimeMillis; }
    bool isLifo() { return lifo; }

    // TODO
    GenericKeyedObjectPool<K,V>* createPool()
    {
        return new GenericKeyedObjectPool<K,V>(
            factory,
            maxActive,
            whenExhaustedAction,
            maxWait,
            maxIdle,
            maxTotal,
            minIdle,
            testOnBorrow,
            testOnReturn,
            timeBetweenEvictionRunsMillis,
            numTestsPerEvictionRun,
            minEvictableIdleTimeMillis,
            testWhileIdle,
            lifo
        );
    }

    ~GenericKeyedObjectPoolFactory() {/*delete &factory;*/} // SEGMENTATION FAULT

  private:
    HR_SHARED_PTR<KeyedPoolableObjectFactory<K,V> > factory;

    int maxIdle;
    int maxActive;
    int maxTotal;
    int minIdle;
    long maxWait;
    WhenExhaustedAction whenExhaustedAction;
    bool testOnBorrow;
    bool testOnReturn;
    bool testWhileIdle;
    long timeBetweenEvictionRunsMillis;
    int numTestsPerEvictionRun;
    long minEvictableIdleTimeMillis;
    bool lifo;
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_GENERICKEYEDOBJECTPOOLFACTORY_H */
