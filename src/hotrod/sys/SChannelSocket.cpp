#include <fstream>
#include <sstream>

#include "infinispan/hotrod/exceptions.h"
#include "hotrod/sys/Log.h"
#include "SChannelSocket.h"
#include "WinDef.h"
#include "Cryptuiapi.h"

#pragma comment(lib, "Cryptui.lib")


#define IO_BUFFER_SIZE  0x10000

#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "WSock32.Lib")
#pragma comment(lib, "Crypt32.Lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "MSVCRTD.lib")
#pragma comment(lib, "Secur32.lib")

using namespace infinispan::hotrod::sys;
HCERTSTORE SChannelSocket::hMemStore = NULL;
SChannelSocket::SChannelInitializer SChannelSocket::initializer;

SChannelSocket::SChannelInitializer::SChannelInitializer() {
	WSADATA wsaData;
	g_pSSPI=InitSecurityInterface();
	if (g_pSSPI == NULL)
	{
        throw HotRodClientException("InitSecurityInferface: initialization failure");
	}
	if (WSAStartup(0x0101, &wsaData))
	{
        throw HotRodClientException("Could not initialize winsock");
	}
}

SChannelSocket::SChannelSocket(const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile, const std::string& _hostName) :
	m_serverCAPath(_serverCAPath), m_serverCAFile(_serverCAFile), m_clientCertificateFile(_clientCertificateFile), m_hostName(_hostName)
    {}

SChannelSocket::~SChannelSocket()
{
	cleanup();
}

