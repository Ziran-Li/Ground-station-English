#include "stdafx.h"
#include "WinsockTCP2.h"
#include "WSADebug.h"
#include "common/CommonMacro.h"

#pragma comment(lib, "Ws2_32.lib")

#define MSG_CLOSE_EVENT        (WM_APP + 1) // �\�P�b�g���N���[�Y����悤�Ƃ��Ă���
#define MSG_READ_EVENT         (WM_APP + 2) // �f�[�^����M����
#define MSG_WRITE_EVENT        (WM_APP + 3) // �Ă� send �\�ɂȂ���
#define MSG_READ_TIMEOUT_EVENT (WM_APP + 4) // ��莞�Ԏ�M����������
#define MSG_DISCONNECT_EVENT   (WM_APP + 5) // �N���C�A���g�Ƃ̐ڑ������

DWORD _HostName2ValidAddr(LPCSTR lpHostName);

//������������������������������������������������������������������������������������������������
//	WSABase
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	�X�^�e�B�b�N�ϐ�
//*******************************************************************************
UINT WSABase::m_WSAStartCount = 0;

//*******************************************************************************
//	�\�z�E�j��
//*******************************************************************************
WSABase::WSABase()
{
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("WSABase_Initializing_Mutex"));
	::WaitForSingleObject(hMutex, INFINITE);

	if( WSABase::m_WSAStartCount == 0 ) { 
		::WSADATA wsaData;
		if( int nRet = ::WSAStartup(MAKEWORD(2,0), &wsaData) ) {
			// ��O�G���[
			CWSADebug::WSAFormatMessage(nRet, _T("At: WSAStartup"));
		} else {
			// ����
		}
	}

	WSABase::m_WSAStartCount++;

	::CloseHandle(hMutex);
}

WSABase::~WSABase()
{
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("WSABase_Initializing_Mutex"));
	::WaitForSingleObject(hMutex, INFINITE);

	if( WSABase::m_WSAStartCount > 0 ) {
		WSABase::m_WSAStartCount--;
	}

	if( WSABase::m_WSAStartCount == 0 ) {
		::WSACleanup();
	}

	::CloseHandle(hMutex);
}

//������������������������������������������������������������������������������������������������
//	WSAConnection
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	�\�z�E�j��
//*******************************************************************************
WSAConnection::WSAConnection()
{
	m_sock                  = INVALID_SOCKET;
	m_IPv4dw                = NULL;
	m_myID                  = 0;
	m_msecReceiveTimeout    = 0;
	m_hExitEvent            = NULL;

	m_flagConnected         = FALSE;

	m_pParentMessageThread = NULL;
	m_hParentMessageEvent  = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hSendMessageMutex    = ::CreateMutex(NULL, FALSE, NULL);
}

WSAConnection::~WSAConnection()
{
	// ��M�X���b�h�̏I���ҋ@
	m_ReceiveThread.WaitThreadExit(); // �T�[�o�[�܂��̓N���C�A���g�����M�X���b�h���I�������̂�҂�
	m_ReceiveThread.CloseThread();

	// �����I�u�W�F�N�g�̔j��
	::CloseHandle(m_hParentMessageEvent);
	::CloseHandle(m_hSendMessageMutex);
}

BOOL WSAConnection::Create(SOCKET sock, DWORD IPv4Addr, DWORD msecReceiveTimeout, HANDLE hExitEvent, unsigned int connectionID, 
	                       Win32Thread* pParentMessageThread)
{
	// ���쒆�̃X���b�h���E���Ă���

	// ��M�X���b�h�𐶐�
	if( m_ReceiveThread.CreateSuspendThread(_ReceiveProc, (LPVOID)this) == FALSE ) {
		// ��O�G���[
		CWSADebug::puts(_T("�X���b�h�𐶐��ł��܂���ł���"));
		return FALSE;
	}

	// �l��ݒ�
	m_sock               = sock;
	m_IPv4dw             = IPv4Addr;
	m_myID               = connectionID;
	m_msecReceiveTimeout = msecReceiveTimeout;
	m_hExitEvent         = hExitEvent;

	m_flagConnected      = TRUE;

	m_pParentMessageThread = pParentMessageThread;

	return TRUE;
}

void WSAConnection::Resume(BOOL WaitThreadExit)
{ 
	m_ReceiveThread.ResumeThread();
	if( WaitThreadExit ) {
		m_ReceiveThread.WaitThreadExit();
	}
}

size_t WSAConnection::SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking)
{
	if( IsConnected() == FALSE ) return 0;

	size_t nTotalSend = 0;

	while( nTotalSend < nSize ) {
		int nSend = ::send(m_sock, static_cast<const char*>(pBuff) + nTotalSend, nSize - nTotalSend, 0);

		if( nSend == SOCKET_ERROR ) {
			if( ::WSAGetLastError() == WSAEWOULDBLOCK ) {
				if( Blocking == FALSE ) {
					return nTotalSend;
				}
			} else {
				// ��O
				CWSADebug::WSAPrintLastError(_T("At: send"));
				return nTotalSend;
			}
		} else {
			nTotalSend += static_cast<size_t>(nSend);
		}
	}

	return nTotalSend;
}

size_t WSAConnection::ReceiveData(LPVOID pBuff, size_t nSize, BOOL Blocking)
{
	if( IsConnected() == FALSE ) return 0;

	size_t nTotalReceive = 0;

	while( nTotalReceive < nSize ) {
		int nReceive = ::recv(m_sock, static_cast<char*>(pBuff) + nTotalReceive, nSize - nTotalReceive, 0);

		if( nReceive == SOCKET_ERROR ) {
			if( WSAGetLastError() == WSAEWOULDBLOCK ) {
				if( Blocking == FALSE ) {
					break;
				}
			} else {
				// ��O
				CWSADebug::WSAPrintLastError(_T("At: recv"));
				break;
			}
		} else if( nReceive == 0 ) {
			// �\�P�b�g������ꂽ�ꍇ
			break;

		} else {
			nTotalReceive += static_cast<size_t>(nReceive);
		}
	}

	return nTotalReceive;
}

