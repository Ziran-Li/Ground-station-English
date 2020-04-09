//
//	Winsock用デバッギングクラス
//
//	X-Monitor のソリューションで開発中．
//	開発完了次第ライブラリに移動
//
//	stdafx.h に CWSADEBUG_ENGLISHMODE を定義すると英語のメッセージを表示します．
//

#pragma once

#include <winsock2.h>
#include "debug.h"

class CWSADebug : public CDebug
{
public:
	static void WSAPrintLastError(LPCTSTR pAdditionalMessage = NULL);
	static void WSAPrintNetworkEvents(WSANETWORKEVENTS events);
	static void WSAFormatMessage(int WSAErrorCode, LPCTSTR pAdditionalMessage = NULL);
};