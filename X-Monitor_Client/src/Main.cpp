
// Main.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "Main.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// global変数(XMLデータ管理クラスで使用)
map<int, string> dataTypeMap;


// CMainApp

BEGIN_MESSAGE_MAP(CMainApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMainApp コンストラクション

CMainApp::CMainApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CMainApp オブジェクトです。

CMainApp theApp;


// CMainApp 初期化

BOOL CMainApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	//SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));


	// COMの初期化
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (!SUCCEEDED(hr))
	{
		AfxMessageBox(_T("COMの初期化失敗"));
	}

	// --------------------------------------------------
	//	X-Monitor との接続
	//
	//	IPアドレスを変更することで，異なるコンピュータ間の通信が可能になります．
	//	ただし，ネットワーク装置を介するため，データの遅延が大きくなります．
	//	（同一コンピュータ内でも数十msの遅延が生じます．）
	//
	//	ポート番号は，X-Monitor と一致させてください．（デフォルト 53799）
	//	ポート番号が一致する X-Monitor と通信を行います．
	//	
	//	X-Monitor を複数起動し，それぞれ異なるポート番号を設定しておくことで，
	//	クライアントプログラムを別々の X-Monitor と接続させることができます．
	//	例：
	//		X-Monitor1(設定：53799) ⇔ クライアントプログラムA (接続先：53799)
	//		X-Monitor2(設定：53800) ⇔ クライアントプログラムB (接続先：53800)
	// --------------------------------------------------
	m_Network.Initialize("127.0.0.1", 53799);

	CMainDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <キャンセル> で消された時のコードを
		//  記述してください。
	}

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

BOOL CMainApp::GetReceiveData(std::vector<std::string> &recvData, DWORD msecTimeout)
{
	return m_Network.GetData(recvData, msecTimeout);
}

BOOL CMainApp::SendData(const std::string &sendData)
{
	return m_Network.SendData(sendData);
}

int CMainApp::ExitInstance()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。

	// COMの終了処理
	CoUninitialize();

	return CWinAppEx::ExitInstance();
}