void WSAConnection::SendMessageToParent(UINT Msg)
{
	if( m_pParentMessageThread && m_hParentMessageEvent ) {
		// ���b�N��������i�Q�̈قȂ�X���b�h�����̊֐������s����ƃ��b�Z�[�W�����������O�ɔ����Ă��܂��ꍇ������j
		::WaitForSingleObject(m_hSendMessageMutex, INFINITE);

		::ResetEvent(m_hParentMessageEvent);
		m_pParentMessageThread->PostThreadMessage(Msg, reinterpret_cast<WPARAM>(this), reinterpret_cast<LPARAM>(m_hParentMessageEvent));
		::WaitForSingleObject(m_hParentMessageEvent, INFINITE); // �|�X�g��̃X���b�h���C�x���g�I�u�W�F�N�g���V�O�i����Ԃɂ���܂őҋ@����

		// �A�����b�N������
		::ReleaseMutex(m_hSendMessageMutex);
	}
}

void WSAConnection::PostDisconnectMessageToParent()
{
	if( m_pParentMessageThread && m_hParentMessageEvent ) {
		m_pParentMessageThread->PostThreadMessage(MSG_DISCONNECT_EVENT, reinterpret_cast<WPARAM>(static_cast<LPVOID>(this)), reinterpret_cast<LPARAM>(&m_ReceiveThread));
	}
}

UINT WSAConnection::_ReceiveProc(LPVOID pParam)
{
	WSAConnection* pThis = static_cast<WSAConnection*>(pParam);
	HANDLE hEvent;

	CWSADebug::puts(_T("��M���J�n���܂�"));

	// �C�x���g����
	hEvent = ::WSACreateEvent();
	if( hEvent == WSA_INVALID_EVENT ) {
		// ��O�G���[
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
		goto ENDLINE;
	}

	// �C�x���g�̔����v����ݒ�
	int ret = ::WSAEventSelect(pThis->m_sock, hEvent, FD_WRITE | FD_READ | FD_CLOSE);
	if( ret == SOCKET_ERROR ) {
		// ��O�G���[
		CWSADebug::WSAPrintLastError(_T("At: WSAEventSelect"));
		::WSACloseEvent(hEvent);
		goto ENDLINE;
	}

	// �C�x���g�̔z������
	HANDLE hEvents[2] = {hEvent, pThis->m_hExitEvent};

	// �I�����߂���M����܂Ń��[�v
	while( CheckQuitMessage() == FALSE ) {

		// �C�x���g�҂�
		DWORD dwret = ::WSAWaitForMultipleEvents(2, hEvents, FALSE, pThis->m_msecReceiveTimeout, FALSE);

		if( dwret == WSA_WAIT_TIMEOUT ) {
			// ��M�^�C���A�E�g �� �ڑ����؂ꂽ�H

			// ���z�֐��R�[���i�f�t�H���g�F�Đڑ��j
			BOOL Resume = pThis->OnReceiveTimeout();
			pThis->SendMessageToParent(MSG_READ_TIMEOUT_EVENT);
			
			if( Resume == FALSE ) {
				// �߂�l FALSE �ōĐڑ����L�����Z��
				continue;
			}

			CWSADebug::puts(_T("��M���^�C���A�E�g���܂���"));
			
			break;

		} else if( dwret == WSA_WAIT_FAILED ) {
			CWSADebug::puts(_T("At: WSAWaitForMultipleEvents"));
			break;
		}

		// �X���b�h�I���C�x���g���������������ׂ�
		if( dwret - WSA_WAIT_EVENT_0 == 1 ) {
			// m_hExitEvent ���V�O�i����� �� �I��
			CWSADebug::puts(_T("��M�ҋ@��Ԃ���X���b�h���I�����܂�"));
			break;
		}

		// �C�x���g�̔������R���擾
		WSANETWORKEVENTS events;
		if( ::WSAEnumNetworkEvents(pThis->m_sock, hEvent, &events) == SOCKET_ERROR ) {
			// ��O�G���[
			CWSADebug::WSAPrintLastError(_T("At: WSAEnumNetworkEvents"));
			break;
		} else if( (events.lNetworkEvents & (FD_WRITE | FD_READ | FD_CLOSE)) == 0 ) {
			// ��O�G���[
			if( ::WSAGetLastError() != WSAEWOULDBLOCK ) {
				CWSADebug::puts(_T("�z��O�̗v���ɂ��C�x���g���������܂���"));
				CWSADebug::WSAPrintNetworkEvents(events);
			}
		}

		if( events.lNetworkEvents & FD_CLOSE ) {
			// �l�b�g���[�N����ؒf���ꂽ
			CWSADebug::puts(_T("�l�b�g���[�N����ؒf����܂���"));

			break;
		}

		if( events.lNetworkEvents & FD_READ ) {
			// ��M�C�x���g
			
			// ���z�֐��R�[��
			pThis->OnReceive();
			pThis->SendMessageToParent(MSG_READ_EVENT);
		}


		if( events.lNetworkEvents & FD_WRITE ) {
			// ���M�C�x���g
			
			// ���z�֐��R�[��
			pThis->OnSend();
			pThis->SendMessageToParent(MSG_WRITE_EVENT);
		}
	}

	// �C�x���g����
	::WSAEventSelect(pThis->m_sock, hEvent, 0);

	// �C�x���g�J��
	::WSACloseEvent(hEvent);

ENDLINE:

	pThis->m_flagConnected = FALSE;
	
	// �X���b�h�𔲂���Ƃ��͐ڑ����؂�
	if( ::shutdown(pThis->m_sock, SD_BOTH) == SOCKET_ERROR ) {
		CWSADebug::WSAPrintLastError(_T("At: shutdown"));
	}	

	// �\�P�b�g���N���[�Y����
	if( ::closesocket(pThis->m_sock) == SOCKET_ERROR ) {
		CWSADebug::WSAPrintLastError(_T("At: closesocket"));
	}

	// �ؒf�C�x���g
	pThis->OnClose();
	pThis->SendMessageToParent(MSG_CLOSE_EVENT);

	// �e�ɐڑ��̐ؒf��ʒm���āC�o�^���������Ă��炤
	pThis->PostDisconnectMessageToParent();

	CWSADebug::printf(_T("��M�p�X���b�h(0x%x)���I�����܂���\r\n"), ::GetCurrentThreadId());

	return 0;
}

//������������������������������������������������������������������������������������������������
//	WSATcpServer
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	�\�z�E�j��
//*******************************************************************************
WSATcpServerBase::WSATcpServerBase()
{
	// Accept �X���b�h�I���p�C�x���g�̐���
	m_hAcceptExitEvent = ::WSACreateEvent();
	if( m_hAcceptExitEvent == WSA_INVALID_EVENT ) {
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
	} 

	// Client �I���p�C�x���g�̐���
	m_hConnectionExitEvent = ::WSACreateEvent();
	if( m_hConnectionExitEvent == WSA_INVALID_EVENT ) {
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
	}

	m_sock = INVALID_SOCKET;
	m_msecReceiveTimeout = N_DEFAULT_TIMEOUT;
}

