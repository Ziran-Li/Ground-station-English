
#include "stdafx.h"

#include "WSADebug.h"

#ifdef CDEBUG_DEBUGMODE
#ifndef _DEBUG
#define _DEBUG
#endif
#endif

void CWSADebug::WSAPrintLastError(LPCTSTR pAdditionalMessage)
{
	CWSADebug::WSAFormatMessage(::WSAGetLastError(), pAdditionalMessage);
}

void CWSADebug::WSAFormatMessage(int WSAErrorCode, LPCTSTR pAdditionalMessage)
{
#ifdef _DEBUG

	LPCTSTR errmessage;

	switch( WSAErrorCode ) {
#ifdef CWSADEBUG_ENGLISHMODE
		case WSAEINTR:              errmessage = _T("Interrupted function call."); break;
		case WSAEACCES:             errmessage = _T("Permission denied."); break;
		case WSAEFAULT:             errmessage = _T("Bad address."); break;
		case WSAEINVAL:             errmessage = _T("Invalid argument."); break;
		case WSAEMFILE:             errmessage = _T("Too many open files."); break;
		case WSAEWOULDBLOCK:        errmessage = _T("Resource temporarily unavailable."); break;
		case WSAEINPROGRESS:        errmessage = _T("Operation now in progress."); break;
		case WSAEALREADY:           errmessage = _T("Operation already in progress."); break;
		case WSAENOTSOCK:           errmessage = _T("Socket operation on nonsocket."); break;
		case WSAEDESTADDRREQ:       errmessage = _T("Destination address required."); break;
		case WSAEMSGSIZE:           errmessage = _T("Message too long."); break;
		case WSAEPROTOTYPE:         errmessage = _T("Protocol wrong type for socket."); break;
		case WSAENOPROTOOPT:        errmessage = _T("Bad protocol option."); break;
		case WSAEPROTONOSUPPORT :   errmessage = _T("Protocol not supported."); break;
		case WSAESOCKTNOSUPPORT:    errmessage = _T("Socket type not supported."); break;
		case WSAEOPNOTSUPP:         errmessage = _T("Operation not supported."); break;
		case WSAEPFNOSUPPORT:       errmessage = _T("Protocol family not supported."); break;
		case WSAEAFNOSUPPORT:       errmessage = _T("Address family not supported by protocol family."); break;
		case WSAEADDRINUSE:         errmessage = _T("Address already in use."); break;
		case WSAEADDRNOTAVAIL:      errmessage = _T("Cannot assign requested address."); break;
		case WSAENETDOWN:           errmessage = _T("Network is down."); break;
		case WSAENETUNREACH:        errmessage = _T("Network is unreachable."); break;
		case WSAENETRESET:          errmessage = _T("Network dropped connection on reset."); break;
		case WSAECONNABORTED:       errmessage = _T("Software caused connection abort."); break;
		case WSAECONNRESET:         errmessage = _T("Connection reset by peer."); break;
		case WSAENOBUFS:            errmessage = _T("No buffer space available."); break;
		case WSAEISCONN:            errmessage = _T("Socket is already connected."); break;
		case WSAENOTCONN:           errmessage = _T("Socket is not connected."); break;
		case WSAESHUTDOWN:          errmessage = _T("Cannot send after socket shutdown."); break;
		case WSAETIMEDOUT:          errmessage = _T("Connection timed out."); break;
		case WSAECONNREFUSED:       errmessage = _T("Connection refused."); break;
		case WSAEHOSTDOWN:          errmessage = _T("Host is down."); break;
		case WSAEHOSTUNREACH:       errmessage = _T("No route to host."); break;
		case WSAEPROCLIM:           errmessage = _T("Too many processes."); break;
		case WSASYSNOTREADY:        errmessage = _T("Network subsystem is unavailable."); break;
		case WSAVERNOTSUPPORTED:    errmessage = _T("Winsock.dll version out of range."); break;
		case WSANOTINITIALISED:     errmessage = _T("Successful WSAStartup not yet performed."); break;
		case WSAEDISCON:            errmessage = _T("Graceful shutdown in progress."); break;
		case WSATYPE_NOT_FOUND:     errmessage = _T("Class type not found."); break;
		case WSAHOST_NOT_FOUND:     errmessage = _T("Host not found. No such host is known."); break;
		case WSATRY_AGAIN:          errmessage = _T("Nonauthoritative host not found."); break;
		case WSANO_RECOVERY:        errmessage = _T("This is a nonrecoverable error."); break;
		case WSANO_DATA:            errmessage = _T("Valid name, no data record of requested type."); break;
#ifdef WSA_INVALID_HANDLE
		case WSA_INVALID_HANDLE:    errmessage = _T("Specified event object handle is invalid."); break;
#endif
#ifdef WSA_INVALID_PARAMETER
		case WSA_INVALID_PARAMETER: errmessage = _T("One or more parameters are invalid."); break;
#endif
#ifdef WSA_IO_INCOMPLETE
		case WSA_IO_INCOMPLETE:     errmessage = _T("Overlapped I/O event object not in signaled state."); break;
#endif
#ifdef WSA_IO_PENDING
		case WSA_IO_PENDING:        errmessage = _T("Overlapped operations will complete later."); break;
#endif
#ifdef WSA_NOT_ENOUGH_MEMORY
		case WSA_NOT_ENOUGH_MEMORY: errmessage = _T("Insufficient memory available."); break;
#endif
#ifdef WSA_OPERATION_ABORTED
		case WSA_OPERATION_ABORTED: errmessage = _T("Overlapped operation aborted."); break;
#endif
#ifdef WSAINVALIDPROCTABLE
		case WSAINVALIDPROCTABLE:   errmessage = _T("Invalid procedure table from service provider."); break;
#endif
#ifdef WSAINVALIDPROVIDER
		case WSAINVALIDPROVIDER:    errmessage = _T("Invalid service provider version number."); break;
#endif
#ifdef WSAPROVIDERFAILEDINIT
		case WSAPROVIDERFAILEDINIT: errmessage = _T("Unable to initialize a service provider."); break;
#endif
#ifdef WSASYSCALLFAILURE
		case WSASYSCALLFAILURE:     errmessage = _T("System call failure."); break;
#endif
		default:                    errmessage = _T("Corresponding error message not found."); break;

#else // #ifdef CWSADEBUG_ENGLISHMODE

		case WSAEINTR:              errmessage = _T("関数呼び出しに割り込みがありました。"); break;
		case WSAEACCES:             errmessage = _T("アクセスは拒否されました。"); break;
		case WSAEFAULT:             errmessage = _T("アドレスが正しくありません。"); break;
		case WSAEINVAL:             errmessage = _T("無効な引数です。"); break;
		case WSAEMFILE:             errmessage = _T("開いているファイルが多すぎます。"); break;
		case WSAEWOULDBLOCK:        errmessage = _T("リソースが一時的に利用できなくなっています。"); break;
		case WSAEINPROGRESS:        errmessage = _T("操作は現在実行中です。"); break;
		case WSAEALREADY:           errmessage = _T("操作は既に実行中です。"); break;
		case WSAENOTSOCK:           errmessage = _T("非ソケットに対してソケット操作を試みました。"); break;
		case WSAEDESTADDRREQ:       errmessage = _T("送信先のアドレスが必要です。"); break;
		case WSAEMSGSIZE:           errmessage = _T("メッセージが長すぎます。"); break;
		case WSAEPROTOTYPE:         errmessage = _T("プロトコルの種類がソケットに対して正しくありません。"); break;
		case WSAENOPROTOOPT:        errmessage = _T("プロトコルのオプションが正しくありません。"); break;
		case WSAEPROTONOSUPPORT :   errmessage = _T("プロトコルがサポートされていません。"); break;
		case WSAESOCKTNOSUPPORT:    errmessage = _T("サポートされていないソケットの種類です。"); break;
		case WSAEOPNOTSUPP:         errmessage = _T("操作がソケット上でサポートされていません。"); break;
		case WSAEPFNOSUPPORT:       errmessage = _T("プロトコル ファミリがサポートされていません。"); break;
		case WSAEAFNOSUPPORT:       errmessage = _T("プロトコル ファミリはアドレス ファミリをサポートしていません。"); break;
		case WSAEADDRINUSE:         errmessage = _T("アドレスは既に使用中です。"); break;
		case WSAEADDRNOTAVAIL:      errmessage = _T("要求されたアドレスを割り当てられません。"); break;
		case WSAENETDOWN:           errmessage = _T("ネットワークがダウンしています。"); break;
		case WSAENETUNREACH:        errmessage = _T("ICMP ネットワークに到達できません。"); break;
		case WSAENETRESET:          errmessage = _T("ネットワークがリセットされたため切断されました。"); break;
		case WSAECONNABORTED:       errmessage = _T("ソフトウェアによって接続が中止されました。"); break;
		case WSAECONNRESET:         errmessage = _T("ピアによって接続がリセットされました。"); break;
		case WSAENOBUFS:            errmessage = _T("バッファ領域がサポートされていません。"); break;
		case WSAEISCONN:            errmessage = _T("ソケットは既に接続されています。"); break;
		case WSAENOTCONN:           errmessage = _T("ソケットが接続されていません。"); break;
		case WSAESHUTDOWN:          errmessage = _T("ソケットのシャットダウン後に送信できません。"); break;
		case WSAETIMEDOUT:          errmessage = _T("接続がタイムアウトになりました。"); break;
		case WSAECONNREFUSED:       errmessage = _T("接続が拒否されました。"); break;
		case WSAEHOSTDOWN:          errmessage = _T("ホストがダウンしています。"); break;
		case WSAEHOSTUNREACH:       errmessage = _T("ホストに到達するためのルートがありません。"); break;
		case WSAEPROCLIM:           errmessage = _T("プロセスが多すぎます。"); break;
		case WSASYSNOTREADY:        errmessage = _T("ネットワーク サブシステムが利用できません。"); break;
		case WSAVERNOTSUPPORTED:    errmessage = _T("このバージョンの Winsock.dll はサポートされていません。"); break;
		case WSANOTINITIALISED:     errmessage = _T("WSAStartup の実行に成功していません。"); break;
		case WSAEDISCON:            errmessage = _T("正常なシャットダウン処理が進行中です。"); break;
		case WSATYPE_NOT_FOUND:     errmessage = _T("この種類のクラスが見つかりません。"); break;
		case WSAHOST_NOT_FOUND:     errmessage = _T("ホストが見つかりません。そのようなホストはありません。"); break;
		case WSATRY_AGAIN:          errmessage = _T("ホストが見つかりません。権限を持つサーバーからの応答がありません。"); break;
		case WSANO_RECOVERY:        errmessage = _T("回復不可能なエラーが発生しました。"); break;
		case WSANO_DATA:            errmessage = _T("要求された名前は有効ですが、要求された種類のデータ レコードがありません。"); break;
#ifdef WSA_INVALID_HANDLE
		case WSA_INVALID_HANDLE:    errmessage = _T("指定されたイベント オブジェクトのハンドルが無効です。"); break;
#endif
#ifdef WSA_INVALID_PARAMETER
		case WSA_INVALID_PARAMETER: errmessage = _T("1 つまたは複数のパラメータが無効です。"); break;
#endif
#ifdef WSA_IO_INCOMPLETE
		case WSA_IO_INCOMPLETE:     errmessage = _T("重複した I/O イベントはシグナルされた状態ではありません。"); break;
#endif
#ifdef WSA_IO_PENDING
		case WSA_IO_PENDING:        errmessage = _T("重複した操作は後に完了します。"); break;
#endif
#ifdef WSA_NOT_ENOUGH_MEMORY
		case WSA_NOT_ENOUGH_MEMORY: errmessage = _T("メモリが不足しています。"); break;
#endif
#ifdef WSA_OPERATION_ABORTED
		case WSA_OPERATION_ABORTED: errmessage = _T("重複した操作が取り消されました。"); break;
#endif
#ifdef WSAINVALIDPROCTABLE
		case WSAINVALIDPROCTABLE:   errmessage = _T("サービス プロバイダから返されたプロシージャ テーブルが無効です。"); break;
#endif
#ifdef WSAINVALIDPROVIDER
		case WSAINVALIDPROVIDER:    errmessage = _T("サービス プロバイダのバージョン番号が無効です。"); break;
#endif
#ifdef WSAPROVIDERFAILEDINIT
		case WSAPROVIDERFAILEDINIT: errmessage = _T("サービス プロバイダを初期化できません。"); break;
#endif
#ifdef WSASYSCALLFAILURE
		case WSASYSCALLFAILURE:     errmessage = _T("システム コールが失敗しました。"); break;
#endif
		default:                    errmessage = _T("対応するエラーメッセージはありません。"); break;

#endif // #ifdef CWSADEBUG_ENGLISHMODE
	}

	if( pAdditionalMessage ) {
		CDebug::printf(_T("ErrorCode: %d ErrorMessage: %s %s\n"), WSAErrorCode, errmessage, pAdditionalMessage);
	} else {
		CDebug::printf(_T("ErrorCode: %d ErrorMessage: %s\n"), WSAErrorCode, errmessage);
	}
#endif
}

