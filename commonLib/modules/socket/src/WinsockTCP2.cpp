#include "stdafx.h"
#include "WinsockTCP2.h"
#include "WSADebug.h"
#include "common/CommonMacro.h"

#pragma comment(lib, "Ws2_32.lib")

#define MSG_CLOSE_EVENT        (WM_APP + 1) // ソケットがクローズされようとしている
#define MSG_READ_EVENT         (WM_APP + 2) // データを受信した
#define MSG_WRITE_EVENT        (WM_APP + 3) // 再び send 可能になった
#define MSG_READ_TIMEOUT_EVENT (WM_APP + 4) // 一定時間受信が無かった
#define MSG_DISCONNECT_EVENT   (WM_APP + 5) // クライアントとの接続を閉じた

DWORD _HostName2ValidAddr(LPCSTR lpHostName);

//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
//	WSABase
//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆

//*******************************************************************************
//	スタティック変数
//*******************************************************************************
UINT WSABase::m_WSAStartCount = 0;

//*******************************************************************************
//	構築・破棄
//*******************************************************************************
WSABase::WSABase()
{
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("WSABase_Initializing_Mutex"));
	::WaitForSingleObject(hMutex, INFINITE);

	if( WSABase::m_WSAStartCount == 0 ) { 
		::WSADATA wsaData;
		if( int nRet = ::WSAStartup(MAKEWORD(2,0), &wsaData) ) {
			// 例外エラー
			CWSADebug::WSAFormatMessage(nRet, _T("At: WSAStartup"));
		} else {
			// 成功
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

//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
//	WSAConnection
//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆

//*******************************************************************************
//	構築・破棄
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
	// 受信スレッドの終了待機
	m_ReceiveThread.WaitThreadExit(); // サーバーまたはクライアントから受信スレッドが終了されるのを待つ
	m_ReceiveThread.CloseThread();

	// 同期オブジェクトの破棄
	::CloseHandle(m_hParentMessageEvent);
	::CloseHandle(m_hSendMessageMutex);
}

BOOL WSAConnection::Create(SOCKET sock, DWORD IPv4Addr, DWORD msecReceiveTimeout, HANDLE hExitEvent, unsigned int connectionID, 
	                       Win32Thread* pParentMessageThread)
{
	// 動作中のスレッドを殺してから

	// 受信スレッドを生成
	if( m_ReceiveThread.CreateSuspendThread(_ReceiveProc, (LPVOID)this) == FALSE ) {
		// 例外エラー
		CWSADebug::puts(_T("スレッドを生成できませんでした"));
		return FALSE;
	}

	// 値を設定
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
				// 例外
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
				// 例外
				CWSADebug::WSAPrintLastError(_T("At: recv"));
				break;
			}
		} else if( nReceive == 0 ) {
			// ソケットが閉じられた場合
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
		// ロックここから（２つの異なるスレッドがこの関数を実行するとメッセージが処理される前に抜けてしまう場合がある）
		::WaitForSingleObject(m_hSendMessageMutex, INFINITE);

		::ResetEvent(m_hParentMessageEvent);
		m_pParentMessageThread->PostThreadMessage(Msg, reinterpret_cast<WPARAM>(this), reinterpret_cast<LPARAM>(m_hParentMessageEvent));
		::WaitForSingleObject(m_hParentMessageEvent, INFINITE); // ポスト先のスレッドがイベントオブジェクトをシグナル状態にするまで待機する

		// アンロックここで
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

	CWSADebug::puts(_T("受信を開始します"));

	// イベント生成
	hEvent = ::WSACreateEvent();
	if( hEvent == WSA_INVALID_EVENT ) {
		// 例外エラー
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
		goto ENDLINE;
	}

	// イベントの発生要因を設定
	int ret = ::WSAEventSelect(pThis->m_sock, hEvent, FD_WRITE | FD_READ | FD_CLOSE);
	if( ret == SOCKET_ERROR ) {
		// 例外エラー
		CWSADebug::WSAPrintLastError(_T("At: WSAEventSelect"));
		::WSACloseEvent(hEvent);
		goto ENDLINE;
	}

	// イベントの配列を作る
	HANDLE hEvents[2] = {hEvent, pThis->m_hExitEvent};

	// 終了命令を受信するまでループ
	while( CheckQuitMessage() == FALSE ) {

		// イベント待ち
		DWORD dwret = ::WSAWaitForMultipleEvents(2, hEvents, FALSE, pThis->m_msecReceiveTimeout, FALSE);

		if( dwret == WSA_WAIT_TIMEOUT ) {
			// 受信タイムアウト → 接続が切れた？

			// 仮想関数コール（デフォルト：再接続）
			BOOL Resume = pThis->OnReceiveTimeout();
			pThis->SendMessageToParent(MSG_READ_TIMEOUT_EVENT);
			
			if( Resume == FALSE ) {
				// 戻り値 FALSE で再接続をキャンセル
				continue;
			}

			CWSADebug::puts(_T("受信がタイムアウトしました"));
			
			break;

		} else if( dwret == WSA_WAIT_FAILED ) {
			CWSADebug::puts(_T("At: WSAWaitForMultipleEvents"));
			break;
		}

		// スレッド終了イベントが発生したか調べる
		if( dwret - WSA_WAIT_EVENT_0 == 1 ) {
			// m_hExitEvent がシグナル状態 → 終了
			CWSADebug::puts(_T("受信待機状態からスレッドを終了します"));
			break;
		}

		// イベントの発生理由を取得
		WSANETWORKEVENTS events;
		if( ::WSAEnumNetworkEvents(pThis->m_sock, hEvent, &events) == SOCKET_ERROR ) {
			// 例外エラー
			CWSADebug::WSAPrintLastError(_T("At: WSAEnumNetworkEvents"));
			break;
		} else if( (events.lNetworkEvents & (FD_WRITE | FD_READ | FD_CLOSE)) == 0 ) {
			// 例外エラー
			if( ::WSAGetLastError() != WSAEWOULDBLOCK ) {
				CWSADebug::puts(_T("想定外の要因によりイベントが発生しました"));
				CWSADebug::WSAPrintNetworkEvents(events);
			}
		}

		if( events.lNetworkEvents & FD_CLOSE ) {
			// ネットワークから切断された
			CWSADebug::puts(_T("ネットワークから切断されました"));

			break;
		}

		if( events.lNetworkEvents & FD_READ ) {
			// 受信イベント
			
			// 仮想関数コール
			pThis->OnReceive();
			pThis->SendMessageToParent(MSG_READ_EVENT);
		}


		if( events.lNetworkEvents & FD_WRITE ) {
			// 送信イベント
			
			// 仮想関数コール
			pThis->OnSend();
			pThis->SendMessageToParent(MSG_WRITE_EVENT);
		}
	}

	// イベント解除
	::WSAEventSelect(pThis->m_sock, hEvent, 0);

	// イベント開放
	::WSACloseEvent(hEvent);

ENDLINE:

	pThis->m_flagConnected = FALSE;
	
	// スレッドを抜けるときは接続も切る
	if( ::shutdown(pThis->m_sock, SD_BOTH) == SOCKET_ERROR ) {
		CWSADebug::WSAPrintLastError(_T("At: shutdown"));
	}	

	// ソケットをクローズする
	if( ::closesocket(pThis->m_sock) == SOCKET_ERROR ) {
		CWSADebug::WSAPrintLastError(_T("At: closesocket"));
	}

	// 切断イベント
	pThis->OnClose();
	pThis->SendMessageToParent(MSG_CLOSE_EVENT);

	// 親に接続の切断を通知して，登録を解除してもらう
	pThis->PostDisconnectMessageToParent();

	CWSADebug::printf(_T("受信用スレッド(0x%x)が終了しました\r\n"), ::GetCurrentThreadId());

	return 0;
}

