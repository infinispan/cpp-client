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
				SChannelSocket(const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile, const std::string& _hostName);
				virtual ~SChannelSocket();
				// Inherited via Socket
				static SChannelSocket*  create(const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile, const std::string& _hostName);

				virtual void    connect(const std::string & host, int port, int timeout) override;
				virtual void    close() override;
				virtual void    setTcpNoDelay(bool tcpNoDelay) override;
				virtual void    setTimeout(int timeout) override;
				virtual size_t  read(char *p, size_t n) override;
				virtual void    write(const char *p, size_t n) override;
				virtual int     getSocket() override;

			private:
				static void     displayWinSockError(DWORD ErrCode);
				static void     displayWinVerifyTrustError(DWORD Status);
				static INT      connectToServer(std::string host, int iPortNumber, SOCKET * pSocket);
				static DWORD    verifyServerCertificate(HCERTSTORE hCertStore, PCCERT_CONTEXT pServerCert, std::string host, DWORD dwCertFlags);
				static void     logAndThrow(const std::string& host, const int port, const std::string& msg);

				SECURITY_STATUS readDecrypt(const DWORD bufsize, size_t *read_counter);
				DWORD           encryptSend(size_t len, SecPkgContext_StreamSizes Sizes);
				SECURITY_STATUS clientHandshakeLoop(BOOL doInitialRead, SecBuffer  *pExtraData);
				SECURITY_STATUS performClientHandshake(std::string host, SecBuffer  *pExtraData);
                LONG DisconnectFromServer();
				void            cleanup();

				class SChannelInitializer
				{
				public:
					SChannelInitializer();
					PSecurityFunctionTable g_pSSPI;
				};

				static SChannelInitializer initializer;
                static PCCERT_CONTEXT  pServerContext, pClientContext;
                static HCERTSTORE hMemStore;

				PCCERT_CONTEXT pRemoteCertContext = NULL;
				std::string    m_serverCAPath;
				std::string    m_serverCAFile;
				std::string    m_clientCertificateFile;
                std::string    m_hostName;
				SOCKET            Client_Socket;
				CredHandle        hCred;
				struct _SecHandle hCtxt;
				CtxtHandle   hContext;
				PBYTE        pbRBuffer= NULL;
				PBYTE        pbWBuffer= NULL;
				PBYTE        recvBuff= NULL;
				DWORD        cbRBuffer;
				unsigned int ready_bytes=0;
				unsigned int offset_bytes=0;
				boolean      isContextInitialized = false;
				boolean      isCredsInitialized = false;
				boolean      onlyVerified = true;
			};

		}
	}
}
#endif  /* ISPN_HOTROD_SYS_SCHANNELSOCKET_H */
