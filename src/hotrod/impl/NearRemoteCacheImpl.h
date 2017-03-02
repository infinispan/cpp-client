/*
 * NearRemoteCache.h
 *
 *  Created on: Nov 29, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_NEARREMOTECACHE_H_
#define INCLUDE_INFINISPAN_HOTROD_NEARREMOTECACHE_H_
#include "hotrod/impl/RemoteCacheImpl.h"
#include "hotrod/impl/RemoteCacheManagerImpl.h"
#include "hotrod/impl/CustomClientListener.h"
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <mutex>

namespace infinispan {
namespace hotrod {

class NearRemoteCacheImpl: public RemoteCacheImpl {
public:

    NearRemoteCacheImpl(RemoteCacheManagerImpl& rcm, std::string cacheName,
            const NearCacheConfiguration& conf) :
            RemoteCacheImpl(rcm, cacheName), maxEntries(conf.getMaxEntries()), cl() {
    }

    virtual void *get(RemoteCacheBase& rcb, const void* key) {
        VersionedValue version;
        return getWithVersion(rcb, key, &version);
    }

    virtual void *put(RemoteCacheBase& rcb, const void *key, const void* val,
            uint64_t life, uint64_t idle) {
        std::vector<char> kbuf;
        rcb.baseKeyMarshall(key, kbuf);
        removeElementFromMap(kbuf);
        return RemoteCacheImpl::put(rcb, key, val, life, idle);
    }

    virtual void *replace(RemoteCacheBase& rcb, const void *key,
            const void* val, uint64_t life, uint64_t idle) {
        std::vector<char> kbuf;
        rcb.baseKeyMarshall(key, kbuf);
        removeElementFromMap(kbuf);
        return RemoteCacheImpl::replace(rcb, key, val, life, idle);
    }

    virtual bool replaceWithVersion(RemoteCacheBase& rcb, const void* k,
            const void* v, uint64_t version, uint64_t life, uint64_t idle) {
        bool replaced = RemoteCacheImpl::replaceWithVersion(rcb, k, v, version,
                life, idle);
        if (replaced) {
            std::vector<char> kbuf;
            rcb.baseKeyMarshall(k, kbuf);
            removeElementFromMap(kbuf);
        }
        return replaced;
    }

    virtual void *remove(RemoteCacheBase& rcb, const void* key) {
        std::vector<char> kbuf;
        rcb.baseKeyMarshall(key, kbuf);
        removeElementFromMap(kbuf);
        return RemoteCacheImpl::remove(rcb, key);
    }
    virtual bool removeWithVersion(RemoteCacheBase& rcb, const void* k,
            uint64_t version) {
        bool removed = RemoteCacheImpl::removeWithVersion(rcb, k, version);
        if (removed) {
            std::vector<char> kbuf;
            rcb.baseKeyMarshall(k, kbuf);
            removeElementFromMap(kbuf);
        }
        return removed;
    }
    virtual void *getWithVersion(RemoteCacheBase& rcb, const void *key,
            VersionedValue* version) {
        std::vector<char> kbuf, vbuf;
        rcb.baseKeyMarshall(key, kbuf);
        if (_nearMap.find(kbuf) == _nearMap.end()) {
            void* value = RemoteCacheImpl::getWithVersion(rcb, key, version);
            if (value)
            {
                VersionedValueImpl<std::vector<char> > valueForMap;
                rcb.baseValueMarshall(value, vbuf);
                valueForMap.setValue(vbuf);
                valueForMap.setVersion(version->version);
                addElementToMap(kbuf, valueForMap);
            }
            return value;
        }
        version->version = _nearMap[kbuf].getVersion();
        return rcb.baseValueUnmarshall(_nearMap[kbuf].getValue());
    }
    virtual void clear() {
        RemoteCacheImpl::clear();
        clearMap();
    }
    virtual void init(operations::OperationsFactory* operationsFactory) {
        RemoteCacheImpl::init(operationsFactory);
        startListener();
    }
private:
    unsigned int maxEntries;
    std::map<std::vector<char>, VersionedValueImpl<std::vector<char> > > _nearMap;
    std::deque<std::vector<char> > _nearFifo;
    std::mutex _nearMutex;
    std::vector<std::vector<char> > filterFactoryParams;
    std::vector<std::vector<char> > converterFactoryParams;
    event::CustomClientListener cl;
    void addElementToMap(std::vector<char>& key,
            VersionedValueImpl<std::vector<char>>& value) {
        std::lock_guard<std::mutex> guard(_nearMutex);
        if (maxEntries > 0) {
            _nearFifo.push_back(key);
            if (maxEntries > 0 && _nearMap.size() >= maxEntries) {
                // Remove oldest element
                while (!_nearFifo.empty()
                        && _nearMap.find(_nearFifo.front()) == _nearMap.end()) {
                    _nearFifo.pop_front();
                }
                if (!_nearFifo.empty()) {
                    _nearMap.erase(_nearFifo.front());
                    _nearFifo.pop_front();
                }
            }
        }
        _nearMap[key] = value;
    }

    void removeElementFromMap(std::vector<char>& key) {
        std::lock_guard<std::mutex> guard(_nearMutex);
        auto it = std::find(_nearFifo.begin(), _nearFifo.end(), key);
        if (it != _nearFifo.end()) {
            _nearFifo.erase(it);
            _nearMap.erase(key);
        }
    }

    void clearMap() {
        std::lock_guard<std::mutex> guard(_nearMutex);
        _nearFifo.clear();
        _nearMap.clear();
    }

    void invalidateCache() {
        std::lock_guard<std::mutex> guard(_nearMutex);
        _nearMap.clear();
    }
    void startListener() {
        std::string convStr("___eager-key-value-version-converter");
        cl.converterFactoryName = std::vector<char>(convStr.begin(),
                convStr.end());
        cl.useRawData = true;
        std::function<void(ClientCacheEntryCustomEvent ce)> f =
                [this] (ClientCacheEntryCustomEvent ce) {listener(this->_nearMap, ce);};
        std::function < void() > failOverHandler =
                [this] () {
                    this->invalidateCache();
                };
        cl.add_listener(f);
        this->addClientListener(cl, filterFactoryParams, converterFactoryParams,
                failOverHandler);
    }
    void listener(
            std::map<std::vector<char>, VersionedValueImpl<std::vector<char> > > &map,
            ClientCacheEntryCustomEvent &ce) {
        // bytearray format is <keyLen[1]><key[keyLen]><valueLen[1]><value[valueLen]>
        const std::vector<char> &v = ce.getEventData();
        if (v.size() == 0)
            return;
        unsigned int sizeKey = v[0];
        if (sizeKey == 0)
            return;
        auto i = v.begin() + 1;
        auto f = v.begin() + 1 + sizeKey;
        std::vector<char> key(i, f);
        unsigned int sizeValue;
        std::vector<char> value;
        if (sizeKey + 1 < v.size()) {
            sizeValue = *(v.data() + sizeKey + 1);
            const char* i1 = v.data() + sizeKey + 2;
            const char* f1 = v.data() + sizeKey + 2 + sizeValue;
            value = std::vector<char>(i1, f1);
        }
        else
        {
            // If no value it's an entry removed event
            removeElementFromMap(key);
            return;
        }
        unsigned long version = 0;
        if (v.size() - sizeKey - sizeValue - 2 >= 8) {
            for (unsigned int i = 0; i < 8; i++) {
                version = (version << 8) + v[sizeKey + sizeValue + 2 + i];
            }
        }
        VersionedValueImpl<std::vector<char> > vv;
        vv.setValue(value);
        vv.setVersion(version);
        addElementToMap(key, vv);
    }

};

}
}

#endif /* INCLUDE_INFINISPAN_HOTROD_NEARREMOTECACHE_H_ */
