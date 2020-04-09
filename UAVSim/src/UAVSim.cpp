
// UAVSim.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "UAVSim.h"
#include "UAVSimDlg.h"
#include "MMTimer.h"
#include "Codec.h"
#include "common/StandardDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUAVSimApp

BEGIN_MESSAGE_MAP(CUAVSimApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUAVSimApp コンストラクション

CUAVSimApp::CUAVSimApp()
{
	// 再起動マネージャーをサポートします
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CUAVSimApp オブジェクトです。

CUAVSimApp theApp;


// CUAVSimApp 初期化

BOOL CUAVSimApp::InitInstance()
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

	CWinApp::InitInstance();


	// ダイアログにシェル ツリー ビューまたはシェル リスト ビュー コントロールが
	// 含まれている場合にシェル マネージャーを作成します。
	CShellManager *pShellManager = new CShellManager;

	// MFC コントロールでテーマを有効にするために、"Windows ネイティブ" のビジュアル マネージャーをアクティブ化
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	// スレッド生成（WaitForSingleObject可）
	m_pWorkThread = AfxBeginThread(ThreadFunc, (LPVOID)this, 0, 0, CREATE_SUSPENDED); // 停止状態のスレッドを生成
	m_pWorkThread->m_bAutoDelete = FALSE; // スレッド終了時の自動開放を抑止

	CUAVSimDlg dlg;
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
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: ダイアログの作成に失敗しました。アプリケーションは予期せずに終了します。\n");
		TRACE(traceAppMsg, 0, "警告: ダイアログで MFC コントロールを使用している場合、#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS を指定できません。\n");
	}

	// スレッドに終了命令をポスト
	m_pWorkThread->PostThreadMessageW(WM_QUIT, 0, 0);
	::WaitForSingleObject(m_pWorkThread->m_hThread, INFINITE);
	delete m_pWorkThread; // スレッドを破棄

	// 上で作成されたシェル マネージャーを削除します。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

//	バックグラウンド処理をするためのスレッド
#define N_LOOPCYCLE 5                         // 10ms 毎に nLoopCount をインクリメント
#define N_SAMPLING_TIME 20                     // サンプリングタイム(20ms)

UINT CUAVSimApp::ThreadFunc(LPVOID pParam)
{
	CUAVSimApp* pThis = (CUAVSimApp*)pParam;
	CMMTimer    mmTimer;
	DWORD       tickNext20ms;
	DWORD       tickNext250ms;
	BOOL		flagSendData = FALSE;

	mmTimer.OpenTimer(N_LOOPCYCLE, NULL);
	tickNext20ms = mmTimer.timeGetTime();
	tickNext250ms = mmTimer.timeGetTime();

	// シリアルポートをオープンする
	pThis->OpenWirelessLink();

	while (true) {
		mmTimer.WaitTimeoutEvent();

		volatile DWORD nowtime = mmTimer.timeGetTime();
		BOOL flag20msSync = FALSE;
		BOOL flag250msSync = FALSE;

		// 確実に指定時間で TRUE になるフラグ
		if (tickNext20ms <= nowtime) {
			while (tickNext20ms <= nowtime){
				tickNext20ms += N_SAMPLING_TIME;
			}
			flag20msSync = TRUE;
		}
		if (tickNext250ms <= nowtime) {
			while (tickNext250ms <= nowtime){
				tickNext250ms += 250;
			}
			flag250msSync = TRUE;
		}

		// スレッドメッセージのチェック
		MSG  msg;
		bool flagExit = false;
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			while (GetMessage(&msg, NULL, 0, 0) >= 0) {

				switch (msg.message) {
				case WM_QUIT: // 終了命令
					flagExit = true;
					break;

				case WM_SEND_TEST_DATA:			// テストデータの送信
					pThis->SendTestData();
					pThis->AppendLog(_T("テストデータ送信（1回）"));
					break;

				case WM_START_SEND_TEST_DATA:	// テストデータの連続送信開始
					flagSendData = TRUE;
					pThis->AppendLog(_T("テストデータ送信開始"));
					break;

				case WM_STOP_SEND_TEST_DATA:	// テストデータの連続送信停止
					flagSendData = FALSE;
					pThis->AppendLog(_T("テストデータ送信終了"));
					break;

				}

				// メッセージが残ってなかったらループを抜ける
				if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == 0) {
					break;
				}
			}
		}
		if (flagExit) break; // ループを抜ける

		// 連続送信
		if (flagSendData && flag250msSync){
			pThis->SendTestData();
		}

		// 受信確認
		if (flag20msSync){
			pThis->RecvTestData();
		}

		mmTimer.ResetTimeoutEvent();
	}

	mmTimer.CloseTimer();

	return 0;
}

