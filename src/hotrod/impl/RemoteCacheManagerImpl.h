#ifndef ISPN_HOTROD_REMOTECACHEMANAGERIMPL_H
#define ISPN_HOTROD_REMOTECACHEMANAGERIMPL_H

#include <hotrod/impl/event/ClientListenerNotifier.h>
#include "infinispan/hotrod/Configuration.h"
#include "hotrod/impl/RemoteCacheImpl.h"
#include "hotrod/impl/protocol/Codec.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/operations/PingOperation.h"
#include "hotrod/sys/Mutex.h"

#include <map>

using namespace infinispan::hotrod::event;
namespace infinispan {
namespace hotrod {

class RemoteCacheManagerImpl
{
  public:
    RemoteCacheManagerImpl(bool start = true);
    RemoteCacheManagerImpl(const std::map<std::string,std::string>& properties, bool start_); // Deprecated
	RemoteCacheManagerImpl(const Configuration& configuration, bool start = true);

	std::shared_ptr<RemoteCacheImpl> createRemoteCache(bool forceReturnValue, NearCacheConfiguration nc);
	std::shared_ptr<RemoteCacheImpl> createRemoteCache(const std::string& name, bool forceReturnValue, NearCacheConfiguration nc);

	void start();
    void stop();
    bool isStarted();
    const Configuration& getConfiguration();
    bool clusterSwitch();
    bool clusterSwitch(std::string clusterName);

    ClientListenerNotifier& getListenerNotifier() {
		return *listenerNotifier;
	}
  private:
    sys::Mutex lock;
    bool started;
    Configuration configuration;
    protocol::Codec* codec;
    int defaultCacheTopologyId;

    typedef std::pair<std::shared_ptr<RemoteCacheImpl>, bool> RemoteCacheHolder;
    std::map<std::string, RemoteCacheHolder> cacheName2RemoteCache;

    operations::PingResult ping(RemoteCacheImpl& remoteCache);
    std::shared_ptr<transport::TransportFactory> transportFactory;
    std::shared_ptr<ClientListenerNotifier> listenerNotifier;

    void startRemoteCache(RemoteCacheImpl& remoteCache, bool forceReturnValue);
    void stopRemoteCache(RemoteCacheImpl& remoteCache);

};

}} // namespace infinispan::hotrod

#endif  /* ISPN_HOTROD_REMOTECACHEMANAGERIMPL_H */
