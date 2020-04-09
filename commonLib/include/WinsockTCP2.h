#pragma once

#include <winsock2.h>
#include <map>

#include "WSADebug.h"

#include "Win32Thread.h"
#include "common/CommonMacro.h"

//*******************************************************************************
//	winsock �����������p���N���X
//
//	�����F
//		�{�N���X�̓T�[�o�[�܂��̓N���C�A���g�̊��N���X�ƂȂ邱�ƂŁC�ŏ��̃C���X
//		�^���X�\�z���� winsock ������������D
//*******************************************************************************
class WSABase
{
private:
	static UINT m_WSAStartCount; // WSAStartup �����s������

public:
	enum {
		N_DEFAULT_TIMEOUT = 3000,
		N_CONNECT_TIMEOUT = 3000,
	};

public:
	WSABase();
	virtual ~WSABase();
};

//*******************************************************************************
//	�P�ʐڑ��N���X
//
//	�����F
//		�P�̐ڑ��ɑ΂��P�̃C���X�^���X���\�z����C�S������D
//		�{�N���X�͌p����̃N���X�ɂ���ċ@�\��⊮�E�ǉ�����邱�Ƃɂ���ėl�X�Ȏ�
//		���ɑΉ�����D���M�E��M�o�b�t�@�Ȃǂ͌p����̃N���X�Ŏ������邱�ƁD
//
//	�T�[�o�[�N���X�ɑ΂�������F
//		�T�[�o�[�N���X�͖{�N���X���e���v���[�g�����Ƃ��Ĉ����D�T�[�o�[�N���X���Ŗ{
//		�N���X�͌X�̐ڑ��ɑ΂��ăC���X�^���X������C�����̐ڑ������ŏ��������
//		�ڂ𕉂��D
//
//	�N���C�A���g�N���X�ɑ΂�������F
//		�y�܂�����`�z
//
//	�R���X�g���N�^�F
//		�����Ȃ�
//
//	���J�֐��F
//		Create      - accept �܂��� connect �Ŏ擾�����\�P�b�g��ݒ肵�C�ڑ��̏���
//		              �����s��
//		Resume      - �C���X�^���X�̓�����J�n����
//		IsConnected - �ڑ������݂��ǂ������ׂ�
//		SendData    - �f�[�^�𑗐M����
//		ReceiveData - ��M�����f�[�^�����o��
//		
//	�[���C�x���g�֐��F
//		OnSend    - send �֐����s��ɍĂ� send �֐������s�\�ɂȂ����Ƃ��ɌĂ΂��D
//		            ��Ɍp����̃N���X�Ɏ����������M�o�b�t�@�̊Ǘ��ɗ��p����D
//		OnReceive - �f�[�^����M���Crecv �֐������s�\�ɂȂ����ۂɌĂ΂��D��Ɍp
//		            ����̃N���X�Ɏ���������M�o�b�t�@�̊Ǘ��C�p�P�b�g�̏����ɗ��p
//		            ����D
//		OnClose   - ����̐ؒf�C���g�̏I���C�^�C���A�E�g�C�G���[���̗��R�ɂ��ڑ�
//		            ���I�������ۂɌĂ΂��D
//		OnReceiveTimeout 
//		          - �ݒ肵���^�C���A�E�g���Ԃ̊ԁC�f�[�^�̎�M�����������ۂɌĂ΂�
//		            ��DTRUE ��Ԃ��Ɛؒf���CFALSE ��Ԃ��Ɛڑ����ێ�����D
//
//	�h���N���X�������̒��ӓ_�F
//		�[���C�x���g�֐��́C�����̃X���b�h����Ă΂��DOnSend, OnReceive�Ńo�b�t�@
//		�ɃA�N�Z�X����悤�ȏꍇ�́C�o�b�t�@�� send, recv �ɑ΂��Ĕr�����䂪�K�v��
//		�Ȃ�D
//		�[���C�x���g�֐����Ăԓ����X���b�h�̓��ꐫ�͕ۏႳ��Ȃ��D
//*******************************************************************************
class WSAConnection
{
protected:
	SOCKET m_sock;               // �ڑ��̃\�P�b�g
	DWORD  m_IPv4dw;             // �ʐM����� IPv4 �A�h���X
	unsigned int m_myID;         // �ڑ�ID
private:
	DWORD  m_msecReceiveTimeout; // ��M�̃^�C���A�E�g����
	HANDLE m_hExitEvent;         // �e�N���X���N�����I���C�x���g