//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
//	WSATcpServer
//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆

//*******************************************************************************
//	構築・破棄
//*******************************************************************************
WSATcpServerBase::WSATcpServerBase()
{
	// Accept スレッド終了用イベントの生成
	m_hAcceptExitEvent = ::WSACreateEvent();
	if( m_hAcceptExitEvent == WSA_INVALID_EVENT ) {
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
	} 

	// Client 終了用イベントの生成
	m_hConnectionExitEvent = ::WSACreateEvent();
	if( m_hConnectionExitEvent == WSA_INVALID_EVENT ) {
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
	}

	m_sock = INVALID_SOCKET;
	m_msecReceiveTimeout = N_DEFAULT_TIMEOUT;
}

WSATcpServerBase::~WSATcpServerBase()
{
	// スレッド終了用イベントの開放
	::WSACloseEvent(m_hAcceptExitEvent);
	::WSACloseEvent(m_hConnectionExitEvent);
}

//*******************************************************************************
//	初期化
//*******************************************************************************
BOOL WSATcpServerBase::Initialize(unsigned short nPortNo, unsigned int nBacklogConnection, DWORD msecReceiveTimeout)
{
	// コンストラクタでの初期化が失敗していないか調べる
	if( IsConstructionSucceeded() == FALSE ) {
		CWSADebug::puts(_T("コンストラクタでの初期化に失敗していたため処理を中断します"));
		return FALSE;
	}

	// 一度リセット
	Destroy();

	// サーバーのソケット生成
	SOCKET newsock = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if( newsock == INVALID_SOCKET ) {
		// 例外エラー
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
		//  例外エラー
		CWSADebug::WSAPrintLastError(_T("At: bind"));
		::closesocket(newsock);
		return FALSE;
	}

	// 接続待ちにする
	if( listen(newsock, nBacklogConnection) ) {
		// 例外エラー
		CWSADebug::WSAPrintLastError(_T("At: listen"));
		::closesocket(newsock);
		return FALSE;
	}

	// スレッド生成
	if( m_AcceptThread.CreateSuspendThread(_AcceptProc, (LPVOID)this) == FALSE ) {
		// 例外エラー
		CWSADebug::puts(_T("スレッドを生成できませんでした"));
		::closesocket(newsock);
		return FALSE;
	}

	if( m_ClientMessageThread.CreateSuspendThread(_ClientMessageProc, (LPVOID)this) == FALSE ) {
		// 例外エラー
		CWSADebug::puts(_T("スレッドを生成できませんでした"));
		::closesocket(newsock);
		return FALSE;
	}

	// 全部成功したのでソケット・パラメータを保存
	m_msecReceiveTimeout = msecReceiveTimeout;
	m_sock = newsock;

	// 初期化イベント
	OnInitialize();

	// 開始
	m_ClientMessageThread.ResumeThread();
	m_AcceptThread.ResumeThread();

	return TRUE;
}

