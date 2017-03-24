/*
 * AuthOperation.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: rigazilla
 */

#include <hotrod/impl/operations/AuthOperation.h>

namespace infinispan {
namespace hotrod {
namespace operations {


AuthOperation::~AuthOperation() {
    // TODO Auto-generated destructor stub
}

AuthOperation::AuthOperation(Codec& codec, Transport &transport,
                        std::vector<char> &saslMechanism, std::vector<char>& response) :
             HotRodOperation(codec, 0, std::vector<char>(), topology), codec(codec)
           , transport(transport),  saslMechanism(saslMechanism), response(response) {
   }

   std::vector<char> AuthOperation::execute() {
       std::unique_ptr<protocol::HeaderParams> params(writeHeader(transport, AUTH_REQUEST));
      transport.writeArray(saslMechanism);
      transport.writeArray(response);
      transport.flush();

      readHeaderAndValidate(transport, *params);
      bool complete = transport.readByte() > 0;
      std::vector<char> c = transport.readArray();
      return complete ? std::vector<char>() : c;
   }
}
}
}

