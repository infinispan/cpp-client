#ifndef ISPN_HOTROD_REMOTECACHEBASE_H
#define ISPN_HOTROD_REMOTECACHEBASE_H



#include <infinispan/hotrod/CacheTopologyInfo.h>
#include "infinispan/hotrod/ClientListener.h"
#include "infinispan/hotrod/ContinuousQueryListener.h"
#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/Flag.h"
#include "infinispan/hotrod/MetadataValue.h"
#include "infinispan/hotrod/Query.h"
#include "infinispan/hotrod/QueryUtils.h"
#include "infinispan/hotrod/BasicTypesProtoStreamMarshaller.h"
#include <map>
#include <set>
#include <vector>

using namespace org::infinispan::query::remote::client;
using namespace infinispan::hotrod::event;

namespace infinispan {
namespace hotrod {

namespace operations {
class OperationsFactory;
}

namespace event {
template <class K, class V> class CacheClientListener;
#ifndef SWIGCSHARP
template <typename... Params> class ContinuousQueryListener;
#endif
}
typedef void (*MarshallHelperFn) (void*, const void*, std::vector<char> &);
typedef void* (*UnmarshallHelperFn) (void*, const std::vector<char> &);

class KeyUnmarshallerFtor;
class ValueUnmarshallerFtor;
class RemoteCacheImpl;

class RemoteCacheBase
{
public:
    virtual ~RemoteCacheBase() {}
protected:
    HR_EXTERN const char *base_getName();
    HR_EXTERN void *base_get(const void *key);
    HR_EXTERN void *base_put(const void *key, const void *value, int64_t life, int64_t idle);
    HR_EXTERN void *base_putIfAbsent(const void *key, const void *value, int64_t life, int64_t idle);
    HR_EXTERN void *base_replace(const void *key, const void *value, int64_t life, int64_t idle);
    HR_EXTERN void *base_remove(const void *key);
    HR_EXTERN bool  base_containsKey(const void *key);
    HR_EXTERN void  base_ping();
    HR_EXTERN bool  base_replaceWithVersion(const void *key, const void *value, int64_t version, int64_t life, int64_t idle);
    HR_EXTERN bool  base_removeWithVersion(const void *key, int64_t version);
    HR_EXTERN void *base_getWithVersion(const void* key, VersionedValue* version);
    HR_EXTERN void *base_getWithMetadata(const void* key, MetadataValue* metadata);
    HR_EXTERN void  base_getBulk(int size, std::map<void*, void*> &mbuf);
    HR_EXTERN void  base_keySet(int scope, std::vector<void*> &sbuf);
    HR_EXTERN void  base_stats(std::map<std::string,std::string> &sbuf);
    HR_EXTERN void  base_clear();
    HR_EXTERN uint64_t  base_size();
    HR_EXTERN void  base_withFlags(Flag flag);
    HR_EXTERN std::vector<unsigned char> base_execute(const std::string &cmdName,  const std::map<std::string,std::string>& args);
    HR_EXTERN CacheTopologyInfo base_getCacheTopologyInfo();
    HR_EXTERN QueryResponse base_query(const QueryRequest &qr);
    HR_EXTERN std::vector<unsigned char> base_query_char(std::vector<unsigned char> qr, size_t size);

	HR_EXTERN void base_addClientListener(ClientListener &clientListener, const std::vector<std::vector<char> > filterFactoryParam, const std::vector<std::vector<char> > converterFactoryParams, const std::function<void()> &recoveryCallback);
	HR_EXTERN void base_removeClientListener(ClientListener &clientListener);