//*******************************************************************************
//	リセット
//*******************************************************************************
void WSATcpServerBase::Destroy()
{
	// 接続用スレッドを終了させる
	if( ::WSASetEvent(m_hAcceptExitEvent) == FALSE ) {
		CWSADebug::puts(_T("At: WSASetEvent"));
	}
	m_AcceptThread.WaitThreadExit();
	m_AcceptThread.CloseThread();

	// 全ての受信用スレッドを終了させる
	while( GetClientCount() ) {
		// 全ての受信用スレッドを終了させる
		if( ::WSASetEvent(m_hConnectionExitEvent) == FALSE ) {
			CWSADebug::puts(_T("At: WSASetEvent"));
		}
	}

	// クライアントメッセージ受信用スレッドを終了させる
	m_ClientMessageThread.PostQuitMessage();
	m_ClientMessageThread.WaitThreadExit();
	m_ClientMessageThread.CloseThread();

	// イベントハンドルのリセット
	::WSAResetEvent(m_hAcceptExitEvent);
	::WSAResetEvent(m_hConnectionExitEvent);

	CWSADebug::puts(_T("Destroy 完了"));
}

//*******************************************************************************
//	コンストラクタでの初期化が成功したかどうかを返す
//*******************************************************************************
BOOL WSATcpServerBase::IsConstructionSucceeded() const
{
	return (m_hAcceptExitEvent != WSA_INVALID_EVENT) && (m_hConnectionExitEvent != WSA_INVALID_EVENT);
}

//*******************************************************************************
//	初期化が行われたかどうかを返す
//*******************************************************************************
BOOL WSATcpServerBase::IsInitialized() const
{
	return m_sock != INVALID_SOCKET;
}

