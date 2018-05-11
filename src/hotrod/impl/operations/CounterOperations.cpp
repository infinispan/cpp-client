/*
 * CounterOperations.cpp
 *
 *  Created on: May 3, 2018
 *      Author: rigazilla
 */

#include "hotrod/impl/operations/CounterOperations.h"
#include <vector>
#include <set>

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod;

void BaseCounterOperation::writeName(infinispan::hotrod::transport::Transport& transport, std::string counterName) {
    std::vector<char> counterNameAsVector(counterName.begin(), counterName.end());
    transport.writeArray(counterNameAsVector);
}

void BaseCounterOperation::writeConfiguration(infinispan::hotrod::transport::Transport& transport,
        const CounterConfiguration& c) {
    uint8_t flags = 0;
    switch (c.getType()) {
    case CounterType::WEAK:
        flags |= 0x01;
        break;
    case CounterType::BOUNDED_STRONG:
        flags |= 0x02;
        break;
    case CounterType::UNBOUNDED_STRONG:
        break;
    }

    switch (c.getStorage()) {
    case Storage::PERSISTENT:
        flags |= 0x04;
        break;
    case Storage::VOLATILE:
        break;
    }
    transport.writeByte(flags);

    if (c.getType() == CounterType::WEAK) {
        transport.writeVInt(c.getConcurrencyLevel());
    }

    if (c.getType() == CounterType::BOUNDED_STRONG)
            {
        transport.writeLong(c.getLowerBound());
        transport.writeLong(c.getUpperBound());
    }
    transport.writeLong(c.getInitialValue());
}

CounterConfiguration BaseCounterOperation::readConfiguration(infinispan::hotrod::transport::Transport& transport) {
    uint8_t flags;
    CounterType ct;
    Storage st;
    long concurrentLevel = 0;
    long lower = 0;
    long upper = 0;
    long initial = 0;
    flags = transport.readByte();
    switch (flags & 0x03) {
    case 0x00:
        ct = CounterType::UNBOUNDED_STRONG;
        break;
    case 0x01:
        ct = CounterType::WEAK;
        break;
    case 0x02:
        ct = CounterType::BOUNDED_STRONG;
        break;
    default:
        throw HotRodClientException(std::string("Unknown counter type: ") + std::to_string(flags));
    }
    st = (flags & 0x04) ? Storage::PERSISTENT : Storage::VOLATILE;

    if (ct == WEAK) {
        concurrentLevel = transport.readVInt();
    }
    if (ct == BOUNDED_STRONG) {
        lower = transport.readLong();
        upper = transport.readLong();
    }
    initial = transport.readLong();
    return CounterConfiguration(initial, lower, upper, concurrentLevel, ct, st);
}

bool DefineCounterOperation::executeOperation(infinispan::hotrod::transport::Transport& transport) {
    TRACE("Executing DefineCounterOperation(flags=%u)", flags);
    std::unique_ptr<HeaderParams> params(RetryOnFailureOperation<bool>::writeHeader(transport, COUNTER_CREATE_REQUEST));
    writeName(transport, counterName);
    writeConfiguration(transport, configuration);
    transport.flush();
    uint8_t status = readHeaderAndValidate(transport, *params);
    TRACE("Finished DefineCounterOperation");
    return isSuccess(status);
}

CounterConfiguration GetCounterConfigurationOperation::executeOperation(
        infinispan::hotrod::transport::Transport& transport) {
    TRACE("Executing GetConfigurationOperation(flags=%u)", flags);
    std::unique_ptr<HeaderParams> params(
            RetryOnFailureOperation<CounterConfiguration>::writeHeader(transport, COUNTER_GET_CONFIGURATION_REQUEST));
    writeName(transport, counterName);
    transport.flush();
    uint8_t status = readHeaderAndValidate(transport, *params);
    if (!isSuccess(status)) {
        throw HotRodClientException(
                std::string("Error in CounterGetConfiguration operation, counter name: ") + counterName + " status: "
                        + std::to_string(status));
    }
    auto ret = readConfiguration(transport);
    TRACE("Finished GetConfigurationOperation");
    return ret;
}