INT SChannelSocket::connectToServer(std::string host, int iPortNumber, SOCKET * pSocket)
{
	SOCKET Socket;
	struct sockaddr_in sin;
	struct hostent *hp;
	auto pszServerName = (LPSTR)host.c_str();
	
	Socket = socket(PF_INET, SOCK_STREAM, 0);
	if (Socket == INVALID_SOCKET)
	{
		ERROR("**** Error %d creating socket\n", WSAGetLastError());
		displayWinSockError(WSAGetLastError());
		return WSAGetLastError();
	}
	sin.sin_family = AF_INET;
	sin.sin_port = htons((u_short)iPortNumber);
	if ((hp = gethostbyname(pszServerName)) == NULL)
	{
		ERROR("**** Error returned by gethostbyname\n");
		displayWinSockError(WSAGetLastError());
		return WSAGetLastError();
	}
	else
	{
		memcpy(&sin.sin_addr, hp->h_addr, 4);
	}
	if (::connect(Socket, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		ERROR("**** Error %d connecting to \"%s\" (%s)\n", WSAGetLastError(), pszServerName, inet_ntoa(sin.sin_addr));
		closesocket(Socket);
		displayWinSockError(WSAGetLastError());
		return WSAGetLastError();
	}
	*pSocket = Socket;
	return SEC_E_OK;
}

SECURITY_STATUS SChannelSocket::clientHandshakeLoop(BOOL doInitialRead, SecBuffer *pExtraData)
{

	SecBufferDesc   OutBuffer, InBuffer;
	SecBuffer       InBuffers[2], OutBuffers[1];
	DWORD           dwSSPIFlags, dwSSPIOutFlags, cbData, cbIoBuffer;
	TimeStamp       tsExpiry;
	SECURITY_STATUS scRet;
	BOOL            fDoRead;

	static UCHAR IoBuffer[IO_BUFFER_SIZE];

	dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT | ISC_REQ_CONFIDENTIALITY |
		ISC_RET_EXTENDED_ERROR | ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_STREAM;

	cbIoBuffer = 0;
	fDoRead = doInitialRead;

	// Loop until the handshake is finished or an error occurs.
	scRet = SEC_I_CONTINUE_NEEDED;
	while (scRet == SEC_I_CONTINUE_NEEDED ||
		scRet == SEC_E_INCOMPLETE_MESSAGE ||
		scRet == SEC_I_INCOMPLETE_CREDENTIALS)
	{
		if (0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE) // Read data from server.
		{
			if (fDoRead)
			{
				cbData = recv(Client_Socket, (PCHAR)IoBuffer + cbIoBuffer, IO_BUFFER_SIZE - cbIoBuffer, 0);
				if (cbData == SOCKET_ERROR)
				{
					ERROR("**** Error %d reading data from server\n", WSAGetLastError());
					scRet = SEC_E_INTERNAL_ERROR;
					break;
				}
				else if (cbData == 0)
				{
					ERROR("**** Server unexpectedly disconnected\n");
					scRet = SEC_E_INTERNAL_ERROR;
					break;
				}
				DEBUG("%d bytes of handshake data received\n", cbData);
				cbIoBuffer += cbData;
			}
			else
				fDoRead = TRUE;
		}

		// Set up the input buffers. Buffer 0 is used to pass in data
		// received from the server. Schannel will consume some or all
		// of this. Leftover data (if any) will be placed in buffer 1 and
		// given a buffer type of SECBUFFER_EXTRA.
		InBuffers[0].pvBuffer = IoBuffer;
		InBuffers[0].cbBuffer = cbIoBuffer;
		InBuffers[0].BufferType = SECBUFFER_TOKEN;

		InBuffers[1].pvBuffer = NULL;
		InBuffers[1].cbBuffer = 0;
		InBuffers[1].BufferType = SECBUFFER_EMPTY;

		InBuffer.cBuffers = 2;
		InBuffer.pBuffers = InBuffers;
		InBuffer.ulVersion = SECBUFFER_VERSION;


		// Set up the output buffers. These are initialized to NULL
		// so as to make it less likely we'll attempt to free random
		// garbage later.
		OutBuffers[0].pvBuffer = NULL;
		OutBuffers[0].BufferType = SECBUFFER_TOKEN;
		OutBuffers[0].cbBuffer = 0;

		OutBuffer.cBuffers = 1;
		OutBuffer.pBuffers = OutBuffers;
		OutBuffer.ulVersion = SECBUFFER_VERSION;

		// Call InitializeSecurityContext.
		scRet = InitializeSecurityContext(&hCred,
			&hContext,
			m_hostName.empty() ? NULL : (LPSTR)m_hostName.c_str(),
			dwSSPIFlags,
			0,
			SECURITY_NATIVE_DREP,
			&InBuffer,
			0,
			NULL,
			&OutBuffer,
			&dwSSPIOutFlags,
			&tsExpiry);


		// If InitializeSecurityContext was successful (or if the error was
		// one of the special extended ones), send the contends of the output
		// buffer to the server.
		if (scRet == SEC_E_OK ||
			scRet == SEC_I_CONTINUE_NEEDED ||
			FAILED(scRet) && (dwSSPIOutFlags & ISC_RET_EXTENDED_ERROR))
		{
			if (OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
			{
				cbData = send(Client_Socket, (const PCHAR)OutBuffers[0].pvBuffer, OutBuffers[0].cbBuffer, 0);
				if (cbData == SOCKET_ERROR || cbData == 0)
				{
				 ERROR("**** Error %d sending data to server\n", WSAGetLastError());
					displayWinSockError(WSAGetLastError());
					initializer.g_pSSPI->FreeContextBuffer(OutBuffers[0].pvBuffer);
					initializer.g_pSSPI->DeleteSecurityContext(&hContext);
					return SEC_E_INTERNAL_ERROR;
				}
				// Free output buffer.
				initializer.g_pSSPI->FreeContextBuffer(OutBuffers[0].pvBuffer);
				OutBuffers[0].pvBuffer = NULL;
			}
		}

		// If InitializeSecurityContext returned SEC_E_INCOMPLETE_MESSAGE,
		// then we need to read more data from the server and try again.
		if (scRet == SEC_E_INCOMPLETE_MESSAGE) continue;

		// If InitializeSecurityContext returned SEC_E_OK, then the
		// handshake completed successfully.
		if (scRet == SEC_E_OK)
		{
			// If the "extra" buffer contains data, this is encrypted application
			// protocol layer stuff. It needs to be saved. The application layer
			// will later decrypt it with DecryptMessage.
			DEBUG("Handshake was successful\n");

			if (InBuffers[1].BufferType == SECBUFFER_EXTRA)
			{
				pExtraData->pvBuffer = LocalAlloc(LMEM_FIXED, InBuffers[1].cbBuffer);
				if (pExtraData->pvBuffer == NULL) 
				{
					ERROR("**** Out of memory (2)\n");
					return SEC_E_INTERNAL_ERROR;
				}

				MoveMemory(pExtraData->pvBuffer,
					IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer),
					InBuffers[1].cbBuffer);
				pExtraData->cbBuffer = InBuffers[1].cbBuffer;
				pExtraData->BufferType = SECBUFFER_TOKEN;
				DEBUG("%d bytes of app data was bundled with handshake data\n", pExtraData->cbBuffer);
			}
			else
			{
				pExtraData->pvBuffer = NULL;
				pExtraData->cbBuffer = 0;
				pExtraData->BufferType = SECBUFFER_EMPTY;
			}
			break; // Bail out to quit
		}

		// Check for fatal error.
		if (FAILED(scRet))
		{
			ERROR("**** Error 0x%x returned by InitializeSecurityContext (2)\n", scRet);
			break;
		}

		// If InitializeSecurityContext returned SEC_I_INCOMPLETE_CREDENTIALS,
		// then the server just requested client authentication.
		if (scRet == SEC_I_INCOMPLETE_CREDENTIALS)
		{
			// Busted. The server has requested client authentication and
			// the credential we supplied didn't contain a client certificate.
			// This function will read the list of trusted certificate
			// authorities ("issuers") that was received from the server
			// and attempt to find a suitable client certificate that
			// was issued by one of these. If this function is successful,
			// then we will connect using the new certificate. Otherwise,
			// we will attempt to connect anonymously (using our current credentials).
			//getNewClientCredentials();
			// Go around again.
			fDoRead = FALSE;
			scRet = SEC_I_CONTINUE_NEEDED;
			continue;
		}

		// Copy any leftover data from the "extra" buffer, and go around again.
		if (InBuffers[1].BufferType == SECBUFFER_EXTRA)
		{
			MoveMemory(IoBuffer, IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer), InBuffers[1].cbBuffer);
			cbIoBuffer = InBuffers[1].cbBuffer;
		}
		else
			cbIoBuffer = 0;
	}

	// Delete the security context in the case of a fatal error.
	if (FAILED(scRet)) DeleteSecurityContext(&hContext);
	return scRet;
}