//*******************************************************************************
//	接続用スレッド
//
//	listenで設定した数以上に接続を受け入れてしまう問題あり
//*******************************************************************************
UINT WSATcpServerBase::_AcceptProc(LPVOID pParam)
{
	WSATcpServerBase* pThis = static_cast<WSATcpServerBase*>(pParam);
	HANDLE            hAcceptEvent;

	CWSADebug::printf(_T("AcceptProc開始 ThreadID:%d\n"), pThis->m_AcceptThread.GetId()); 

	hAcceptEvent = ::WSACreateEvent();
	if( hAcceptEvent == WSA_INVALID_EVENT ) {
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
		return 0;
	}

	int ret = ::WSAEventSelect(pThis->m_sock, hAcceptEvent, FD_ACCEPT); // accept が非ブロッキングになる
	if( ret == SOCKET_ERROR ) {
		CWSADebug::WSAPrintLastError(_T("At: WSAEventSelect"));
		::WSACloseEvent(hAcceptEvent);
		return 0;
	}

	// m_AcceptExitEvent を非シグナル状態にしておく
	if( ::WSAResetEvent(pThis->m_hAcceptExitEvent) == FALSE ) {
		CWSADebug::WSAPrintLastError(_T("At: WSAResetEvent"));
		::WSACloseEvent(hAcceptEvent);
		return 0;
	}

	// イベントの配列を作る
	HANDLE hEvents[2] = {hAcceptEvent, pThis->m_hAcceptExitEvent};

	while( 1 ) {

		DWORD dwret = ::WSAWaitForMultipleEvents(2, hEvents, FALSE, INFINITE, FALSE);
		if( (dwret < WSA_WAIT_EVENT_0) || (dwret > (WSA_WAIT_EVENT_0 + 1)) ) {
			// 例外エラー
			CWSADebug::WSAPrintLastError(_T("At: WSAWaitForMultipleEvents"));
			break;
		}

		// m_hAcceptExitEvent がシグナル状態になったか調べる
		if( dwret - WSA_WAIT_EVENT_0 == 1 ) {
			// シグナル状態 → 終了
			CWSADebug::puts(_T("Acceptを終了します"));
			break;
		}

		// イベントの発生理由を取得
		::WSANETWORKEVENTS events;
		int ret = ::WSAEnumNetworkEvents(pThis->m_sock, hAcceptEvent, &events);
		if( ret == SOCKET_ERROR ) {
			// 例外エラー
			CWSADebug::WSAPrintLastError(_T("At: WSAEnumNetworkEvents"));
			break;
		} else if( (events.lNetworkEvents & FD_ACCEPT) == 0 ) {
			// 例外エラー
			CWSADebug::puts(_T("想定外の要因によりイベントが発生しました。"));
			CWSADebug::WSAPrintNetworkEvents(events);
			// break;
		}

		// ACCEPT イベント
		if( events.lNetworkEvents & FD_ACCEPT ) {
			
			// Accept 処理
			::sockaddr_in client;
			int client_len = sizeof(client);
			SOCKET client_sock = ::accept(pThis->m_sock, (struct sockaddr*)&client, &client_len);
			if( client_sock == INVALID_SOCKET ) {
				// 例外エラー
				CWSADebug::WSAPrintLastError(_T("At: accept"));
				break;
			}

			// Accept 成功
			WSAConnection* pNewClient = pThis->_CreateNewClient(client_sock, client.sin_addr.S_un.S_addr, pThis->m_hConnectionExitEvent, &pThis->m_ClientMessageThread);

			// Accept イベント
			pThis->_OnAccept(pNewClient);

			// 稼動開始
			pNewClient->Resume();
		}
	}

	// 解除
	::WSAEventSelect(pThis->m_sock, hAcceptEvent, 0);

	// 開放
	::WSACloseEvent(hAcceptEvent);

	// ソケットを開放
	if( ::closesocket(pThis->m_sock) == SOCKET_ERROR ) {
		CWSADebug::WSAPrintLastError(_T("At: closesocket"));
	}
	pThis->m_sock = INVALID_SOCKET;

	CWSADebug::printf(_T("接続用スレッド(0x%x)が終了しました\r\n"), ::GetCurrentThreadId());

	return 0;
}

//*******************************************************************************
//	クライアントからのメッセージ受信用スレッド
//*******************************************************************************
UINT WSATcpServerBase::_ClientMessageProc(LPVOID pParam) 
{
	WSATcpServerBase* pThis = static_cast<WSATcpServerBase*>(pParam);
	::MSG msg;

	while( ::GetMessage(&msg, NULL, 0, 0) >= 0 ) {
		// メッセージループ

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
				pThread->WaitThreadExit(); // スレッドが終了するまで待機
				pThis->_DestroyClient(reinterpret_cast<LPVOID>(msg.wParam));
				break;
		}
	}

	CWSADebug::printf(_T("メッセージ受信用スレッド(0x%x)が終了しました\r\n"), ::GetCurrentThreadId());

	return 0;
}


WSATcpClientBase::WSATcpClientBase()
{
	// Accept スレッド終了用イベントの生成
	m_hConnectExitEvent = ::WSACreateEvent();
	if( m_hConnectExitEvent == WSA_INVALID_EVENT ) {
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
	} 

	// Client 終了用イベントの生成
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

	// スレッド終了用イベントの開放
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
	// コンストラクタでの初期化が失敗していないか調べる
	if( IsConstructionSucceeded() == FALSE ) {
		CWSADebug::puts(_T("コンストラクタでの初期化に失敗していたため処理を中断します"));
		return FALSE;
	}

	// 一度リセット
	Destroy();

	// スレッド生成
	if( m_ConnectThread.CreateSuspendThread(_ConnectProc, (LPVOID)this) == FALSE ) {
		// 例外エラー
		CWSADebug::puts(_T("スレッドを生成できませんでした"));
		return FALSE;
	}

	// 全部成功したのでパラメータを保存
	m_msecReceiveTimeout_t  = msecReceiveTimeout;
	m_ServerAddr.sin_family = AF_INET;
	m_ServerAddr.sin_port   = htons(nPortNo);

	if( lpHostName ) {
		// ホスト名指定
		m_ServerAddr.sin_addr.S_un.S_addr = NULL;
		::strcpy_s(m_HostName, sizeof(m_HostName), lpHostName);
	} else {
		// IP アドレス指定
		m_ServerAddr.sin_addr.S_un.S_addr = IPv4Addr;
		m_HostName[0] = '\0';
	}

	// 初期化フラグセット
	m_flagInitialized = TRUE;

	// 仮想関数コール
	OnInitialize();

	// 接続開始
	m_ConnectThread.ResumeThread();

	return TRUE;
}