WSATcpServerBase::~WSATcpServerBase()
{
	// �X���b�h�I���p�C�x���g�̊J��
	::WSACloseEvent(m_hAcceptExitEvent);
	::WSACloseEvent(m_hConnectionExitEvent);
}

//*******************************************************************************
//	������
//*******************************************************************************
BOOL WSATcpServerBase::Initialize(unsigned short nPortNo, unsigned int nBacklogConnection, DWORD msecReceiveTimeout)
{
	// �R���X�g���N�^�ł̏����������s���Ă��Ȃ������ׂ�
	if( IsConstructionSucceeded() == FALSE ) {
		CWSADebug::puts(_T("�R���X�g���N�^�ł̏������Ɏ��s���Ă������ߏ����𒆒f���܂�"));
		return FALSE;
	}

	// ��x���Z�b�g
	Destroy();

	// �T�[�o�[�̃\�P�b�g����
	SOCKET newsock = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if( newsock == INVALID_SOCKET ) {
		// ��O�G���[
		CWSADebug::WSAPrintLastError(_T("At: WSASocket"));
		return FALSE;
	}

	::sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port   = ::htons(nPortNo);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	int nRet = ::bind(newsock, (struct sockaddr*)&addr, sizeof(addr));
	if( nRet ) {
		//  ��O�G���[
		CWSADebug::WSAPrintLastError(_T("At: bind"));
		::closesocket(newsock);
		return FALSE;
	}

	// �ڑ��҂��ɂ���
	if( listen(newsock, nBacklogConnection) ) {
		// ��O�G���[
		CWSADebug::WSAPrintLastError(_T("At: listen"));
		::closesocket(newsock);
		return FALSE;
	}

	// �X���b�h����
	if( m_AcceptThread.CreateSuspendThread(_AcceptProc, (LPVOID)this) == FALSE ) {
		// ��O�G���[
		CWSADebug::puts(_T("�X���b�h�𐶐��ł��܂���ł���"));
		::closesocket(newsock);
		return FALSE;
	}

	if( m_ClientMessageThread.CreateSuspendThread(_ClientMessageProc, (LPVOID)this) == FALSE ) {
		// ��O�G���[
		CWSADebug::puts(_T("�X���b�h�𐶐��ł��܂���ł���"));
		::closesocket(newsock);
		return FALSE;
	}

	// �S�����������̂Ń\�P�b�g�E�p�����[�^��ۑ�
	m_msecReceiveTimeout = msecReceiveTimeout;
	m_sock = newsock;

	// �������C�x���g
	OnInitialize();

	// �J�n
	m_ClientMessageThread.ResumeThread();
	m_AcceptThread.ResumeThread();

	return TRUE;
}

//*******************************************************************************
//	���Z�b�g
//*******************************************************************************
void WSATcpServerBase::Destroy()
{
	// �ڑ��p�X���b�h���I��������
	if( ::WSASetEvent(m_hAcceptExitEvent) == FALSE ) {
		CWSADebug::puts(_T("At: WSASetEvent"));
	}
	m_AcceptThread.WaitThreadExit();
	m_AcceptThread.CloseThread();

	// �S�Ă̎�M�p�X���b�h���I��������
	while( GetClientCount() ) {
		// �S�Ă̎�M�p�X���b�h���I��������
		if( ::WSASetEvent(m_hConnectionExitEvent) == FALSE ) {
			CWSADebug::puts(_T("At: WSASetEvent"));
		}
	}

	// �N���C�A���g���b�Z�[�W��M�p�X���b�h���I��������
	m_ClientMessageThread.PostQuitMessage();
	m_ClientMessageThread.WaitThreadExit();
	m_ClientMessageThread.CloseThread();

	// �C�x���g�n���h���̃��Z�b�g
	::WSAResetEvent(m_hAcceptExitEvent);
	::WSAResetEvent(m_hConnectionExitEvent);

	CWSADebug::puts(_T("Destroy ����"));
}

//*******************************************************************************
//	�R���X�g���N�^�ł̏������������������ǂ�����Ԃ�
//*******************************************************************************
BOOL WSATcpServerBase::IsConstructionSucceeded() const
{
	return (m_hAcceptExitEvent != WSA_INVALID_EVENT) && (m_hConnectionExitEvent != WSA_INVALID_EVENT);
}

//*******************************************************************************
//	���������s��ꂽ���ǂ�����Ԃ�
//*******************************************************************************
BOOL WSATcpServerBase::IsInitialized() const
{
	return m_sock != INVALID_SOCKET;
}

