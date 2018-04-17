/*
 * RemoteCacheManagerAdmin.h
 *
 *  Created on: Apr 10, 2018
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_REMOTECACHEMANAGERADMIN_H_
#define INCLUDE_INFINISPAN_HOTROD_REMOTECACHEMANAGERADMIN_H_

#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include <vector>
#include <set>

namespace infinispan {
namespace hotrod {
namespace transport {
class TransportFactory;
}

namespace operations {
class OperationsFactory;
}

namespace protocol {
class Codec;
}

using namespace transport;
using namespace operations;
using namespace protocol;

enum AdminFlag {
    PERMANENT=0
};

class HR_EXTERN RemoteCacheManagerAdmin {
public:
    RemoteCacheManagerAdmin(RemoteCacheManager& cacheManager, std::function<void(std::string&)> remover, std::shared_ptr<OperationsFactory> of);
    /**
     * Creates a cache on the remote server cluster using the specified template.
     *
     * @param name the name of the cache to create
     * @param model the template to use for the cache. If null, the configuration marked as default on the server
     *                 will be used
     * @return the cache
     */
    template <class K, class V> RemoteCache<K, V>& createCache(const std::string name, std::string model) {
        createCache(name, model, "@@cache@create");
        return cacheManager.getCache<K,V>(name);
    }

    /**
     * Creates a cache on the remote server cluster using the specified configuration
     *
     * @param name the name of the cache to create
     * @param conf the configuration in an XML format respecting the schema accepted by the server
     * @return the cache
     */
    template <class K, class V> RemoteCache<K, V>& createCacheWithXml(const std::string name, std::string conf) {
        createCacheWithXml(name, conf, "@@cache@create");
        return cacheManager.getCache<K,V>(name);

    }

    /**
     * Retrieves a an existing cache on the remote server cluster. If it doesn't exist, it will be created using the
     * specified template.
     *
     * @param name the name of the cache to create
     * @param model the template to use for the cache. If null, the configuration marked as default on the server
     *                 will be used
     * @return the cache
     */
    template <class K, class V> RemoteCache<K, V>& getOrCreateCache(const std::string name, std::string model) {
        createCache(name, model, "@@cache@getorcreate");
        return cacheManager.getCache<K,V>(name);
    }

    /**
     * Retrieves a an existing cache on the remote server cluster. If it doesn't exist, it will be created using the
     * specified configuration.
     *
     * @param name the name of the cache to create
     * @param conf a concrete configuration that will be sent to the server. The XML representation of this
     *                      configuration must use the Infinispan embedded configuration schema in a version supported
     *                      by the server.
     * @return the cache
     * @throws HotRodClientException
     */
    template <class K, class V> RemoteCache<K, V>& getOrCreateCacheWithXml(const std::string name, std::string conf) {
        createCacheWithXml(name, conf, "@@cache@getorcreate");
        return cacheManager.getCache<K,V>(name);
    }

    /**
     * Removes a cache from the remote server cluster.
     *
     * @param name the name of the cache to remove
     */
    void removeCache(std::string name);

    /**
     * Performs a mass reindexing of the specified cache. The command will return immediately and the reindexing will
     * be performed asynchronously
     * @param name the name of the cache to reindex
     * @throws HotRodClientException
     */
    void reindexCache(std::string name);

    RemoteCacheManagerAdmin& withFlags(std::set<AdminFlag> flags);
    void createCache(const std::string name, std::string model, std::string command);
    void createCacheWithXml(const std::string name, std::string conf, std::string command);

private:
    std::string flags2Params(const std::set<AdminFlag>& flags) const;

    void executeAdminOperation(std::string adminCmd, std::map<std::string, std::string >& param);

    static const std::vector<char> CACHE_NAME;
    static const std::vector<char> CACHE_TEMPLATE;
    static const std::vector<char> CACHE_CONFIGURATION;
    static const std::vector<char> CACHE_FLAGS;
    static const std::string FLAG_LABELS[];
    std::shared_ptr<OperationsFactory> operationsFactory;
    std::function<void(std::string&)> remover;

    RemoteCacheManager& cacheManager;
#ifdef SWIGJAVA           // Swig needs public access to flags
public:
#endif
    std::set<AdminFlag> flags;
};

}} // namespace infinispan::hotrod



#endif /* INCLUDE_INFINISPAN_HOTROD_REMOTECACHEMANAGERADMIN_H_ */