    RemoteCacheBase() {}
    HR_EXTERN void setMarshallers(void* rc, MarshallHelperFn kf, MarshallHelperFn vf, UnmarshallHelperFn ukf, UnmarshallHelperFn uvf);

#ifndef SWIGCSHARP
    template<class K, class V, typename... Params>
	void base_addContinuousQueryListener(ContinuousQueryListener<K, V, Params...>& cql) {
		static char CONTINUOUS_QUERY_FILTER_FACTORY_NAME[] =
				"continuous-query-filter-converter-factory";
		cql.cl.filterFactoryName = std::vector<char>(
				CONTINUOUS_QUERY_FILTER_FACTORY_NAME,
				CONTINUOUS_QUERY_FILTER_FACTORY_NAME
						+ strlen(CONTINUOUS_QUERY_FILTER_FACTORY_NAME));
		cql.cl.converterFactoryName = std::vector<char>(
				CONTINUOUS_QUERY_FILTER_FACTORY_NAME,
				CONTINUOUS_QUERY_FILTER_FACTORY_NAME
						+ strlen(CONTINUOUS_QUERY_FILTER_FACTORY_NAME));
		BasicTypesProtoStreamMarshaller<std::string> paramMarshaller;
		std::vector<std::vector<char> > filterFactoryParams;
		std::vector<char> param;
		paramMarshaller.marshall(cql.getQuery(), param);
		filterFactoryParams.push_back(param);
		std::vector<std::vector<char> > converterFactoryParams;
		cql.cl.useRawData = true;
		cql.listenerCustomEvent =
				[this, &cql](ClientCacheEntryCustomEvent e) {
					ContinuousQueryResult r;
					WrappedMessage wm;
					wm.ParseFromArray(e.getEventData().data(), e.getEventData().size());
					r.ParseFromString(wm.wrappedmessagebytes());
					auto resultType = r.resulttype();
					auto i=0;
					std::tuple<Params...> tp= popTuple<Params...>(r.projection(), i) ;
					K* k = (K*)this->baseKeyUnmarshall(std::vector<char>(r.key().begin(), r.key().end()));
					switch (resultType) {
						case ContinuousQueryResult_ResultType_JOINING:
						cql.getJoiningListener()(*k, tp);
						break;
						case ContinuousQueryResult_ResultType_LEAVING:
						cql.getLeavingListener()(*k, tp);
						break;
						case ContinuousQueryResult_ResultType_UPDATED:
						cql.getUpdatedListener()(*k, tp);
						break;
						default:
						//uIgnore unknown types
						break;
					}
				};
		cql.cl.add_listener(cql.listenerCustomEvent);
		this->base_addClientListener(cql.cl, filterFactoryParams,
				converterFactoryParams, cql.getFailoverListener());
    }
    template <class K, class V>
	void base_addContinuousQueryListener(ContinuousQueryListener<K, V>& cql) {
		static char CONTINUOUS_QUERY_FILTER_FACTORY_NAME[] =
				"continuous-query-filter-converter-factory";
		cql.cl.filterFactoryName = std::vector<char>(
				CONTINUOUS_QUERY_FILTER_FACTORY_NAME,
				CONTINUOUS_QUERY_FILTER_FACTORY_NAME
						+ strlen(CONTINUOUS_QUERY_FILTER_FACTORY_NAME));
		cql.cl.converterFactoryName = std::vector<char>(
				CONTINUOUS_QUERY_FILTER_FACTORY_NAME,
				CONTINUOUS_QUERY_FILTER_FACTORY_NAME
						+ strlen(CONTINUOUS_QUERY_FILTER_FACTORY_NAME));
		BasicTypesProtoStreamMarshaller<std::string> paramMarshaller;
		std::vector<std::vector<char> > filterFactoryParams;
		std::vector<char> param;
		paramMarshaller.marshall(cql.getQuery(), param);
		filterFactoryParams.push_back(param);
		std::vector<std::vector<char> > converterFactoryParams;
		cql.cl.useRawData = true;
		cql.listenerCustomEvent =
				[this, &cql](ClientCacheEntryCustomEvent e) {
					ContinuousQueryResult r;
					WrappedMessage wm;
					wm.ParseFromArray(e.getEventData().data(), e.getEventData().size());
					r.ParseFromString(wm.wrappedmessagebytes());
					auto resultType = r.resulttype();
					K* k = (K*)this->baseKeyUnmarshall(std::vector<char>(r.key().begin(), r.key().end()));
					V* v = (V*)this->baseValueUnmarshall(std::vector<char>(r.value().begin(), r.value().end()));
					switch (resultType) {
						case ContinuousQueryResult_ResultType_JOINING:
						cql.getJoiningListener()(*k, *v);
						break;
						case ContinuousQueryResult_ResultType_LEAVING:
						cql.getLeavingListener()(*k, *v);
						break;
						case ContinuousQueryResult_ResultType_UPDATED:
						cql.getUpdatedListener()(*k, *v);
						break;
						default:
						//uIgnore unknown types
						break;
					}
				};
		cql.cl.add_listener(cql.listenerCustomEvent);
		this->base_addClientListener(cql.cl, filterFactoryParams,
				converterFactoryParams, cql.getFailoverListener());
	}
#endif


private:
    std::shared_ptr<RemoteCacheImpl> impl; // pointer to RemoteCacheImpl;
    void *remoteCachePtr=nullptr; // TODO: pointer to self, is it really necessary?
    MarshallHelperFn baseKeyMarshallFn;
    MarshallHelperFn baseValueMarshallFn;
    HR_EXTERN void baseKeyMarshall(const void* k, std::vector<char> &buf);
    HR_EXTERN void baseValueMarshall(const void* v, std::vector<char> &buf);

    UnmarshallHelperFn baseKeyUnmarshallFn;
    UnmarshallHelperFn baseValueUnmarshallFn;
    HR_EXTERN void* baseKeyUnmarshall(const std::vector<char> &buf);
    HR_EXTERN void* baseValueUnmarshall(const std::vector<char> &buf);

friend class RemoteCacheManager;
friend class RemoteCacheImpl;
friend class NearRemoteCacheImpl;
friend class KeyUnmarshallerFtor;
friend class ValueUnmarshallerFtor;
template <class K, class V>
friend class ::infinispan::hotrod::event::CacheClientListener;
#ifndef SWIGCSHARP
template <typename... Params>
friend class ::infinispan::hotrod::event::ContinuousQueryListener;
#endif
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHEBASE_H */
