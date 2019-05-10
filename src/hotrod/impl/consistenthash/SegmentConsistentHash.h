/*
 * SegmentConsistentHash.h
 *
 *  Created on: Feb 25, 2016
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_CONSISTENTHASH_SEGMENTCONSISTENTHASH_H_
#define SRC_HOTROD_IMPL_CONSISTENTHASH_SEGMENTCONSISTENTHASH_H_


#include <hotrod/impl/consistenthash/ConsistentHash.h>
#include "hotrod/impl/hash/MurmurHash3.h"
#include "infinispan/hotrod/exceptions.h"
#include "hotrod/sys/Log.h"


namespace infinispan {
namespace hotrod {
namespace consistenthash {

class SegmentConsistentHash: public ConsistentHash {
public:
	SegmentConsistentHash() : segmentOwners(), numSegments(0), segmentSize(0) {};
	virtual ~SegmentConsistentHash() {};
	void init(std::vector<std::vector<transport::InetSocketAddress>> _segmentOwners, uint32_t _numSegments) {
	      segmentOwners = _segmentOwners;
	      numSegments = _numSegments;
	      segmentSize = getSegmentSize(_numSegments);
	   }

    void init(std::map<transport::InetSocketAddress, std::set<int32_t> > & /*servers2Hash*/,
				int32_t /*numKeyOwners*/, int32_t /*hashSpace*/) { throw UnsupportedOperationException(); }

    const infinispan::hotrod::transport::InetSocketAddress& getServer(const std::vector<char>& key);

    int32_t getNormalizedHash(int32_t objectId) { return hash(objectId) & 0x7fffffff; }

    int32_t getNormalizedHash(const std::vector<char>& key) { return hash(key.data(),key.size()) & 0x7fffffff; }

private:

    static uint32_t hash(const void *key, size_t size) {
    	return MurmurHash3::hash(key, size);
    }
    static uint32_t hash(int32_t key) {
    	return MurmurHash3::hash(key);
    }

    uint32_t getSegmentSize(uint32_t num) {
	  return (uint32_t)(0x7FFFFFFFUL/num)+1;
    }

    uint32_t getSegment(const std::vector<char>& key);

    virtual std::map<transport::InetSocketAddress, std::vector<int> > getSegmentsByServers();

    std::vector<std::vector<transport::InetSocketAddress>> segmentOwners;

    uint32_t numSegments;
    uint32_t segmentSize;
};


} /* namespace consistenthash*/
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_CONSISTENTHASH_SEGMENTCONSISTENTHASH_H_ */