	BOOL   m_flagConnected;      // �ڑ����Ă��邩�ǂ���

	Win32Thread m_ReceiveThread; // ��M�X���b�h

	// �T�[�o�[�̂ݎg�p
	Win32Thread* m_pParentMessageThread; // �e�̃��b�Z�[�W��ʒm�p�X���b�h
	HANDLE       m_hParentMessageEvent;  //
	HANDLE       m_hSendMessageMutex;    // SendMessageToParent ���قȂ�X���b�h���畽�s���Ď��s����Ȃ��悤�ɂ��邽�߂̃~���[�e�b�N�X

public:
	WSAConnection();
	virtual ~WSAConnection();

	// �ғ��\�ȏ�Ԃɂ���
	BOOL Create(SOCKET sock, DWORD IPv4Addr, DWORD msecReceiveTimeout, HANDLE hExitEvent, unsigned int connectionID = 0,
		Win32Thread* pParentMessageThread = NULL);

	// ��M�X���b�h�̉ғ����J�n����
	void Resume(BOOL WaitThreadExit = FALSE);

	// ��Ԏ擾�p
	BOOL IsConnected() const { return m_flagConnected; }
	unsigned int GetConnectionID() const { return m_myID; }

	// �ʐM�p�֐�
	size_t SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking = FALSE);
	size_t ReceiveData(LPVOID pBuff, size_t nSize, BOOL Blocking = FALSE);

protected:
	//
	virtual void OnSend() {}    // ���M�s�\�ȏ�Ԃ��瑗�M�\�ȏ�ԂɂȂ�����Ă΂��.
	virtual void OnReceive() {} // �f�[�^����M�����Ƃ��ɌĂ΂��
	virtual void OnClose() {}
	virtual BOOL OnReceiveTimeout() {return FALSE;} // m_msecReceiveTimeout �̊ԃf�[�^�����Ȃ�������Ă΂��

private:
	void SendMessageToParent(UINT Msg);
	void PostDisconnectMessageToParent();

	static UINT __stdcall _ReceiveProc(LPVOID pParam); // ��M�p�X���b�h
};

//*******************************************************************************
//	TCP �T�[�o�[�N���X�̊�{�N���X
//
//	�����F
//		�T�[�o�[�N���X�̋��ʎ��������D�e���v���[�g�N���X�ɂ���ăw�b�_����剻����
//		�̂�h�����߂̂��́D
//*******************************************************************************
class WSATcpServerBase : private WSABase
{
protected:
	typedef UINT (*ThreadProc)(LPVOID pParam);

protected:
	SOCKET m_sock; // �T�[�o�[�̃\�P�b�g

	DWORD  m_msecReceiveTimeout; // �^�C���A�E�g�܂ł̎��ԁi�N���C�A���g�Ɍp�������j

private:
	HANDLE m_hAcceptExitEvent;     // Accept  �X���b�h���I�������邽�߂̃C�x���g
	HANDLE m_hConnectionExitEvent; // Receive �X���b�h���I�������邽�߂̃C�x���g

	Win32Thread m_AcceptThread;        // �ڑ��m���p�X���b�h
	Win32Thread m_ClientMessageThread; // �N���C�A���g����̃��b�Z�[�W���󂯎��X���b�h

public:
	WSATcpServerBase();
	virtual ~WSATcpServerBase();

	BOOL Initialize(unsigned short nPortNo, unsigned int nBacklogConnection, DWORD msecReceiveTimeout = N_DEFAULT_TIMEOUT);
	void Destroy();

	// ��Ԏ擾
	virtual BOOL IsConstructionSucceeded() const; // �R���X�g���N�^������ɏI��������
	virtual BOOL IsInitialized() const;           // �����������s���ꂽ��
	virtual int  GetClientCount() const = 0;      // �ڑ����̃N���C�A���g�̐���Ԃ�

protected:
	// �I�[�o�[���C�h�\�Ȋ֐�
	virtual void OnInitialize     ()                       = 0;
	virtual void _OnAccept        (WSAConnection* pClient) = 0;
	virtual void _OnClose         (WSAConnection* pClient) = 0;
	virtual void _OnReceive       (WSAConnection* pClient) = 0;
	virtual void _OnSend          (WSAConnection* pClient) = 0;
	virtual void _OnReceiveTimeout(WSAConnection* pClient) = 0;

