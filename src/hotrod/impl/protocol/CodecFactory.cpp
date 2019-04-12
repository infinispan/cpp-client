

#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/protocol/Codec20.h"
#include "hotrod/impl/protocol/Codec21.h"
#include "hotrod/impl/protocol/Codec22.h"
#include "hotrod/impl/protocol/Codec23.h"
#include "hotrod/impl/protocol/Codec24.h"
#include "hotrod/impl/protocol/Codec25.h"
#include "hotrod/impl/protocol/Codec26.h"
#include "hotrod/impl/protocol/Codec27.h"
#include "hotrod/impl/protocol/Codec28.h"
#include "infinispan/hotrod/Configuration.h"
#include "hotrod/sys/RunOnce.h"

#include <iostream>

namespace infinispan {
namespace hotrod {
namespace protocol {

CodecFactory::CodecFactory() {
    codecMap[Configuration::PROTOCOL_VERSION_20] = new Codec20();
    codecMap[Configuration::PROTOCOL_VERSION_21] = new Codec21();
    codecMap[Configuration::PROTOCOL_VERSION_22] = new Codec22();
    codecMap[Configuration::PROTOCOL_VERSION_23] = new Codec23();
    codecMap[Configuration::PROTOCOL_VERSION_24] = new Codec24();
    codecMap[Configuration::PROTOCOL_VERSION_25] = new Codec25();
    codecMap[Configuration::PROTOCOL_VERSION_26] = new Codec26();
    codecMap[Configuration::PROTOCOL_VERSION_27] = new Codec27();
    codecMap[Configuration::PROTOCOL_VERSION_28] = new Codec28();
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
