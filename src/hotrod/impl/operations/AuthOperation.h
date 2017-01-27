/*
 * AuthOperation.h
 *
 *  Created on: Jan 26, 2017
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_OPERATIONS_AUTHOPERATION_H_
#define SRC_HOTROD_IMPL_OPERATIONS_AUTHOPERATION_H_

#include "hotrod/impl/operations/HotRodOperation.h"
#include "hotrod/impl/transport/Transport.h"
namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::transport;
using namespace infinispan::hotrod::protocol;

class AuthOperation: HotRodOperation<std::vector<char> > {
public:
    AuthOperation();
    AuthOperation(Codec& codec, Transport &transport, std::vector<char> &saslMechanism, std::vector<char>& response);
    virtual std::vector<char> execute();
    virtual ~AuthOperation();
private:
    Codec& codec;
    Transport& transport;
    std::vector<char>& saslMechanism;
    std::vector<char>& response;
    Topology topology;

};
}
}
}

#endif /* SRC_HOTROD_IMPL_OPERATIONS_AUTHOPERATION_H_ */