//	メインウィンドウの初期化が完了したときの処理
void CUAVSimApp::OnMainWndInit(CWnd* pMainWnd)
{
	// ワークスレッドの実行を開始する
	if (m_pWorkThread) {
		m_pWorkThread->ResumeThread();
	}

	AppendLog(_T("ワーカスレッド実行開始"));
}

// ログ出力
BOOL CUAVSimApp::AppendLog(LPCTSTR str)
{
	{
		CExclusiveObject< std::queue<CString> >::ACCESSOR pLogStringQueue = this->m_LogStringQueue.GetAccessor();
		pLogStringQueue->push(str);
	}

	return PostMessageToMainWnd_Safe(WM_APPEND_LOG) == 0;
}

// ログキューからメッセージをポップする
BOOL CUAVSimApp::PopLog(CString& str)
{
	CExclusiveObject< std::queue<CString> >::ACCESSOR pLogStringQueue = this->m_LogStringQueue.GetAccessor();

	if (pLogStringQueue->empty() == false) {
		str = pLogStringQueue->front();
		pLogStringQueue->pop();

		return TRUE;
	}
	else {
		return FALSE;
	}
}

// ワーカスレッドにメッセージを送る
BOOL CUAVSimApp::PostMessageToWorkThread(UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pWorkThread->PostThreadMessageW(message, wParam, lParam);
}

// シリアルポートをオープンする
BOOL CUAVSimApp::OpenWirelessLink()
{
	if (m_mainLink.GetCurrentSciPort() != COMPORT_NO) {
		// 接続中のポートと設定が異なるとき
		m_mainLink.CloseSci();
		return Connect();
	}
	else {
		// COM ポートに接続しているときは，切断をチェックする
		if (m_mainLink.IsPortEnable() == FALSE) {
			// 切断された → 再接続を試みる
			AppendLog(_T("MainLink が切断されました"));
			return Connect(); // [MLK, SET]
		}

		CString s;
		s.Format(_T("MainLink へ接続済みです (COM%d)"), COMPORT_NO);
		AppendLog(s);

		return TRUE;
	}
}

