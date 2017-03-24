/*
 * AuthMechListOperation.cpp
 *
 *  Created on: Jan 27, 2017
 *      Author: rigazilla
 */

#include <hotrod/impl/operations/AuthMechListOperation.h>

namespace infinispan {
namespace hotrod {
namespace operations {

AuthMechListOperation::AuthMechListOperation(Codec& codec, Transport &transport) :
         HotRodOperation(codec, 0, std::vector<char>(), topology), codec(codec), transport(transport) {
}
AuthMechListOperation::~AuthMechListOperation() {
    // TODO Auto-generated destructor stub
}

std::vector<std::string> AuthMechListOperation::execute() {
    std::vector<std::string> result;

    std::unique_ptr<protocol::HeaderParams> params(writeHeader(transport, AUTH_MECH_LIST_REQUEST));
   transport.flush();

   readHeaderAndValidate(transport, *params);
   int mechCount = transport.readVInt();

   for (int i = 0; i < mechCount; i++) {
      result.push_back(transport.readString());
   }
   return result;
}
} /* namespace operations */
} /* namespace hotrod */
} /* namespace infinispan */
