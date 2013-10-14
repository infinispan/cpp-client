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
    static Codec* getCodec(const char* version);
    static CodecFactory& getInstance();

  private:
    CodecFactory();
    ~CodecFactory();
    std::map<std::string, Codec*> codecMap;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODECFACTORY_H