//	シリアルリンクの接続
BOOL CUAVSimApp::Connect()
{
	SCIPORT ComPortNo = (SCIPORT)COMPORT_NO;

	if (ISOK(m_mainLink.Initialize(ComPortNo))) {
		// オープンに成功

		// 各パケットのサイズを登録(受信用)
		m_mainLink.RxRegisterFormat(PACKETID_COMMAND, PACKETSIZE_COMMAND);
		m_mainLink.RxRegisterFormat(PACKETID_REPLY, PACKETSIZE_REPLY);
		m_mainLink.RxRegisterFormat(PACKETID_INPUTDATA, PACKETSIZE_INPUTDATA);
		m_mainLink.RxRegisterFormat(PACKETID_INPUTDATA_6R, PACKETSIZE_INPUTDATA_6R);
		m_mainLink.RxRegisterFormat(PACKETID_INPUTDATA_12R, PACKETSIZE_INPUTDATA_12R);
		m_mainLink.RxRegisterFormat(PACKETID_INPUTDATA_MIN, PACKETSIZE_INPUTDATA_MIN);
		m_mainLink.RxRegisterFormat(PACKETID_IMU_PS_DATA, PACKETSIZE_IMU_PS_DATA);
		m_mainLink.RxRegisterFormat(PACKETID_MAGDATA, PACKETSIZE_MAGDATA);
		m_mainLink.RxRegisterFormat(PACKETID_GPSDATA, PACKETSIZE_GPSDATA);
		m_mainLink.RxRegisterFormat(PACKETID_GPSINFO, PACKETSIZE_GPSINFO);
		m_mainLink.RxRegisterFormat(PACKETID_NAVDATA, PACKETSIZE_NAVDATA);
		m_mainLink.RxRegisterFormat(PACKETID_NAVDATA_EX, PACKETSIZE_NAVDATA_EX);
		m_mainLink.RxRegisterFormat(PACKETID_REFDATA, PACKETSIZE_REFDATA);
		m_mainLink.RxRegisterFormat(PACKETID_WAYPOINTDATA, PACKETSIZE_WAYPOINTDATA);
		m_mainLink.RxRegisterFormat(PACKETID_HOMEDATA, PACKETSIZE_HOMEDATA);
		m_mainLink.RxRegisterFormat(PACKETID_SYSTEMDATA, PACKETSIZE_SYSTEMDATA);
		m_mainLink.RxRegisterFormat(PACKETID_ERRORDATA, PACKETSIZE_ERRORDATA);
		m_mainLink.RxRegisterFormat(PACKETID_ACTIONPOINT, PACKETSIZE_ACTIONPOINT);
		m_mainLink.RxRegisterFormat(PACKETID_ACTIONPOINT_ACK, PACKETSIZE_ACTIONPOINT_ACK);
		m_mainLink.RxRegisterFormat(PACKETID_MULTIWAYPOINTDATA, PACKETSIZE_MULTIWAYPOINTDATA);
		m_mainLink.RxRegisterFormat(PACKETID_MULTIWAYPOINT_ACK, PACKETSIZE_MULTIWAYPOINT_ACK);
		m_mainLink.RxRegisterFormat(PACKETID_FIRMWARE_INFO, PACKETSIZE_FIRMWARE_INFO);
		m_mainLink.RxRegisterFormat(PACKETID_PROPODATA, PACKETSIZE_PROPODATA);
		m_mainLink.RxRegisterFormat(PACKETID_EEPROMDATA, PACKETSIZE_EEPROMDATA);
		m_mainLink.RxRegisterFormat(PACKETID_EEPROM_COMMAND, PACKETSIZE_EEPROM_COMMAND);
		m_mainLink.RxRegisterFormat(PACKETID_DEBUGDATA_H, PACKETSIZE_DEBUGDATA_H);
		m_mainLink.RxRegisterFormat(PACKETID_DEBUGDATA_L, PACKETSIZE_DEBUGDATA_L);

		// コマンド(GCSへの送信データ)バッファの作成(送信用)
		std::vector<BOOL> continuousSend(CMD_BUF_NUM, FALSE);
		m_mainLink.MakeCmdBuf(continuousSend, false);

		CString s;
		s.Format(_T("MainLink への接続に成功 (COM%d)"), ComPortNo);
		AppendLog(s);

		return TRUE;
	}
	else {
		// 失敗
		CString s;
		s.Format(_T("MainLink に接続できません (COM%d)"), ComPortNo);
		AppendLog(s);

		return FALSE;
	}
}

// テストデータを送信する
void CUAVSimApp::SendTestData()
{
	//SendInputData_6R();
	//SendInputDataPacket_12R();
	//SendImuPsDataPacket();
	SendGpsInfo();
	SendNavDataEx();
	SendSystemData();
	SendActionPoint();
	SendCommand(COMMAND_REQUEST_TIME);
	SendCommand(COMMAND_SLEEP);

	// 無線パケット作成
	m_mainLink.TxFlush();

	//CString s;
	//s.Format(_T("テストデータ送信 (t=%d)"), ::timeGetTime());
	//AppendLog(s);
}