SECURITY_STATUS SChannelSocket::performClientHandshake(std::string host, SecBuffer* pExtraData)
{
	SecBuffer       OutBuffers[1];
	DWORD           dwSSPIFlags, dwSSPIOutFlags, cbData;
	TimeStamp       tsExpiry;
	SECURITY_STATUS scRet;
	SecBufferDesc   OutBuffer;
	auto pszServerName = (LPSTR)host.c_str();

	dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT | ISC_REQ_CONFIDENTIALITY |
		ISC_RET_EXTENDED_ERROR | ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_STREAM;

	//  Initiate a ClientHello message and generate a token.
	OutBuffers[0].pvBuffer = NULL;
	OutBuffers[0].BufferType = SECBUFFER_TOKEN;
	OutBuffers[0].cbBuffer = 0;

	OutBuffer.cBuffers = 1;
	OutBuffer.pBuffers = OutBuffers;
	OutBuffer.ulVersion = SECBUFFER_VERSION;

	scRet = InitializeSecurityContext(&hCred,
		NULL,
		pszServerName,
		dwSSPIFlags,
		0,
		SECURITY_NATIVE_DREP,
		NULL,
		0,
		&hContext,
		&OutBuffer,
		&dwSSPIOutFlags,
		&tsExpiry);

	if (scRet != SEC_I_CONTINUE_NEEDED)
	{ 
		ERROR("**** Error %d returned by InitializeSecurityContext (1)\n", scRet);
		return scRet;
	}

	// Send response to server if there is one.
	if (OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
	{
		cbData = send(Client_Socket, (const PCHAR)OutBuffers[0].pvBuffer, OutBuffers[0].cbBuffer, 0);
		if (cbData == SOCKET_ERROR || cbData == 0)
		{
			ERROR("**** Error %d sending data to server (1)\n", WSAGetLastError());
			FreeContextBuffer(OutBuffers[0].pvBuffer);
			DeleteSecurityContext(&hContext);
			return SEC_E_INTERNAL_ERROR;
		}
		DEBUG("%d bytes of handshake data sent\n", cbData);
		//{ PrintHexDump(cbData, OutBuffers[0].pvBuffer); printf("\n"); }
		FreeContextBuffer(OutBuffers[0].pvBuffer); // Free output buffer.
		OutBuffers[0].pvBuffer = NULL;
	}
	return clientHandshakeLoop(TRUE, pExtraData);
}

void SChannelSocket::displayWinVerifyTrustError(DWORD Status)
{
	LPSTR pszName = NULL;

	switch (Status)
	{
	case CERT_E_EXPIRED:                pszName = "CERT_E_EXPIRED";                 break;
	case CERT_E_VALIDITYPERIODNESTING:  pszName = "CERT_E_VALIDITYPERIODNESTING";   break;
	case CERT_E_ROLE:                   pszName = "CERT_E_ROLE";                    break;
	case CERT_E_PATHLENCONST:           pszName = "CERT_E_PATHLENCONST";            break;
	case CERT_E_CRITICAL:               pszName = "CERT_E_CRITICAL";                break;
	case CERT_E_PURPOSE:                pszName = "CERT_E_PURPOSE";                 break;
	case CERT_E_ISSUERCHAINING:         pszName = "CERT_E_ISSUERCHAINING";          break;
	case CERT_E_MALFORMED:              pszName = "CERT_E_MALFORMED";               break;
	case CERT_E_UNTRUSTEDROOT:          pszName = "CERT_E_UNTRUSTEDROOT";           break;
	case CERT_E_CHAINING:               pszName = "CERT_E_CHAINING";                break;
	case TRUST_E_FAIL:                  pszName = "TRUST_E_FAIL";                   break;
	case CERT_E_REVOKED:                pszName = "CERT_E_REVOKED";                 break;
	case CERT_E_UNTRUSTEDTESTROOT:      pszName = "CERT_E_UNTRUSTEDTESTROOT";       break;
	case CERT_E_REVOCATION_FAILURE:     pszName = "CERT_E_REVOCATION_FAILURE";      break;
	case CERT_E_CN_NO_MATCH:            pszName = "CERT_E_CN_NO_MATCH";             break;
	case CERT_E_WRONG_USAGE:            pszName = "CERT_E_WRONG_USAGE";             break;
	default:                            pszName = "(unknown)";                      break;
	}
	ERROR("Error 0x%x (%s) returned by CertVerifyCertificateChainPolicy!\n", Status, pszName);
}

DWORD CustomVerifyCertificate(HCERTSTORE hCertStore, PCCERT_CONTEXT pSubjectContext) {
    DWORD           dwFlags;
    PCCERT_CONTEXT  pIssuerContext;

    if (!(pSubjectContext = CertDuplicateCertificateContext(pSubjectContext)))
        return FALSE;
    do {
        dwFlags = CERT_STORE_REVOCATION_FLAG | CERT_STORE_SIGNATURE_FLAG |
            CERT_STORE_TIME_VALIDITY_FLAG;
        pIssuerContext = CertGetIssuerCertificateFromStore(hCertStore,
            pSubjectContext, 0, &dwFlags);
        CertFreeCertificateContext(pSubjectContext);
        if (pIssuerContext) {
            pSubjectContext = pIssuerContext;
            if (dwFlags & CERT_STORE_NO_CRL_FLAG)
                dwFlags &= ~(CERT_STORE_NO_CRL_FLAG | CERT_STORE_REVOCATION_FLAG);
            if (dwFlags) break;
        }
        else
        {
            DWORD lastErr = GetLastError();
            PCCERT_CONTEXT  pSelfContext = NULL;
            if (lastErr == CRYPT_E_SELF_SIGNED)
            {
                pSelfContext = CertGetSubjectCertificateFromStore(hCertStore, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, pSubjectContext->pCertInfo);
                lastErr = GetLastError();
                if (pSelfContext == NULL)
                {
                    return CERT_E_UNTRUSTEDROOT;
                }
                return SEC_E_OK;
            }
            else
                return lastErr;
        }
    } while (pIssuerContext);
    return FALSE;
}

DWORD SChannelSocket::verifyServerCertificate(HCERTSTORE hCertStore, PCCERT_CONTEXT pServerCert, std::string host, DWORD dwCertFlags)
{
	HTTPSPolicyCallbackData  polHttps;
	CERT_CHAIN_POLICY_PARA   PolicyPara;
	CERT_CHAIN_POLICY_STATUS PolicyStatus;
	CERT_CHAIN_PARA          ChainPara;
	PCCERT_CHAIN_CONTEXT     pChainContext = NULL;
	DWORD                    cchServerName, Status;
	LPSTR rgszUsages[] = { szOID_PKIX_KP_SERVER_AUTH,
		szOID_SERVER_GATED_CRYPTO,
		szOID_SGC_NETSCAPE };
	auto pszServerName = (PSTR)host.c_str();
	DWORD cUsages = sizeof(rgszUsages) / sizeof(LPSTR);
	PWSTR   pwszServerName = NULL;

    if (pServerCert == NULL)
	{
		if (pChainContext)  CertFreeCertificateChain(pChainContext);
		return SEC_E_WRONG_PRINCIPAL;
	}

	// Convert server name to unicode.
	if (pszServerName == NULL || strlen(pszServerName) == 0)
	{
		return SEC_E_WRONG_PRINCIPAL;
	}
    if (hCertStore != NULL)
    {
        Status=CustomVerifyCertificate(hCertStore, pServerCert);
        if (Status != SEC_E_OK)
        {
            displayWinVerifyTrustError(Status);
        }
        return Status;
    }
    else
    {
        cchServerName = MultiByteToWideChar(CP_ACP, 0, pszServerName, -1, NULL, 0);
        pwszServerName = (PWSTR)LocalAlloc(LMEM_FIXED, cchServerName * sizeof(WCHAR));
        if (pwszServerName == NULL)
        {
            return SEC_E_INSUFFICIENT_MEMORY;
        }

        cchServerName = MultiByteToWideChar(CP_ACP, 0, pszServerName, -1, pwszServerName, cchServerName);
        if (cchServerName == 0)
        {
            if (pwszServerName) LocalFree(pwszServerName);
            return SEC_E_WRONG_PRINCIPAL;
        }

        // Build certificate chain.
        ZeroMemory(&ChainPara, sizeof(ChainPara));
        ChainPara.cbSize = sizeof(ChainPara);
        ChainPara.RequestedUsage.dwType = USAGE_MATCH_TYPE_OR;
        ChainPara.RequestedUsage.Usage.cUsageIdentifier = cUsages;
        ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = rgszUsages;

        if (!CertGetCertificateChain(NULL,
            pServerCert,
            NULL,
            pServerCert->hCertStore,
            &ChainPara,
            0,
            NULL,
            &pChainContext))
        {
            Status = GetLastError();
            ERROR("Error 0x%x returned by CertGetCertificateChain!\n", Status);
            if (pChainContext)  CertFreeCertificateChain(pChainContext);
            if (pwszServerName) LocalFree(pwszServerName);
            return Status;
        }

        // Validate certificate chain.
        ZeroMemory(&polHttps, sizeof(HTTPSPolicyCallbackData));
        polHttps.cbStruct = sizeof(HTTPSPolicyCallbackData);
        polHttps.dwAuthType = AUTHTYPE_SERVER;
        polHttps.fdwChecks = dwCertFlags;
        polHttps.pwszServerName = pwszServerName;

        memset(&PolicyPara, 0, sizeof(PolicyPara));
        PolicyPara.cbSize = sizeof(PolicyPara);
        PolicyPara.pvExtraPolicyPara = &polHttps;

        memset(&PolicyStatus, 0, sizeof(PolicyStatus));
        PolicyStatus.cbSize = sizeof(PolicyStatus);

        if (!CertVerifyCertificateChainPolicy(CERT_CHAIN_POLICY_SSL,
            pChainContext,
            &PolicyPara,
            &PolicyStatus))
        {
            Status = GetLastError();
            ERROR("Error 0x%x returned by CertVerifyCertificateChainPolicy!\n", Status);
            if (pChainContext)  CertFreeCertificateChain(pChainContext);
            if (pwszServerName) LocalFree(pwszServerName);
            return Status;
        }

        if (PolicyStatus.dwError)
        {
            Status = PolicyStatus.dwError;
            displayWinVerifyTrustError(Status);
            if (pChainContext)  CertFreeCertificateChain(pChainContext);
            if (pwszServerName) LocalFree(pwszServerName);
            return Status;
        }

        Status = SEC_E_OK;
        if (pChainContext)  CertFreeCertificateChain(pChainContext);
        if (pwszServerName) LocalFree(pwszServerName);
        return Status;
    }
}

bool is_pem_filename(const std::string &str)
{
    static const std::string &suffix = ".pem";
    return str.size() >= 4 &&
        str.compare(str.size() - 4, 4, suffix) == 0;
}

void SChannelSocket::setupCertStoreServer()
{
    HANDLE           hFile;
    char             servCert[8192];
    DWORD            readLen;
    BYTE             derServCert[8192];
    DWORD            derCertLen = 8192;
    PCCERT_CONTEXT   pServerContext = NULL;
    // Read the user provided file with the certificate to validate the server 
    hFile = CreateFile(m_serverCAFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "**** Error %d. Failed to open server certificate file %s.\n", GetLastError(), m_serverCAFile.c_str());
    }

    if (!ReadFile(hFile, servCert, 8192, &readLen, NULL))
    {
        fprintf(stderr, "**** Error %d. Failed to open read certificate file %s.\n", GetLastError(), m_serverCAFile.c_str());
    }
    CloseHandle(hFile);
    
    // Build the certificates store
    // .pem if pem all others are supposed to be p12 pfx
    if (is_pem_filename(m_serverCAFile.c_str()))
    {
        if (!CryptStringToBinary(servCert, readLen, CRYPT_STRING_BASE64_ANY, derServCert, &derCertLen, NULL, NULL))
        {
            printf("**** Error 0x%x returned by CryptStringToBinary server cert\n", GetLastError());
            logAndThrow("ERROR");
        }
        pServerContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
            (BYTE*)derServCert,
            derCertLen);
        if (pServerContext == NULL)
        {
            printf("**** Error 0x%x returned by CertCreateCertificateContext. Cannot create certificate. File corrupted?\n", GetLastError());
            logAndThrow("ERROR");
        }
        if (!(SChannelSocket::hMemStore = CertOpenStore(
            CERT_STORE_PROV_MEMORY,   // The memory provider type
            0,                        // The encoding type is not needed
            NULL,                     // Use the default HCRYPTPROV
            0,                        // Accept the default dwFlags
            NULL                      // pvPara is not used
        )))
        {
            printf("**** Error 0x%x returned by CertOpenStore\n", GetLastError());
            logAndThrow("ERROR");
        }
        if (!CertAddCertificateContextToStore(SChannelSocket::hMemStore, pServerContext,
            CERT_STORE_ADD_REPLACE_EXISTING,
            NULL
        ))
        {
            printf("**** Error 0x%x returned by CertAddCertificateContextToStore\n", GetLastError());
            logAndThrow("ERROR");
        }
    }
    else
    {
        // p12 can be imported directly as certstore
        CRYPT_DATA_BLOB sblob;
        sblob.cbData = (DWORD)readLen;
        sblob.pbData = (BYTE*)servCert;

        // Importing the cert. The file cannot be password protected
        HCERTSTORE servCertStore = PFXImportCertStore(&sblob, L"", CRYPT_MACHINE_KEYSET);
        if (servCertStore == NULL) {
            printf("PFXImportCertStore failed. hr=0x%x\n", GetLastError());
            logAndThrow("ERROR");
        }
    }
}

