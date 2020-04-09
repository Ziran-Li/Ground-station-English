
// Main.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル

#include "XMonitorSyncClientExtension.h"

// CMainApp:
// このクラスの実装については、Main.cpp を参照してください。
//

class CMainApp : public CWinAppEx
{
public:
	CMainApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

protected:
	XMonitorSyncClientExt m_Network;

public:
	BOOL IsXMonitorConnected() { return m_Network.IsConnected(); }
	BOOL GetReceiveData(std::vector<std::string> &recvData, DWORD msecTimeout);
	BOOL SendData(const std::string &sendData);

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual int ExitInstance();
};

extern CMainApp theApp;