//*******************************************************************************
//	�ڑ��p�X���b�h
//
//	listen�Őݒ肵�����ȏ�ɐڑ����󂯓���Ă��܂���肠��
//*******************************************************************************
UINT WSATcpServerBase::_AcceptProc(LPVOID pParam)
{
	WSATcpServerBase* pThis = static_cast<WSATcpServerBase*>(pParam);
	HANDLE            hAcceptEvent;

	CWSADebug::printf(_T("AcceptProc�J�n ThreadID:%d\n"), pThis->m_AcceptThread.GetId()); 

	hAcceptEvent = ::WSACreateEvent();
	if( hAcceptEvent == WSA_INVALID_EVENT ) {
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
		return 0;
	}

	int ret = ::WSAEventSelect(pThis->m_sock, hAcceptEvent, FD_ACCEPT); // accept ����u���b�L���O�ɂȂ�
	if( ret == SOCKET_ERROR ) {
		CWSADebug::WSAPrintLastError(_T("At: WSAEventSelect"));
		::WSACloseEvent(hAcceptEvent);
		return 0;
	}

	// m_AcceptExitEvent ���V�O�i����Ԃɂ��Ă���
	if( ::WSAResetEvent(pThis->m_hAcceptExitEvent) == FALSE ) {
		CWSADebug::WSAPrintLastError(_T("At: WSAResetEvent"));
		::WSACloseEvent(hAcceptEvent);
		return 0;
	}

	// �C�x���g�̔z������
	HANDLE hEvents[2] = {hAcceptEvent, pThis->m_hAcceptExitEvent};

	while( 1 ) {

		DWORD dwret = ::WSAWaitForMultipleEvents(2, hEvents, FALSE, INFINITE, FALSE);
		if( (dwret < WSA_WAIT_EVENT_0) || (dwret > (WSA_WAIT_EVENT_0 + 1)) ) {
			// ��O�G���[
			CWSADebug::WSAPrintLastError(_T("At: WSAWaitForMultipleEvents"));
			break;
		}

		// m_hAcceptExitEvent ���V�O�i����ԂɂȂ��������ׂ�
		if( dwret - WSA_WAIT_EVENT_0 == 1 ) {
			// �V�O�i����� �� �I��
			CWSADebug::puts(_T("Accept���I�����܂�"));
			break;
		}

		// �C�x���g�̔������R���擾
		::WSANETWORKEVENTS events;
		int ret = ::WSAEnumNetworkEvents(pThis->m_sock, hAcceptEvent, &events);
		if( ret == SOCKET_ERROR ) {
			// ��O�G���[
			CWSADebug::WSAPrintLastError(_T("At: WSAEnumNetworkEvents"));
			break;
		} else if( (events.lNetworkEvents & FD_ACCEPT) == 0 ) {
			// ��O�G���[
			CWSADebug::puts(_T("�z��O�̗v���ɂ��C�x���g���������܂����B"));
			CWSADebug::WSAPrintNetworkEvents(events);
			// break;
		}

		// ACCEPT �C�x���g
		if( events.lNetworkEvents & FD_ACCEPT ) {
			
			// Accept ����
			::sockaddr_in client;
			int client_len = sizeof(client);
			SOCKET client_sock = ::accept(pThis->m_sock, (struct sockaddr*)&client, &client_len);
			if( client_sock == INVALID_SOCKET ) {
				// ��O�G���[
				CWSADebug::WSAPrintLastError(_T("At: accept"));
				break;
			}

			// Accept ����
			WSAConnection* pNewClient = pThis->_CreateNewClient(client_sock, client.sin_addr.S_un.S_addr, pThis->m_hConnectionExitEvent, &pThis->m_ClientMessageThread);

			// Accept �C�x���g
			pThis->_OnAccept(pNewClient);

			// �ғ��J�n
			pNewClient->Resume();
		}
	}

	// ����
	::WSAEventSelect(pThis->m_sock, hAcceptEvent, 0);

	// �J��
	::WSACloseEvent(hAcceptEvent);

	// �\�P�b�g���J��
	if( ::closesocket(pThis->m_sock) == SOCKET_ERROR ) {
		CWSADebug::WSAPrintLastError(_T("At: closesocket"));
	}
	pThis->m_sock = INVALID_SOCKET;

	CWSADebug::printf(_T("�ڑ��p�X���b�h(0x%x)���I�����܂���\r\n"), ::GetCurrentThreadId());

	return 0;
}

//*******************************************************************************
//	�N���C�A���g����̃��b�Z�[�W��M�p�X���b�h
//*******************************************************************************
UINT WSATcpServerBase::_ClientMessageProc(LPVOID pParam) 
{
	WSATcpServerBase* pThis = static_cast<WSATcpServerBase*>(pParam);
	::MSG msg;

	while( ::GetMessage(&msg, NULL, 0, 0) >= 0 ) {
		// ���b�Z�[�W���[�v

		if( msg.message == WM_QUIT ) break;
		
		switch( msg.message ) {
			case MSG_CLOSE_EVENT:
				pThis->_OnClose(reinterpret_cast<WSAConnection*>(msg.wParam));
				::SetEvent(reinterpret_cast<HANDLE>(msg.lParam));
				break;

			case MSG_READ_EVENT:
				pThis->_OnReceive(reinterpret_cast<WSAConnection*>(msg.wParam));
				::SetEvent(reinterpret_cast<HANDLE>(msg.lParam));
				break;

			case MSG_WRITE_EVENT:
				pThis->_OnSend(reinterpret_cast<WSAConnection*>(msg.wParam));
				::SetEvent(reinterpret_cast<HANDLE>(msg.lParam));
				break;

			case MSG_READ_TIMEOUT_EVENT:
				pThis->_OnReceiveTimeout(reinterpret_cast<WSAConnection*>(msg.wParam));
				::SetEvent(reinterpret_cast<HANDLE>(msg.lParam));
				break;

			case MSG_DISCONNECT_EVENT:
				Win32Thread* pThread = reinterpret_cast<Win32Thread*>(msg.lParam);
				pThread->WaitThreadExit(); // �X���b�h���I������܂őҋ@
				pThis->_DestroyClient(reinterpret_cast<LPVOID>(msg.wParam));
				break;
		}
	}

	CWSADebug::printf(_T("���b�Z�[�W��M�p�X���b�h(0x%x)���I�����܂���\r\n"), ::GetCurrentThreadId());

	return 0;
}


WSATcpClientBase::WSATcpClientBase()
{
	// Accept �X���b�h�I���p�C�x���g�̐���
	m_hConnectExitEvent = ::WSACreateEvent();
	if( m_hConnectExitEvent == WSA_INVALID_EVENT ) {
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
	} 

	// Client �I���p�C�x���g�̐���
	m_hConnectionExitEvent = ::WSACreateEvent();
	if( m_hConnectionExitEvent == WSA_INVALID_EVENT ) {
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
	}

	ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));
	m_HostName[0] = '0';
	m_flagInitialized = FALSE;
	m_msecReceiveTimeout_t = N_DEFAULT_TIMEOUT;
}

WSATcpClientBase::~WSATcpClientBase()
{
	Destroy();

	// �X���b�h�I���p�C�x���g�̊J��
	::WSACloseEvent(m_hConnectExitEvent);
	::WSACloseEvent(m_hConnectionExitEvent);
}

BOOL WSATcpClientBase::Initialize(DWORD IPv4Addr, WORD nPortNo, DWORD msecReceiveTimeout)
{
	return _Initialize(IPv4Addr, NULL, nPortNo, msecReceiveTimeout);
}

BOOL WSATcpClientBase::Initialize(BYTE ip1, BYTE ip2, BYTE ip3, BYTE ip4, WORD nPortNo, DWORD msecReceiveTimeout)
{
	return _Initialize(ip1 + ((DWORD)ip2 << 8) + ((DWORD)ip3 << 16) + ((DWORD)ip4 << 24), NULL, nPortNo, msecReceiveTimeout);
}

BOOL WSATcpClientBase::Initialize(LPCSTR lpHostName, WORD nPortNo, DWORD msecReceiveTimeout)
{
	return _Initialize(NULL, lpHostName, nPortNo, msecReceiveTimeout);
}