void CWSADebug::WSAPrintNetworkEvents(WSANETWORKEVENTS events)
{
#ifdef _DEBUG
	CDebug::puts(_T("------------------------------"));
	CDebug::printf(_T("lNetworkEvents = %d\n"), events.lNetworkEvents);
	if( events.lNetworkEvents & FD_ACCEPT ) {
		CDebug::puts(_T("FD_ACCEPT - incoming connections"));
	}
	if( events.lNetworkEvents & FD_ADDRESS_LIST_CHANGE ) {
		CDebug::puts(_T("FD_ADDRESS_LIST_CHANGE - local address list changes for the address family of the socket"));
	}
	if( events.lNetworkEvents & FD_GROUP_QOS ) {
		CDebug::puts(_T("FD_GROUP_QOS - socket group QOS changes"));
	}
	if( events.lNetworkEvents & FD_QOS ) {
		CDebug::puts(_T("FD_QOS - socket QOS changes"));
	}
	if( events.lNetworkEvents & FD_OOB ) {
		CDebug::puts(_T("FD_OOB - arrival of OOB (out of band) data"));
	}
	if( events.lNetworkEvents & FD_READ ) {
		CDebug::puts(_T("FD_READ - readiness for reading"));
	}
	if( events.lNetworkEvents & FD_WRITE ) {
		CDebug::puts(_T("FD_WRITE - readiness for writing"));
	}
	if( events.lNetworkEvents & FD_CONNECT ) {
		CDebug::puts(_T("FD_CONNECT - completed connection or multipoint join operation"));
	}
	if( events.lNetworkEvents & FD_CLOSE ) {
		CDebug::puts(_T("FD_CLOSE - socket closure"));
	}
	if( events.lNetworkEvents & FD_ROUTING_INTERFACE_CHANGE ) {
		CDebug::puts(_T("FD_ROUTING_INTERFACE_CHANGE - routing interface changes for the specified destination"));
	}
	CDebug::puts(_T("------------------------------"));

#endif
}