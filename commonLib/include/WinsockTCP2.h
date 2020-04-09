#pragma once

#include <winsock2.h>
#include <map>

#include "WSADebug.h"

#include "Win32Thread.h"
#include "common/CommonMacro.h"

//*******************************************************************************
//	winsock 自動初期化用基底クラス
//
//	説明：
//		本クラスはサーバーまたはクライアントの基底クラスとなることで，最初のインス
//		タンス構築時に winsock を初期化する．
//*******************************************************************************
class WSABase
{
private:
	static UINT m_WSAStartCount; // WSAStartup を実行したか

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
//	単位接続クラス
//
//	説明：
//		１つの接続に対し１つのインスタンスが構築され，担当する．
//		本クラスは継承先のクラスによって機能を補完・追加されることによって様々な実
//		装に対応する．送信・受信バッファなどは継承先のクラスで実装すること．
//
//	サーバークラスに対する役割：
//		サーバークラスは本クラスをテンプレート引数として扱う．サーバークラス内で本
//		クラスは個々の接続に対してインスタンス化され，複数の接続を並列で処理する役
//		目を負う．
//
//	クライアントクラスに対する役割：
//		【まだ未定義】
//
//	コンストラクタ：
//		引数なし
//
//	公開関数：
//		Create      - accept または connect で取得したソケットを設定し，接続の初期
//		              化を行う
//		Resume      - インスタンスの動作を開始する
//		IsConnected - 接続が健在かどうか調べる
//		SendData    - データを送信する
//		ReceiveData - 受信したデータを取り出す
//		
//	擬似イベント関数：
//		OnSend    - send 関数実行後に再び send 関数を実行可能になったときに呼ばれる．
//		            主に継承先のクラスに実装した送信バッファの管理に利用する．
//		OnReceive - データを受信し，recv 関数が実行可能になった際に呼ばれる．主に継
//		            承先のクラスに実装した受信バッファの管理，パケットの処理に利用
//		            する．
//		OnClose   - 相手の切断，自身の終了，タイムアウト，エラー等の理由により接続
//		            が終了した際に呼ばれる．
//		OnReceiveTimeout 
//		          - 設定したタイムアウト時間の間，データの受信が無かった際に呼ばれ
//		            る．TRUE を返すと切断し，FALSE を返すと接続を維持する．
//
//	派生クラス実装時の注意点：
//		擬似イベント関数は，内部のスレッドから呼ばれる．OnSend, OnReceiveでバッファ
//		にアクセスするような場合は，バッファや send, recv に対して排他制御が必要と
//		なる．
//		擬似イベント関数を呼ぶ内部スレッドの同一性は保障されない．
//*******************************************************************************
class WSAConnection
{
protected:
	SOCKET m_sock;               // 接続のソケット
	DWORD  m_IPv4dw;             // 通信相手の IPv4 アドレス
	unsigned int m_myID;         // 接続ID
private:
	DWORD  m_msecReceiveTimeout; // 受信のタイムアウト時間
	HANDLE m_hExitEvent;         // 親クラスが起こす終了イベント

	BOOL   m_flagConnected;      // 接続しているかどうか

	Win32Thread m_ReceiveThread; // 受信スレッド

	// サーバーのみ使用
	Win32Thread* m_pParentMessageThread; // 親のメッセージを通知用スレッド
	HANDLE       m_hParentMessageEvent;  //
	HANDLE       m_hSendMessageMutex;    // SendMessageToParent が異なるスレッドから平行して実行されないようにするためのミューテックス

public:
	WSAConnection();
	virtual ~WSAConnection();

	// 稼動可能な状態にする
	BOOL Create(SOCKET sock, DWORD IPv4Addr, DWORD msecReceiveTimeout, HANDLE hExitEvent, unsigned int connectionID = 0,
		Win32Thread* pParentMessageThread = NULL);

	// 受信スレッドの稼動を開始する
	void Resume(BOOL WaitThreadExit = FALSE);

	// 状態取得用
	BOOL IsConnected() const { return m_flagConnected; }
	unsigned int GetConnectionID() const { return m_myID; }