void SChannelSocket::setupCertClient(SCHANNEL_CRED& schannelCred)
{
    // User provided the certificate to validate the client against the server
    // Read it and build certificate and set credentials for the schannel
    HANDLE           hFile;
    BYTE             clientCertStore[8192];
    DWORD            clientCertStoreLen = 8192;
    CRYPT_DATA_BLOB blob;
    PCCERT_CONTEXT pClientContext;

    // User provided the certificate to validate the server in a file
    // Read it and build certificate and certificates store
    hFile = CreateFile(m_clientCertificateFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "**** Error %d. Failed to open server certificate file %s.\n", GetLastError(), m_clientCertificateFile.c_str());
    }

    if (!ReadFile(hFile, clientCertStore, 8192, &clientCertStoreLen, NULL))
    {
        fprintf(stderr, "**** Error %d. Failed to open read certificate file %s.\n", GetLastError(), m_clientCertificateFile.c_str());
    }
    CloseHandle(hFile);
    blob.cbData = (DWORD)clientCertStoreLen;
    blob.pbData = clientCertStore;

    HCERTSTORE certStore = PFXImportCertStore(&blob, L"", CRYPT_EXPORTABLE);
    if (certStore == NULL) {
        printf("PFXImportCertStore failed. hr=0x%x\n", GetLastError());
        logAndThrow("ERROR");
    }

    pClientContext = CertFindCertificateInStore(certStore, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING
        , 0, CERT_FIND_ANY, NULL, NULL);

    if (pClientContext == NULL)
    {
        printf("**** Error 0x%x returned by CertCreateCertificateContext. Cannot create certificate. File corrupted?\n", GetLastError());
        logAndThrow("ERROR");
    }
    schannelCred.cCreds = 1;
    schannelCred.paCred = &pClientContext;
}

