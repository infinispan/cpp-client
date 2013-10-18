#include "infinispan/hotrod/SslConfigurationBuilder.h"

namespace infinispan {
namespace hotrod {

SslConfigurationBuilder::SslConfigurationBuilder(ConfigurationBuilder& builder) :
        ConfigurationChildBuilder(builder) {
}

SslConfiguration SslConfigurationBuilder::create()
{
    return SslConfiguration();
}

SslConfigurationBuilder& SslConfigurationBuilder::read(SslConfiguration& /*bean*/) {
    return *this;
}

}}
