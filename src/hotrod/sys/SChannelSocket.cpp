#include <fstream>
#include <sstream>

#include "infinispan/hotrod/exceptions.h"
#include "hotrod/sys/Log.h"
#include "SChannelSocket.h"
#include "WinDef.h"


#define IO_BUFFER_SIZE  0x10000

#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "WSock32.Lib")
#pragma comment(lib, "Crypt32.Lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "MSVCRTD.lib")
#pragma comment(lib, "Secur32.lib")

using namespace infinispan::hotrod::sys;
HCERTSTORE SChannelSocket::hMyCertStore = NULL;


SChannelSocket::SChannelInitializer SChannelSocket::initializer;

SChannelSocket::SChannelInitializer::SChannelInitializer() {
	WSADATA wsaData;
	g_pSSPI=InitSecurityInterface();
	if (g_pSSPI == NULL)
	{
		logAndThrow("", 0, "InitSecurityInferface: initialization failure");
	}
	if (WSAStartup(0x0101, &wsaData))
	{
		logAndThrow("",0,"Could not initialize winsock");
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

void SChannelSocket::getNewClientCredentials()
{

	CredHandle                        newHCreds;
	SecPkgContext_IssuerListInfoEx    IssuerListInfo;
	PCCERT_CHAIN_CONTEXT              pChainContext;
	CERT_CHAIN_FIND_BY_ISSUER_PARA    FindByIssuerPara;
	PCCERT_CONTEXT                    pCertContext;
	TimeStamp                         tsExpiry;
	SECURITY_STATUS                   Status;
	SCHANNEL_CRED                     SchannelCred;

	// Read list of trusted issuers from schannel.
	Status = initializer.g_pSSPI->QueryContextAttributes(&hContext, SECPKG_ATTR_ISSUER_LIST_EX, (PVOID)&IssuerListInfo);
	if (Status != SEC_E_OK)
	{
		ERROR("Error 0x%x querying issuer list info\n", Status); return;
	}

	// Enumerate the client certificates.
	ZeroMemory(&FindByIssuerPara, sizeof(FindByIssuerPara));

	FindByIssuerPara.cbSize = sizeof(FindByIssuerPara);
	FindByIssuerPara.pszUsageIdentifier = szOID_PKIX_KP_CLIENT_AUTH;
	FindByIssuerPara.dwKeySpec = 0;
	FindByIssuerPara.cIssuer = IssuerListInfo.cIssuers;
	FindByIssuerPara.rgIssuer = IssuerListInfo.aIssuers;
	pChainContext = NULL;
	while (TRUE)
	{   // Find a certificate chain.
		pChainContext = CertFindChainInStore(hMyCertStore,
			X509_ASN_ENCODING,
			0,
			CERT_CHAIN_FIND_BY_ISSUER,
			&FindByIssuerPara,
			pChainContext);
		if (pChainContext == NULL) 
		{ 
			ERROR("Error 0x%x finding cert chain\n", GetLastError()); break;
		}
		DEBUG("\ncertificate chain found\n");

		// Get pointer to leaf certificate context.
		pCertContext = pChainContext->rgpChain[0]->rgpElement[0]->pCertContext;

		// Create schannel credential
		ZeroMemory(&SchannelCred, sizeof(SchannelCred));
		SchannelCred.dwVersion = SCHANNEL_CRED_VERSION;
		SchannelCred.cCreds = 1;
		SchannelCred.paCred = &pCertContext;
        if (!m_hostName.empty())
        {
            SchannelCred.dwFlags |= SCH_CRED_SNI_CREDENTIAL;
        }
		Status = initializer.g_pSSPI->AcquireCredentialsHandleA(NULL,                   // Name of principal
			UNISP_NAME_A,           // Name of package
			SECPKG_CRED_OUTBOUND,   // Flags indicating use
			NULL,                   // Pointer to logon ID
			&SchannelCred,          // Package specific data
			NULL,                   // Pointer to GetKey() func
			NULL,                   // Value to pass to GetKey()
			&newHCreds,                // (out) Cred Handle
			&tsExpiry);            // (out) Lifetime (optional)
		isCredsInitialized = true;
		if (Status != SEC_E_OK) 
		{ 
			ERROR("**** Error 0x%x returned by AcquireCredentialsHandle\n", Status);
			continue;
		}
		DEBUG("\nnew schannel credential created\n");
		initializer.g_pSSPI->FreeCredentialsHandle(&hCred); // Destroy the old credentials.
		hCred = newHCreds;
	}
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
		scRet = initializer.g_pSSPI->InitializeSecurityContextA(&hCred,
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
			getNewClientCredentials();
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
	if (FAILED(scRet)) initializer.g_pSSPI->DeleteSecurityContext(&hContext);
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

	scRet = initializer.g_pSSPI->InitializeSecurityContextA(&hCred,
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
			initializer.g_pSSPI->FreeContextBuffer(OutBuffers[0].pvBuffer);
			initializer.g_pSSPI->DeleteSecurityContext(&hContext);
			return SEC_E_INTERNAL_ERROR;
		}
		DEBUG("%d bytes of handshake data sent\n", cbData);
		//{ PrintHexDump(cbData, OutBuffers[0].pvBuffer); printf("\n"); }
		initializer.g_pSSPI->FreeContextBuffer(OutBuffers[0].pvBuffer); // Free output buffer.
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


DWORD SChannelSocket::verifyServerCertificate(PCCERT_CONTEXT pServerCert, std::string host, DWORD dwCertFlags)
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

void SChannelSocket::connect(const std::string & host, int port, int timeout)
{
	SCHANNEL_CRED SchannelCred;
	DWORD   dwProtocol = SP_PROT_TLS1; // SP_PROT_TLS1; // SP_PROT_PCT1; SP_PROT_SSL2; SP_PROT_SSL3; 0=default
	ALG_ID  aiKeyExch = 0; // = default; CALG_DH_EPHEM; CALG_RSA_KEYX;
	SECURITY_STATUS  Status;
	DWORD            cSupportedAlgs = 0;
	ALG_ID           rgbSupportedAlgs[16];
	TimeStamp        tsExpiry;
	SecBuffer  ExtraData;
	std::ifstream is( m_serverCAFile.c_str(), std::ios::binary);
	if (is) {
		is.seekg(0, is.end);
		int length = is.tellg();
		is.seekg(0, is.beg);
	    BYTE * certificate = new BYTE[length];
		is.read((PCHAR)certificate, length);
		PCCERT_CONTEXT pCertContext;

		// If a user name is specified, then attempt to find a client
		// certificate. Otherwise, just create a NULL credential.
		if (!m_clientCertificateFile.empty())
		{
			// Open the "MY" certificate store, where IE stores client certificates.
			// Windows maintains 4 stores -- MY, CA, ROOT, SPC.
			if (hMyCertStore == NULL)
			{
				hMyCertStore = CertOpenSystemStore(0, "MY");
				if (!hMyCertStore)
				{
					printf("**** Error 0x%x returned by CertOpenSystemStore\n", GetLastError());
					logAndThrow(host, port, "ERROR");
				}
			}

			// Find client certificate. Note that this sample just searches for a
			// certificate that contains the user name somewhere in the subject name.
			// A real application should be a bit less casual.
			pCertContext = CertFindCertificateInStore(hMyCertStore,                     // hCertStore
				X509_ASN_ENCODING,             // dwCertEncodingType
				0,                                             // dwFindFlags
				CERT_FIND_SUBJECT_STR_A
				,// dwFindType
				m_clientCertificateFile.c_str(),       // *pvFindPara
				NULL);                                 // pPrevCertContext
		
			if (pCertContext == NULL)
			{
				ERROR("**** Error 0x%x returned by CertFindCertificateInStore\n", GetLastError());
				if (GetLastError() == CRYPT_E_NOT_FOUND) 
					ERROR("CRYPT_E_NOT_FOUND - property doesn't exist\n");
				logAndThrow(host, port, "ERROR");
			}
		}
		else
		{
			pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING, certificate, length);
		}
		ZeroMemory(&SchannelCred, sizeof(SchannelCred));
		SchannelCred.dwVersion = SCHANNEL_CRED_VERSION;
		if (pCertContext)
		{
			SchannelCred.cCreds = 1;
			SchannelCred.paCred = &pCertContext;
		}

		SchannelCred.grbitEnabledProtocols = dwProtocol;

		if (aiKeyExch) rgbSupportedAlgs[cSupportedAlgs++] = aiKeyExch;

		if (cSupportedAlgs)
		{
			SchannelCred.cSupportedAlgs = cSupportedAlgs;
			SchannelCred.palgSupportedAlgs = rgbSupportedAlgs;
		}

		SchannelCred.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS;

		// The SCH_CRED_MANUAL_CRED_VALIDATION flag is specified because
		// this sample verifies the server certificate manually.
		// Applications that expect to run on WinNT, Win9x, or WinME
		// should specify this flag and also manually verify the server
		// certificate. Applications running on newer versions of Windows can
		// leave off this flag, in which case the InitializeSecurityContext
		// function will validate the server certificate automatically.
		SchannelCred.dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION;
        if (!m_hostName.empty())
        {
            SchannelCred.dwFlags |= SCH_CRED_SNI_CREDENTIAL;
        }


		// Create an SSPI credential.
		Status = SChannelSocket::initializer.g_pSSPI->AcquireCredentialsHandleA(NULL,                 // Name of principal    
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
		if (pCertContext) CertFreeCertificateContext(pCertContext);

		Client_Socket = INVALID_SOCKET;
		connectToServer(host, port, &Client_Socket);
		performClientHandshake(m_hostName.empty() ? host : m_hostName, &ExtraData);
		isContextInitialized = true;
		// Authenticate server's credentials. Get server's certificate.
		Status = initializer.g_pSSPI->QueryContextAttributes(&hContext, SECPKG_ATTR_REMOTE_CERT_CONTEXT, (PVOID)&pRemoteCertContext);
		if (Status != SEC_E_OK)
		{
			cleanup();
			ERROR("Error 0x%x querying remote certificate\n", Status);
			logAndThrow(host, port, "Error querying remote certificate\n");
		} //
		DEBUG("----- Server Credentials Authenticated \n");
		// Attempt to validate server certificate.
		if (this->onlyVerified)
		{
		Status = verifyServerCertificate(pRemoteCertContext, host, 0x00001000  /*SECURITY_FLAG_IGNORE_CERT_CN_INVALID*/);
		if (Status) 
		{ 
			ERROR("**** Error 0x%x authenticating server credentials!\n", Status);
			cleanup(); 
			logAndThrow(host,port,"**** Error 0x%x authenticating server credentials!\n");
		}
		// The server certificate did not validate correctly. At this point, we cannot tell
		// if we are connecting to the correct server, or if we are connecting to a
		// "man in the middle" attack server - Best to just abort the connection.
		DEBUG("----- Server Certificate Verified\n");
		// Free the server certificate context.
		}
		CertFreeCertificateContext(pRemoteCertContext);
		pRemoteCertContext = NULL;
		DEBUG("----- Server certificate context released \n");

		// Display connection info.
		displayConnectionInfo(); 
		DEBUG("----- Secure Connection Info\n");
		SecPkgContext_StreamSizes Sizes;
		initializer.g_pSSPI->QueryContextAttributes(&hContext, SECPKG_ATTR_STREAM_SIZES, &Sizes);
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
	DWORD              cbData, length;
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
//      if( scRet == SEC_E_INCOMPLETE_MESSAGE - Input buffer has partial encrypted record, read more
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
// http://msdn.microsoft.com/en-us/library/aa375378(VS.85).aspx
// The encrypted message is encrypted in place, overwriting the original contents of its buffer.
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
	return cbData; // send( Socket, pbIoBuffer,    Sizes.cbHeader + strlen(pbMessage) + Sizes.cbTrailer,  0 );
}

void SChannelSocket::write(const char *p, size_t length)
{
	PBYTE pbIoBuffer;
	SecPkgContext_StreamSizes Sizes;
	initializer.g_pSSPI->QueryContextAttributes(&hContext, SECPKG_ATTR_STREAM_SIZES, &Sizes);
	auto cbIoBufferLength = Sizes.cbHeader + Sizes.cbMaximumMessage + Sizes.cbTrailer;
	memcpy(pbWBuffer+Sizes.cbHeader, p, length);
	encryptSend(length, Sizes);
}

size_t SChannelSocket::read(char *p, size_t length)
{
	unsigned int count = this->ready_bytes - this->offset_bytes;
	// On emprty buffer read bytes from TLS
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

void SChannelSocket::logAndThrow(const std::string& host, const int port, const std::string& msg) {
	DEBUG(msg.c_str());
	throw TransportException(host, port, msg, -1);
}

void SChannelSocket::cleanup()
{
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

	// Close "MY" certificate store.
	if (hMyCertStore) CertCloseStore(hMyCertStore, 0);
	hMyCertStore = NULL;

	if (recvBuff != NULL) LocalFree(recvBuff);
	if (pbWBuffer != NULL) LocalFree(pbWBuffer);
	if (pbRBuffer != NULL) LocalFree(pbRBuffer);
	recvBuff = pbWBuffer = pbRBuffer = NULL;
}

void SChannelSocket::displayWinSockError(DWORD ErrCode)
{
	LPSTR pszName = NULL; // http://www.sockets.com/err_lst1.htm#WSANO_DATA

	switch (ErrCode) // http://msdn.microsoft.com/en-us/library/ms740668(VS.85).aspx
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

void SChannelSocket::displayConnectionInfo()
{
	SECURITY_STATUS Status;
	SecPkgContext_ConnectionInfo ConnectionInfo;

	Status = initializer.g_pSSPI->QueryContextAttributes(&hContext, SECPKG_ATTR_CONNECTION_INFO, (PVOID)&ConnectionInfo);
	if (Status != SEC_E_OK) { printf("Error 0x%x querying connection info\n", Status); return; }

	printf("\n");

	switch (ConnectionInfo.dwProtocol)
	{
	case SP_PROT_TLS1_CLIENT:
		DEBUG("Protocol: TLS1\n");
		break;

	case SP_PROT_SSL3_CLIENT:
		DEBUG("Protocol: SSL3\n");
		break;

	case SP_PROT_PCT1_CLIENT:
		DEBUG("Protocol: PCT\n");
		break;

	case SP_PROT_SSL2_CLIENT:
		DEBUG("Protocol: SSL2\n");
		break;

	default:
		DEBUG("Protocol: 0x%x\n", ConnectionInfo.dwProtocol);
	}

	switch (ConnectionInfo.aiCipher)
	{
	case CALG_RC4:
		DEBUG("Cipher: RC4\n");
		break;

	case CALG_3DES:
		DEBUG("Cipher: Triple DES\n");
		break;

	case CALG_RC2:
		DEBUG("Cipher: RC2\n");
		break;

	case CALG_DES:
	case CALG_CYLINK_MEK:
		DEBUG("Cipher: DES\n");
		break;

	case CALG_SKIPJACK:
		DEBUG("Cipher: Skipjack\n");
		break;

	default:
		DEBUG("Cipher: 0x%x\n", ConnectionInfo.aiCipher);
	}

	DEBUG("Cipher strength: %d\n", ConnectionInfo.dwCipherStrength);

	switch (ConnectionInfo.aiHash)
	{
	case CALG_MD5:
		DEBUG("Hash: MD5\n");
		break;

	case CALG_SHA:
		DEBUG("Hash: SHA\n");
		break;

	default:
		DEBUG("Hash: 0x%x\n", ConnectionInfo.aiHash);
	}

	DEBUG("Hash strength: %d\n", ConnectionInfo.dwHashStrength);

	switch (ConnectionInfo.aiExch)
	{
	case CALG_RSA_KEYX:
	case CALG_RSA_SIGN:
		DEBUG("Key exchange: RSA\n");
		break;

	case CALG_KEA_KEYX:
		DEBUG("Key exchange: KEA\n");
		break;

	case CALG_DH_EPHEM:
		DEBUG("Key exchange: DH Ephemeral\n");
		break;

	default:
		DEBUG("Key exchange: 0x%x\n", ConnectionInfo.aiExch);
	}

	DEBUG("Key exchange strength: %d\n", ConnectionInfo.dwExchStrength);
}