void ResetCounterOperation::executeOperation(infinispan::hotrod::transport::Transport& transport) {
    TRACE("Executing ResetOperation(flags=%u)", flags);
    std::unique_ptr<HeaderParams> params(RetryOnFailureOperation<void>::writeHeader(transport, COUNTER_RESET_REQUEST));
    writeName(transport, counterName);
    transport.flush();
    uint8_t status = readHeaderAndValidate(transport, *params);
    if (!isSuccess(status) && status != KEY_DOES_NOT_EXIST_STATUS) {
        throw HotRodClientException(
                std::string("Error in CounterReset operation, counter name: ") + counterName + " status: "
                        + std::to_string(status));
    }
    TRACE("Finished ResetOperation");
}

void RemoveCounterOperation::executeOperation(infinispan::hotrod::transport::Transport& transport) {
    TRACE("Executing RemoveOperation(flags=%u)", flags);
    std::unique_ptr<HeaderParams> params(RetryOnFailureOperation<void>::writeHeader(transport, COUNTER_REMOVE_REQUEST));
    writeName(transport, counterName);
    transport.flush();
    uint8_t status = readHeaderAndValidate(transport, *params);
    if (!isSuccess(status) && status != KEY_DOES_NOT_EXIST_STATUS) {
        throw HotRodClientException(
                std::string("Error in CounterRemove operation, counter name: ") + counterName + " status: "
                        + std::to_string(status));
    }
    TRACE("Finished RemoveOperation");
}

std::set<std::string> GetCounterNamesOperation::executeOperation(infinispan::hotrod::transport::Transport& transport) {
    TRACE("Executing GetCounterNamesOperation(flags=%u)", flags);
    std::unique_ptr<HeaderParams> params(
            RetryOnFailureOperation<std::set<std::string>>::writeHeader(transport, COUNTER_GET_NAMES_REQUEST));
    uint8_t status = readHeaderAndValidate(transport, *params);
    std::set<std::string> ret;
    if (!isSuccess(status)) {
        auto num = transport.readVInt();
        for (uint32_t i = 0; i < num; i++) {
            auto name = transport.readString();
            ret.insert(name);
        }
    }
    TRACE("Finished GetCounterNamesOperation");
    return ret;
}

bool IsCounterDefinedOperation::executeOperation(infinispan::hotrod::transport::Transport& transport) {
    TRACE("Executing IsCounterDefinedOperation(flags=%u)", flags);
    std::unique_ptr<HeaderParams> params(RetryOnFailureOperation<bool>::writeHeader(transport, COUNTER_CREATE_REQUEST));
    writeName(transport, counterName);
    transport.flush();
    uint8_t status = readHeaderAndValidate(transport, *params);
    TRACE("Finished IsCounterDefinedOperation");
    return status==0x00;
}

long GetCounterValueOperation::executeOperation(infinispan::hotrod::transport::Transport& transport) {
    TRACE("Executing IsCounterDefinedOperation(flags=%u)", flags);
    std::unique_ptr<HeaderParams> params(RetryOnFailureOperation<long>::writeHeader(transport, COUNTER_GET_REQUEST));
    writeName(transport, counterName);
    transport.flush();
    uint8_t status = readHeaderAndValidate(transport, *params);
    if (!isSuccess(status)) {
        throw HotRodClientException(
                std::string("Error in GetCountervalue operation, counter name: ") + counterName + " status: "
                        + std::to_string(status));
    }
    auto ret = (long)transport.readLong();
    TRACE("Finished IsCounterDefinedOperation");
    return ret;
}