void WSATcpClientBase::Destroy()
{
	// _ConnectProc へ終了を通知しておく
	m_ConnectThread.PostQuitMessage(); 

	// 受信用スレッドで終了イベントを発生させる
	if( ::WSASetEvent(m_hConnectionExitEvent) == FALSE ) {
		CWSADebug::puts("At: WSASetEvent");
	}

	// connect スレッドで終了イベントを発生させる
	if( ::WSASetEvent(m_hConnectExitEvent) == FALSE ) {
		CWSADebug::puts("At: WSASetEvent");
	}

	// そのうち _ConnectProc が終了するので待つ
	m_ConnectThread.WaitThreadExit();
	m_ConnectThread.CloseThread();

	// イベントハンドルのリセット
	::WSAResetEvent(m_hConnectionExitEvent);
	::WSAResetEvent(m_hConnectExitEvent);

	m_flagInitialized = FALSE;

	CWSADebug::puts(_T("Destroy 完了"));

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
	WSAConnection* pConnection = NULL;         // NULL のときはエラーもしくは終了イベント発生時

	CWSADebug::puts(_T("接続を開始します"));

	// ソケットの作成
	SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if( sock == INVALID_SOCKET ) {
		// 例外エラー
		CWSADebug::WSAPrintLastError(_T("At: WSASocket"));
		return NULL;
	}

	// イベント生成
	hConnectEvent = WSACreateEvent();
	if( hConnectEvent == WSA_INVALID_EVENT ) {
		// 例外エラー
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
		closesocket(sock);
		return NULL;
	}

	// イベントの発生要因を設定
	int ret = WSAEventSelect(sock, hConnectEvent, FD_CONNECT); // connect が非ブロッキングになる
	if( ret == SOCKET_ERROR ) {
		// 例外エラー
		CWSADebug::WSAPrintLastError(_T("At: WSAEventSelect"));
		WSACloseEvent(hConnectEvent);
		closesocket(sock);
		return NULL;
	}

	HANDLE hEvents[2] = {hConnectEvent, m_hConnectExitEvent};

	// 終了命令を受信するまでループ
	while( true ) {

		if( ServerAddr.sin_addr.S_un.S_addr == NULL ) {
			// ホスト名指定の場合
			ServerAddr.sin_addr.S_un.S_addr = _HostName2ValidAddr(m_HostName); 
		}

		// サーバーに接続
		if( WSAConnect(sock, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR ) {
			int err = WSAGetLastError();
			if( (err != WSAEWOULDBLOCK) && (err != WSAEALREADY ) ) {
				// 例外エラー
				CWSADebug::WSAPrintLastError(_T("At: WSAConnect"));
				pConnection = NULL;
				break;
			}
		}

		// イベント発生まで待機
		DWORD dwret = WSAWaitForMultipleEvents(2, hEvents, FALSE, N_CONNECT_TIMEOUT, FALSE); // 自分のPC相手だと止まらない
		if( dwret == WSA_WAIT_TIMEOUT ) {
			// 待機のタイムアウト（非エラー）
			continue;

		} else if( (dwret != WSA_WAIT_EVENT_0) && (dwret != WSA_WAIT_EVENT_0 + 1) ) {
			// タイムアウト以外（例外エラー）
			CWSADebug::WSAPrintLastError(_T("At: WSAWaitForMultipleEvents"));
			pConnection = NULL;
			break;
		}

		// スレッド終了イベントが発生したか調べる
		if( dwret - WSA_WAIT_EVENT_0 == 1 ) {
			// m_hConnectExitEvent がシグナル状態 → 終了
			CWSADebug::puts(_T("接続待機状態からスレッドを終了します(1)"));
			pConnection = NULL;
			break;
		}

		// 予備
		dwret = WSAWaitForMultipleEvents(1, &m_hConnectExitEvent, FALSE, 0, FALSE);
		if( dwret == WSA_WAIT_EVENT_0 ) {
			// イベント発生 → 終了
			CWSADebug::puts(_T("接続待機状態からスレッドを終了します(2)"));
			pConnection = NULL;
			break;
		}

		// イベントの発生理由を取得
		WSANETWORKEVENTS events;
		int ret = WSAEnumNetworkEvents(sock, hConnectEvent, &events);
		if( ret == SOCKET_ERROR ) {
			// 例外エラー
			CWSADebug::WSAPrintLastError(_T("At: WSAEnumNetworkEvents"));
			pConnection = NULL;
			break;
		} else if( (events.lNetworkEvents & FD_CONNECT) == 0 ) {
			// 例外エラー
			CWSADebug::puts(_T("想定外の要因によりイベントが発生しました。")); // たまに起こる
		}

		if((events.lNetworkEvents & FD_CONNECT) && (events.iErrorCode[FD_CONNECT_BIT] == 0)) {
			// 接続OK

			pConnection = _CreateClient(sock, m_ServerAddr.sin_addr.S_un.S_addr, m_hConnectionExitEvent);
			break;

		} else {
			// 接続してない
			Sleep(100); // WSAWaitForMultipleEvents がすぐ抜けた場合ここで待つ
		}
	}

	// 解除
	WSAEventSelect(sock, hConnectEvent, 0);

	// 開放
	WSACloseEvent(hConnectEvent);

	// エラー時のソケット開放
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
			// 接続に成功
			CWSADebug::puts(_T("接続に成功しました"));

			// connect イベント
			pThis->OnConnect();

		} else {
			// エラーもしくは終了メッセージ
			break;
		}

		pConnection->Resume(TRUE); // 通信を開始
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
		CWSADebug::puts(_T("見つかりません"));
		return FALSE;
	}

	if( pHost->h_length != 4 ) {
		CWSADebug::puts(_T("IPv4 ではありません！"));
		return FALSE;
	}

	CWSADebug::printf("公式名 = %s\n" , pHost->h_name);

	for(int iCount = 0 ; pHost->h_aliases[iCount] ; iCount++) {
		CWSADebug::printf("別名 = %s\n" , pHost->h_aliases[iCount]);
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
		// 取得できたIPが1つだけのとき
		n = 0;

	} else {
		// IPが複数取得できたとき

		// ホストの情報が取得できるIPを返す
		for( int i = 0; i < ip_list_count; i++ ) {
			pHost = gethostbyaddr(reinterpret_cast<const char*>(&ip_list[i].S_un.S_addr), 4, AF_INET);
			if( pHost ) {
				n = i;
				break;
			}
		}

		if( n >= ip_list_count ) {
			// エラー
			CWSADebug::puts(_T("ホストの情報が1つも取得できませんでした"));
			return NULL;
		}
	}

	return ip_list[n].S_un.S_addr;
}