BOOL WSATcpClientBase::_Initialize(DWORD  IPv4Addr, LPCSTR lpHostName, WORD nPortNo, DWORD msecReceiveTimeout)
{
	// �R���X�g���N�^�ł̏����������s���Ă��Ȃ������ׂ�
	if( IsConstructionSucceeded() == FALSE ) {
		CWSADebug::puts(_T("�R���X�g���N�^�ł̏������Ɏ��s���Ă������ߏ����𒆒f���܂�"));
		return FALSE;
	}

	// ��x���Z�b�g
	Destroy();

	// �X���b�h����
	if( m_ConnectThread.CreateSuspendThread(_ConnectProc, (LPVOID)this) == FALSE ) {
		// ��O�G���[
		CWSADebug::puts(_T("�X���b�h�𐶐��ł��܂���ł���"));
		return FALSE;
	}

	// �S�����������̂Ńp�����[�^��ۑ�
	m_msecReceiveTimeout_t  = msecReceiveTimeout;
	m_ServerAddr.sin_family = AF_INET;
	m_ServerAddr.sin_port   = htons(nPortNo);

	if( lpHostName ) {
		// �z�X�g���w��
		m_ServerAddr.sin_addr.S_un.S_addr = NULL;
		::strcpy_s(m_HostName, sizeof(m_HostName), lpHostName);
	} else {
		// IP �A�h���X�w��
		m_ServerAddr.sin_addr.S_un.S_addr = IPv4Addr;
		m_HostName[0] = '\0';
	}

	// �������t���O�Z�b�g
	m_flagInitialized = TRUE;

	// ���z�֐��R�[��
	OnInitialize();

	// �ڑ��J�n
	m_ConnectThread.ResumeThread();

	return TRUE;
}

void WSATcpClientBase::Destroy()
{
	// _ConnectProc �֏I����ʒm���Ă���
	m_ConnectThread.PostQuitMessage(); 

	// ��M�p�X���b�h�ŏI���C�x���g�𔭐�������
	if( ::WSASetEvent(m_hConnectionExitEvent) == FALSE ) {
		CWSADebug::puts("At: WSASetEvent");
	}

	// connect �X���b�h�ŏI���C�x���g�𔭐�������
	if( ::WSASetEvent(m_hConnectExitEvent) == FALSE ) {
		CWSADebug::puts("At: WSASetEvent");
	}

	// ���̂��� _ConnectProc ���I������̂ő҂�
	m_ConnectThread.WaitThreadExit();
	m_ConnectThread.CloseThread();

	// �C�x���g�n���h���̃��Z�b�g
	::WSAResetEvent(m_hConnectionExitEvent);
	::WSAResetEvent(m_hConnectExitEvent);

	m_flagInitialized = FALSE;

	CWSADebug::puts(_T("Destroy ����"));

}

BOOL WSATcpClientBase::IsConstructionSucceeded()
{
	return (m_hConnectExitEvent != WSA_INVALID_EVENT) && (m_hConnectionExitEvent != WSA_INVALID_EVENT);
}

BOOL WSATcpClientBase::IsInitialized() 
{
	return m_flagInitialized;
}

