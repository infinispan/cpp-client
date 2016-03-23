/*
 * SegmentConsistentHash.cpp
 *
 *  Created on: Feb 25, 2016
 *      Author: rigazilla
 */

#include <hotrod/impl/consistenthash/SegmentConsistentHash.h>

namespace infinispan {
namespace hotrod {
namespace consistenthash {

const infinispan::hotrod::transport::InetSocketAddress& SegmentConsistentHash::getServer(const std::vector<char>& key) {
   int segmentId = getSegment(key);
   return segmentOwners[segmentId][0];
}
uint32_t SegmentConsistentHash::getSegment(const std::vector<char>& key)
{
   return getNormalizedHash(key)/segmentSize;
}

std::map<transport::InetSocketAddress, std::vector<int> > SegmentConsistentHash::getSegmentsByServers() {
	std::map<transport::InetSocketAddress, std::vector<int> > m;
	for (unsigned int i = 0; i< segmentOwners.size(); i++)
	{
		for (unsigned int j = 0; j< segmentOwners[i].size(); j++)
		{
			m[segmentOwners[i][j]].push_back(i);
		}
	}
	return m;
}



} /* namespace consistenthash */
} /* namespace hotrod */
} /* namespace infinispan */