// テストデータを受信する
void CUAVSimApp::RecvTestData()
{
	PACKETID PacketID; // 受信したパケットID
	int packetCount = 0;	// 受信したパケット数(パケットIDが認識できた数)

	while (ISOK(m_mainLink.RxBegin(&PacketID))) {
		// パケットを受信
		switch (PacketID) {
		case PACKETID_COMMAND:
			DecodeCommand();
			break;

		case PACKETID_REPLY:
			DecodeReply();
			break;

		case PACKETID_ACTIONPOINT_ACK:
			DecodeActionPointAck();
			break;

		case PACKETID_WAYPOINTDATA:
			DecodeWayPointData();
			break;

		case PACKETID_MULTIWAYPOINTDATA:
			DecodeMultiWayPointData();
			break;

		default:
			AppendLog(L"受信（不正パケット）");
			break;
		}

		++packetCount;

		_UBYTE rssi;
		m_mainLink.RxEnd(&rssi);
	}

}

// メインウィンドウにSendMessageする(ブロックする)
LRESULT CUAVSimApp::SendMessageToMainWnd_Safe(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (CWnd* pMainWnd = this->GetMainWnd()) {
		if (pMainWnd->GetSafeHwnd()) {
			return pMainWnd->SendMessage(message, wParam, lParam);
		}
	}

	return 0;
}

// メインウィンドウにPostMessageする(ブロックしない)
LRESULT CUAVSimApp::PostMessageToMainWnd_Safe(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (CWnd* pMainWnd = this->GetMainWnd()) {
		if (pMainWnd->GetSafeHwnd()) {
			return pMainWnd->PostMessage(message, wParam, lParam);
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// テストパケット送信
/////////////////////////////////////////////////////////////////////////////

// UAV/GCSに対するコマンド
void CUAVSimApp::SendCommand(UAV_COMMAND cmdId)
{
	static _UWORD cnt = 0x0000;

	switch(cmdId){

	case COMMAND_SLEEP:
		m_mainLink.TxBegin(PACKETID_COMMAND);
		m_mainLink.TxPush<_UWORD>(COMMAND_SLEEP);
		m_mainLink.TxPush<_UBYTE>(0x00);
		m_mainLink.TxPush<_UBYTE>(0x00);
		m_mainLink.TxPush<_UBYTE>(0x00);
		m_mainLink.TxPush<_UBYTE>(0x00);
		m_mainLink.TxEnd(CMD_BUF_CMD_SLEEP);
		break;

	case COMMAND_REQUEST_TIME:
		if(cnt == 0){
			AppendLog(L"送信（COMMAND_REQUEST_TIME）");
			m_mainLink.TxBegin(PACKETID_COMMAND);
			m_mainLink.TxPush<_UWORD>(COMMAND_REQUEST_TIME);
			m_mainLink.TxPush<_UBYTE>(0x00);
			m_mainLink.TxPush<_UBYTE>(0x00);
			m_mainLink.TxPush<_UBYTE>(0x00);
			m_mainLink.TxPush<_UBYTE>(0x00);
			m_mainLink.TxEnd(CMD_BUF_REQ_TIME);
		}
		break;

	default:
		break;
	}

	cnt = (cnt++) % 1000;	// 1000でリセット
}

// COMMAND_PACKETに対する返答
void CUAVSimApp::SendReply()
{

}

// 制御系の制御入力データ
void CUAVSimApp::SendInputData()
{

}

// 6個のロータに対する入力データが付属した制御系の制御入力データ
void CUAVSimApp::SendInputData_6R()
{
	static _UWORD cnt = 0x0000;

	m_mainLink.TxBegin(PACKETID_INPUTDATA_6R);
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(520.0+(double)cnt++));
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(521.0));
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(522.0));
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(523.0));
	m_mainLink.TxPush<_UWORD>(400);
	m_mainLink.TxPush<_SWORD>(-500);
	m_mainLink.TxPush<_SWORD>(-600);
	m_mainLink.TxPush<_SWORD>(-700);
	m_mainLink.TxPush<_UWORD>(800);
	m_mainLink.TxPush<_UWORD>(900);
	m_mainLink.TxPush<_UWORD>(1000);
	m_mainLink.TxPush<_UWORD>(2003);
	m_mainLink.TxPush<_UWORD>(1);
	m_mainLink.TxPush<_UWORD>(0);
	m_mainLink.TxEnd(CMD_BUF_INPUTDATA_6R);

	cnt = cnt % 1000;	// 1000でリセット
}

