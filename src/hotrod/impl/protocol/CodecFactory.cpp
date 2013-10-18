

#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/protocol/Codec12.h"
#include "infinispan/hotrod/Configuration.h"
#include "hotrod/sys/RunOnce.h"

#include <iostream>

namespace infinispan {
namespace hotrod {
namespace protocol {

CodecFactory::CodecFactory() {
    codecMap[Configuration::PROTOCOL_VERSION_12] = new Codec12();
}

CodecFactory::~CodecFactory() {
    for (std::map<std::string, Codec*>::iterator it=codecMap.begin(); it!=codecMap.end(); ++it) {
        delete it->second;
    }
}

CodecFactory& CodecFactory::getInstance() {
    static CodecFactory instance;

    return instance;
}

Codec* CodecFactory::getCodec(const char* version) {
    return CodecFactory::getInstance().codecMap[version];
}

}}} // namespace