	// WSATcpServer �ɂ����鏈��
	virtual WSAConnection* _CreateNewClient(SOCKET ClientSock, DWORD IPv4Addr, HANDLE hConnectionExitEvent, Win32Thread* pClientMessageThread) = 0;
	virtual void _DestroyClient(LPVOID ClientKey) = 0;

protected: 
	static UINT __stdcall _AcceptProc(LPVOID pParam);        // �N���C�A���g�̐ڑ����󂯕t���C�ڑ����I�u�W�F�N�g�����邽�߂̃X���b�h
	static UINT __stdcall _ClientMessageProc(LPVOID pParam); // �N���C�A���g����̃��b�Z�[�W���������邽�߂̃X���b�h
};

//*******************************************************************************
//	TCP �T�[�o�[�N���X
//
//	�����F
//*******************************************************************************
template <class ClientConnection = WSAConnection> class WSATcpServer : public WSATcpServerBase
{
protected:
	typedef std::map<LPVOID, ClientConnection*> ClientMap;
	typedef ClientConnection ClientType;

protected:
	ClientMap m_ClientMap;
	HANDLE    m_hClientMapMutex;  
	unsigned int m_connectionID;    // �ڑ�����肷��ԍ��i�V�K�ڑ����ƂɃC���N�������g����j

protected:
	// �r������p�̊֐�
	void _LockClientMap() const   { ::WaitForSingleObject(m_hClientMapMutex, INFINITE); }
	void _UnlockClientMap() const { ::ReleaseMutex(m_hClientMapMutex); }

public:
	WSATcpServer()
	{
		m_hClientMapMutex = ::CreateMutex(NULL, FALSE, NULL);
		m_connectionID = 0;
	}
	virtual ~WSATcpServer()
	{
		::CloseHandle(m_hClientMapMutex);
		Destroy();
	}

	// ��Ԏ擾
	virtual BOOL IsConstructionSucceeded() const
	{ 
		return WSATcpServerBase::IsConstructionSucceeded() && (m_hClientMapMutex != NULL);
	}
	int GetClientCount() const
	{
		_LockClientMap();
		int ret = m_ClientMap.size();
		_UnlockClientMap();
		return ret;
	}

protected:
	// �I�[�o�[���C�h�\�Ȋ֐�
	virtual void OnInitialize    ()                          {}
	virtual void OnAccept        (ClientConnection* pClient) {}
	virtual void OnClose         (ClientConnection* pClient) {}
	virtual void OnReceive       (ClientConnection* pClient) {}
	virtual void OnSend          (ClientConnection* pClient) {}
	virtual void OnReceiveTimeout(ClientConnection* pClient) {}

private:
	void _OnAccept        (WSAConnection* pClient) { OnAccept        (static_cast<ClientConnection*>(pClient)); }
	void _OnClose         (WSAConnection* pClient) { OnClose         (static_cast<ClientConnection*>(pClient)); }
	void _OnReceive       (WSAConnection* pClient) { OnReceive       (static_cast<ClientConnection*>(pClient)); }
	void _OnSend          (WSAConnection* pClient) { OnSend          (static_cast<ClientConnection*>(pClient)); }
	void _OnReceiveTimeout(WSAConnection* pClient) { OnReceiveTimeout(static_cast<ClientConnection*>(pClient)); }

private:
	// WSATcpServerBase ����Ă΂�鏈��
	WSAConnection* _CreateNewClient(SOCKET ClientSock, DWORD IPv4Addr, HANDLE hConnectionExitEvent, Win32Thread* pClientMessageThread)
	{
		ClientConnection* pNewClient = new ClientConnection();

		pNewClient->Create(ClientSock, IPv4Addr, m_msecReceiveTimeout, hConnectionExitEvent, ++m_connectionID, pClientMessageThread);

		_LockClientMap();

		std::pair<ClientMap::iterator, bool> res = m_ClientMap.insert(ClientMap::value_type(static_cast<WSAConnection*>(pNewClient), pNewClient));
		if( res.second == false ) {
			CWSADebug::printf(_T("Key(%x)��insert�Ɏ��s�I\r\n"), pNewClient);
		}

		_UnlockClientMap();

		return pNewClient;
	}

	// �N���C�A���g�̍폜����
	void _DestroyClient(LPVOID ClientKey)
	{
		_LockClientMap();

		ClientMap::iterator it = m_ClientMap.find(ClientKey);

		if( it != m_ClientMap.end() ) {
			// �o�^����
			delete it->second;
			m_ClientMap.erase(it);
		} else {
			CWSADebug::printf(_T("Key(%x)��������܂���I\r\n"), ClientKey);
		}
		_UnlockClientMap();
	}
};

