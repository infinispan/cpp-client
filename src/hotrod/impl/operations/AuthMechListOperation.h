/*
 * AuthMechListOperation.h
 *
 *  Created on: Jan 27, 2017
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_OPERATIONS_AUTHMECHLISTOPERATION_H_
#define SRC_HOTROD_IMPL_OPERATIONS_AUTHMECHLISTOPERATION_H_

#include "hotrod/impl/operations/HotRodOperation.h"
#include "hotrod/impl/transport/Transport.h"
#include <vector>
namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::transport;
using namespace infinispan::hotrod::protocol;

class AuthMechListOperation : HotRodOperation<std::vector<std::string> > {
public:
    AuthMechListOperation(Codec& codec, Transport &transport);
    virtual std::vector<std::string> execute();
    virtual ~AuthMechListOperation();

private:
    Codec& codec;
    Transport& transport;
    Topology topology;
};

} /* namespace operations */
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_OPERATIONS_AUTHMECHLISTOPERATION_H_ */