long AddAndGetCounterValueOperation::executeOperation(infinispan::hotrod::transport::Transport& transport) {
    TRACE("Executing GetCounterValue(flags=%u)", flags);
    std::unique_ptr<HeaderParams> params(RetryOnFailureOperation<long>::writeHeader(transport, COUNTER_ADD_AND_GET_REQUEST));
    writeName(transport, counterName);
    transport.writeLong(delta);
    transport.flush();
    uint8_t status = readHeaderAndValidate(transport, *params);
    assertBoundaries(status);
    TRACE("Finished GetCounterValue");
    if (!isSuccess(status)) {
        throw HotRodClientException(
                std::string("Error in GetCounterValue operation, counter name: ") + counterName + " status: "
                        + std::to_string(status));
    }
    return (long)transport.readLong();
}

void AddAndGetCounterValueOperation::assertBoundaries(short status) {
   if (status == NOT_EXECUTED_WITH_PREVIOUS) {
      if (delta > 0) {
          throw CounterUpperBoundException(counterName);
      } else {
          throw CounterLowerBoundException(counterName);
      }
   }
}

long CompareAndSwapCounterValueOperation::executeOperation(infinispan::hotrod::transport::Transport& transport) {
    TRACE("Executing CompareAndSwapOperation(flags=%u)", flags);
    std::unique_ptr<HeaderParams> params(RetryOnFailureOperation<long>::writeHeader(transport, COUNTER_CAS_REQUEST));
    writeName(transport, counterName);
    transport.writeLong(expect);
    transport.writeLong(update);
    transport.flush();
    uint8_t status = readHeaderAndValidate(transport, *params);
    assertBoundaries(status);
    if (status != NO_ERROR_STATUS) {
        throw HotRodClientException(
                std::string("Error in CompareAndSwap operation, counter name: ") + counterName + " status: "
                        + std::to_string(status));
    }
    auto ret = (long)transport.readLong();
    TRACE("Finished CompareAndSwapDefinedOperation");
    return ret;
}

void CompareAndSwapCounterValueOperation::assertBoundaries(short status) {
   if (status == NOT_EXECUTED_WITH_PREVIOUS) {
      if (update >= cc.getUpperBound()) {
          throw CounterUpperBoundException(counterName);
      } else {
          throw CounterLowerBoundException(counterName);
      }
   }
}

Transport* AddCounterListenerOperation::executeOperation(infinispan::hotrod::transport::Transport& transport) {
    TRACE("Executing AddCounterListenerOperation(flags=%u)", flags);
    failed = false;
    std::unique_ptr<HeaderParams> params(
            RetryOnFailureOperation<Transport*>::writeHeader(transport, COUNTER_ADD_LISTENER_REQUEST));
    writeName(transport, counterName);
    transport.writeArray(listenerId);
    transport.flush();
    uint8_t status = readHeaderAndValidate(transport, *params);
    if (status != NO_ERROR_STATUS) {
        failed = true;
        return nullptr;
    }
    return &transport;
}

void AddCounterListenerOperation::releaseTransport(transport::Transport* t) {
    if (failed || !keepTransport) {
        RetryOnFailureOperation::releaseTransport(t);
        failed = false;
    }
}

void AddCounterListenerOperation::invalidateTransport(const infinispan::hotrod::transport::InetSocketAddress& addr,
        transport::Transport* transport) {
    if (transport) {
        transportFactory->invalidateTransport(addr, transport);
    }
}

bool RemoveCounterListenerOperation::executeOperation(infinispan::hotrod::transport::Transport& transport) {
    TRACE("Executing RemoveCounterListenerOperation(flags=%u)", flags);
    std::unique_ptr<HeaderParams> params(
            RetryOnFailureOperation<bool>::writeHeader(transport, COUNTER_REMOVE_LISTENER_REQUEST));
    writeName(transport, counterName);
    transport.writeArray(listenerId);
    transport.flush();
    uint8_t status = readHeaderAndValidate(transport, *params);
    return status == NO_ERROR_STATUS;
}

}
}
}

