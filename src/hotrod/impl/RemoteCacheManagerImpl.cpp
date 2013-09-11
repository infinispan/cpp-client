

#include "hotrod/impl/RemoteCacheManagerImpl.h"
#include "hotrod/impl/RemoteCacheImpl.h"
#include "hotrod/impl/configuration/ConfigurationBuilder.h"
#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/operations/OperationsFactory.h"

namespace infinispan {
namespace hotrod {

//using namespace configuration;
using namespace protocol;
using namespace transport;
using namespace operations;

const std::string ISPN_CLIENT_HOTROD_SERVER_LIST("infinispan.client.hotrod.server_list");

RemoteCacheManagerImpl::RemoteCacheManagerImpl(bool start_)
  : transportFactory(0), started(false),
    configuration(ConfigurationBuilder().build()), codec(0), topologyId(0)
{
	if (start_) start();
}

RemoteCacheManagerImpl::RemoteCacheManagerImpl(const std::map<std::string,std::string>& properties, bool start_)
  : transportFactory(0), started(false),
    configuration(ConfigurationBuilder().build()), codec(0), topologyId(0)
{
  std::map<std::string,std::string>::const_iterator server_prop;

  server_prop = properties.find(ISPN_CLIENT_HOTROD_SERVER_LIST);
  if(server_prop != properties.end()) {
      std::string serverList = server_prop->second;
      configuration = ConfigurationBuilder().addServers(serverList).build();
  }

  if (start_) start();
}

void RemoteCacheManagerImpl::start() {
	codec = CodecFactory::getCodec(configuration.getProtocolVersion().c_str());
    if (!isStarted()) {
        transportFactory = TransportFactory::newInstance();
        transportFactory->start(*codec, configuration, topologyId);

        started = true;
	}
}

void RemoteCacheManagerImpl::stop() {
    if (isStarted()) {
        transportFactory->destroy();
        delete transportFactory;
        transportFactory = NULL;

        started = false;
    }
}

bool RemoteCacheManagerImpl::isStarted() {
    return started;
}

const Configuration& RemoteCacheManagerImpl::getConfiguration() {
    return configuration;
}

void RemoteCacheManagerImpl::initCache(
    RemoteCacheImpl& cache,  bool forceReturnValue)
{
    OperationsFactory* operationsFactory = new OperationsFactory(
        transportFactory,
        cache.getName(),
        topologyId,
        forceReturnValue,
        *CodecFactory::getCodec(configuration.getProtocolVersion().c_str()));

	cache.init(cache.getName(), operationsFactory);
}

}} // namespace infinispan::hotrod