void SChannelSocket::connect(const std::string & host, int port, int timeout)
{
    SCHANNEL_CRED    SchannelCred;
    DWORD            dwProtocol = SP_PROT_TLS1;
    ALG_ID           aiKeyExch = 0;
    SECURITY_STATUS  Status;
    DWORD            cSupportedAlgs = 0;
    ALG_ID           rgbSupportedAlgs[16];
    TimeStamp        tsExpiry;
    SecBuffer        ExtraData;

    m_host = host;
    m_port = port;
    // Setup server certificate data
    if (hMemStore==NULL && !m_serverCAFile.empty())
    {
        setupCertStoreServer();
    }

    ZeroMemory(&SchannelCred, sizeof(SchannelCred));
    SchannelCred.dwVersion = SCHANNEL_CRED_VERSION;

    // Setup client certificate data
    if (!m_clientCertificateFile.empty())
    {
        setupCertClient(SchannelCred);
    }

    SchannelCred.grbitEnabledProtocols = dwProtocol;
    if (aiKeyExch) rgbSupportedAlgs[cSupportedAlgs++] = aiKeyExch;

    if (cSupportedAlgs)
    {
        SchannelCred.cSupportedAlgs = cSupportedAlgs;
        SchannelCred.palgSupportedAlgs = rgbSupportedAlgs;
    }

    SchannelCred.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS;
    SchannelCred.dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION;
    if (!m_hostName.empty())
    {
        SchannelCred.dwFlags |= SCH_CRED_SNI_CREDENTIAL;
    }

    // Create an SSPI credential.
    Status = AcquireCredentialsHandle(NULL,                 // Name of principal    
        UNISP_NAME_A,         // Name of package
        SECPKG_CRED_OUTBOUND, // Flags indicating use
        NULL,                 // Pointer to logon ID
        &SchannelCred,        // Package specific data
        NULL,                 // Pointer to GetKey() func
        NULL,                 // Value to pass to GetKey()
        &hCred,              // (out) Cred Handle
        &tsExpiry);          // (out) Lifetime (optional)
    if (Status != SEC_E_OK)
    {
        ERROR("**** Error 0x%x returned by AcquireCredentialsHandle\n", Status);
    }

    Client_Socket = INVALID_SOCKET;
    // Setup the socket
    connectToServer(host, port, &Client_Socket);
    // Setup security
    performClientHandshake(m_hostName.empty() ? m_host : m_hostName, &ExtraData);
    isContextInitialized = true;
    // Authenticate server's credentials. Get server's certificate.
    Status = QueryContextAttributes(&hContext, SECPKG_ATTR_REMOTE_CERT_CONTEXT, (PVOID)&pRemoteCertContext);
    if (Status != SEC_E_OK)
    {
        cleanup();
        ERROR("Error 0x%x querying remote certificate\n", Status);
        logAndThrow("Error querying remote certificate\n");
    } 
    // Attempt to validate server certificate.
    if (this->onlyVerified)
    {
        Status = verifyServerCertificate(hMemStore, pRemoteCertContext, host, 0x00001000  /*SECURITY_FLAG_IGNORE_CERT_CN_INVALID*/);
        if (Status)
        {
            ERROR("**** Error 0x%x the server certificate did not validate correctly.\n", Status);
            cleanup();
            logAndThrow("**** The server certificate did not validate correctly.\n");
        }
    }
    CertFreeCertificateContext(pRemoteCertContext);
    pRemoteCertContext = NULL;
    DEBUG("----- Server certificate context released \n");
    SecPkgContext_StreamSizes Sizes;
    QueryContextAttributes(&hContext, SECPKG_ATTR_STREAM_SIZES, &Sizes);
    auto cbIoBufferLength = Sizes.cbHeader + Sizes.cbMaximumMessage + Sizes.cbTrailer;
    pbRBuffer = (PBYTE)LocalAlloc(LMEM_FIXED, cbIoBufferLength);
    pbWBuffer = (PBYTE)LocalAlloc(LMEM_FIXED, cbIoBufferLength);
    recvBuff = (PBYTE)LocalAlloc(LMEM_FIXED, cbIoBufferLength);
    this->cbRBuffer = 0;
    if (ExtraData.cbBuffer > 0)
    {
        MoveMemory(recvBuff, ExtraData.pvBuffer, ExtraData.cbBuffer);
        this->cbRBuffer = ExtraData.cbBuffer;
        LocalFree(ExtraData.pvBuffer);
    }
}