WSAConnection* WSATcpClientBase::_MakeConnection()
{
	HANDLE hConnectEvent;
	::sockaddr_in  ServerAddr  = m_ServerAddr;
	WSAConnection* pConnection = NULL;         // NULL �̂Ƃ��̓G���[�������͏I���C�x���g������

	CWSADebug::puts(_T("�ڑ����J�n���܂�"));

	// �\�P�b�g�̍쐬
	SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if( sock == INVALID_SOCKET ) {
		// ��O�G���[
		CWSADebug::WSAPrintLastError(_T("At: WSASocket"));
		return NULL;
	}

	// �C�x���g����
	hConnectEvent = WSACreateEvent();
	if( hConnectEvent == WSA_INVALID_EVENT ) {
		// ��O�G���[
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
		closesocket(sock);
		return NULL;
	}

	// �C�x���g�̔����v����ݒ�
	int ret = WSAEventSelect(sock, hConnectEvent, FD_CONNECT); // connect ����u���b�L���O�ɂȂ�
	if( ret == SOCKET_ERROR ) {
		// ��O�G���[
		CWSADebug::WSAPrintLastError(_T("At: WSAEventSelect"));
		WSACloseEvent(hConnectEvent);
		closesocket(sock);
		return NULL;
	}

	HANDLE hEvents[2] = {hConnectEvent, m_hConnectExitEvent};

	// �I�����߂���M����܂Ń��[�v
	while( true ) {

		if( ServerAddr.sin_addr.S_un.S_addr == NULL ) {
			// �z�X�g���w��̏ꍇ
			ServerAddr.sin_addr.S_un.S_addr = _HostName2ValidAddr(m_HostName); 
		}

		// �T�[�o�[�ɐڑ�
		if( WSAConnect(sock, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR ) {
			int err = WSAGetLastError();
			if( (err != WSAEWOULDBLOCK) && (err != WSAEALREADY ) ) {
				// ��O�G���[
				CWSADebug::WSAPrintLastError(_T("At: WSAConnect"));
				pConnection = NULL;
				break;
			}
		}

		// �C�x���g�����܂őҋ@
		DWORD dwret = WSAWaitForMultipleEvents(2, hEvents, FALSE, N_CONNECT_TIMEOUT, FALSE); // ������PC���肾�Ǝ~�܂�Ȃ�
		if( dwret == WSA_WAIT_TIMEOUT ) {
			// �ҋ@�̃^�C���A�E�g�i��G���[�j
			continue;

		} else if( (dwret != WSA_WAIT_EVENT_0) && (dwret != WSA_WAIT_EVENT_0 + 1) ) {
			// �^�C���A�E�g�ȊO�i��O�G���[�j
			CWSADebug::WSAPrintLastError(_T("At: WSAWaitForMultipleEvents"));
			pConnection = NULL;
			break;
		}

		// �X���b�h�I���C�x���g���������������ׂ�
		if( dwret - WSA_WAIT_EVENT_0 == 1 ) {
			// m_hConnectExitEvent ���V�O�i����� �� �I��
			CWSADebug::puts(_T("�ڑ��ҋ@��Ԃ���X���b�h���I�����܂�(1)"));
			pConnection = NULL;
			break;
		}

		// �\��
		dwret = WSAWaitForMultipleEvents(1, &m_hConnectExitEvent, FALSE, 0, FALSE);
		if( dwret == WSA_WAIT_EVENT_0 ) {
			// �C�x���g���� �� �I��
			CWSADebug::puts(_T("�ڑ��ҋ@��Ԃ���X���b�h���I�����܂�(2)"));
			pConnection = NULL;
			break;
		}

		// �C�x���g�̔������R���擾
		WSANETWORKEVENTS events;
		int ret = WSAEnumNetworkEvents(sock, hConnectEvent, &events);
		if( ret == SOCKET_ERROR ) {
			// ��O�G���[
			CWSADebug::WSAPrintLastError(_T("At: WSAEnumNetworkEvents"));
			pConnection = NULL;
			break;
		} else if( (events.lNetworkEvents & FD_CONNECT) == 0 ) {
			// ��O�G���[
			CWSADebug::puts(_T("�z��O�̗v���ɂ��C�x���g���������܂����B")); // ���܂ɋN����
		}

		if((events.lNetworkEvents & FD_CONNECT) && (events.iErrorCode[FD_CONNECT_BIT] == 0)) {
			// �ڑ�OK

			pConnection = _CreateClient(sock, m_ServerAddr.sin_addr.S_un.S_addr, m_hConnectionExitEvent);
			break;

		} else {
			// �ڑ����ĂȂ�
			Sleep(100); // WSAWaitForMultipleEvents �������������ꍇ�����ő҂�
		}
	}

	// ����
	WSAEventSelect(sock, hConnectEvent, 0);

	// �J��
	WSACloseEvent(hConnectEvent);

	// �G���[���̃\�P�b�g�J��
	if( pConnection == NULL ) {
		closesocket(sock);
	}

	return pConnection;
}

UINT WSATcpClientBase::_ConnectProc(LPVOID pParam)
{
	WSATcpClientBase* pThis = reinterpret_cast<WSATcpClientBase*>(pParam);

	while( CheckQuitMessage() == FALSE ) {
		WSAConnection* pConnection = pThis->_MakeConnection();
		
		if( pConnection ) {
			// �ڑ��ɐ���
			CWSADebug::puts(_T("�ڑ��ɐ������܂���"));

			// connect �C�x���g
			pThis->OnConnect();

		} else {
			// �G���[�������͏I�����b�Z�[�W
			break;
		}

		pConnection->Resume(TRUE); // �ʐM���J�n
	}

	return 0;
}

DWORD _HostName2ValidAddr(LPCSTR lpHostName)
{
	LPHOSTENT pHost;
	IN_ADDR   ip_list[256];
	int       ip_list_count = 0;
	int       n;

	pHost = gethostbyname(lpHostName);
	if( pHost == NULL ) {
		CWSADebug::puts(_T("������܂���"));
		return FALSE;
	}

	if( pHost->h_length != 4 ) {
		CWSADebug::puts(_T("IPv4 �ł͂���܂���I"));
		return FALSE;
	}

	CWSADebug::printf("������ = %s\n" , pHost->h_name);

	for(int iCount = 0 ; pHost->h_aliases[iCount] ; iCount++) {
		CWSADebug::printf("�ʖ� = %s\n" , pHost->h_aliases[iCount]);
	}

	for(int iCount = 0 ; pHost->h_addr_list[iCount] ; iCount++) {
		CWSADebug::printf("IP = %d.%d.%d.%d\n" ,
			ip_list[iCount].S_un.S_un_b.s_b1 = (BYTE)*((pHost->h_addr_list[iCount])) ,
			ip_list[iCount].S_un.S_un_b.s_b2 = (BYTE)*((pHost->h_addr_list[iCount]) + 1) ,
			ip_list[iCount].S_un.S_un_b.s_b3 = (BYTE)*((pHost->h_addr_list[iCount]) + 2) ,
			ip_list[iCount].S_un.S_un_b.s_b4 = (BYTE)*((pHost->h_addr_list[iCount]) + 3)
		);
		
		ip_list_count++;
		if( ip_list_count >= sizeof(ip_list) / sizeof(ip_list[0]) ) {
			break;
		}
	}
	
	if( ip_list_count == 1 ) {
		// �擾�ł���IP��1�����̂Ƃ�
		n = 0;

	} else {
		// IP�������擾�ł����Ƃ�

		// �z�X�g�̏�񂪎擾�ł���IP��Ԃ�
		for( int i = 0; i < ip_list_count; i++ ) {
			pHost = gethostbyaddr(reinterpret_cast<const char*>(&ip_list[i].S_un.S_addr), 4, AF_INET);
			if( pHost ) {
				n = i;
				break;
			}
		}

		if( n >= ip_list_count ) {
			// �G���[
			CWSADebug::puts(_T("�z�X�g�̏��1���擾�ł��܂���ł���"));
			return NULL;
		}
	}

	return ip_list[n].S_un.S_addr;
}

//������������������������������������������������������������������������������������������������
//	WSAUdpBase
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	�\�z�E�j��
//*******************************************************************************
WSAUdpBase::WSAUdpBase()
{
	// ��M�X���b�h�I���p�C�x���g�̐���
	m_hExitEvent = ::WSACreateEvent();
	if (m_hExitEvent == WSA_INVALID_EVENT) {
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
	}

	m_sock = INVALID_SOCKET;
	m_msecReceiveTimeout	= N_DEFAULT_TIMEOUT;
}

WSAUdpBase::~WSAUdpBase()
{
	// ��M�X���b�h�I���p�C�x���g�̉��
	::WSACloseEvent(m_hExitEvent);
}


//*******************************************************************************
//	���Z�b�g
//*******************************************************************************
void WSAUdpBase::Destroy()
{
	// ��M�X���b�h���I��������
	if (::WSASetEvent(m_hExitEvent) == FALSE) {
		CWSADebug::puts(_T("At: WSASetEvent"));
	}
	m_ReceiveThread.WaitThreadExit();
	m_ReceiveThread.CloseThread();

	// �C�x���g�n���h���̃��Z�b�g
	::WSAResetEvent(m_hExitEvent);

	CWSADebug::puts(_T("Destroy ����"));
}

//*******************************************************************************
//	��M�X���b�h�̉ғ����J�n����
//*******************************************************************************
void WSAUdpBase::Resume(BOOL WaitThreadExit)
{
	m_ReceiveThread.ResumeThread();
	if (WaitThreadExit) {
		m_ReceiveThread.WaitThreadExit();
	}
}

//*******************************************************************************
//	�R���X�g���N�^�ł̏������������������ǂ�����Ԃ�
//*******************************************************************************
BOOL WSAUdpBase::IsConstructionSucceeded() const
{
	return m_hExitEvent != WSA_INVALID_EVENT;
}

//*******************************************************************************
//	���������s��ꂽ���ǂ�����Ԃ�
//*******************************************************************************
BOOL WSAUdpBase::IsInitialized() const
{
	return m_sock != INVALID_SOCKET;
}

//*******************************************************************************
//	������
//*******************************************************************************
BOOL WSAUdpBase::Initialize(DWORD msecReceiveTimeout)
{
	// �R���X�g���N�^�ł̏����������s���Ă��Ȃ������ׂ�
	if (IsConstructionSucceeded() == FALSE) {
		CWSADebug::puts(_T("�R���X�g���N�^�ł̏������Ɏ��s���Ă������ߏ����𒆒f���܂�"));
		return FALSE;
	}

	// ��x���Z�b�g
	Destroy();

	// UDP�\�P�b�g����
	SOCKET newsock = ::WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (newsock == INVALID_SOCKET) {
		// ��O�G���[
		CWSADebug::WSAPrintLastError(_T("At: WSASocket"));
		return FALSE;
	}

	// ��M�X���b�h�𐶐�
	if (m_ReceiveThread.CreateSuspendThread(_ReceiveProc, (LPVOID)this) == FALSE) {
		// ��O�G���[
		CWSADebug::puts(_T("�X���b�h�𐶐��ł��܂���ł���"));
		::closesocket(newsock);
		return FALSE;
	}

	// �S�����������̂Ń\�P�b�g�E�p�����[�^��ۑ�
	m_msecReceiveTimeout = msecReceiveTimeout;
	m_sock = newsock;

	// �������C�x���g
	OnInitialize();

	return TRUE;
}

//*******************************************************************************
//	���M
//*******************************************************************************
size_t WSAUdpBase::SendData(LPCVOID pBuff, size_t nSize, const ::sockaddr *toAddr, BOOL Blocking)
{
	if (IsInitialized() == FALSE) return 0;

	size_t nTotalSend = 0;

	while (nTotalSend < nSize) {
		int nSend = ::sendto(m_sock, static_cast<const char*>(pBuff)+nTotalSend, nSize - nTotalSend, 0, toAddr, sizeof(::sockaddr));

		if (nSend == SOCKET_ERROR) {
			if (::WSAGetLastError() == WSAEWOULDBLOCK) {
				if (Blocking == FALSE) {
					return nTotalSend;
				}
			}
			else {
				// ��O
				CWSADebug::WSAPrintLastError(_T("At: send"));
				return nTotalSend;
			}
		}
		else {
			nTotalSend += static_cast<size_t>(nSend);
		}
	}

	return nTotalSend;
}

//*******************************************************************************
//	��M
//*******************************************************************************
size_t WSAUdpBase::ReceiveData(LPVOID pBuff, size_t nSize, ::sockaddr *fromAddr)
{
	if (IsInitialized() == FALSE) return 0;

	int fromLen = sizeof(::sockaddr);

	int nReceive = ::recvfrom(m_sock, static_cast<char*>(pBuff), nSize, 0, fromAddr, &fromLen);

	if (nReceive == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			// ��O
			CWSADebug::WSAPrintLastError(_T("At: recv"));
		}

		return 0;
	}

	return nReceive;
}