	// 通信用関数
	size_t SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking = FALSE);
	size_t ReceiveData(LPVOID pBuff, size_t nSize, BOOL Blocking = FALSE);

protected:
	//
	virtual void OnSend() {}    // 送信不可能な状態から送信可能な状態になったら呼ばれる.
	virtual void OnReceive() {} // データを受信したときに呼ばれる
	virtual void OnClose() {}
	virtual BOOL OnReceiveTimeout() {return FALSE;} // m_msecReceiveTimeout の間データが来なかったら呼ばれる

private:
	void SendMessageToParent(UINT Msg);
	void PostDisconnectMessageToParent();

	static UINT __stdcall _ReceiveProc(LPVOID pParam); // 受信用スレッド
};

//*******************************************************************************
//	TCP サーバークラスの基本クラス
//
//	説明：
//		サーバークラスの共通実装部分．テンプレートクラスによってヘッダが肥大化する
//		のを防ぐためのもの．
//*******************************************************************************
class WSATcpServerBase : private WSABase
{
protected:
	typedef UINT (*ThreadProc)(LPVOID pParam);

protected:
	SOCKET m_sock; // サーバーのソケット

	DWORD  m_msecReceiveTimeout; // タイムアウトまでの時間（クライアントに継承される）

private:
	HANDLE m_hAcceptExitEvent;     // Accept  スレッドを終了させるためのイベント
	HANDLE m_hConnectionExitEvent; // Receive スレッドを終了させるためのイベント

	Win32Thread m_AcceptThread;        // 接続確立用スレッド
	Win32Thread m_ClientMessageThread; // クライアントからのメッセージを受け取るスレッド

public:
	WSATcpServerBase();
	virtual ~WSATcpServerBase();

	BOOL Initialize(unsigned short nPortNo, unsigned int nBacklogConnection, DWORD msecReceiveTimeout = N_DEFAULT_TIMEOUT);
	void Destroy();

	// 状態取得
	virtual BOOL IsConstructionSucceeded() const; // コンストラクタが正常に終了したか
	virtual BOOL IsInitialized() const;           // 初期化が実行されたか
	virtual int  GetClientCount() const = 0;      // 接続中のクライアントの数を返す

protected:
	// オーバーライド可能な関数
	virtual void OnInitialize     ()                       = 0;
	virtual void _OnAccept        (WSAConnection* pClient) = 0;
	virtual void _OnClose         (WSAConnection* pClient) = 0;
	virtual void _OnReceive       (WSAConnection* pClient) = 0;
	virtual void _OnSend          (WSAConnection* pClient) = 0;
	virtual void _OnReceiveTimeout(WSAConnection* pClient) = 0;

	// WSATcpServer にさせる処理
	virtual WSAConnection* _CreateNewClient(SOCKET ClientSock, DWORD IPv4Addr, HANDLE hConnectionExitEvent, Win32Thread* pClientMessageThread) = 0;
	virtual void _DestroyClient(LPVOID ClientKey) = 0;

protected: 
	static UINT __stdcall _AcceptProc(LPVOID pParam);        // クライアントの接続を受け付け，接続をオブジェクト化するためのスレッド
	static UINT __stdcall _ClientMessageProc(LPVOID pParam); // クライアントからのメッセージを処理するためのスレッド
};

