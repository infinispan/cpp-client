/*
 * DataFormat.h
 *
 *  Created on: Sep 6, 2018
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_DATAFORMAT_H_
#define INCLUDE_INFINISPAN_HOTROD_DATAFORMAT_H_

#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/Marshaller.h"

#include <map>
#include <vector>
#include <memory>

namespace infinispan {
namespace hotrod {

// A data structure for media type as in MIME type
// only the typeSubtype field is in use at the moment

struct MediaType {
	HR_EXTERN MediaType(const char* typeSubtype = nullptr);
	std::string type;
	std::string subType;
	std::string typeSubtype;
	double weight = 0;
	std::map<std::string, std::string> params;
	std::string toString() const {
		return typeSubtype;
	}
};

// Well know media type. This table is used by the hotrod protocol
static constexpr const char* APPLICATION_JSON_TYPE = "application/json";
static constexpr const char* APPLICATION_OCTET_STREAM_TYPE =
		"application/octet-stream";
static constexpr const char* APPLICATION_OBJECT_TYPE =
		"application/x-java-object";
static constexpr const char* APPLICATION_PDF_TYPE = "application/pdf";
static constexpr const char* APPLICATION_RTF_TYPE = "application/rtf";
static constexpr const char* APPLICATION_SERIALIZED_OBJECT_TYPE =
		"application/x-java-serialized-object";
static constexpr const char* APPLICATION_XML_TYPE = "application/xml";
static constexpr const char* APPLICATION_ZIP_TYPE = "application/zip";
static constexpr const char* APPLICATION_JBOSS_MARSHALLING_TYPE =
		"application/x-jboss-marshalling";
static constexpr const char* APPLICATION_PROTOSTREAM_TYPE =
		"application/x-protostream";
static constexpr const char* APPLICATION_UNKNOWN_TYPE = "application/unknown";
static constexpr const char* WWW_FORM_URLENCODED_TYPE =
		"application/x-www-form-urlencoded";
static constexpr const char* IMAGE_GIF_TYPE = "image/gif";
static constexpr const char* IMAGE_JPEG_TYPE = "image/jpeg";
static constexpr const char* IMAGE_PNG_TYPE = "image/png";
static constexpr const char* TEXT_CSS_TYPE = "text/css";
static constexpr const char* TEXT_CSV_TYPE = "text/csv";
static constexpr const char* TEXT_PLAIN_TYPE = "text/plain";
static constexpr const char* TEXT_HTML_TYPE = "text/html";
static constexpr const char* APPLICATION_INFINISPAN_MARSHALLING_TYPE =
		"application/x-infinispan-marshalling";
static constexpr const char* APPLICATION_INFINISPAN_BINARY_TYPE =
		"application/x-infinispan-binary";
static constexpr const char* APPLICATION_PROTOSTUFF_TYPE =
		"application/x-protostuff";
static constexpr const char* APPLICATION_KRYO_TYPE = "application/x-kryo";

// This is a map between well know media types and a numerical identifier
// which will be transmitted to the server
struct IdsMediaTypes {
	static std::map<std::string, int> typesIds;
	static int getByMediaType(std::string mediaType);
	static std::string getById(int id);
};

// A struct representing an entry (key,value) media type
struct EntryMediaTypes {
	MediaType keyMediaType;
	MediaType valueMediaType;
};

// A template struct containing the media type and the key value marshallers
template <class K, class V> struct DataFormat : EntryMediaTypes {
    void keyMarshall(const K& k, std::vector<char>& buff) {
        keyMarshaller->marshall(k, buff);
    }
    K* keyUnmarshall(const std::vector<char>& buff) {
        return keyMarshaller->unmarshall(buff);
    }
    void valueMarshall(const V& k, std::vector<char>& buff) {
        valueMarshaller->marshall(k, buff);
    }
    V* valueUnmarshall(const std::vector<char>& buff) {
        return valueMarshaller->unmarshall(buff);
    }
    std::shared_ptr<Marshaller<K> > keyMarshaller;
    std::shared_ptr<Marshaller<V> > valueMarshaller;
};

}
}

#endif /* INCLUDE_INFINISPAN_HOTROD_DATAFORMAT_H_ */
