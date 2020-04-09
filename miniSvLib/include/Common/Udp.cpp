#include "Udp.h"
#include <WinSock2.h>
#pragma comment(lib, "wsock32.lib")

_UDWORD Udp::s_Count = 0; //!< インスタンス生成カウント

Udp::Udp(const char* HostName, _UWORD PortSend, _UWORD PortRecv)
{
	if (Udp::s_Count++ == 0)
	{
		::WSADATA wsaData;
		::WSAStartup(MAKEWORD(2, 0), &wsaData);
	}

	m_pSock    = NULL;
	m_pAddrOut = NULL;
	m_pAddrIn  = NULL;

	m_RxLength = 0;
	m_RxReaded = 0;

	HOSTENT* pHostent = ::gethostbyname(HostName);

	if (pHostent == NULL) return;

	// 送信用
	SOCKADDR_IN addr_out;
	::memset(&addr_out, 0, sizeof(addr_out));

	::memcpy(&(addr_out.sin_addr), pHostent->h_addr_list[0], pHostent->h_length);
	addr_out.sin_family = AF_INET;
	addr_out.sin_port = ::htons(PortSend);

	// 受信用
	SOCKADDR_IN addr_in;
	::memset(&addr_in, 0, sizeof(addr_in));

	addr_in.sin_addr.s_addr = ::htonl(INADDR_ANY);
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = ::htons(PortRecv);

	m_pAddrOut = new SOCKADDR_IN(addr_out);
	m_pAddrIn  = new SOCKADDR_IN(addr_in);
}

Udp::~Udp()
{
	Close();

	if (m_pSock)    delete m_pSock;
	if (m_pAddrOut) delete m_pAddrOut;
	if (m_pAddrIn)  delete m_pAddrIn;

	if (--Udp::s_Count == 0)
	{
		::WSACleanup();
	}
}

CRESULT Udp::Open()
{
	CRESULT ret = C_FAIL;

	if (m_pSock == NULL && m_pAddrOut)
	{
		m_pSock = new SOCKET;

		*m_pSock = socket(AF_INET, SOCK_DGRAM, 0);

		::bind(*m_pSock, (LPSOCKADDR)m_pAddrIn, sizeof(*m_pAddrIn));

		// ノンブロッキング設定
		u_long val = 1;
		::ioctlsocket(*m_pSock, FIONBIO, &val);

		debug_printf("UDP Open Succeeded (Tx=%d.%d.%d.%d:%d Rx=%d)\n",
			m_pAddrOut->sin_addr.S_un.S_un_b.s_b1,
			m_pAddrOut->sin_addr.S_un.S_un_b.s_b2,
			m_pAddrOut->sin_addr.S_un.S_un_b.s_b3,
			m_pAddrOut->sin_addr.S_un.S_un_b.s_b4,
			::htons(m_pAddrOut->sin_port),
			::htons(m_pAddrIn->sin_port));

		ret = C_OK;
	}
	else
	{
		debug_puts("UDP Open Failed");
	}

	return ret;
}

void Udp::Close()
{
	if (m_pSock)
	{
		::closesocket(*m_pSock);
		delete m_pSock;
		m_pSock = NULL;
	}
}

bool Udp::IsEnable() const
{
	return true; // 【暫定】
}

// データ受信関数
bool Udp::IsReceiveData()
{
	_Receive();

	return (m_RxReaded < m_RxLength);
}

_UDWORD Udp::IsTransmittable()
{
	return m_pSock ? (_UDWORD) - 1 : 0;
}

CRESULT Udp::WriteElement(_UBYTE chr)
{
	CRESULT ret = C_FAIL;

	if (m_pSock)
	{
		int n = ::sendto(*m_pSock, reinterpret_cast<const char*>(&chr), 1, 0, (LPSOCKADDR)m_pAddrOut, sizeof(*m_pAddrOut));

		ret = n == 1 ? C_OK : C_FAIL;
	}

	return ret;
}

CRESULT Udp::ReadString(void* ptr, _UWORD len)
{
	CRESULT ret = C_FAIL;

	_Receive();

	if (m_RxReaded + len <= m_RxLength)
	{
		memcpy_s(ptr, len, m_RxBuffer + m_RxReaded, len);
		m_RxReaded += len;
		ret = C_OK;
	}

	return ret;
}

CRESULT Udp::WriteString(const _UBYTE* ptr, _UWORD len)
{
	CRESULT ret = C_FAIL;

	if (m_pSock && len)
	{
		int n = ::sendto(*m_pSock, reinterpret_cast<const char*>(ptr), len, 0, (LPSOCKADDR)m_pAddrOut, sizeof(*m_pAddrOut));

		ret = n == len ? C_OK : C_FAIL;
	}

	return ret;
}

CRESULT Udp::ReadElement(_UBYTE* ptr)
{
	CRESULT ret = C_FAIL;

	_Receive();

	if (m_RxReaded < m_RxLength)
	{
		*ptr = m_RxBuffer[m_RxReaded++];
		ret = C_OK;
	}

	return ret;
}

void Udp::_Receive()
{
	if (m_pSock)
	{
		if (m_RxReaded == m_RxLength)
		{
			// 内部バッファエンプティ

			int n = ::recvfrom(*m_pSock, reinterpret_cast<char*>(m_RxBuffer), sizeof(m_RxBuffer), 0, NULL, NULL);

			if (n > 0)
			{
				// 受信OK

				m_RxLength = n;
				m_RxReaded = 0;
			}

			//debug_printf("%d ", n);
		}
	}
}