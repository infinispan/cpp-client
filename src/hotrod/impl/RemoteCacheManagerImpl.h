#ifndef ISPN_HOTROD_REMOTECACHEMANAGERIMPL_H
#define ISPN_HOTROD_REMOTECACHEMANAGERIMPL_H



#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/Handle.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "hotrod/impl/configuration/Configuration.h"
#include "hotrod/impl/protocol/Codec.h"
#include "hotrod/impl/transport/TransportFactory.h"

#include <map>

namespace infinispan {
namespace hotrod {

class RemoteCacheManagerImpl
{
  public:
    RemoteCacheManagerImpl(bool start = true);
    //RemoteCacheManagerImpl(const Configuration& configuration, bool start = true);
	RemoteCacheManagerImpl(const std::map<std::string,std::string>& properties, bool start = true);

    void start();
    void stop();
    bool isStarted();
    const Configuration& getConfiguration();
    void initCache(RemoteCacheImpl& cache, bool forceReturnValue);

  private:
    transport::TransportFactory* transportFactory;
    // TODO: volatile
    bool started;
    Configuration configuration;
    protocol::Codec* codec;
    // TODO: atomic, initialized to 1
    int64_t topologyId;
};

}} // namespace infinispan::hotrod

#endif  /* ISPN_HOTROD_REMOTECACHEMANAGERIMPL_H */
