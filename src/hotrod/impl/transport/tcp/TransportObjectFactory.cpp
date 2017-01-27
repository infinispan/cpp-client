#include <hotrod/impl/transport/tcp/SSLTcpTransport.h>
#include <hotrod/impl/transport/tcp/SChannelTcpTransport.h>
#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"
#include "hotrod/impl/operations/AuthOperation.h"
#include "hotrod/impl/operations/AuthMechListOperation.h"
#include <sasl/sasl.h>
#include <sasl/saslplug.h>
#include <string.h>
namespace infinispan {
namespace hotrod {

using protocol::Codec;
using namespace operations;

namespace transport {

void saslfail(int why, const char *what)
{
    // TODO: error handling
    std::string s("Sasl error ");
    s.append(std::to_string(why)).append(" : ").append(what);
    throw HotRodClientException(s);
}

static char simple_data[]="name";

static int simple(void *context __attribute__((unused)),
          int id,
          const char **result,
          unsigned *len)
{
    *result=simple_data;
    if (len) *len=5;
    return SASL_OK;
}

static char secret_data[]="password";

static int
getsecret(sasl_conn_t *conn,
      void *context __attribute__((unused)),
      int id,
      sasl_secret_t **psecret)
{
    size_t len;
    static sasl_secret_t *x;

    /* paranoia check */
    if (! conn || ! psecret || id != SASL_CB_PASS)
    return SASL_BADPARAM;

    len = 8;

    x = (sasl_secret_t *) realloc(x, sizeof(sasl_secret_t) + len);

    x->len = len;
    strcpy((char *)x->data, secret_data);

    *psecret = x;
    return SASL_OK;
}

TransportObjectFactory::TransportObjectFactory(
    Codec& c, TcpTransportFactory& factory)
    : tcpTransportFactory(factory),
      codec(c)
{
    // TODO: move in the initialization sequence
    /* callbacks we support */

    static sasl_callback_t callbacks[] = {
       {
              SASL_CB_AUTHNAME, (sasl_callback_ft)&simple, NULL
            }, {
              SASL_CB_PASS, (sasl_callback_ft)&getsecret, NULL
            },
        SASL_CB_LIST_END, NULL, NULL

    };

    int r;
    /* initialize the sasl library */
    r = sasl_client_init(callbacks);
    if (r != SASL_OK) saslfail(r, "initializing libsasl");
}

void do_sasl_authentication(Codec& codec, Transport& t) {
    sasl_conn_t *conn;
    int r;
    const char *data;
    const char *chosenmech;
    int len;

    AuthMechListOperation am(codec, t);
    std::vector<std::string> respOpAm(am.execute());

    r = sasl_client_new("hotrod", "localhost", nullptr, nullptr, nullptr, 0, &conn);
    if (r != SASL_OK)
        saslfail(r, "allocating connection state");
    r = sasl_client_start(conn, "PLAIN", NULL, &data, (unsigned int *) &len, &chosenmech);
    if (r != SASL_OK && r != SASL_CONTINUE) {
        saslfail(r, "starting SASL negotiation");
    }
    /* we send up to 3 strings;
     the mechanism chosen, the presence of initial response,
     and optionally the initial response */
    std::vector<char> mech(chosenmech, chosenmech + strlen(chosenmech)), resp(data, data + len);
    AuthOperation a(codec, t, mech, resp);
    std::vector<char> respOp(a.execute());
    while (r == SASL_CONTINUE) {
        r = sasl_client_step(conn, respOp.data(), respOp.size(), NULL, &data, (unsigned int *) &len);
        std::vector<char> mech(chosenmech, chosenmech + strlen(chosenmech)), resp(data, data + len);
        AuthOperation a(codec, t, mech, resp);
        respOp = a.execute();
    }
    if (r != SASL_OK && r != SASL_CONTINUE) {
        saslfail(r, "performing SASL negotiation");
    }
}

TcpTransport& TransportObjectFactory::makeObject(const InetSocketAddress& address) {
    if(tcpTransportFactory.isSslEnabled()) {
#if !defined _WIN32 && !defined _WIN64
       TcpTransport *t = (new SSLTcpTransport(address, tcpTransportFactory,
                tcpTransportFactory.getSslServerCAPath(), tcpTransportFactory.getSslServerCAFile(), tcpTransportFactory.getSslClientCertificateFile(), tcpTransportFactory.getSniHostName()));
       if (tcpTransportFactory.getConfiguration().getSecurityConfiguration().getAuthenticationConfiguration().isEnabled())
       {
          do_sasl_authentication(codec, *t);
       }
       return *t;
#else
       TcpTransport *t =  (new SChannelTcpTransport(address, tcpTransportFactory,
	            tcpTransportFactory.getSslServerCAPath(), tcpTransportFactory.getSslServerCAFile(), tcpTransportFactory.getSslClientCertificateFile(), tcpTransportFactory.getSniHostName()));
       if (tcpTransportFactory.getConfiguration().getSecurityConfiguration().getAuthenticationConfiguration().isEnabled())
       {
          do_sasl_authentication(codec, *t);
       }
       return *t;
#endif

	} else {
        TcpTransport* t = new TcpTransport(address, tcpTransportFactory);
        if (tcpTransportFactory.getConfiguration().getSecurityConfiguration().getAuthenticationConfiguration().isEnabled())
        {
           do_sasl_authentication(codec, *t);
        }
        return *t;
    }
}

bool TransportObjectFactory::validateObject(const InetSocketAddress& /*address*/, TcpTransport& transport) {
    return ping(transport) == SUCCESS;
}

void TransportObjectFactory::destroyObject(const InetSocketAddress& /*address*/, TcpTransport& transport) {
    transport.destroy();
    delete &transport;
}

void TransportObjectFactory::activateObject(const InetSocketAddress& /*address*/, TcpTransport& /*transport*/) {
    // empty
}

void TransportObjectFactory::passivateObject(
    const InetSocketAddress& /*address*/, TcpTransport& /*transport*/)
{
    // empty
}

PingResult TransportObjectFactory::ping(
    TcpTransport& tcpTransport)
{
    Topology tid(-1);
    PingOperation po(codec, tid, tcpTransport);
    return po.execute();
}

}}} // namespace infinispan::hotrod::transport