//*******************************************************************************
//	TCP サーバークラス
//
//	説明：
//*******************************************************************************
template <class ClientConnection = WSAConnection> class WSATcpServer : public WSATcpServerBase
{
protected:
	typedef std::map<LPVOID, ClientConnection*> ClientMap;
	typedef ClientConnection ClientType;

protected:
	ClientMap m_ClientMap;
	HANDLE    m_hClientMapMutex;  
	unsigned int m_connectionID;    // 接続を特定する番号（新規接続ごとにインクリメントする）

protected:
	// 排他制御用の関数
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

	// 状態取得
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
	// オーバーライド可能な関数
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
	// WSATcpServerBase から呼ばれる処理
	WSAConnection* _CreateNewClient(SOCKET ClientSock, DWORD IPv4Addr, HANDLE hConnectionExitEvent, Win32Thread* pClientMessageThread)
	{
		ClientConnection* pNewClient = new ClientConnection();

		pNewClient->Create(ClientSock, IPv4Addr, m_msecReceiveTimeout, hConnectionExitEvent, ++m_connectionID, pClientMessageThread);

		_LockClientMap();

		std::pair<ClientMap::iterator, bool> res = m_ClientMap.insert(ClientMap::value_type(static_cast<WSAConnection*>(pNewClient), pNewClient));
		if( res.second == false ) {
			CWSADebug::printf(_T("Key(%x)のinsertに失敗！\r\n"), pNewClient);
		}

		_UnlockClientMap();

		return pNewClient;
	}

	// クライアントの削除解除
	void _DestroyClient(LPVOID ClientKey)
	{
		_LockClientMap();

		ClientMap::iterator it = m_ClientMap.find(ClientKey);

		if( it != m_ClientMap.end() ) {
			// 登録解除
			delete it->second;
			m_ClientMap.erase(it);
		} else {
			CWSADebug::printf(_T("Key(%x)が見つかりません！\r\n"), ClientKey);
		}
		_UnlockClientMap();
	}
};

class WSATcpClientBase : private WSABase
{
protected:
	sockaddr_in m_ServerAddr; // 接続先のIPアドレスとポート番号
	char   m_HostName[64];    // 接続先のホスト名（ホスト名指定で初期化されたときのみ使用）
	BOOL   m_flagInitialized; // 初期化されたかどうか

	DWORD  m_msecReceiveTimeout_t; // タイムアウトまでの時間（WSAConnection の m_msecReceiveTimeout へ継承）

private:
	HANDLE m_hConnectExitEvent;    // Accept  スレッドを終了させるためのイベント
	HANDLE m_hConnectionExitEvent; // Receive スレッドを終了させるためのイベント

	Win32Thread m_ConnectThread; // 接続・再接続確立用スレッド
public:
	WSATcpClientBase();
	virtual ~WSATcpClientBase();

	BOOL Initialize(DWORD  IPv4Addr,                        WORD nPortNo, DWORD msecReceiveTimeout = N_DEFAULT_TIMEOUT);
	BOOL Initialize(BYTE ip1, BYTE ip2, BYTE ip3, BYTE ip4, WORD nPortNo, DWORD msecReceiveTimeout = N_DEFAULT_TIMEOUT);
	BOOL Initialize(LPCSTR lpHostName,                      WORD nPortNo, DWORD msecReceiveTimeout = N_DEFAULT_TIMEOUT);
	void Destroy();

	// 状態取得
	virtual BOOL IsConstructionSucceeded(); // コンストラクタが正常に終了したか
	virtual BOOL IsInitialized();           // 初期化されたか

protected:
	// オーバーライド可能な関数
	virtual void OnInitialize() = 0;
	virtual void OnConnect()    = 0;

	// WSATcpClient にさせる処理
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
		WSATcpClientBase::Destroy(); // ClientConnection のデストラクタが呼ばれる前に通信を切断する
	}

	// WSAConnection の関数公開

	// 通信用関数
	inline size_t SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking = FALSE)   { return ClientConnection::SendData(pBuff, nSize, Blocking);    }
	inline size_t ReceiveData(LPVOID pBuff, size_t nSize, BOOL Blocking = FALSE) { return ClientConnection::ReceiveData(pBuff, nSize, Blocking); }

	// 状態取得
	inline BOOL IsConnected() const { return ClientConnection::IsConnected(); }

protected:
	// オーバーライド可能な関数
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
//	UDP サーバ/クライアントクラスの基本クラス
//
//	説明：UDPのサーバとクライアント共通の処理を定義
//	
//*******************************************************************************
class WSAUdpBase : public WSABase
{
protected:
	typedef UINT(*ThreadProc)(LPVOID pParam);

protected:
	SOCKET m_sock; // ソケット
	DWORD  m_msecReceiveTimeout; // タイムアウトまでの時間（クライアントに継承される）

private:
	Win32Thread m_ReceiveThread; // 受信スレッド
	HANDLE m_hExitEvent;         // 受信スレッドを終了させるためのイベント

public:
	WSAUdpBase();
	virtual ~WSAUdpBase();

