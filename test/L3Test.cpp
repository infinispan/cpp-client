#include "hotrod/impl/consistenthash/ConsistentHashFactory.h"
#include "hotrod/impl/consistenthash/ConsistentHash.h"
#include "hotrod/impl/consistenthash/ConsistentHashV1.h"
#include "hotrod/impl/consistenthash/ConsistentHashV2.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <assert.h>

volatile int passFail = 0;
volatile int available = 0;

using namespace infinispan::hotrod;
using infinispan::hotrod::protocol::HotRodConstants;
using namespace infinispan::hotrod::consistenthash;
using namespace infinispan::hotrod::transport;

void consistentHashFactoryTest(uint32_t hashVersion) {
    ConsistentHashFactory* hashFactory = new ConsistentHashFactory();
    ConsistentHash* hash = hashFactory->newConsistentHash(hashVersion);
    hrbytes data(const_cast<char*> ("acbde"), 5);
    uint32_t i1 = hash->getNormalizedHash(data);
    uint32_t i2 = hash->getNormalizedHash(data);
    uint32_t i3 = hash->getNormalizedHash(data);

    assert(i1 == i2);
    assert(i3 == i2);
    assert(i1 == i3);
    std::cout << "consistentHashFactoryTest for hash version " << hashVersion << " ok " << std::endl;

    delete hashFactory;
}

std::map<InetSocketAddress, std::set<int32_t> > createServer2HashMap(){
    std::ostringstream oss;
    std::map<InetSocketAddress, std::set<int32_t> > map;
    uint32_t clusterSize = 10;
    for (uint32_t i = 0; i < clusterSize; i++) {
        oss << i;
        std::string host = "host" + oss.str();
        int16_t port = i;
        int32_t hashCode = i;
        //std::cout << host << ":" << port << std::endl;
        InetSocketAddress address(host, port);
        if (address.getAddress().compare("host01") == 0) {
            InetSocketAddress duplicate("host012", 2);
            address = duplicate;
        }

        std::map<InetSocketAddress, std::set<int32_t> >::iterator it =
                map.find(address);
        if (it == map.end()) {
            std::set<int32_t> hashes;
            hashes.insert(hashCode);
            map.insert(
                    std::pair<InetSocketAddress, std::set<int32_t> >(address,
                            hashes));
        } else {
            hashCode = 12345; //some totally different hash for duplicate
            it->second.insert(hashCode);
        }
    }
    return map;
}

void consistentHashInitTest(uint32_t hashVersion, std::string* host) {
    std::map<InetSocketAddress, std::set<int32_t> > servers2Hash = createServer2HashMap();
    int32_t nKeyOwners = 10;
    int32_t hSpace = 100000;

    ConsistentHashFactory* hashFactory = new ConsistentHashFactory();
    ConsistentHash* hash = hashFactory->newConsistentHash(hashVersion);

    //init hash with server map
    hash->init(servers2Hash, nKeyOwners, hSpace);

    //lookup server
    std::vector<char> hostCopy(host->size()+1);
    /* warning free approach to get char* from string* */
    std::copy(host->begin(),host->end(), hostCopy.begin());
    InetSocketAddress address = hash->getServer(hrbytes(&hostCopy[0], host->length()));

    std::cout << "consistentHash for hash version " << hashVersion << " returned " << address.getAddress() << std::endl;
    delete hashFactory;
}

int main(int, char**) {
    consistentHashFactoryTest(HotRodConstants::CONSISTENT_HASH_V1);
    consistentHashFactoryTest(HotRodConstants::CONSISTENT_HASH_V2);

    std::string* h = new std::string("host01234:4");
    consistentHashInitTest(HotRodConstants::CONSISTENT_HASH_V1, h);
    consistentHashInitTest(HotRodConstants::CONSISTENT_HASH_V2, h);
    delete h;

    h = new std::string("host0123456:6");
    consistentHashInitTest(HotRodConstants::CONSISTENT_HASH_V1, h);
    consistentHashInitTest(HotRodConstants::CONSISTENT_HASH_V2, h);
    delete h;
    return passFail;
}
