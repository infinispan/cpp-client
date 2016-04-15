/*
 * QueryUtils.h
 *
 *  Created on: Apr 7, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_QUERYUTILS_H_
#define INCLUDE_INFINISPAN_HOTROD_QUERYUTILS_H_
#include "org/infinispan/query/remote/client/query.pb.h"
#include <tuple>
using namespace org::infinispan::protostream;

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
				sample_bank_account::User u1;
				u1.ParseFromString(wmn.wrappedmessagebytes());
				res.push_back(u1);
			}
    	}
    }
    return true;
}

template <typename T> T unwrapSingleValue(const WrappedMessage& wm);

template <> std::string unwrapSingleValue<std::string>(const WrappedMessage& wm)
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

template <> int unwrapSingleValue<int>(const WrappedMessage& wm)
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


template <typename H, typename... Params> std::tuple<H, Params...> popTuple(QueryResponse &resp,  int &k)
{
	H s = unwrapSingleValue<H>(resp.results(k++));
	std::tuple<Params...> p=popTuple<Params... >(resp,k);
	return std::tuple_cat(std::tie(s),p);
}

template<>
std::tuple<std::string> popTuple<std::string>(QueryResponse & resp, int &k)
		{
	  std::string s(unwrapSingleValue<std::string>(resp.results(k++)));
	  return std::make_tuple<std::string>(std::move(s));
}

template<>
std::tuple<int> popTuple<int>(QueryResponse & resp, int &k)
		{
	  int s(unwrapSingleValue<int>(resp.results(k++)));
	  return std::make_tuple<int>(std::move(s));
}


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

//bool popTuple(QueryResponse &/*resp*/, int /*k*/)
//{
//	return true;
//}


//template<typename H, typename... Params> std::tuple<H,Params...> popTuple1(QueryResponse &resp, int k, H &head, Params... T)
//{
//	if (resp.projectionsize() == 0) {
//		return false;
//	}
//	int numTuple = resp.results_size() / resp.projectionsize();
//	for (int i = 0; i < numTuple; i++) {
//				head=resp.results(1).wrappedstring();
//				popTuple1(resp, T...);
//			}
//	return true;
//}

//template<typename... Params> bool unwrapProjection(QueryResponse &resp, std::vector<std::tuple<Params...> > &prjRes)
//{
//	if (resp.projectionsize() == 0) {
//		return false;
//	}
//	int numTuple = resp.results_size() / resp.projectionsize();
//	int k = 0;
//	for (int i = 0; i < numTuple; i++) {
//				std::tuple<Params...> tp;
//				popTuple1<Params...>(resp, k, tp) ;
//				prjRes.push_back(tp);
//			}
//	return true;
//}





#endif /* INCLUDE_INFINISPAN_HOTROD_QUERYUTILS_H_ */
