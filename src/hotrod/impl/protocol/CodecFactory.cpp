

#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/protocol/Codec13.h"
#include "hotrod/impl/protocol/Codec20.h"
#include "hotrod/impl/protocol/Codec21.h"
#include "hotrod/impl/protocol/Codec22.h"
#include "hotrod/impl/protocol/Codec23.h"
#include "hotrod/impl/protocol/Codec24.h"
#include "infinispan/hotrod/Configuration.h"
#include "hotrod/sys/RunOnce.h"

#include <iostream>

namespace infinispan {
namespace hotrod {
namespace protocol {

CodecFactory::CodecFactory() {
    codecMap[Configuration::PROTOCOL_VERSION_10] = new Codec10();
    codecMap[Configuration::PROTOCOL_VERSION_11] = new Codec11();
    codecMap[Configuration::PROTOCOL_VERSION_12] = new Codec12();
    codecMap[Configuration::PROTOCOL_VERSION_13] = new Codec13();
    codecMap[Configuration::PROTOCOL_VERSION_20] = new Codec20();
    codecMap[Configuration::PROTOCOL_VERSION_21] = new Codec21();
    codecMap[Configuration::PROTOCOL_VERSION_22] = new Codec22();
    codecMap[Configuration::PROTOCOL_VERSION_23] = new Codec23();
    codecMap[Configuration::PROTOCOL_VERSION_24] = new Codec24();
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
