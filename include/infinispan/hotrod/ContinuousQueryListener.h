/*
 * ContinuousQueryListener.h
 *
 *  Created on: Aug 3, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_CONTINUOUSQUERYLISTENER_H_
#define INCLUDE_INFINISPAN_HOTROD_CONTINUOUSQUERYLISTENER_H_

#include "infinispan/hotrod/ClientEvent.h"
#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/ClientListener.h"
#include "infinispan/hotrod/RemoteCacheBase.h"
#include <tuple>
#include <vector>
#include <list>
#include <functional>

using namespace infinispan::hotrod;
using namespace infinispan::hotrod::event;

namespace infinispan {
namespace hotrod {

namespace protocol {
class Codec20;
}
template <class K, class V>
class RemoteCache;

namespace event {
template <class K, class V> class CacheClientListener;
template <typename... Params> class ContinuousQueryListener;

template <class K, class V>
class ContinuousQueryListener<K,V>
{
public:
	ContinuousQueryListener(RemoteCache<K,V>& rc, const std::string& query)
			: cl(rc), query(query) {}
	const std::function<void(K, V)>& getJoiningListener() const {
		return joiningListener;
	}

	void setJoiningListener(const std::function<void(K, V)>& joiningListener) {
		this->joiningListener = joiningListener;
	}

	const std::function<void(K, V)>& getLeavingListener() const {
		return leavingListener;
	}

	void setLeavingListener(const std::function<void(K, V)>& leavingListener) {
		this->leavingListener = leavingListener;
	}

	const std::function<void(K, V)>& getUpdatedListener() const {
		return updatedListener;
	}

	void setUpdatedListener(const std::function<void(K, V)>& updatedListener) {
		this->updatedListener = updatedListener;
	}

	const std::function<void()>& getFailoverListener() const {
		return failoverListener;
	}

	void setFailoverListener(const std::function<void()>& failoverListener) {
		this->failoverListener = failoverListener;
	}

	const std::string& getQuery() const {
		return query;
	}
	std::function<void(ClientCacheEntryCustomEvent)> listenerCustomEvent;
	CacheClientListener<K,V> cl;

private:
	std::string query;
    std::function<void(K, V)> joiningListener;
    std::function<void(K, V)> leavingListener;
    std::function<void(K, V)> updatedListener;
    std::function<void()> failoverListener;
};

template <class K, class V, typename... Params>
class ContinuousQueryListener<K, V, Params...>
{
public:
	ContinuousQueryListener(RemoteCache<K,V>& rc, const std::string& query)
			: cl(rc), query(query) {}

	const std::function<void(K, std::tuple<Params...>)>& getJoiningListener() const {
		return joiningListener;
	}

	void setJoiningListener(const std::function<void(K, std::tuple<Params...>)>& joiningListener) {
		this->joiningListener = joiningListener;
	}

	const std::function<void(K, std::tuple<Params...>)>& getLeavingListener() const {
		return leavingListener;
	}

	void setLeavingListener(const std::function<void(K, std::tuple<Params...>)>& leavingListener) {
		this->leavingListener = leavingListener;
	}

	const std::function<void(K, std::tuple<Params...>)>& getUpdatedListener() const {
		return updatedListener;
	}

	void setUpdatedListener(const std::function<void(K, std::tuple<Params...>)>& updatedListener) {
		this->updatedListener = updatedListener;
	}

	const std::function<void()>& getFailoverListener() const {
		return failoverListener;
	}

	void setFailoverListener(const std::function<void()>& failoverListener) {
		this->failoverListener = failoverListener;
	}

	const std::string& getQuery() const {
		return query;
	}
	std::function<void(ClientCacheEntryCustomEvent)> listenerCustomEvent;
	CacheClientListener<K, V> cl;

private:
	std::string query;
	std::function<void(K, std::tuple<Params...>)> joiningListener;
    std::function<void(K, std::tuple<Params...>)> leavingListener;
    std::function<void(K, std::tuple<Params...>)> updatedListener;
    std::function<void()> failoverListener;
};
}}}


#endif /* INCLUDE_INFINISPAN_HOTROD_CONTINUOUSQUERYLISTENER_H_ */
