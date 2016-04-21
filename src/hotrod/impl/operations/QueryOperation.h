/*
 * QueryOperation.h
 *
 *  Created on: Apr 1, 2016
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_OPERATIONS_QUERYOPERATION_H_
#define SRC_HOTROD_IMPL_OPERATIONS_QUERYOPERATION_H_

#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "query.pb.h"
#include <memory>
namespace infinispan {
namespace hotrod {
namespace operations {

using namespace org::infinispan::query::remote::client;
using namespace infinispan::hotrod::operations;

class QueryOperation : public RetryOnFailureOperation<QueryResponse>{
public:
	QueryOperation(const protocol::Codec& _codec,
			std::shared_ptr<transport::TransportFactory> _transportFactory,
			const std::vector<char>& _cacheName, Topology& _topologyId,
			uint32_t _flags, const QueryRequest& _queryRequest) :
			RetryOnFailureOperation<QueryResponse>(_codec, _transportFactory,
					_cacheName, _topologyId, _flags), queryRequest(
					_queryRequest) {}
	virtual ~QueryOperation();

	transport::Transport& getTransport(int /*retryCount*/) {
			return this->transportFactory->getTransport(this->cacheName);
	}

	QueryResponse executeOperation(transport::Transport &t) {
        std::unique_ptr<protocol::HeaderParams> params(
            &(this->writeHeader(t, QUERY_REQUEST)));

    int size = queryRequest.ByteSize();
    std::vector<char> queryToChar(size);

    queryRequest.SerializeToArray(queryToChar.data(),size);
    t.writeArray(queryToChar);
    t.flush();
    //int8_t status= codec.readHeader(t,*params);


    int8_t status=readHeaderAndValidate(t, *params);

    QueryResponse qr;
    if (status == NO_ERROR_STATUS) {
			std::vector<char> responseBytes = t.readArray();
			qr.ParseFromArray(responseBytes.data(), responseBytes.size());
		}
	return qr;
 }

private:
	const QueryRequest &queryRequest;
};


}}} // namespace infinispan::hotrod::operations

#endif /* SRC_HOTROD_IMPL_OPERATIONS_QUERYOPERATION_H_ */
