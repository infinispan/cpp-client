

#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/protocol/Codec12.h"
#include "hotrod/impl/configuration/Configuration.h"
#include "hotrod/sys/RunOnce.h"

#include <iostream>

namespace infinispan {
namespace hotrod {
namespace protocol {

using namespace sys;

std::map<std::string, Codec*> CodecFactory::codecMap;


Codec* CodecFactory::getCodec(const char* version) {
    RunOnce initCodecMap(&init);
    initCodecMap.runOnce();

    Codec* result = CodecFactory::codecMap[version];

    return result;
}

void CodecFactory::init() {
    codecMap[Configuration::PROTOCOL_VERSION_12] = new Codec12();
}

}}} // namespace
