/*
 * CounterOperations.h
 *
 *  Created on: May 3, 2018
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_OPERATIONS_COUNTEROPERATIONS_H_
#define SRC_HOTROD_IMPL_OPERATIONS_COUNTEROPERATIONS_H_

#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/VersionedOperationResponse.h"
#include "infinispan/hotrod/Flag.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "infinispan/hotrod/CounterConfiguration.h"

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

class BaseCounterOperation {
protected:
    BaseCounterOperation() :
            counterName() {
    }
    BaseCounterOperation(std::string counterName) :
            counterName(counterName) {
    }
    void writeName(infinispan::hotrod::transport::Transport& transport, std::string counterName);
    void writeConfiguration(infinispan::hotrod::transport::Transport& transport, const CounterConfiguration& c);
    CounterConfiguration readConfiguration(infinispan::hotrod::transport::Transport& transport);

    const std::string counterName;
};

class DefineCounterOperation: public BaseCounterOperation, public RetryOnFailureOperation<bool>
{
public:
    DefineCounterOperation(protocol::Codec& codec, std::shared_ptr<transport::TransportFactory> transportFactory,
            Topology& topologyId, uint32_t flags, std::string counterName, CounterConfiguration configuration) :
            BaseCounterOperation(counterName), RetryOnFailureOperation<bool>(codec, transportFactory,
                    std::vector<char>(), topologyId, flags), configuration(configuration) {
    }
    bool executeOperation(infinispan::hotrod::transport::Transport& transport);
    private:
    const CounterConfiguration configuration;
};

class GetCounterConfigurationOperation: public BaseCounterOperation,
        public RetryOnFailureOperation<CounterConfiguration>
{
public:
    GetCounterConfigurationOperation(protocol::Codec& codec,
            std::shared_ptr<transport::TransportFactory> transportFactory, Topology& topologyId, uint32_t flags,
            std::string counterName) :
            BaseCounterOperation(counterName), RetryOnFailureOperation<CounterConfiguration>(codec, transportFactory,
                    std::vector<char>(), topologyId, flags) {
    }
    CounterConfiguration executeOperation(infinispan::hotrod::transport::Transport& transport);
};

class ResetCounterOperation: public BaseCounterOperation, public RetryOnFailureOperation<void>
{
public:
    ResetCounterOperation(protocol::Codec& codec, std::shared_ptr<transport::TransportFactory> transportFactory,
            Topology& topologyId, uint32_t flags, std::string counterName) :
            BaseCounterOperation(counterName), RetryOnFailureOperation<void>(codec, transportFactory,
                    std::vector<char>(), topologyId, flags) {
    }
    void executeOperation(infinispan::hotrod::transport::Transport& transport);
};

class RemoveCounterOperation: public BaseCounterOperation, public RetryOnFailureOperation<void>
{
public:
    RemoveCounterOperation(protocol::Codec& codec, std::shared_ptr<transport::TransportFactory> transportFactory,
            Topology& topologyId, uint32_t flags, std::string counterName) :
            BaseCounterOperation(counterName), RetryOnFailureOperation<void>(codec, transportFactory,
                    std::vector<char>(), topologyId, flags) {
    }
    void executeOperation(infinispan::hotrod::transport::Transport& transport);
};

class GetCounterNamesOperation: public BaseCounterOperation, public RetryOnFailureOperation<std::set<std::string> >
{
public:
    GetCounterNamesOperation(protocol::Codec& codec, std::shared_ptr<transport::TransportFactory> transportFactory,
            Topology& topologyId, uint32_t flags) :
            BaseCounterOperation(), RetryOnFailureOperation<std::set<std::string> >(codec, transportFactory,
                    std::vector<char>(), topologyId, flags) {
    }
    std::set<std::string> executeOperation(infinispan::hotrod::transport::Transport& transport);
};

class IsCounterDefinedOperation: public BaseCounterOperation, public RetryOnFailureOperation<bool>
{
public:
    IsCounterDefinedOperation(protocol::Codec& codec, std::shared_ptr<transport::TransportFactory> transportFactory,
            Topology& topologyId, uint32_t flags, std::string counterName) :
            BaseCounterOperation(counterName), RetryOnFailureOperation<bool>(codec, transportFactory,
                    std::vector<char>(), topologyId, flags) {
    }
    bool executeOperation(infinispan::hotrod::transport::Transport& transport);
};

class GetCounterValueOperation: public BaseCounterOperation, public RetryOnFailureOperation<long>
{
public:
    GetCounterValueOperation(protocol::Codec& codec, std::shared_ptr<transport::TransportFactory> transportFactory,
            Topology& topologyId, uint32_t flags, std::string counterName) :
            BaseCounterOperation(counterName), RetryOnFailureOperation<long>(codec, transportFactory,
                    std::vector<char>(), topologyId, flags) {
    }
    long executeOperation(infinispan::hotrod::transport::Transport& transport);
};

class AddAndGetCounterValueOperation: public BaseCounterOperation, public RetryOnFailureOperation<long>
{
public:
    AddAndGetCounterValueOperation(protocol::Codec& codec,
            std::shared_ptr<transport::TransportFactory> transportFactory,
            Topology& topologyId, uint32_t flags, std::string counterName, long delta) :
            BaseCounterOperation(counterName), RetryOnFailureOperation<long>(codec, transportFactory,
                    std::vector<char>(), topologyId, flags), delta(delta) {
    }
    long executeOperation(infinispan::hotrod::transport::Transport& transport);
    private:
    long delta;

    void assertBoundaries(short status);
};

class CompareAndSwapCounterValueOperation: public BaseCounterOperation, public RetryOnFailureOperation<long> {
public:
    CompareAndSwapCounterValueOperation(protocol::Codec& codec,
            std::shared_ptr<transport::TransportFactory> transportFactory,
            Topology& topologyId, uint32_t flags, std::string counterName, long expect, long update,
            CounterConfiguration& cc) :
            BaseCounterOperation(counterName), RetryOnFailureOperation<long>(codec, transportFactory,
                    std::vector<char>(), topologyId, flags), expect(expect), update(update), cc(cc) {
    }
    long executeOperation(infinispan::hotrod::transport::Transport& transport);
    private:
    long expect;
    long update;
    CounterConfiguration& cc;

    void assertBoundaries(short status);

};

class AddCounterListenerOperation: public BaseCounterOperation, public RetryOnFailureOperation<Transport*> {
public:
    AddCounterListenerOperation(protocol::Codec& codec, std::shared_ptr<transport::TransportFactory> transportFactory,
            Topology& topologyId, uint32_t flags, std::string counterName, std::vector<char> listenerId,
            bool keepTransport) :
            BaseCounterOperation(counterName), RetryOnFailureOperation<Transport*>(codec, transportFactory,
                    std::vector<char>(), topologyId, flags), listenerId(listenerId), keepTransport(keepTransport), failed(
                    false) {
    }
    Transport* executeOperation(infinispan::hotrod::transport::Transport& transport);
    void releaseTransport(transport::Transport* t);
    void invalidateTransport(const infinispan::hotrod::transport::InetSocketAddress& addr,
            transport::Transport* transport);

private:
    std::vector<char> listenerId;
    bool keepTransport;
    bool failed;
};

class RemoveCounterListenerOperation: public BaseCounterOperation, public RetryOnFailureOperation<bool> {
public:
    RemoveCounterListenerOperation(protocol::Codec& codec,
            std::shared_ptr<transport::TransportFactory> transportFactory,
            Topology& topologyId, uint32_t flags, std::string counterName, std::vector<char> listenerId) :
            BaseCounterOperation(counterName), RetryOnFailureOperation<bool>(codec, transportFactory,
                    std::vector<char>(), topologyId, flags), listenerId(listenerId) {
    }
    bool executeOperation(infinispan::hotrod::transport::Transport& transport);
    private:
    std::vector<char> listenerId;
};


}
}
} // namespace infinispan::hotrod::operations

#endif /* SRC_HOTROD_IMPL_OPERATIONS_COUNTEROPERATIONS_H_ */