//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
//	WSAUdpBase
//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆

//*******************************************************************************
//	構築・破棄
//*******************************************************************************
WSAUdpBase::WSAUdpBase()
{
	// 受信スレッド終了用イベントの生成
	m_hExitEvent = ::WSACreateEvent();
	if (m_hExitEvent == WSA_INVALID_EVENT) {
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
	}

	m_sock = INVALID_SOCKET;
	m_msecReceiveTimeout	= N_DEFAULT_TIMEOUT;
}

WSAUdpBase::~WSAUdpBase()
{
	// 受信スレッド終了用イベントの解放
	::WSACloseEvent(m_hExitEvent);
}


//*******************************************************************************
//	リセット
//*******************************************************************************
void WSAUdpBase::Destroy()
{
	// 受信スレッドを終了させる
	if (::WSASetEvent(m_hExitEvent) == FALSE) {
		CWSADebug::puts(_T("At: WSASetEvent"));
	}
	m_ReceiveThread.WaitThreadExit();
	m_ReceiveThread.CloseThread();

	// イベントハンドルのリセット
	::WSAResetEvent(m_hExitEvent);

	CWSADebug::puts(_T("Destroy 完了"));
}

//*******************************************************************************
//	受信スレッドの稼動を開始する
//*******************************************************************************
void WSAUdpBase::Resume(BOOL WaitThreadExit)
{
	m_ReceiveThread.ResumeThread();
	if (WaitThreadExit) {
		m_ReceiveThread.WaitThreadExit();
	}
}

//*******************************************************************************
//	コンストラクタでの初期化が成功したかどうかを返す
//*******************************************************************************
BOOL WSAUdpBase::IsConstructionSucceeded() const
{
	return m_hExitEvent != WSA_INVALID_EVENT;
}

//*******************************************************************************
//	初期化が行われたかどうかを返す
//*******************************************************************************
BOOL WSAUdpBase::IsInitialized() const
{
	return m_sock != INVALID_SOCKET;
}

