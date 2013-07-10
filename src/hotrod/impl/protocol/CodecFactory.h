#ifndef ISPN_HOTROD_PROTOCOL_CODECFACTORY_H
#define ISPN_HOTROD_PROTOCOL_CODECFACTORY_H

#include <map>
#include <string>

namespace infinispan {
namespace hotrod {

namespace protocol {
class Codec;

class CodecFactory
{
  public:
    static const Codec* getCodec(const char* version);

  private:
    CodecFactory();
    static std::map<std::string, Codec*> codecMap;
    static bool initialized;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODECFACTORY_H
