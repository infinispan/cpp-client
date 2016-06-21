#ifndef ISPN_HOTROD_SYS_SCHANNELSOCKET_H
#define ISPN_HOTROD_SYS_SCHANNELSOCKET_H

#define SECURITY_WIN32

#include <winsock.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <schannel.h>
#include <security.h>
#include <sspi.h>
#include <hotrod/sys/Socket.h>

namespace infinispan {
	namespace hotrod {
		namespace sys {

			class SChannelSocket : public infinispan::hotrod::sys::Socket
			{
			public:
				SChannelSocket(const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile);
				virtual ~SChannelSocket();
				// Inherited via Socket
				virtual void connect(const std::string & host, int port, int timeout) override;
				virtual void close() override;
				virtual void setTcpNoDelay(bool tcpNoDelay) override;
				virtual void setTimeout(int timeout) override;
				virtual size_t read(char * p, size_t n) override;
				virtual void write(const char * p, size_t n) override;
				virtual int getSocket() override;
				static SChannelSocket* create(const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile);
			private:
				Socket *m_socket;
				std::string m_serverCAPath;
				std::string m_serverCAFile;
				std::string m_clientCertificateFile;
				SOCKET            Client_Socket;
				CredHandle        hCred;
				struct _SecHandle hCtxt;
				CtxtHandle hContext;
				PBYTE pbRBuffer;
				PBYTE pbWBuffer;
				unsigned int ready_bytes;
				unsigned int offset_bytes;
				boolean isContextInitialized = false;
				boolean isCredsInitialized = false;
				boolean onlyVerified = false;
				HCERTSTORE hMyCertStore = NULL;
				class SChannelInitializer
				{
				public:
					SChannelInitializer();
					PSecurityFunctionTable g_pSSPI;
				};
				static SChannelInitializer initializer;
				SECURITY_STATUS clientHandshakeLoop(BOOL doInitialRead, SecBuffer *pExtraData);
				void getNewClientCredentials(PSecurityFunctionTable g_pSSPI, CredHandle *phCreds, CtxtHandle *phContext);
				SECURITY_STATUS performClientHandshake(std::string host,SecBuffer *     pExtraData);
				SECURITY_STATUS readDecrypt(PSecurityFunctionTable g_pSSPI, SOCKET Socket, PCredHandle phCreds, CtxtHandle * phContext, PBYTE pbIoBuffer, const DWORD bufsize, size_t *read_counter);
				static DWORD verifyServerCertificate(PCCERT_CONTEXT pServerCert, std::string host, DWORD dwCertFlags);
				void cleanup();
				static void logAndThrow(const std::string& host, const int port, const std::string& msg);
			};

		}
	}
}
#endif  /* ISPN_HOTROD_SYS_SCHANNELSOCKET_H */