//*******************************************************************************
//	初期化
//*******************************************************************************
BOOL WSAUdpBase::Initialize(DWORD msecReceiveTimeout)
{
	// コンストラクタでの初期化が失敗していないか調べる
	if (IsConstructionSucceeded() == FALSE) {
		CWSADebug::puts(_T("コンストラクタでの初期化に失敗していたため処理を中断します"));
		return FALSE;
	}

	// 一度リセット
	Destroy();

	// UDPソケット生成
	SOCKET newsock = ::WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (newsock == INVALID_SOCKET) {
		// 例外エラー
		CWSADebug::WSAPrintLastError(_T("At: WSASocket"));
		return FALSE;
	}

	// 受信スレッドを生成
	if (m_ReceiveThread.CreateSuspendThread(_ReceiveProc, (LPVOID)this) == FALSE) {
		// 例外エラー
		CWSADebug::puts(_T("スレッドを生成できませんでした"));
		::closesocket(newsock);
		return FALSE;
	}

	// 全部成功したのでソケット・パラメータを保存
	m_msecReceiveTimeout = msecReceiveTimeout;
	m_sock = newsock;

	// 初期化イベント
	OnInitialize();

	return TRUE;
}

//*******************************************************************************
//	送信
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
				// 例外
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
//	受信
//*******************************************************************************
size_t WSAUdpBase::ReceiveData(LPVOID pBuff, size_t nSize, ::sockaddr *fromAddr)
{
	if (IsInitialized() == FALSE) return 0;

	int fromLen = sizeof(::sockaddr);

	int nReceive = ::recvfrom(m_sock, static_cast<char*>(pBuff), nSize, 0, fromAddr, &fromLen);

	if (nReceive == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			// 例外
			CWSADebug::WSAPrintLastError(_T("At: recv"));
		}

		return 0;
	}

	return nReceive;
}

//*******************************************************************************
//	受信用スレッド
//*******************************************************************************
UINT WSAUdpBase::_ReceiveProc(LPVOID pParam)
{
	WSAUdpBase* pThis = static_cast<WSAUdpBase*>(pParam);
	HANDLE hEvent;

	CWSADebug::puts(_T("受信を開始します"));

	// イベント生成
	hEvent = ::WSACreateEvent();
	if (hEvent == WSA_INVALID_EVENT) {
		// 例外エラー
		CWSADebug::WSAPrintLastError(_T("At: WSACreateEvent"));
		goto ENDLINE;
	}

	// イベントの発生要因を設定
	int ret = ::WSAEventSelect(pThis->m_sock, hEvent, FD_WRITE | FD_READ | FD_CLOSE);
	if (ret == SOCKET_ERROR) {
		// 例外エラー
		CWSADebug::WSAPrintLastError(_T("At: WSAEventSelect"));
		::WSACloseEvent(hEvent);
		goto ENDLINE;
	}

	// イベントの配列を作る
	HANDLE hEvents[2] = { hEvent, pThis->m_hExitEvent };

	// 終了命令を受信するまでループ
	while (CheckQuitMessage() == FALSE) {

		// イベント待ち
		DWORD dwret = ::WSAWaitForMultipleEvents(2, hEvents, FALSE, pThis->m_msecReceiveTimeout, FALSE);

		if (dwret == WSA_WAIT_TIMEOUT) {
			// 受信タイムアウト

			// 仮想関数コール
			if (pThis->OnReceiveTimeout() == TRUE) {
				CWSADebug::puts(_T("受信がタイムアウトしました"));
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

		// スレッド終了イベントが発生したか調べる
		if (dwret - WSA_WAIT_EVENT_0 == 1) {
			// m_hExitEvent がシグナル状態 → 終了
			CWSADebug::puts(_T("受信待機状態からスレッドを終了します"));
			break;
		}

		// イベントの発生理由を取得
		WSANETWORKEVENTS events;
		if (::WSAEnumNetworkEvents(pThis->m_sock, hEvent, &events) == SOCKET_ERROR) {
			// 例外エラー
			CWSADebug::WSAPrintLastError(_T("At: WSAEnumNetworkEvents"));
			break;
		}
		else if ((events.lNetworkEvents & (FD_WRITE | FD_READ | FD_CLOSE)) == 0) {
			// 例外エラー
			if (::WSAGetLastError() != WSAEWOULDBLOCK) {
				CWSADebug::puts(_T("想定外の要因によりイベントが発生しました"));
				CWSADebug::WSAPrintNetworkEvents(events);
			}
		}

		if (events.lNetworkEvents & FD_CLOSE) {
			// ネットワークから切断された
			CWSADebug::puts(_T("ネットワークから切断されました"));

			break;
		}

		if (events.lNetworkEvents & FD_READ) {
			// 受信イベント

			// 仮想関数コール
			pThis->OnReceive();
		}

		if (events.lNetworkEvents & FD_WRITE) {
			// 送信イベント

			// 仮想関数コール
			pThis->OnSend();
		}
	}

	// イベント解除
	::WSAEventSelect(pThis->m_sock, hEvent, 0);

	// イベント開放
	::WSACloseEvent(hEvent);

ENDLINE:

	// スレッドを抜けるときは接続も切る
	if (::shutdown(pThis->m_sock, SD_BOTH) == SOCKET_ERROR) {
		CWSADebug::WSAPrintLastError(_T("At: shutdown"));
	}

	// ソケットをクローズする
	if (::closesocket(pThis->m_sock) == SOCKET_ERROR) {
		CWSADebug::WSAPrintLastError(_T("At: closesocket"));
	}

	// 切断イベント
	pThis->OnClose();

	CWSADebug::printf(_T("受信用スレッド(0x%x)が終了しました\r\n"), ::GetCurrentThreadId());

	return 0;
}

//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
//	WSAUdpServer
//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
//*******************************************************************************
//	構築・破棄
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
//	初期化
//*******************************************************************************
BOOL WSAUdpServer::Initialize(unsigned short nPortNo, DWORD msecReceiveTimeout)
{
	// ベースクラスの初期化処理
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
		//  例外エラー
		CWSADebug::WSAPrintLastError(_T("At: bind"));
		::closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		return FALSE;
	}

	return TRUE;
}

//*******************************************************************************
//	送信
//*******************************************************************************
size_t WSAUdpServer::SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking)
{
	// 直近のデータを受信したクライアントに対して送信する。
	// 一度もデータを受信していない場合は、何もしない。
	if (m_isReceivedData == TRUE) {
		return WSAUdpBase::SendData(pBuff, nSize, (const struct ::sockaddr *)(&m_clientAddr), Blocking);
	}
	else{
		return 0;
	}
}

