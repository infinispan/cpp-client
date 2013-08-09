#ifndef ISPN_HOTROD_TRANSPORT_PROPSKEYEDOBJECTPOOL_H
#define ISPN_HOTROD_TRANSPORT_PROPSKEYEDOBJECTPOOL_H



#include "hotrod/impl/transport/tcp/KeyedPoolableObjectFactory.h"
#include "hotrod/impl/transport/tcp/GenericKeyedObjectPoolFactory.h"
#include "hotrod/impl/configuration/ConnectionPoolConfiguration.h"

namespace infinispan {
namespace hotrod {
namespace transport {

template <class K, class V> class PropsKeyedObjectPoolFactory :
  public GenericKeyedObjectPoolFactory<K,V>
{
  public:
	PropsKeyedObjectPoolFactory(
      KeyedPoolableObjectFactory<K,V>& factory_,
      const ConnectionPoolConfiguration& configuration)
      : GenericKeyedObjectPoolFactory<K,V>(
        factory_,
        configuration.getMaxActive(),
        mapExhaustedAction(configuration.getExhaustedAction()),
        configuration.getMaxWait(),
        configuration.getMaxIdle(),
        configuration.getMaxTotal(),
        configuration.getMinIdle(),
        configuration.isTestOnBorrow(),
        configuration.isTestOnReturn(),
        configuration.isTestWhileIdle(),
        configuration.getTimeBetweenEvictionRuns(),
        configuration.getNumTestsPerEvictionRun(),
        configuration.getMinEvictableIdleTime(),
        configuration.isLifo())
    { }

  private:
	WhenExhaustedAction mapExhaustedAction(ExhaustedAction exhaustedAction) {
      switch(exhaustedAction) {
      case CREATE_NEW:
    	  return WHEN_EXHAUSTED_GROW;
      case EXCEPTION:
    	  return WHEN_EXHAUSTED_FAIL;
      case WAIT:
      default:
    	  return WHEN_EXHAUSTED_BLOCK;
      }
	}
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_PROPSKEYEDOBJECTPOOL_H */