void SChannelSocket::close()
{
	cleanup();
}

void SChannelSocket::setTcpNoDelay(bool tcpNoDelay)
{
}

void SChannelSocket::setTimeout(int timeout)
{
}

void throwIOErr(const std::string& host, int port, const PCHAR msg, int errnum) {
	std::ostringstream m;
	m << msg;
	m << " (host: " << host;
	m << " port: " << port << ")";

	if (errnum != 0) {
		CHAR buf[200];
		if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errnum, 0, buf, 200, NULL) != 0) {
			m << " " << buf;
		}
		else {
			m << " Unknown WSA error " << errnum;
		}
	}
	throw std::exception(m.str().c_str());
}

SECURITY_STATUS SChannelSocket::readDecrypt(const DWORD bufsize, size_t *read_counter)
{
	SecBuffer          ExtraBuffer;
	SecBuffer          *pDataBuffer, *pExtraBuffer;

	SECURITY_STATUS    scRet;
	SecBufferDesc      Message;
	SecBuffer          Buffers[4];
	DWORD              cbData;
	int i;

	// Read data from server until done.
	*read_counter = 0;
	scRet = SEC_E_INCOMPLETE_MESSAGE;
	do
	{
		if (cbRBuffer == 0 || scRet == SEC_E_INCOMPLETE_MESSAGE) // get the data
		{
			cbData = recv(Client_Socket, (PCHAR) recvBuff, bufsize, 0);
			if (cbData == SOCKET_ERROR)
			{
				ERROR("**** Error %d reading data from server\n", WSAGetLastError());
				scRet = SEC_E_INTERNAL_ERROR;
				break;
			}
			else if (cbData == 0) // Server disconnected.
			{
				if (cbRBuffer)
				{
					ERROR("**** Server unexpectedly disconnected\n");
					scRet = SEC_E_INTERNAL_ERROR;
					return scRet;
				}
				else
					break; // All Done
			}
			else // success
			{
				DEBUG("%d bytes of (encrypted) application data received\n", cbData);
				cbRBuffer += cbData;
			}
		}

		// Decrypt the received data.
		Buffers[0].pvBuffer = recvBuff;
		Buffers[0].cbBuffer = cbRBuffer;
		Buffers[0].BufferType = SECBUFFER_DATA;             // Initial Type of the buffer 1
		Buffers[1].BufferType = SECBUFFER_EMPTY;            // Initial Type of the buffer 2
		Buffers[2].BufferType = SECBUFFER_EMPTY;            // Initial Type of the buffer 3
		Buffers[3].BufferType = SECBUFFER_EMPTY;            // Initial Type of the buffer 4

		Message.ulVersion = SECBUFFER_VERSION;              // Version number
		Message.cBuffers = 4;                               // Number of buffers - must contain four SecBuffer structures.
		Message.pBuffers = Buffers;                         // Pointer to array of buffers
		scRet = initializer.g_pSSPI->DecryptMessage(&hContext, &Message, 0, NULL);
		if (scRet == SEC_I_CONTEXT_EXPIRED) break;          // Server signalled end of session
		if (scRet != SEC_E_OK &&
			scRet != SEC_I_RENEGOTIATE &&
			scRet != SEC_I_CONTEXT_EXPIRED)
		{
			ERROR("**** Error DecryptMessage ");
			//DisplaySECError((DWORD)scRet);
			return scRet;
		}

		// Locate data and (optional) extra buffers.
		pDataBuffer = NULL;
		pExtraBuffer = NULL;
		for (i = 0; i < 4; i++)
		{
			if (pDataBuffer == NULL && Buffers[i].BufferType == SECBUFFER_DATA) pDataBuffer = &Buffers[i];
			if (pExtraBuffer == NULL && Buffers[i].BufferType == SECBUFFER_EXTRA) pExtraBuffer = &Buffers[i];
		}

		MoveMemory(pbRBuffer+*read_counter, pDataBuffer->pvBuffer, pDataBuffer->cbBuffer);
		*read_counter += pDataBuffer->cbBuffer;

		// Move any "extra" data to the input buffer.
		if (pExtraBuffer)
		{
			MoveMemory(recvBuff, pExtraBuffer->pvBuffer, pExtraBuffer->cbBuffer);
			cbRBuffer = pExtraBuffer->cbBuffer; // printf("cbIoBuffer= %d  \n", cbIoBuffer);

		}
		else
			cbRBuffer = 0;

		// The server wants to perform another handshake sequence.
		if (scRet == SEC_I_RENEGOTIATE)
        {
            printf("Server requested renegotiate!\n");
            scRet = clientHandshakeLoop(FALSE, &ExtraBuffer);
            if (scRet != SEC_E_OK) return scRet;

            if (ExtraBuffer.pvBuffer) // Move any "extra" data to the input buffer.
            {
                MoveMemory(pbRBuffer, ExtraBuffer.pvBuffer, ExtraBuffer.cbBuffer);
                cbRBuffer = ExtraBuffer.cbBuffer;
            }
        }
	} 
	while (scRet == SEC_E_INCOMPLETE_MESSAGE || cbRBuffer>0);
	return SEC_E_OK;
}