//*******************************************************************************
//	受信
//*******************************************************************************
size_t WSAUdpServer::ReceiveData(LPVOID pBuff, size_t nSize)
{
	size_t ret = WSAUdpBase::ReceiveData(pBuff, nSize, (struct ::sockaddr *)(&m_clientAddr));

	if (ret > 0){
		m_isReceivedData = TRUE;
	}

	return ret;
}


//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
//	WSAUdpClient
//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
//*******************************************************************************
//	構築・破棄
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
//	初期化
//*******************************************************************************
BOOL WSAUdpClient::Initialize(DWORD  IPv4Addr, WORD nPortNo, DWORD msecReceiveTimeout)
{
	// ベースクラスの初期化処理
	if (WSAUdpBase::Initialize(msecReceiveTimeout) == FALSE) {
		return FALSE;
	}

	return _Initialize(IPv4Addr, NULL, nPortNo);
}

BOOL WSAUdpClient::Initialize(BYTE ip1, BYTE ip2, BYTE ip3, BYTE ip4, WORD nPortNo, DWORD msecReceiveTimeout)
{
	// ベースクラスの初期化処理
	if (WSAUdpBase::Initialize(msecReceiveTimeout) == FALSE) {
		return FALSE;
	}

	return _Initialize(ip1 + ((DWORD)ip2 << 8) + ((DWORD)ip3 << 16) + ((DWORD)ip4 << 24), NULL, nPortNo);
}

BOOL WSAUdpClient::Initialize(LPCSTR lpHostName, WORD nPortNo, DWORD msecReceiveTimeout)
{
	// ベースクラスの初期化処理
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
		// ホスト名指定
		m_serverAddr.sin_addr.S_un.S_addr = NULL;
		::strcpy_s(m_hostName, sizeof(m_hostName), lpHostName);
	}
	else {
		// IP アドレス指定
		m_serverAddr.sin_addr.S_un.S_addr = IPv4Addr;
		m_hostName[0] = '\0';
	}

	return TRUE;
}

//*******************************************************************************
//	送信
//*******************************************************************************
size_t WSAUdpClient::SendData(LPCVOID pBuff, size_t nSize, BOOL Blocking)
{
	return WSAUdpBase::SendData(pBuff, nSize, (const ::sockaddr *)(&m_serverAddr), Blocking);
}

//*******************************************************************************
//	受信
//*******************************************************************************
size_t WSAUdpClient::ReceiveData(LPVOID pBuff, size_t nSize)
{
	// サーバのアドレス情報は更新不要
	::sockaddr dummy;
	return WSAUdpBase::ReceiveData(pBuff, nSize, &dummy);
}


