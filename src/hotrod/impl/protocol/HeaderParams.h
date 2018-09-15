#ifndef ISPN_HOTROD_PROTOCOL_HEADERPARAMS_H
#define ISPN_HOTROD_PROTOCOL_HEADERPARAMS_H

#include <hotrod/impl/Topology.h>
#include "infinispan/hotrod/Flag.h"
#include <set>
#include <vector>
#include <map>
namespace infinispan {
namespace hotrod {
namespace protocol {

struct MediaType {
    MediaType(const char* typeSubtype = nullptr);
    std::string type;
    std::string subType;
    std::string typeSubtype;
    double weight = 0;
    std::map<std::string, std::string> params;
    std::string toString() const { return typeSubtype; }
};

// Well know media type/subtype
static constexpr const char* APPLICATION_JSON_TYPE = "application/json";
static constexpr const char* APPLICATION_OCTET_STREAM_TYPE = "application/octet-stream";
static constexpr const char* APPLICATION_OBJECT_TYPE = "application/x-java-object";
static constexpr const char* APPLICATION_PDF_TYPE = "application/pdf";
static constexpr const char* APPLICATION_RTF_TYPE = "application/rtf";
static constexpr const char* APPLICATION_SERIALIZED_OBJECT_TYPE = "application/x-java-serialized-object";
static constexpr const char* APPLICATION_XML_TYPE = "application/xml";
static constexpr const char* APPLICATION_ZIP_TYPE = "application/zip";
static constexpr const char* APPLICATION_JBOSS_MARSHALLING_TYPE = "application/x-jboss-marshalling";
static constexpr const char* APPLICATION_PROTOSTREAM_TYPE = "application/x-protostream";
static constexpr const char* APPLICATION_UNKNOWN_TYPE = "application/unknown";
static constexpr const char* WWW_FORM_URLENCODED_TYPE = "application/x-www-form-urlencoded";
static constexpr const char* IMAGE_GIF_TYPE = "image/gif";
static constexpr const char* IMAGE_JPEG_TYPE = "image/jpeg";
static constexpr const char* IMAGE_PNG_TYPE = "image/png";
static constexpr const char* TEXT_CSS_TYPE = "text/css";
static constexpr const char* TEXT_CSV_TYPE = "text/csv";
static constexpr const char* TEXT_PLAIN_TYPE = "text/plain";
static constexpr const char* TEXT_HTML_TYPE = "text/html";
static constexpr const char* APPLICATION_INFINISPAN_MARSHALLING_TYPE = "application/x-infinispan-marshalling";
static constexpr const char* APPLICATION_INFINISPAN_BINARY_TYPE = "application/x-infinispan-binary";
static constexpr const char* APPLICATION_PROTOSTUFF_TYPE = "application/x-protostuff";
static constexpr const char* APPLICATION_KRYO_TYPE = "application/x-kryo";

struct IdsMediaTypes {
    static std::map<std::string, int> typesIds;
    static int getByMediaType(std::string mediaType);
    static std::string getById(int id);
};

struct DataFormat {
    MediaType keyMediaType;
    MediaType valueMediaType;
};

class HeaderParams
{
  public:
	HeaderParams(Topology& topId);
    HeaderParams& setOpCode(uint8_t opCode);
    HeaderParams& setCacheName(const std::vector<char>& cacheName);
    HeaderParams& setFlags(uint32_t flags);
    HeaderParams& setClientIntel(uint8_t clientIntel);
    HeaderParams& setTxMarker(uint8_t txMarker);
    HeaderParams& setMessageId(uint64_t messageId);
    uint64_t getMessageId();
    HeaderParams& setTopologyAge(int topologyAge_);
    DataFormat dataFormat;


  private:
    uint8_t toOpRespCode(uint8_t opCode);

    uint8_t opCode;
    uint8_t opRespCode;
    std::vector<char> cacheName;
    uint32_t flags;
    uint8_t clientIntel;
    uint8_t txMarker;
    Topology& topologyId;
    uint64_t messageId;
    int topologyAge;

    friend class Codec10;
    friend class Codec11;
    friend class Codec12;
    friend class Codec13;
    friend class Codec20;
    friend class Codec21;
    friend class Codec22;
    friend class Codec23;
    friend class Codec24;
    friend class Codec28;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_HEADERPARAMS_H