DWORD SChannelSocket::encryptSend(size_t len, SecPkgContext_StreamSizes Sizes)
{
	SECURITY_STATUS			scRet;				// unsigned long cbBuffer;    // Size of the buffer, in bytes
	SecBufferDesc			Message;			// unsigned long BufferType;  // Type of the buffer (below)
	SecBuffer               Buffers[4];			// void    SEC_FAR * pvBuffer;   // Pointer to the buffer
	DWORD                   cbMessage, cbData;
	PBYTE                   pbMessage;

	pbMessage = pbWBuffer + Sizes.cbHeader;	// Offset by "header size"
	cbMessage = (DWORD)len;

	// Encrypt the HTTP request.
	Buffers[0].pvBuffer = pbWBuffer;                            // Pointer to buffer 1
	Buffers[0].cbBuffer = Sizes.cbHeader;                        // length of header
	Buffers[0].BufferType = SECBUFFER_STREAM_HEADER;		     // Type of the buffer

	Buffers[1].pvBuffer = pbMessage;                             // Pointer to buffer 2
	Buffers[1].cbBuffer = cbMessage;                             // length of the message
	Buffers[1].BufferType = SECBUFFER_DATA;                      // Type of the buffer

	Buffers[2].pvBuffer = pbMessage + cbMessage;         // Pointer to buffer 3
	Buffers[2].cbBuffer = Sizes.cbTrailer;               // length of the trailor
	Buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;    // Type of the buffer

	Buffers[3].pvBuffer = SECBUFFER_EMPTY;                    // Pointer to buffer 4
	Buffers[3].cbBuffer = SECBUFFER_EMPTY;                    // length of buffer 4
	Buffers[3].BufferType = SECBUFFER_EMPTY;                  // Type of the buffer 4


	Message.ulVersion = SECBUFFER_VERSION;    // Version number
	Message.cBuffers = 4;                     // Number of buffers - must contain four SecBuffer structures.
	Message.pBuffers = Buffers;               // Pointer to array of buffers
	scRet = initializer.g_pSSPI->EncryptMessage(&hContext, 0, &Message, 0); // must contain four SecBuffer structures.
	if (FAILED(scRet)) 
	{ 
		printf("**** Error 0x%x returned by EncryptMessage\n", scRet); return scRet;
	}

	// Send the encrypted data to the server.                                            len                                                                         flags
	cbData = send(Client_Socket, (const PCHAR)pbWBuffer, Buffers[0].cbBuffer + Buffers[1].cbBuffer + Buffers[2].cbBuffer, 0);

	DEBUG("%d bytes of encrypted data sent\n", cbData);
	return cbData; 
}

void SChannelSocket::write(const char *p, size_t length)
{
	SecPkgContext_StreamSizes Sizes;
	initializer.g_pSSPI->QueryContextAttributes(&hContext, SECPKG_ATTR_STREAM_SIZES, &Sizes);
	auto cbIoBufferLength = Sizes.cbHeader + Sizes.cbMaximumMessage + Sizes.cbTrailer;
	memcpy(pbWBuffer+Sizes.cbHeader, p, length);
	encryptSend(length, Sizes);
}

size_t SChannelSocket::read(char *p, size_t length)
{
	unsigned int count = this->ready_bytes - this->offset_bytes;
	// On empty buffer read bytes from TLS
	if (count <= 0)
	{
		SecPkgContext_StreamSizes Sizes;
		size_t read_counter;
		initializer.g_pSSPI->QueryContextAttributes(&hContext, SECPKG_ATTR_STREAM_SIZES, &Sizes);
		auto cbIoBufferLength = Sizes.cbHeader + Sizes.cbMaximumMessage + Sizes.cbTrailer;
		readDecrypt(cbIoBufferLength, &read_counter);
		offset_bytes = 0;
		count=ready_bytes = read_counter;
	}
	// Copy the bytes into the user buffer
	if (count <= length)
	{
		memcpy(p, pbRBuffer + offset_bytes, count);
		offset_bytes = ready_bytes = 0;
		return count;
	}
	else
	{
		memcpy(p, pbRBuffer + offset_bytes, length);
		offset_bytes+=length;
		return length;
	}
}

int SChannelSocket::getSocket()
{
	return 0;
}

SChannelSocket* SChannelSocket::create(const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile, const std::string& _hostName) {
	return new SChannelSocket(_serverCAPath, _serverCAFile, _clientCertificateFile, _hostName);
}

void SChannelSocket::logAndThrow(const std::string& msg) {
	DEBUG(msg.c_str());
	throw TransportException(m_host, m_port, msg, -1);
}

