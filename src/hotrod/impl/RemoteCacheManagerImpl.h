#ifndef ISPN_HOTROD_REMOTECACHEMANAGERIMPL_H
#define ISPN_HOTROD_REMOTECACHEMANAGERIMPL_H

#include "infinispan/hotrod/Configuration.h"
#include "hotrod/impl/RemoteCacheImpl.h"
#include "hotrod/impl/protocol/Codec.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/operations/PingOperation.h"
#include "hotrod/sys/Mutex.h"
#include "hotrod/impl/event/ClientListenerNotifier.h"

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

	RemoteCacheImpl *createRemoteCache(bool forceReturnValue);
	RemoteCacheImpl *createRemoteCache(const std::string& name, bool forceReturnValue);

	void start();
    void stop();
    bool isStarted();
    const Configuration& getConfiguration();
    ClusterStatus clusterSwitch();
    ClusterStatus clusterSwitch(std::string clusterName);


	ClientListenerNotifier*& getListenerNotifier() {
		return listenerNotifier;
	}

	ClientListenerNotifier*& getListenerNotifier() {
		return listenerNotifier;
	}

  private:
    sys::Mutex lock;
    bool started;
    Configuration configuration;
    protocol::Codec* codec;
    int defaultCacheTopologyId;

    typedef std::pair<portable::counting_ptr<RemoteCacheImpl>, bool> RemoteCacheHolder;
    std::map<std::string, RemoteCacheHolder> cacheName2RemoteCache;

    operations::PingResult ping(RemoteCacheImpl& remoteCache);
    std::shared_ptr<transport::TransportFactory> transportFactory;
    ClientListenerNotifier *listenerNotifier;

    void startRemoteCache(RemoteCacheImpl& remoteCache, bool forceReturnValue);
};

}} // namespace infinispan::hotrod

#endif  /* ISPN_HOTROD_REMOTECACHEMANAGERIMPL_H */
