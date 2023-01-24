#ifndef ISPN_HOTROD_OPERATIONS_EXECUTECMDOPERATION_H
#define ISPN_HOTROD_OPERATIONS_EXECUTECMDOPERATION_H

#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/operations/AbstractKeyOperation.h"

#include <set>

namespace infinispan
{
    namespace hotrod
    {
        class Topology;
        namespace operations
        {

            class ExecuteCmdOperation : public RetryOnFailureOperation<std::vector<char>>
            {
            protected:
                ExecuteCmdOperation(
                    const infinispan::hotrod::protocol::Codec &codec_,
                    std::shared_ptr<transport::TransportFactory> transportFactory_,
                    const std::vector<char> &cacheName_,
                    Topology &topologyId_,
                    uint32_t flags_,
                    const std::vector<char> &cmdName_,
                    const std::map<std::vector<char>, std::vector<char>> &cmdArgs_,
                    EntryMediaTypes *df);
                std::vector<char> sendExecuteOperation(
                    transport::Transport &transport,
                    uint8_t opCode,
                    uint8_t opRespCode);
                std::vector<char> executeOperation(infinispan::hotrod::transport::Transport &transport);
                const std::vector<char> &cmdName;
                const std::map<std::vector<char>, std::vector<char>> &cmdArgs;
                friend class OperationsFactory;
            };

            class ExecuteCmdKeyOperation : public AbstractKeyOperation<std::vector<char>>
            {
            protected:
                ExecuteCmdKeyOperation(
                    const infinispan::hotrod::protocol::Codec &codec_,
                    std::shared_ptr<transport::TransportFactory> transportFactory_,
                    const std::vector<char> &key_,
                    const std::vector<char> &cacheName_,
                    Topology &topologyId_,
                    uint32_t flags_,
                    const std::vector<char> &cmdName_,
                    const std::map<std::vector<char>, std::vector<char>> &cmdArgs_,
                    EntryMediaTypes *df);
                std::vector<char> sendExecuteOperation(
                    transport::Transport &transport,
                    uint8_t opCode,
                    uint8_t opRespCode);
                std::vector<char> executeOperation(infinispan::hotrod::transport::Transport &transport);
                const std::vector<char> &cmdName;
                const std::map<std::vector<char>, std::vector<char>> &cmdArgs;
                friend class OperationsFactory;
            };

        }
    }
} // namespace

#endif /* ISPN_HOTROD_EXECUTECMDOPERATION_H */