// 12個のロータに対する入力データが付属した制御系の制御入力データ
void CUAVSimApp::SendInputData_12R()
{
	static _UWORD cnt = 0x0000;

	m_mainLink.TxBegin(PACKETID_INPUTDATA_12R);
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(520.0+(double)cnt++));
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(521.0));
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(522.0));
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(523.0));
	m_mainLink.TxPush<_UWORD>(400);
	m_mainLink.TxPush<_SWORD>(-500);
	m_mainLink.TxPush<_SWORD>(-600);
	m_mainLink.TxPush<_SWORD>(-700);
	m_mainLink.TxPush<_UWORD>(800);
	m_mainLink.TxPush<_UWORD>(900);
	m_mainLink.TxPush<_UWORD>(1000);
	m_mainLink.TxPush<_UWORD>(1100);
	m_mainLink.TxPush<_UWORD>(1200);
	m_mainLink.TxPush<_UWORD>(1300);
	m_mainLink.TxPush<_UWORD>(1400);
	m_mainLink.TxPush<_UWORD>(1500);
	m_mainLink.TxPush<_UWORD>(1600);
	m_mainLink.TxPush<_UWORD>(1700);
	m_mainLink.TxPush<_UWORD>(1800);
	m_mainLink.TxPush<_UWORD>(1900);
	m_mainLink.TxEnd(CMD_BUF_INPUTDATA_12R);

	cnt = cnt % 1000;	// 1000でリセット
}

// 最小限の制御系の制御入力データ
void CUAVSimApp::SendInputData_Min()
{

}

// IMUセンサ・気圧センサのデータ
void CUAVSimApp::SendImuPsData()
{
	static _UWORD cnt = 0x0000;

	m_mainLink.TxBegin(PACKETID_IMU_PS_DATA);
	m_mainLink.TxPush(MiniSvCodec::Conv1000To2Byte((double)cnt++));
	m_mainLink.TxPush(MiniSvCodec::Conv1000To2Byte(10.0));
	m_mainLink.TxPush(MiniSvCodec::Conv1000To2Byte(20.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(30.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(40.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(50.0));
	m_mainLink.TxPush(MiniSvCodec::Conv1To2Byte(100));
	m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(-60.0));
	m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(-70.0));
	m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(-80.0));
	m_mainLink.TxPush(MiniSvCodec::Conv10000To3Byte(-90.0), 3);
	m_mainLink.TxEnd(CMD_BUF_IMU_PS_DATA);

	cnt = cnt % 10;	// 10でリセット
}

// IMUの磁場データ
void CUAVSimApp::SendMagData()
{

}

// GPS生データ
void CUAVSimApp::SendGpsData()
{

}

