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
class QueryRequest : public QueryRequestProtobuf
{
public:
  inline bool has_jpqlstring() const { return has_querystring(); }
  inline void clear_jpqlstring() { clear_querystring(); }
  /**
   * Deprecated, use querystring() instead
   * \return the query string for this request
   */
  inline const ::std::string& jpqlstring() const { return querystring(); }
  /**
   * Deprecated, use set_querystring() instead
   * set the query string
   * \param value the query string
   */
  inline void set_jpqlstring(const ::std::string& value) { set_querystring(value); }
  /**
   * set the query string
   * \param value the query string as zero terminated char*
   */
  inline void set_jpqlstring(const char* value) { set_querystring(value); }
  /**
   * Deprecated, use set_querystring() instead
   * set the query string
   * \param value the query string as char*
   * \param size the length
   */
  inline void set_jpqlstring(const char* value, size_t size) { set_querystring(value, size); }
  inline ::std::string* mutable_jpqlstring() { return mutable_querystring(); }
  inline ::std::string* release_jpqlstring() { return release_querystring(); }
  inline void set_allocated_jpqlstring(::std::string* querystring) { set_allocated_querystring(querystring); }
};

}}}}}

#endif /* INCLUDE_INFINISPAN_HOTROD_QUERY_H_ */
