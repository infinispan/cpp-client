#ifndef ISPN_HOTROD_TRANSPORT_GENERICKEYEDOBJECTPOOL_H
#define ISPN_HOTROD_TRANSPORT_GENERICKEYEDOBJECTPOOL_H

#include <map>
#include <iterator>

namespace infinispan {
namespace hotrod {
namespace transport {

template <class K, class V> class GenericKeyedObjectPoolFactory;

enum WhenExhaustedAction {
  WHEN_EXHAUSTED_GROW,
  WHEN_EXHAUSTED_FAIL,
  WHEN_EXHAUSTED_BLOCK
};

template <class K, class V> class GenericKeyedObjectPool
{
  public:
    GenericKeyedObjectPool(
      KeyedPoolableObjectFactory<K,V>& factory_,
      int maxActive_,
      WhenExhaustedAction whenExhaustedAction_,
      long maxWait_,
      int maxIdle_,
      int maxTotal_,
      int minIdle_,
      bool testOnBorrow_,
      bool testOnReturn_,
      long timeBetweenEvictionRuns_,
      int numTestsPerEvictionRun_,
      long minEvictableIdleTime_,
      bool testWhileIdle_,
      bool lifo_)
      : factory(factory_), maxIdle(maxIdle_), maxActive(maxActive_),
        maxTotal(maxTotal_), minIdle(minIdle_), maxWait(maxWait_),
        whenExhaustedAction(whenExhaustedAction_), testOnBorrow(testOnBorrow_),
        testOnReturn(testOnReturn_), testWhileIdle(testWhileIdle_),
        timeBetweenEvictionRunsMillis(timeBetweenEvictionRuns_),
        numTestsPerEvictionRun(numTestsPerEvictionRun_),
        minEvictableIdleTimeMillis(minEvictableIdleTime_), lifo(lifo_) { }

    // TODO: ALL IMPLEMENTATION

    int getNumActive() { return 0; }
    int getNumActive(const K& key) { return 0; }
    int getNumIdle() { return 0; }
    int getNumIdle(const K& key) { return 0; }

    int getMaxActive() { return maxActive; }

    //////////

    void returnObject(const K& key, V& val) {
      (void)key;
      (void)val;
    }

    V& borrowObject(const K& key) {
        V* val = poolMap[key];
        if (!val) {
            val = &factory.makeObject(key);
            poolMap[key] = val;
        }
        factory.activateObject(key, *val);
        return *val;
    }

    void invalidateObject(const K& key, V* val) {
        poolMap[key] = NULL;
        factory.destroyObject(key, *val);
    }

    void clear() {
        for(typename std::map<K,V*>::iterator iter = poolMap.begin() ; iter != poolMap.end() ; ++iter) {
            clear(iter->first);
        }
    }

    void clear(const K& key) {
        V* val = poolMap[key];
        if (val) {
            factory.destroyObject(key, *val);
        }
        poolMap[key] = 0;
    }

    void preparePool(const K& key) {
        poolMap[key] = NULL;
    }

    void close() {
        clear();
    }

    void addObject(const K& key) {
        V& val = factory.makeObject(key);
        poolMap[key] = &val;
    }

  private:
    KeyedPoolableObjectFactory<K,V>& factory;

    std::map<K,V*> poolMap;
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

#endif  /* ISPN_HOTROD_TRANSPORT_GENERICKEYEDOBJECTPOOL_H */