class WSATcpClientBase : private WSABase
{
protected:
	sockaddr_in m_ServerAddr; // �ڑ����IP�A�h���X�ƃ|�[�g�ԍ�
	char   m_HostName[64];    // �ڑ���̃z�X�g���i�z�X�g���w��ŏ��������ꂽ�Ƃ��̂ݎg�p�j
	BOOL   m_flagInitialized; // ���������ꂽ���ǂ���

	DWORD  m_msecReceiveTimeout_t; // �^�C���A�E�g�܂ł̎��ԁiWSAConnection �� m_msecReceiveTimeout �֌p���j

private:
	HANDLE m_hConnectExitEvent;    // Accept  �X���b�h���I�������邽�߂̃C�x���g
	HANDLE m_hConnectionExitEvent; // Receive �X���b�h���I�������邽�߂̃C�x���g

	Win32Thread m_ConnectThread; // �ڑ��E�Đڑ��m���p�X���b�h
public:
	WSATcpClientBase();
	virtual ~WSATcpClientBase();

	BOOL Initialize(DWORD  IPv4Addr,                        WORD nPortNo, DWORD msecReceiveTimeout = N_DEFAULT_TIMEOUT);
	BOOL Initialize(BYTE ip1, BYTE ip2, BYTE ip3, BYTE ip4, WORD nPortNo, DWORD msecReceiveTimeout = N_DEFAULT_TIMEOUT);
	BOOL Initialize(LPCSTR lpHostName,                      WORD nPortNo, DWORD msecReceiveTimeout = N_DEFAULT_TIMEOUT);
	void Destroy();

	// ��Ԏ擾
	virtual BOOL IsConstructionSucceeded(); // �R���X�g���N�^������ɏI��������
	virtual BOOL IsInitialized();           // ���������ꂽ��

protected:
	// �I�[�o�[���C�h�\�Ȋ֐�
	virtual void OnInitialize() = 0;
	virtual void OnConnect()    = 0;

	// WSATcpClient �ɂ����鏈��
	virtual WSAConnection* _CreateClient(SOCKET ClientSock, DWORD IPv4Addr, HANDLE hClientExitEvent) = 0;

private:
	BOOL _Initialize(DWORD  IPv4Addr, LPCSTR lpHostName, WORD nPortNo, DWORD msecReceiveTimeout);

	WSAConnection* _MakeConnection();

	static UINT __stdcall _ConnectProc(LPVOID pParam);
};

template<class ClientConnection = WSAConnection> class WSATcpClient : public WSATcpClientBase, protected ClientConnection //WSATcpClient<ClientConnection>
{
public:
	virtual ~WSATcpClient()
	{
		WSATcpClientBase::Destroy(); // ClientConnection �̃f�X�g���N�^���Ă΂��O�ɒʐM��ؒf����
	}

	// WSAConnection �̊֐����J

	// �ʐM�p�֐�
	inline size_t SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking = FALSE)   { return ClientConnection::SendData(pBuff, nSize, Blocking);    }
	inline size_t ReceiveData(LPVOID pBuff, size_t nSize, BOOL Blocking = FALSE) { return ClientConnection::ReceiveData(pBuff, nSize, Blocking); }

	// ��Ԏ擾
	inline BOOL IsConnected() const { return ClientConnection::IsConnected(); }

protected:
	// �I�[�o�[���C�h�\�Ȋ֐�
	virtual void OnInitialize()     {}
	virtual void OnConnect()        {}
	//virtual void OnClose()          {}
	//virtual void OnReceive()        {}
	//virtual void OnSend()           {}
	//virtual BOOL OnReceiveTimeout() { return TRUE; }

private:
	WSAConnection* _CreateClient(SOCKET ClientSock, DWORD IPv4Addr, HANDLE hConnectionExitEvent)
	{
		if( ClientConnection::Create(ClientSock, IPv4Addr, m_msecReceiveTimeout_t, hConnectionExitEvent) ) {
			return this;
		} else {
			return NULL;
		}
	}
};

//*******************************************************************************
//	UDP �T�[�o/�N���C�A���g�N���X�̊�{�N���X
//
//	�����FUDP�̃T�[�o�ƃN���C�A���g���ʂ̏������`
//	
//*******************************************************************************
class WSAUdpBase : public WSABase
{
protected:
	typedef UINT(*ThreadProc)(LPVOID pParam);

protected:
	SOCKET m_sock; // �\�P�b�g
	DWORD  m_msecReceiveTimeout; // �^�C���A�E�g�܂ł̎��ԁi�N���C�A���g�Ɍp�������j

private:
	Win32Thread m_ReceiveThread; // ��M�X���b�h
	HANDLE m_hExitEvent;         // ��M�X���b�h���I�������邽�߂̃C�x���g

public:
	WSAUdpBase();
	virtual ~WSAUdpBase();

