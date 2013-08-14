

#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/protocol/Codec12.h"
#include "hotrod/impl/configuration/Configuration.h"

#include <iostream>

namespace infinispan {
namespace hotrod {
namespace protocol {

bool CodecFactory::initialized = false;
std::map<std::string, Codec*> CodecFactory::codecMap;


Codec* CodecFactory::getCodec(const char* version) {
    if(!initialized) {
        codecMap[Configuration::PROTOCOL_VERSION_12] = new Codec12();
        initialized = true;
    }

    Codec* result = CodecFactory::codecMap[version];

    return result;
}

}}} // namespace