//*******************************************************************************
//	��M�p�X���b�h
//*******************************************************************************
UINT WSAUdpBase::_ReceiveProc(LPVOID pParam)
{
	WSAUdpBase* pThis = static_cast<WSAUdpBase*>(pParam);
	HANDLE hEvent;

	CWSADebug::puts(_T("��M���J�n���܂�"));

	// �C�x���g����
	hEvent = ::WSACreateEvent();
	if (hEvent == WSA_INVALID_EVENT) {
		// ��O�G���[
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
		goto ENDLINE;
	}

	// �C�x���g�̔����v����ݒ�
	int ret = ::WSAEventSelect(pThis->m_sock, hEvent, FD_WRITE | FD_READ | FD_CLOSE);
	if (ret == SOCKET_ERROR) {
		// ��O�G���[
		CWSADebug::WSAPrintLastError(_T("At: WSAEventSelect"));
		::WSACloseEvent(hEvent);
		goto ENDLINE;
	}

	// �C�x���g�̔z������
	HANDLE hEvents[2] = { hEvent, pThis->m_hExitEvent };

	// �I�����߂���M����܂Ń��[�v
	while (CheckQuitMessage() == FALSE) {

		// �C�x���g�҂�
		DWORD dwret = ::WSAWaitForMultipleEvents(2, hEvents, FALSE, pThis->m_msecReceiveTimeout, FALSE);

		if (dwret == WSA_WAIT_TIMEOUT) {
			// ��M�^�C���A�E�g

			// ���z�֐��R�[��
			if (pThis->OnReceiveTimeout() == TRUE) {
				CWSADebug::puts(_T("��M���^�C���A�E�g���܂���"));
				break;
			}
			else {
				continue;
			}
		}
		else if (dwret == WSA_WAIT_FAILED) {
			CWSADebug::puts(_T("At: WSAWaitForMultipleEvents"));
			break;
		}

		// �X���b�h�I���C�x���g���������������ׂ�
		if (dwret - WSA_WAIT_EVENT_0 == 1) {
			// m_hExitEvent ���V�O�i����� �� �I��
			CWSADebug::puts(_T("��M�ҋ@��Ԃ���X���b�h���I�����܂�"));
			break;
		}

		// �C�x���g�̔������R���擾
		WSANETWORKEVENTS events;
		if (::WSAEnumNetworkEvents(pThis->m_sock, hEvent, &events) == SOCKET_ERROR) {
			// ��O�G���[
			CWSADebug::WSAPrintLastError(_T("At: WSAEnumNetworkEvents"));
			break;
		}
		else if ((events.lNetworkEvents & (FD_WRITE | FD_READ | FD_CLOSE)) == 0) {
			// ��O�G���[
			if (::WSAGetLastError() != WSAEWOULDBLOCK) {
				CWSADebug::puts(_T("�z��O�̗v���ɂ��C�x���g���������܂���"));
				CWSADebug::WSAPrintNetworkEvents(events);
			}
		}

		if (events.lNetworkEvents & FD_CLOSE) {
			// �l�b�g���[�N����ؒf���ꂽ
			CWSADebug::puts(_T("�l�b�g���[�N����ؒf����܂���"));

			break;
		}

		if (events.lNetworkEvents & FD_READ) {
			// ��M�C�x���g

			// ���z�֐��R�[��
			pThis->OnReceive();
		}

		if (events.lNetworkEvents & FD_WRITE) {
			// ���M�C�x���g

			// ���z�֐��R�[��
			pThis->OnSend();
		}
	}

	// �C�x���g����
	::WSAEventSelect(pThis->m_sock, hEvent, 0);

	// �C�x���g�J��
	::WSACloseEvent(hEvent);

ENDLINE:

	// �X���b�h�𔲂���Ƃ��͐ڑ����؂�
	if (::shutdown(pThis->m_sock, SD_BOTH) == SOCKET_ERROR) {
		CWSADebug::WSAPrintLastError(_T("At: shutdown"));
	}

	// �\�P�b�g���N���[�Y����
	if (::closesocket(pThis->m_sock) == SOCKET_ERROR) {
		CWSADebug::WSAPrintLastError(_T("At: closesocket"));
	}

	// �ؒf�C�x���g
	pThis->OnClose();

	CWSADebug::printf(_T("��M�p�X���b�h(0x%x)���I�����܂���\r\n"), ::GetCurrentThreadId());

	return 0;
}