	void Destroy();

	// ��M�X���b�h�̉ғ����J�n����
	void Resume(BOOL WaitThreadExit = FALSE);

	// ��Ԏ擾
	virtual BOOL IsConstructionSucceeded() const; // �R���X�g���N�^������ɏI��������
	virtual BOOL IsInitialized() const;           // �����������s���ꂽ��

protected:
	// ������
	BOOL Initialize(DWORD msecReceiveTimeout = N_DEFAULT_TIMEOUT);	// UDP�\�P�b�g�̐����̂�
	
	// �ʐM�p�֐�
	size_t SendData(LPCVOID pBuff, size_t nSize, const sockaddr *toAddr, BOOL Blocking = FALSE);
	size_t ReceiveData(LPVOID pBuff, size_t nSize, sockaddr *fromAddr);

	// �I�[�o�[���C�h���K�v�Ȋ֐�
	virtual void OnInitialize()		= 0;
	virtual void OnClose()			= 0;
	virtual void OnReceive()		= 0;
	virtual void OnSend()			= 0;
	virtual BOOL OnReceiveTimeout() = 0;

protected:
	static UINT __stdcall _ReceiveProc(LPVOID pParam);			// ��M�p�X���b�h
};

//*******************************************************************************
//	UDP �T�[�o�N���X
//
//	�����F
//	
//*******************************************************************************
class WSAUdpServer : public WSAUdpBase
{
protected:
	sockaddr_in m_clientAddr;		// �ڑ����Ă���N���C�A���g�̃A�h���X���(recvFrom�Ŏ擾)
	BOOL m_isReceivedData;			// �N���C�A���g����f�[�^����M�������H

public:
	WSAUdpServer();
	virtual ~WSAUdpServer();

	// ������
	BOOL Initialize(unsigned short nPortNo, DWORD msecReceiveTimeout = WSABase::N_DEFAULT_TIMEOUT);	// bind
	
	// �ʐM�p�֐�
	size_t SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking = FALSE);
	size_t ReceiveData(LPVOID pBuff, size_t nSize);

protected:

	// �I�[�o�[���C�h�\�Ȋ֐�
	virtual void OnInitialize() {}
	virtual void OnClose() {}
	virtual void OnReceive() {}
	virtual void OnSend() {}
	virtual BOOL OnReceiveTimeout() { return FALSE; }	// ��M�����p��
};

//*******************************************************************************
//	UDP �N���C�A���g�N���X
//
//	�����F
//	
//*******************************************************************************
class WSAUdpClient : public WSAUdpBase
{
protected:
	sockaddr_in m_serverAddr; // �T�[�o��IP�A�h���X�ƃ|�[�g�ԍ�
	char   m_hostName[64];    // �T�[�o�̃z�X�g���i�z�X�g���w��ŏ��������ꂽ�Ƃ��̂ݎg�p�j

public:
	WSAUdpClient();
	virtual ~WSAUdpClient();

	// ������
	BOOL Initialize(DWORD  IPv4Addr, WORD nPortNo, DWORD msecReceiveTimeout = WSABase::N_DEFAULT_TIMEOUT);
	BOOL Initialize(BYTE ip1, BYTE ip2, BYTE ip3, BYTE ip4, WORD nPortNo, DWORD msecReceiveTimeout = WSABase::N_DEFAULT_TIMEOUT);
	BOOL Initialize(LPCSTR lpHostName, WORD nPortNo, DWORD msecReceiveTimeout = WSABase::N_DEFAULT_TIMEOUT);

	// �ʐM�p�֐�
	size_t SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking = FALSE);
	size_t ReceiveData(LPVOID pBuff, size_t nSize);

protected:

	// �I�[�o�[���C�h�\�Ȋ֐�
	virtual void OnInitialize() {}
	virtual void OnClose() {}
	virtual void OnReceive() {}
	virtual void OnSend() {}
	virtual BOOL OnReceiveTimeout() { return FALSE; }	// ��M�����p��

private:
	BOOL _Initialize(DWORD  IPv4Addr, LPCSTR lpHostName, WORD nPortNo);
};

