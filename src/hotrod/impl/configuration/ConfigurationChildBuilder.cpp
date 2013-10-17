#include "ConfigurationChildBuilder.h"
#include "ConfigurationBuilder.h"

namespace infinispan {
namespace hotrod {

ConfigurationChildBuilder::ConfigurationChildBuilder(ConfigurationBuilder& builder_) :
        m_builder(builder_) {
}

ServerConfigurationBuilder& ConfigurationChildBuilder::addServer() {
    return m_builder.addServer();
}

ConfigurationBuilder& ConfigurationChildBuilder::addServers(std::string servers) {
    return m_builder.addServers(servers);
}

ConnectionPoolConfigurationBuilder& ConfigurationChildBuilder::connectionPool() {
    return m_builder.connectionPool();
}

ConfigurationBuilder& ConfigurationChildBuilder::connectionTimeout(int connectionTimeout_) {
    return m_builder.connectionTimeout(connectionTimeout_);
}

ConfigurationBuilder& ConfigurationChildBuilder::forceReturnValues(bool forceReturnValues_) {
    return m_builder.forceReturnValues(forceReturnValues_);
}

ConfigurationBuilder& ConfigurationChildBuilder::keySizeEstimate(int keySizeEstimate_) {
    return m_builder.keySizeEstimate(keySizeEstimate_);
}

ConfigurationBuilder& ConfigurationChildBuilder::pingOnStartup(bool pingOnStartup_) {
    return m_builder.pingOnStartup(pingOnStartup_);
}

ConfigurationBuilder& ConfigurationChildBuilder::protocolVersion(std::string protocolVersion_) {
    return m_builder.protocolVersion(protocolVersion_);
}

ConfigurationBuilder& ConfigurationChildBuilder::socketTimeout(int socketTimeout_) {
    return m_builder.socketTimeout(socketTimeout_);
}

SslConfigurationBuilder& ConfigurationChildBuilder::ssl() {
    return m_builder.ssl();
}

ConfigurationBuilder& ConfigurationChildBuilder::tcpNoDelay(bool tcpNoDelay_) {
    return m_builder.tcpNoDelay(tcpNoDelay_);
}

ConfigurationBuilder& ConfigurationChildBuilder::valueSizeEstimate(int valueSizeEstimate_) {
    return m_builder.valueSizeEstimate(valueSizeEstimate_);
}

Configuration ConfigurationChildBuilder::build() {
    return m_builder.build();
}

}} // namespace

