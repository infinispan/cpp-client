/*
 * QueryUtils.h
 *
 *  Created on: Apr 7, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_QUERYUTILS_H_
#define INCLUDE_INFINISPAN_HOTROD_QUERYUTILS_H_

#include "infinispan/hotrod/Query.h"
#include <tuple>
using namespace org::infinispan::protostream;
using namespace google::protobuf;
using namespace org::infinispan::query::remote::client;


// extract a resultset of entities from a QueryResponse obj
template <class T> bool unwrapResults(QueryResponse resp, std::vector<T> &res)
{
	if (resp.projectionsize()>0)
	{  // Query has select
		return false;
	}
    for (int i=0; i<resp.results_size(); i++)
    {
    	const WrappedMessage &wm =resp.results(i);
    	if ( wm.has_wrappedbytes() )
    	{
    		WrappedMessage wmn;
    		wmn.ParseFromString(wm.wrappedbytes());
			if (wmn.has_wrappedmessagebytes()) {
				T u1;
				u1.ParseFromString(wmn.wrappedmessagebytes());
				res.push_back(u1);
			}
    	}
    }
    return true;
}

// typesafe method to extract the value obj from a WrappedMessage obj
template <typename T> T unwrapSingleValue(const WrappedMessage& wm);

template <> inline std::string unwrapSingleValue<std::string>(const WrappedMessage& wm)
{
	if (wm.has_wrappedstring())
	{
	return wm.wrappedstring();
	}
	else
	{
		throw "std::string not found in response";
	}
}

template <> inline int unwrapSingleValue<int>(const WrappedMessage& wm)
{
	if (wm.has_wrappedint32())
	{
	return wm.wrappedint32();
	}
	else if (wm.has_wrappedint64())
	{
		return wm.wrappedint64();
	}
	else
	{
		throw "std::string not found in response";
	}
}

template <typename T> T unwrapSingleResult(const QueryResponse &qr)
{
	return unwrapSingleValue<T>(qr.results(0));
}


#if !defined (_MSC_VER) || (_MSC_VER>=1800)

// typesafe method to turn one projection row of a array of WrappedMessage objs into a tuple
template <typename H, typename... Params> std::tuple<H, Params...> popTuple(const RepeatedPtrField<WrappedMessage >& wMsgs,  int &k)
{
	H s = unwrapSingleValue<H>(wMsgs.Get(k++));
	std::tuple<Params...> p=popTuple<Params... >(wMsgs,k);
	return std::tuple_cat(std::tie(s),p);
}

template<>
inline std::tuple<int> popTuple<int>(const RepeatedPtrField<WrappedMessage >& wMsgs, int &k)
		{
	  int s(unwrapSingleValue<int>(wMsgs.Get(k++)));
	  return std::make_tuple<int>(std::move(s));
}

template<>
inline std::tuple<std::string> popTuple<std::string>(const RepeatedPtrField<WrappedMessage >& wMsgs, int &k)
		{
	  std::string s(unwrapSingleValue<std::string>(wMsgs.Get(k++)));
	  return std::make_tuple<std::string>(std::move(s));
}

// typesafe method to turn one projection row of a QueryResponse obj into a tuple
template <typename H, typename... Params> std::tuple<H, Params...> popTuple(QueryResponse &resp,  int &k)
{
	H s = unwrapSingleValue<H>(resp.results(k++));
	std::tuple<Params...> p=popTuple<Params... >(resp,k);
	return std::tuple_cat(std::tie(s),p);
}

template<>
inline std::tuple<std::string> popTuple<std::string>(QueryResponse& resp, int &k)
		{
	  std::string s(unwrapSingleValue<std::string>(resp.results(k++)));
	  return std::make_tuple<std::string>(std::move(s));
}

template<>
inline std::tuple<int> popTuple<int>(QueryResponse & resp, int &k)
		{
	  int s(unwrapSingleValue<int>(resp.results(k++)));
	  return std::make_tuple<int>(std::move(s));
}

// typesafe method to convert QueryResponse containing a resultset of projections to a std::tuple
template<typename... Params> bool unwrapProjection(QueryResponse &resp, std::vector<std::tuple<Params...> > &prjRes)
{
	if (resp.projectionsize() == 0) {
		return false;
	}
	int numTuple = resp.results_size() / resp.projectionsize();
	int k = 0;
	for (int i = 0; i < numTuple; i++) {
				std::tuple<Params...> tp= popTuple<Params...>(resp, k) ;
				prjRes.push_back(tp);
			}
	return true;
}
#endif

#endif /* INCLUDE_INFINISPAN_HOTROD_QUERYUTILS_H_ */
