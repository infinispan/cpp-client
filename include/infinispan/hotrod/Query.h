/*
 * Query.h
 *
 *  Created on: Nov 23, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_QUERY_H_
#define INCLUDE_INFINISPAN_HOTROD_QUERY_H_
#include "infinispan/hotrod/query.pb.h"
#include <tuple>

using namespace org::infinispan::protostream;

namespace org {
namespace infinispan {
namespace query {
namespace remote {
namespace client {

// jpqlstring field has been renamed to querystring
// in the .proto file.
// This class mantains backward compatibility
/**
 * This class represent a query request.
 */
class QueryRequest
{
private:
	QueryRequestProtobuf qrp;
public:
  inline bool has_jpqlstring() const { return qrp.has_querystring(); }
  inline bool has_querystring() const { return qrp.has_querystring(); }

  inline void clear_jpqlstring() { qrp.clear_querystring(); }
  inline void clear_querystring() { qrp.clear_querystring(); }

  /**
   * Deprecated, use querystring() instead
   * \return the query string for this request
   */
  inline const ::std::string& jpqlstring() const { return qrp.querystring(); }
  inline const ::std::string& querystring() const { return qrp.querystring(); }

  /**
   * Deprecated, use set_querystring() instead
   * set the query string
   * \param value the query string
   */
  inline void set_jpqlstring(const ::std::string& value) { qrp.set_querystring(value); }
  inline void set_querystring(const ::std::string& value) { qrp.set_querystring(value); }

  /**
   * set the query string
   * \param value the query string as zero terminated char*
   */
  inline void set_jpqlstring(const char* value) { qrp.set_querystring(value); }
  inline void set_querystring(const char* value) { qrp.set_querystring(value); }

  /**
   * Deprecated, use set_querystring() instead
   * set the query string
   * \param value the query string as char*
   * \param size the length
   */
  inline void set_jpqlstring(const char* value, size_t size) { qrp.set_querystring(value, size); }
  inline void set_querystring(const char* value, size_t size) { qrp.set_querystring(value, size); }

  inline ::std::string* mutable_jpqlstring() { return qrp.mutable_querystring(); }
  inline ::std::string* mutable_querystring() { return qrp.mutable_querystring(); }

  inline ::std::string* release_jpqlstring() { return qrp.release_querystring(); }
  inline ::std::string* release_querystring() { return qrp.release_querystring(); }

  inline void set_allocated_jpqlstring(::std::string* querystring) { qrp.set_allocated_querystring(querystring); }
  inline void set_allocated_querystring(::std::string* querystring) { qrp.set_allocated_querystring(querystring); }

  inline bool ParseFromArray(const void* data, int size) { return qrp.ParseFromArray(data, size); }
  inline bool SerializeToArray(void* data, int size) const { return qrp.SerializeToArray(data, size); }
  inline int ByteSize() const { return qrp.ByteSize(); }
};

}}}}}

#endif /* INCLUDE_INFINISPAN_HOTROD_QUERY_H_ */
