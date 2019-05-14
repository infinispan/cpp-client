#include <hotrod/impl/transport/tcp/SSLTcpTransport.h>
#include <hotrod/impl/transport/tcp/SChannelTcpTransport.h>
#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/operations/AuthOperation.h"
#include "hotrod/impl/operations/AuthMechListOperation.h"
#if !defined _WIN32 && !defined _WIN64
#include <sasl/sasl.h>
#include <sasl/saslplug.h>
#else
#define SECURITY_WIN32
#define IO_BUFFER_SIZE  0x10000
#include <winsock.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <schannel.h>
#include <security.h>
#include <sspi.h>

#endif
#include <string.h>
namespace infinispan {
namespace hotrod {

using protocol::Codec;
using namespace operations;

namespace transport {

#if !defined _WIN32 && !defined _WIN64
void saslfail(int why, const char *what)
{
    // TODO: error handling
    std::string s("Sasl error ");
    s.append(std::to_string(why)).append(" : ").append(what);
    throw HotRodClientException(s);
}
#endif
TransportObjectFactory::TransportObjectFactory(Codec& c, TransportFactory& factory) :
        tcpTransportFactory(factory), codec(c)  {
    /* callbacks we support */
    int r;
    /* initialize the sasl library */
    /* Some tests don't create a tcpTransportFactory */
#if !defined _WIN32 && !defined _WIN64
    r = sasl_client_init(nullptr);
    if (r != SASL_OK)
        saslfail(r, "initializing libsasl");
#endif
}

void logAndThrow(const TcpTransport& t, const std::string& msg) {
    DEBUG(msg.c_str());
#if !defined _WIN32 && !defined _WIN64
    throw TransportException(t.getServerAddress().getHostname(), t.getServerAddress().getPort(), msg, -1);
#else
    std::ostringstream os;
    DWORD errCode = GetLastError();
    os << msg << " Win error code: 0x" << std::hex << errCode << std::endl;
    throw TransportException(t.getServerAddress().getHostname(), t.getServerAddress().getPort(), os.str(), errCode);
#endif
}

const sasl_callback_t* get_auth_callback(unsigned long id, const AuthenticationConfiguration& conf)
{
    for (auto &var: conf.getCallbackHandler())
    {
        if (var.id == id)
            return &var;
    };
    return nullptr;
}

void do_sasl_authentication(Codec& codec, TcpTransport& t, const AuthenticationConfiguration& conf) {
#if !defined _WIN32 && !defined _WIN64
    sasl_conn_t *conn=nullptr;
    int r;
    const char *data=nullptr;
    const char *chosenmech;
    int len=0;

    AuthMechListOperation am(codec, t);
    std::vector<std::string> respOpAm(am.execute());
    const sasl_callback_t *callbacks =
            conf.getCallbackHandler().data();

    r = sasl_client_new("hotrod", conf.getServerFqdn().c_str(), nullptr, nullptr, callbacks, 0, &conn);
    if (r != SASL_OK)
        saslfail(r, "allocating connection state");
    /* SASL_AUTH_EXTERNAL must not be null if mechanisms are loaded via plugin
     * otherwise EXTERNAL plugin will not be loaded. The passed value has no effect since
     * the server will use the CN provided in the certificate.
     */
    sasl_setprop( conn, SASL_AUTH_EXTERNAL, "load_plugin" );
    std::vector<char> resp;
    if (conf.getSaslMechanism().compare("DIGEST-MD5"))
    {
        r = sasl_client_start(conn, conf.getSaslMechanism().c_str(), NULL, &data, (unsigned int *) &len, &chosenmech);
        resp = std::vector<char>(data, data + len);
    }
    else
    {
        r = sasl_client_start(conn, conf.getSaslMechanism().c_str(), NULL, NULL, 0, &chosenmech);
    }
    if (r != SASL_OK && r != SASL_CONTINUE) {
        saslfail(r, "starting SASL negotiation");
    }
    std::vector<char> mech(chosenmech, chosenmech + strlen(chosenmech));
    /* we send up to 3 strings;
     the mechanism chosen, the presence of initial response,
     and optionally the initial response */
    AuthOperation a(codec, t, mech, resp);
    std::vector<char> respOp = a.execute();
    while (r == SASL_CONTINUE) {
        r = sasl_client_step(conn, respOp.data(), respOp.size(), NULL, &data, (unsigned int *) &len);
        std::vector<char> mech(chosenmech, chosenmech + strlen(chosenmech)), resp(data, data + len);
        AuthOperation a(codec, t, mech, resp);
        respOp = a.execute();
    }
    if (r != SASL_OK && r != SASL_CONTINUE) {
        saslfail(r, "performing SASL negotiation");
    }
#else
    SecBufferDesc   OutBuffer, InBuffer;
    SecBuffer       InBuffers[2], OutBuffers[1];
    DWORD           dwSSPIFlags, dwSSPIOutFlags, cbData, cbIoBuffer;
    TimeStamp       tsExpiry;
    SECURITY_STATUS scRet;
    static UCHAR IoBuffer[IO_BUFFER_SIZE];
    SEC_CHAR name[] = "localhost";

    TimeStamp timestamp;
    // Call InitializeSecurityContext.
    CredHandle        hCred;
    CtxtHandle   *hContext, *hContextNext;
    SOCKET            Client_Socket;

    SECURITY_STATUS   ss;
    TimeStamp         Lifetime;
    SecBuffer         OutSecBuff;
    SecBufferDesc     InBuffDesc;
    SecBuffer         InSecBuff;
    ULONG             ContextAttributes;

    ULONG cPackages = 0; PSecPkgInfo pInfo = NULL;

    const char* choosenmech = conf.getSaslMechanism().data();
    std::vector<char> mech(choosenmech, choosenmech + strlen(choosenmech)), resp;

    if (!strcmp("EXTERNAL", choosenmech))
    {
        AuthOperation a(codec, t, mech, resp);
        std::vector<char> respOp(a.execute());
        return;
    }

    SECURITY_STATUS stat = EnumerateSecurityPackages(&cPackages, &pInfo);
    SEC_WINNT_AUTH_IDENTITY credentials;
    memset(&credentials, '\0', sizeof(SEC_WINNT_AUTH_IDENTITY));
    credentials.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
    const char *username;
    unsigned int userLen;
    sasl_secret_t *secret;
    const char *realm;
    unsigned int realmLen;
    auto sasl_cb_user = get_auth_callback(SASL_CB_USER, conf);
    auto sasl_cb_pass = get_auth_callback(SASL_CB_PASS, conf);
    auto sasl_cb_realm = get_auth_callback(SASL_CB_GETREALM, conf);
    if (sasl_cb_user && sasl_cb_pass && sasl_cb_realm)
    {
        ((int(*)(void *, int, const char**, unsigned*))sasl_cb_user->proc)(sasl_cb_user->context, sasl_cb_user->id, &username, &userLen);
        ((int(*)(void *, void*, int, sasl_secret_t**))sasl_cb_pass->proc)(nullptr, sasl_cb_pass->context, sasl_cb_pass->id, &secret);
        ((int(*)(void *, int, const char**, unsigned*))sasl_cb_realm->proc)(sasl_cb_realm->context, sasl_cb_realm->id, &realm, &realmLen);
        credentials.User = (unsigned char*)username;
        credentials.UserLength = userLen;
        credentials.Password = (unsigned char*)secret->data;
        credentials.PasswordLength = secret->len;
        credentials.Domain = (unsigned char*)realm;
        credentials.DomainLength = realmLen;
        ss = AcquireCredentialsHandle(NULL, "WDigest", SECPKG_CRED_OUTBOUND, NULL, &credentials, NULL, NULL, &hCred, &tsExpiry);
        if (ss!=SEC_E_OK) {
            std::ostringstream os;
            os << "AcquireCredentialsHandle() fails (return value is 0x" << std::hex << ss << ")"; 
            logAndThrow(t, os.str());
        }
    }
    OutBuffers[0].pvBuffer = NULL;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer = 0;

    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    InBuffers[0].pvBuffer = IoBuffer;
    InBuffers[0].cbBuffer = 0x08;
    InBuffers[0].BufferType = SECBUFFER_TOKEN;
    std::string targetName("hotrod/");
    targetName.append(conf.getServerFqdn());
    hContext = new CtxtHandle();
    memset(hContext, 0, sizeof(*hContext));
    scRet = InitializeSecurityContext(&hCred,
        NULL,
        &targetName[0],
        ISC_REQ_ALLOCATE_MEMORY,
        0,
        SECURITY_NATIVE_DREP,
        NULL,
        0,
        hContext,
        &OutBuffer,
        &dwSSPIOutFlags,
        &tsExpiry);
    if (!strcmp("PLAIN", choosenmech))
    {
        resp.insert(resp.end(), username, username + userLen + 1);
        resp.insert(resp.end(), username, username + userLen + 1);
        resp.insert(resp.end(), (char*)secret->data, ((char*)secret->data) + secret->len);
        AuthOperation a(codec, t, mech, resp);
        std::vector<char> respOp(a.execute());
        return;
    }
    AuthOperation a(codec, t, mech, resp);
    std::vector<char> respOp(a.execute());
    memcpy(IoBuffer, respOp.data(), respOp.size());
    InBuffers[0].pvBuffer = IoBuffer;
    InBuffers[0].cbBuffer = respOp.size();
    InBuffers[0].BufferType = SECBUFFER_TOKEN;
    InBuffer.cBuffers = 1;
    InBuffer.pBuffers = InBuffers;
    InBuffer.ulVersion = SECBUFFER_VERSION;
    while (scRet == 0x90312)
    {
        scRet = InitializeSecurityContext(&hCred,
            hContext,
            &targetName[0],
            ISC_REQ_ALLOCATE_MEMORY,
            0,
            SECURITY_NATIVE_DREP,
            &InBuffer,
            0,
            NULL,
            &OutBuffer,
            &dwSSPIOutFlags,
            &tsExpiry);
        std::vector<char> mech(choosenmech, choosenmech + strlen(choosenmech)), resp((char*)OutBuffers[0].pvBuffer, (char*)OutBuffers[0].pvBuffer + OutBuffers[0].cbBuffer);
        AuthOperation a(codec, t, mech, resp);
        std::vector<char> respOp(a.execute());
        memcpy(IoBuffer, respOp.data(), respOp.size());
        InBuffers[0].pvBuffer = IoBuffer;
        InBuffers[0].cbBuffer = respOp.size();
        InBuffers[0].BufferType = SECBUFFER_TOKEN;
    }
#endif
}

TcpTransport& TransportObjectFactory::makeObject(const InetSocketAddress& address) {
    if(tcpTransportFactory.isSslEnabled()) {
#if !defined _WIN32 && !defined _WIN64
       TcpTransport *t = (new SSLTcpTransport(address, tcpTransportFactory,
                tcpTransportFactory.getSslServerCAPath(), tcpTransportFactory.getSslServerCAFile(), tcpTransportFactory.getSslClientCertificateFile(), tcpTransportFactory.getSniHostName()));
       if (tcpTransportFactory.getConfiguration().getSecurityConfiguration().getAuthenticationConfiguration().isEnabled())
       {
          do_sasl_authentication(codec, *t, tcpTransportFactory.getConfiguration().getSecurityConfiguration().getAuthenticationConfiguration());
       }
       return *t;
#else
       TcpTransport *t =  (new SChannelTcpTransport(address, tcpTransportFactory,
	            tcpTransportFactory.getSslServerCAPath(), tcpTransportFactory.getSslServerCAFile(), tcpTransportFactory.getSslClientCertificateFile(), tcpTransportFactory.getSniHostName()));
       if (tcpTransportFactory.getConfiguration().getSecurityConfiguration().getAuthenticationConfiguration().isEnabled())
       {
          do_sasl_authentication(codec, *t, tcpTransportFactory.getConfiguration().getSecurityConfiguration().getAuthenticationConfiguration());
       }
       return *t;
#endif

	} else {
        TcpTransport* t = new TcpTransport(address, tcpTransportFactory);
        if (tcpTransportFactory.getConfiguration().getSecurityConfiguration().getAuthenticationConfiguration().isEnabled())
        {
           do_sasl_authentication(codec, *t,tcpTransportFactory.getConfiguration().getSecurityConfiguration().getAuthenticationConfiguration());
        }
        return *t;
    }
}

bool AbstractObjectFactory::validateObject(const InetSocketAddress& /*address*/, TcpTransport& transport) {
    return ping(transport) == SUCCESS;
}

void TransportObjectFactory::destroyObject(const InetSocketAddress& /*address*/, TcpTransport& transport) {
    transport.destroy();
    delete &transport;
}

void AbstractObjectFactory::activateObject(const InetSocketAddress& /*address*/, TcpTransport& /*transport*/) {
    // empty
}

void AbstractObjectFactory::passivateObject(
    const InetSocketAddress& /*address*/, TcpTransport& /*transport*/)
{
    // empty
}

PingResult TransportObjectFactory::ping(
    TcpTransport& tcpTransport)
{
    Topology tid(-1);
    PingOperation po(codec, tid, tcpTransport, nullptr);
    return po.execute();
}

}}} // namespace infinispan::hotrod::transport