// GPSの付加情報データ
void CUAVSimApp::SendGpsInfo()
{
	static unsigned long long cnt = 0;
	static int n = 0;

	int freq = 1000 / 250;
	_UBYTE gpsNum[] = {7, 8, 9};
	int time[] = {50, 100, 150};

	m_mainLink.TxBegin(PACKETID_GPSINFO);
	m_mainLink.TxPush(gpsNum[n]);
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(-1.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(-2.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(-3.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(-4.0));
	m_mainLink.TxEnd(CMD_BUF_GPSINFO);

	if(cnt == freq*50){
		n = (++n)%3;
		cnt = 0;
	}
	else{
		++cnt;
	}
}

// 航法演算データ
void CUAVSimApp::SendNavData()
{

}

// 航法演算データ・姿勢データ付き
void CUAVSimApp::SendNavDataEx()
{
	static unsigned long long cnt = 0;
	static int n = 0;

	//m_mainLink.TxBegin(PACKETID_NAVDATA2);
	//m_mainLink.TxPush<_UQWORD>(MiniSvCodec::ConvDegTo5Byte((double)cnt++));
	//m_mainLink.TxPush<_UQWORD>(MiniSvCodec::ConvDegTo5Byte(10.0));
	//m_mainLink.TxPush<_SDWORD>(MiniSvCodec::Conv10000To3Byte(20.0));
	//m_mainLink.TxPush<_SWORD>(MiniSvCodec::Conv100To2Byte(30.0));
	//m_mainLink.TxPush<_SWORD>(MiniSvCodec::Conv100To2Byte(40.0));
	//m_mainLink.TxPush<_SWORD>(MiniSvCodec::Conv100To2Byte(50.0));
	//m_mainLink.TxPush<_SWORD>(MiniSvCodec::ConvDegTo2Byte(-65.0));
	//m_mainLink.TxPush<_SWORD>(MiniSvCodec::ConvDegTo2Byte(-75.0));
	//m_mainLink.TxPush<_SWORD>(MiniSvCodec::ConvDegTo2Byte(-85.0));
	//m_mainLink.TxEnd();

	//cnt = cnt % 100;	// 100でリセット

	int freq = 1000 / 250;
	double lat0[] = {35.6309798, 35.630887, 35.6301937, 35.6309798};
	double lon0[] = {140.1031789, 140.1011968, 140.1021838, 140.1031789};
	double alt0[] = {50, 40, 60, 50};
	double roll0[] = {0, 0, 0, 90};
	double pitch0[] = {0, 0, 90, 90};
	double yaw0[] = {0, 90, 90, 90};
	int time[] = {0, 50, 100, 150};
	double lat;
	double lon;
	double alt;
	double roll;
	double pitch;
	double yaw;

	// 線形補間
	lat = (lat0[n+1] - lat0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + lat0[n];
	lon = (lon0[n+1] - lon0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + lon0[n];
	alt = (alt0[n+1] - alt0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + alt0[n];
	roll = (roll0[n+1] - roll0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + roll0[n];
	pitch = (pitch0[n+1] - pitch0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + pitch0[n];
	yaw = (yaw0[n+1] - yaw0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + yaw0[n];

	m_mainLink.TxBegin(PACKETID_NAVDATA_EX);
	m_mainLink.TxPush<_SQWORD>(MiniSvCodec::ConvDegTo5Byte(lat), 5);
	m_mainLink.TxPush<_SQWORD>(MiniSvCodec::ConvDegTo5Byte(lon), 5);
	m_mainLink.TxPush<_SDWORD>(MiniSvCodec::Conv10000To3Byte(alt), 3);
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(10.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(20.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(30.0));
	m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(roll));
	m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(pitch));
	m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(yaw));
	m_mainLink.TxEnd(CMD_BUF_NAVDATA_EX);

	if(cnt == freq*50){
		n = (++n)%3;
		cnt = 0;
	}
	else{
		++cnt;
	}
}

// 制御目標値のデータ
void CUAVSimApp::SendRefData()
{

}

// ウェイポイントのデータおよび設定コマンド
void CUAVSimApp::SendWayPointData(_SQWORD lat, _SQWORD lon, _SDWORD alt, _SWORD yaw, _UBYTE spd)
{
	// 受信したデータと同じデータを返す
	m_mainLink.TxBegin(PACKETID_WAYPOINTDATA);
	m_mainLink.TxPush(lat, 5);
	m_mainLink.TxPush(lon, 5);
	m_mainLink.TxPush(alt, 3);
	m_mainLink.TxPush(yaw);
	m_mainLink.TxPush(spd);
	m_mainLink.TxEnd(CMD_BUF_WAYPOINTDATA);
}

// 帰還地点データ
void CUAVSimApp::SendHomeData()
{

}

// システムの内部状態を表すデータ
void CUAVSimApp::SendSystemData()
{
	static unsigned long long cnt = 0;
	static int n = 0;

	_UBYTE mode[] = {0x10, 0x11, 0x22, 0x10};

	int freq = 1000 / 250;	

	m_mainLink.TxBegin(PACKETID_SYSTEMDATA);
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 起動モード・駆動モード
	m_mainLink.TxPush<_UBYTE>(mode[n]);	// 制御モード・自律モード
	m_mainLink.TxPush<_UDWORD>(0xFFFFFFFF);	// 
	m_mainLink.TxPush<_UDWORD>(0xFFFFFFFF);	// 
	m_mainLink.TxPush<_UBYTE>(0x0F);	// 
	m_mainLink.TxPush<_UBYTE>(0x0F);	// 
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(10.1));	// セル平均電源電圧
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	//
	m_mainLink.TxEnd(CMD_BUF_SYSTEMDATA);

	if(cnt == freq*50){
		n = (++n)%3;
		cnt = 0;
	}
	else{
		++cnt;
	}
}

// UAVで発生したエラーデータ
void CUAVSimApp::SendErrorData()
{

}

// アクション実行地点に関するデータ
void CUAVSimApp::SendActionPoint()
{
	static unsigned long long cnt = 0;
	static int n = 0;

	int freq = 1000 / 250;
	double lat0[] = {35.6309798, 35.630887, 35.6301937, 35.6309798};
	double lon0[] = {140.1031789, 140.1011968, 140.1021838, 140.1031789};
	double alt0[] = {50, 40, 60, 50};
	double roll0[] = {0, 0, 0, 90};
	double pitch0[] = {0, 0, 90, 90};
	double yaw0[] = {0, 90, 90, 90};
	int time[] = {0, 50, 100, 150};
	double lat;
	double lon;
	double alt;
	double roll;
	double pitch;
	double yaw;

	// 線形補間
	lat = (lat0[n+1] - lat0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + lat0[n];
	lon = (lon0[n+1] - lon0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + lon0[n];
	alt = (alt0[n+1] - alt0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + alt0[n];
	roll = (roll0[n+1] - roll0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + roll0[n];
	pitch = (pitch0[n+1] - pitch0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + pitch0[n];
	yaw = (yaw0[n+1] - yaw0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + yaw0[n];

	if(cnt == freq*50){
		m_mainLink.TxBegin(PACKETID_ACTIONPOINT);
		m_mainLink.TxPush<_UBYTE>(0x01);	// アクションタイプ "Shot"
		m_mainLink.TxPush<_UWORD>(n);	
		m_mainLink.TxPush<_SQWORD>(MiniSvCodec::ConvDegTo5Byte(lat), 5);
		m_mainLink.TxPush<_SQWORD>(MiniSvCodec::ConvDegTo5Byte(lon), 5);
		m_mainLink.TxPush<_SDWORD>(MiniSvCodec::Conv10000To3Byte(alt), 3);
		m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(roll));
		m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(pitch));
		m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(yaw));
		m_mainLink.TxPush<_UDWORD>(0x00000000);
		m_mainLink.TxEnd(CMD_BUF_ACTIONPOINT);

		n = (++n)%3;
		cnt = 0;
	}
	else{
		++cnt;
	}
}

// アクション実行地点に関するデータのACK【基地局S/Wでは受信しない】
void CUAVSimApp::SendActionPointAck()
{

}

// マルチウェイポイント転送に対する応答データ
void CUAVSimApp::SendMultiWayPointAck()
{

}

// ファームウェア情報を通知するためのパケット
void CUAVSimApp::SendFirmwareInfo()
{

}

// プロポデータ
void CUAVSimApp::SendPropoData()
{

}

// EEPROM書き込み要求／EEPROM取得データ【基地局S/Wでは受信しない】
void CUAVSimApp::SendEepromData()
{

}

// EEPROM操作コマンド【基地局S/Wでは受信しない】
void CUAVSimApp::SendEepromCommand()
{

}

// 上位制御MCUのデバッグ領域の値
void CUAVSimApp::SendDebugData_H()
{

}

// 下位制御MCUのデバッグ領域の値
void CUAVSimApp::SendDebugData_L()
{

}

//*******************************************************************************
//	デコード（UAV/GCSに対するコマンド）
//*******************************************************************************
void CUAVSimApp::DecodeCommand()
{
	_UBYTE ub;
	_UWORD uw;

	m_mainLink.RxPop(&uw);
	m_mainLink.RxPop(&ub);
	m_mainLink.RxPop(&ub);
	m_mainLink.RxPop(&ub);
	m_mainLink.RxPop(&ub);
}

//*******************************************************************************
//	デコード（COMMAND_PACKETに対する返答）
//*******************************************************************************
void CUAVSimApp::DecodeReply()
{
	_UBYTE param[4];
	_UWORD cmdId;
	_UDWORD dateAndTime;
	_UBYTE year;
	_UBYTE month;
	_UBYTE day;
	_UBYTE hour;
	_UBYTE min;
	_UBYTE sec;
	CString csDateAndTime;

	m_mainLink.RxPop(&cmdId);
	m_mainLink.RxPop(&param[0]);
	m_mainLink.RxPop(&param[1]);
	m_mainLink.RxPop(&param[2]);
	m_mainLink.RxPop(&param[3]);

	switch(cmdId){
	case COMMAND_SLEEP:
		//AppendLog(L"受信（COMMAND_SLEEP Reply）");
		break;

	case COMMAND_REQUEST_TIME:
		AppendLog(L"受信（COMMAND_REQUEST_TIME Reply）");
		memcpy(&dateAndTime, param, sizeof(_UDWORD));
		MiniSvCodec::Dec4ByteToDateAndTime(dateAndTime, &year, &month, &day, &hour, &min, &sec);
		csDateAndTime.Format(L" %04d/%02d/%02d %02d:%02d:%02d", 2000+year, month, day, hour, min, sec);
		AppendLog(csDateAndTime);
		break;
	
	default:
		break;
	}
}

//*******************************************************************************
//	デコード（アクション実行地点データのACK）
//*******************************************************************************
void CUAVSimApp::DecodeActionPointAck()
{
	_UWORD uw;
	_UBYTE ub;
	CString s;

	AppendLog(L"受信（アクションポイント　ACK）");

	// アクションタイプ
	m_mainLink.RxPop(&ub);
	s.Format(_T(" アクションタイプ: %02X"), ub); AppendLog(s);

	// カウント
	m_mainLink.RxPop(&uw);
	s.Format(_T(" カウント: %04X"), uw); AppendLog(s);
}

//*******************************************************************************
//	デコード（ウェイポイントのデータおよび設定コマンド）
//*******************************************************************************
void CUAVSimApp::DecodeWayPointData()
{
	_SQWORD lat, lon;
	_SDWORD alt;
	_SWORD yaw;
	_UBYTE spd;
	double val;
	CString s;

	AppendLog(L"受信（ウェイポイント）");

	// 緯度
	m_mainLink.RxPop(&lat, 5);
	val = MiniSvCodec::Dec5ByteToDeg(lat);
	s.Format(_T(" lat: %f"), val); AppendLog(s);

	// 経度
	m_mainLink.RxPop(&lon, 5);
	val = MiniSvCodec::Dec5ByteToDeg(lon);
	s.Format(_T(" lon: %f"), val); AppendLog(s);

	// 高度
	m_mainLink.RxPop(&alt, 3);
	val = MiniSvCodec::Dec3ByteTo10000(alt);
	s.Format(_T(" alt: %f"), val); AppendLog(s);

	// 方位
	m_mainLink.RxPop(&yaw);
	val = MiniSvCodec::Dec2ByteToDeg(yaw);
	s.Format(_T(" yaw: %f"), val); AppendLog(s);

	// 飛行速度
	m_mainLink.RxPop(&spd);
	val = MiniSvCodec::DecAirSpeed(spd);
	s.Format(_T(" spd: %f"), val); AppendLog(s);

	// リプライパケット生成
	SendWayPointData(lat, lon, alt, yaw, spd);
}

//*******************************************************************************
//	デコード（マルチウェイポイント転送データ）
//*******************************************************************************
void CUAVSimApp::DecodeMultiWayPointData()
{
}