	void Destroy();

	// 受信スレッドの稼動を開始する
	void Resume(BOOL WaitThreadExit = FALSE);

	// 状態取得
	virtual BOOL IsConstructionSucceeded() const; // コンストラクタが正常に終了したか
	virtual BOOL IsInitialized() const;           // 初期化が実行されたか

protected:
	// 初期化
	BOOL Initialize(DWORD msecReceiveTimeout = N_DEFAULT_TIMEOUT);	// UDPソケットの生成のみ
	
	// 通信用関数
	size_t SendData(LPCVOID pBuff, size_t nSize, const sockaddr *toAddr, BOOL Blocking = FALSE);
	size_t ReceiveData(LPVOID pBuff, size_t nSize, sockaddr *fromAddr);

	// オーバーライドが必要な関数
	virtual void OnInitialize()		= 0;
	virtual void OnClose()			= 0;
	virtual void OnReceive()		= 0;
	virtual void OnSend()			= 0;
	virtual BOOL OnReceiveTimeout() = 0;

protected:
	static UINT __stdcall _ReceiveProc(LPVOID pParam);			// 受信用スレッド
};

//*******************************************************************************
//	UDP サーバクラス
//
//	説明：
//	
//*******************************************************************************
class WSAUdpServer : public WSAUdpBase
{
protected:
	sockaddr_in m_clientAddr;		// 接続してくるクライアントのアドレス情報(recvFromで取得)
	BOOL m_isReceivedData;			// クライアントからデータを受信したか？

public:
	WSAUdpServer();
	virtual ~WSAUdpServer();

	// 初期化
	BOOL Initialize(unsigned short nPortNo, DWORD msecReceiveTimeout = WSABase::N_DEFAULT_TIMEOUT);	// bind
	
	// 通信用関数
	size_t SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking = FALSE);
	size_t ReceiveData(LPVOID pBuff, size_t nSize);

protected:

	// オーバーライド可能な関数
	virtual void OnInitialize() {}
	virtual void OnClose() {}
	virtual void OnReceive() {}
	virtual void OnSend() {}
	virtual BOOL OnReceiveTimeout() { return FALSE; }	// 受信処理継続
};

//*******************************************************************************
//	UDP クライアントクラス
//
//	説明：
//	
//*******************************************************************************
class WSAUdpClient : public WSAUdpBase
{
protected:
	sockaddr_in m_serverAddr; // サーバのIPアドレスとポート番号
	char   m_hostName[64];    // サーバのホスト名（ホスト名指定で初期化されたときのみ使用）

public:
	WSAUdpClient();
	virtual ~WSAUdpClient();

	// 初期化
	BOOL Initialize(DWORD  IPv4Addr, WORD nPortNo, DWORD msecReceiveTimeout = WSABase::N_DEFAULT_TIMEOUT);
	BOOL Initialize(BYTE ip1, BYTE ip2, BYTE ip3, BYTE ip4, WORD nPortNo, DWORD msecReceiveTimeout = WSABase::N_DEFAULT_TIMEOUT);
	BOOL Initialize(LPCSTR lpHostName, WORD nPortNo, DWORD msecReceiveTimeout = WSABase::N_DEFAULT_TIMEOUT);

	// 通信用関数
	size_t SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking = FALSE);
	size_t ReceiveData(LPVOID pBuff, size_t nSize);

protected:

	// オーバーライド可能な関数
	virtual void OnInitialize() {}
	virtual void OnClose() {}
	virtual void OnReceive() {}
	virtual void OnSend() {}
	virtual BOOL OnReceiveTimeout() { return FALSE; }	// 受信処理継続

private:
	BOOL _Initialize(DWORD  IPv4Addr, LPCSTR lpHostName, WORD nPortNo);
};