//������������������������������������������������������������������������������������������������
//	WSAUdpServer
//������������������������������������������������������������������������������������������������
//*******************************************************************************
//	�\�z�E�j��
//*******************************************************************************
WSAUdpServer::WSAUdpServer()
: m_isReceivedData(FALSE)
{
	ZeroMemory(&m_clientAddr, sizeof(::sockaddr_in));
}

WSAUdpServer::~WSAUdpServer()
{

}

//*******************************************************************************
//	������
//*******************************************************************************
BOOL WSAUdpServer::Initialize(unsigned short nPortNo, DWORD msecReceiveTimeout)
{
	// �x�[�X�N���X�̏���������
	if (WSAUdpBase::Initialize(msecReceiveTimeout) == FALSE) {
		return FALSE;
	}

	// bind
	::sockaddr_in addr;
	ZeroMemory(&addr, sizeof(::sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(nPortNo);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	int nRet = ::bind(m_sock, (struct ::sockaddr*)&addr, sizeof(addr));
	if (nRet) {
		//  ��O�G���[
		CWSADebug::WSAPrintLastError(_T("At: bind"));
		::closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		return FALSE;
	}

	return TRUE;
}

//*******************************************************************************
//	���M
//*******************************************************************************
size_t WSAUdpServer::SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking)
{
	// ���߂̃f�[�^����M�����N���C�A���g�ɑ΂��đ��M����B
	// ��x���f�[�^����M���Ă��Ȃ��ꍇ�́A�������Ȃ��B
	if (m_isReceivedData == TRUE) {
		return WSAUdpBase::SendData(pBuff, nSize, (const struct ::sockaddr *)(&m_clientAddr), Blocking);
	}
	else{
		return 0;
	}
}

//*******************************************************************************
//	��M
//*******************************************************************************
size_t WSAUdpServer::ReceiveData(LPVOID pBuff, size_t nSize)
{
	size_t ret = WSAUdpBase::ReceiveData(pBuff, nSize, (struct ::sockaddr *)(&m_clientAddr));

	if (ret > 0){
		m_isReceivedData = TRUE;
	}

	return ret;
}


//������������������������������������������������������������������������������������������������
//	WSAUdpClient
//������������������������������������������������������������������������������������������������
//*******************************************************************************
//	�\�z�E�j��
//*******************************************************************************
WSAUdpClient::WSAUdpClient()
{
	ZeroMemory(&m_serverAddr, sizeof(sockaddr_in));
	m_hostName[0] = '0';
}

WSAUdpClient::~WSAUdpClient()
{

}

//*******************************************************************************
//	������
//*******************************************************************************
BOOL WSAUdpClient::Initialize(DWORD  IPv4Addr, WORD nPortNo, DWORD msecReceiveTimeout)
{
	// �x�[�X�N���X�̏���������
	if (WSAUdpBase::Initialize(msecReceiveTimeout) == FALSE) {
		return FALSE;
	}

	return _Initialize(IPv4Addr, NULL, nPortNo);
}

BOOL WSAUdpClient::Initialize(BYTE ip1, BYTE ip2, BYTE ip3, BYTE ip4, WORD nPortNo, DWORD msecReceiveTimeout)
{
	// �x�[�X�N���X�̏���������
	if (WSAUdpBase::Initialize(msecReceiveTimeout) == FALSE) {
		return FALSE;
	}

	return _Initialize(ip1 + ((DWORD)ip2 << 8) + ((DWORD)ip3 << 16) + ((DWORD)ip4 << 24), NULL, nPortNo);
}

BOOL WSAUdpClient::Initialize(LPCSTR lpHostName, WORD nPortNo, DWORD msecReceiveTimeout)
{
	// �x�[�X�N���X�̏���������
	if (WSAUdpBase::Initialize(msecReceiveTimeout) == FALSE) {
		return FALSE;
	}

	return _Initialize(NULL, lpHostName, nPortNo);
}

BOOL WSAUdpClient::_Initialize(DWORD  IPv4Addr, LPCSTR lpHostName, WORD nPortNo)
{
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_port = htons(nPortNo);

	if (lpHostName) {
		// �z�X�g���w��
		m_serverAddr.sin_addr.S_un.S_addr = NULL;
		::strcpy_s(m_hostName, sizeof(m_hostName), lpHostName);
	}
	else {
		// IP �A�h���X�w��
		m_serverAddr.sin_addr.S_un.S_addr = IPv4Addr;
		m_hostName[0] = '\0';
	}

	return TRUE;
}

//*******************************************************************************
//	���M
//*******************************************************************************
size_t WSAUdpClient::SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking)
{
	return WSAUdpBase::SendData(pBuff, nSize, (const ::sockaddr *)(&m_serverAddr), Blocking);
}

//*******************************************************************************
//	��M
//*******************************************************************************
size_t WSAUdpClient::ReceiveData(LPVOID pBuff, size_t nSize)
{
	// �T�[�o�̃A�h���X���͍X�V�s�v
	::sockaddr dummy;
	return WSAUdpBase::ReceiveData(pBuff, nSize, &dummy);
}


