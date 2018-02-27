/*
 * NearCacheConfiguration.h
 *
 *  Created on: Nov 29, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_NEARCACHECONFIGURATION_H_
#define INCLUDE_INFINISPAN_HOTROD_NEARCACHECONFIGURATION_H_


#include "infinispan/hotrod/ImportExport.h"

namespace infinispan {
namespace hotrod {

/**
 * Enumeration of all the near cache mode of operation
 */
enum NearCacheMode {
    DISABLED=0,     /*!< No near cache. */
    INVALIDATED=1   /*!< Near cache in invalidated mode. In invalidated mode an entry in the near cache is create
                     *  only after a get and is removed from the near cache if it's value changes on the server. */
};

/**
 * NearCacheConfiguration the internal representation of the near cache configuration. Applications do not use it directly,
 * instead they build it via a NearCacheConfigurationBuilder
 *
 */

class HR_EXTERN NearCacheConfiguration
{
public:
    NearCacheConfiguration(NearCacheMode mode=DISABLED, int maxEntries=0) : m_mode(mode), m_maxEntries(maxEntries) {}

    unsigned int getMaxEntries() const {
        return m_maxEntries;
    }

    void maxEntries(int maxEntries = 0) {
        this->m_maxEntries = maxEntries;
    }

    NearCacheMode getMode() const {
        return m_mode;
    }

    void mode(NearCacheMode mode = DISABLED) {
        this->m_mode = mode;
    }
private:
    NearCacheMode m_mode=DISABLED;
    unsigned int m_maxEntries=0;
};
}
}


#endif /* INCLUDE_INFINISPAN_HOTROD_NEARCACHECONFIGURATION_H_ */