/*****************************************************************************/
LONG SChannelSocket::DisconnectFromServer()
{
    PBYTE                    pbMessage;
    DWORD                    dwType, dwSSPIFlags, dwSSPIOutFlags, cbMessage, cbData, Status;
    SecBufferDesc OutBuffer;
    SecBuffer     OutBuffers[1];
    TimeStamp     tsExpiry;


    dwType = SCHANNEL_SHUTDOWN; // Notify schannel that we are about to close the connection.

    OutBuffers[0].pvBuffer = &dwType;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer = sizeof(dwType);

    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    Status = initializer.g_pSSPI->ApplyControlToken(&hContext, &OutBuffer);

    // Build an SSL close notify message.
    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT |
        ISC_REQ_REPLAY_DETECT |
        ISC_REQ_CONFIDENTIALITY |
        ISC_RET_EXTENDED_ERROR |
        ISC_REQ_ALLOCATE_MEMORY |
        ISC_REQ_STREAM;

    OutBuffers[0].pvBuffer = NULL;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer = 0;

    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    Status = initializer.g_pSSPI->InitializeSecurityContextA(&hCred,
        &hContext,
        NULL,
        dwSSPIFlags,
        0,
        SECURITY_NATIVE_DREP,
        NULL,
        0,
        &hContext,
        &OutBuffer,
        &dwSSPIOutFlags,
        &tsExpiry);


    pbMessage = (PBYTE)OutBuffers[0].pvBuffer;
    cbMessage = OutBuffers[0].cbBuffer;


    // Send the close notify message to the server.
    if (pbMessage != NULL && cbMessage != 0 && Client_Socket!=INVALID_SOCKET)
    {
        cbData = send(Client_Socket, (const char *)pbMessage, cbMessage, 0);
        if (cbData == SOCKET_ERROR || cbData == 0)
        {
            Status = WSAGetLastError();
            printf("**** Error %d sending close notify\n", Status);
        }
        initializer.g_pSSPI->FreeContextBuffer(pbMessage); // Free output buffer.
        closesocket(Client_Socket); // Close the socket.
        Client_Socket = INVALID_SOCKET;
    }
    initializer.g_pSSPI->DeleteSecurityContext(&hContext); // Free the security context.
    return Status;
}


void SChannelSocket::cleanup()
{
    // Send notify close
    DisconnectFromServer();
	// Free the server certificate context.
	if (pRemoteCertContext)
	{
		CertFreeCertificateContext(pRemoteCertContext);
		pRemoteCertContext = NULL;
	}

	// Free SSPI context handle.
	if (isContextInitialized)
	{
		initializer.g_pSSPI->DeleteSecurityContext(&hContext);
		isContextInitialized = FALSE;
	}

	// Free SSPI credentials handle.
	if (isCredsInitialized)
	{
		initializer.g_pSSPI->FreeCredentialsHandle(&hCred);
		isCredsInitialized = FALSE;
	}

	// Close socket.
	if (Client_Socket != INVALID_SOCKET) closesocket(Client_Socket);

    // Close "In memory store" certificate store.
    if (hMemStore) CertCloseStore(hMemStore, 0);
    hMemStore = NULL;

    if (recvBuff != NULL) LocalFree(recvBuff);
	if (pbWBuffer != NULL) LocalFree(pbWBuffer);
	if (pbRBuffer != NULL) LocalFree(pbRBuffer);
	recvBuff = pbWBuffer = pbRBuffer = NULL;
}

void SChannelSocket::displayWinSockError(DWORD ErrCode)
{
	LPSTR pszName = NULL;
	switch (ErrCode)
	{
	case     10035:  pszName = "WSAEWOULDBLOCK    "; break;
	case     10036:  pszName = "WSAEINPROGRESS    "; break;
	case     10037:  pszName = "WSAEALREADY       "; break;
	case     10038:  pszName = "WSAENOTSOCK       "; break;
	case     10039:  pszName = "WSAEDESTADDRREQ   "; break;
	case     10040:  pszName = "WSAEMSGSIZE       "; break;
	case     10041:  pszName = "WSAEPROTOTYPE     "; break;
	case     10042:  pszName = "WSAENOPROTOOPT    "; break;
	case     10043:  pszName = "WSAEPROTONOSUPPORT"; break;
	case     10044:  pszName = "WSAESOCKTNOSUPPORT"; break;
	case     10045:  pszName = "WSAEOPNOTSUPP     "; break;
	case     10046:  pszName = "WSAEPFNOSUPPORT   "; break;
	case     10047:  pszName = "WSAEAFNOSUPPORT   "; break;
	case     10048:  pszName = "WSAEADDRINUSE     "; break;
	case     10049:  pszName = "WSAEADDRNOTAVAIL  "; break;
	case     10050:  pszName = "WSAENETDOWN       "; break;
	case     10051:  pszName = "WSAENETUNREACH    "; break;
	case     10052:  pszName = "WSAENETRESET      "; break;
	case     10053:  pszName = "WSAECONNABORTED   "; break;
	case     10054:  pszName = "WSAECONNRESET     "; break;
	case     10055:  pszName = "WSAENOBUFS        "; break;
	case     10056:  pszName = "WSAEISCONN        "; break;
	case     10057:  pszName = "WSAENOTCONN       "; break;
	case     10058:  pszName = "WSAESHUTDOWN      "; break;
	case     10059:  pszName = "WSAETOOMANYREFS   "; break;
	case     10060:  pszName = "WSAETIMEDOUT      "; break;
	case     10061:  pszName = "WSAECONNREFUSED   "; break;
	case     10062:  pszName = "WSAELOOP          "; break;
	case     10063:  pszName = "WSAENAMETOOLONG   "; break;
	case     10064:  pszName = "WSAEHOSTDOWN      "; break;
	case     10065:  pszName = "WSAEHOSTUNREACH   "; break;
	case     10066:  pszName = "WSAENOTEMPTY      "; break;
	case     10067:  pszName = "WSAEPROCLIM       "; break;
	case     10068:  pszName = "WSAEUSERS         "; break;
	case     10069:  pszName = "WSAEDQUOT         "; break;
	case     10070:  pszName = "WSAESTALE         "; break;
	case     10071:  pszName = "WSAEREMOTE        "; break;
	case     10091:  pszName = "WSASYSNOTREADY    "; break;
	case     10092:  pszName = "WSAVERNOTSUPPORTED"; break;
	case     10093:  pszName = "WSANOTINITIALISED "; break;
	case     11001:  pszName = "WSAHOST_NOT_FOUND "; break;
	case     11002:  pszName = "WSATRY_AGAIN      "; break;
	case     11003:  pszName = "WSANO_RECOVERY    "; break;
	case     11004:  pszName = "WSANO_DATA        "; break;
	}
	ERROR("Error 0x%x (%s)\n", ErrCode, pszName);
}





