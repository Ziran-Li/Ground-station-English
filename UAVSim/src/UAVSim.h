
// UAVSim.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル

#include <queue>

#include "UavLink.h"
#include "ExclusiveObject.h"

#define WM_SEND_TEST_DATA			(WM_APP + 101)		// テストデータを送信する（1回だけ）
#define WM_START_SEND_TEST_DATA		(WM_APP + 102) 		// テストデータの連続送信開始
#define WM_STOP_SEND_TEST_DATA		(WM_APP + 103) 		// テストデータの連続送信停止

#define COMPORT_NO	(83)	// COMポートの番号　"2" ⇒ "COM2"

// CUAVSimApp: 
// このクラスの実装については、UAVSim.cpp を参照してください。
//

class CUAVSimApp : public CWinApp
{
private:
	// コマンドバッファの定義（コマンドを送る順番に上から並べる）
	enum CmdBufId{
		CMD_BUF_INPUTDATA_6R	= 0,	
		CMD_BUF_INPUTDATA_12R,	
		CMD_BUF_IMU_PS_DATA,				
		CMD_BUF_GPSINFO,					
		CMD_BUF_NAVDATA_EX,					
		CMD_BUF_WAYPOINTDATA,					
		CMD_BUF_SYSTEMDATA,				
		CMD_BUF_ACTIONPOINT,			
		CMD_BUF_REQ_TIME,			
		CMD_BUF_CMD_SLEEP,				// 最後にCOMMAND_SLEEP
		CMD_BUF_NUM						// コマンド用バッファの要素数	
	};

	CWinThread*     m_pWorkThread;	// ワーカスレッド
	CUavLink		m_mainLink;
	CExclusiveObject< std::queue<CString> > m_LogStringQueue;	// WM_APPEND_LOG 用文字列キュー

public:
	CUAVSimApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()

public:
	// メインウィンドウの初期化が完了したらコールされる
	void OnMainWndInit(CWnd* pMainWnd);   

	// 画面にログ出力
	BOOL AppendLog(LPCTSTR str);
	BOOL PopLog(CString& str);

	// ワーカスレッドにメッセージを送る
	BOOL PostMessageToWorkThread(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

private:
	static UINT ThreadFunc(LPVOID pParam);	// ワーカスレッド
	BOOL OpenWirelessLink();
	BOOL Connect();
	void SendTestData();
	void RecvTestData();

	// メインウィンドウに安全にメッセージを送る
	LRESULT SendMessageToMainWnd_Safe(UINT message, WPARAM wParam = 0, LPARAM lParam = 0); // デッドロックの危険があるため使用に注意する．
	LRESULT PostMessageToMainWnd_Safe(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	// テストパケット送信
	void SendCommand(UAV_COMMAND cmdId = COMMAND_SLEEP);	// UAV/GCSに対するコマンド
	void SendReply();				// COMMAND_PACKETに対する返答
	void SendInputData();			// 制御系の制御入力データ
	void SendInputData_6R();		// 6個のロータに対する入力データが付属した制御系の制御入力データ
	void SendInputData_12R();		// 12個のロータに対する入力データが付属した制御系の制御入力データ
	void SendInputData_Min();		// 最小限の制御系の制御入力データ
	void SendImuPsData();			// IMUセンサ・気圧センサのデータ
	void SendMagData();				// IMUの磁場データ
	void SendGpsData();				// GPS生データ
	void SendGpsInfo();				// GPSの付加情報データ
	void SendNavData();				// 航法演算データ
	void SendNavDataEx();			// 航法演算データ・姿勢データ付き
	void SendRefData();				// 制御目標値のデータ
	void SendWayPointData(_SQWORD lat, _SQWORD lon, _SDWORD alt, _SWORD yaw, _UBYTE spd);	// ウェイポイントのデータおよび設定コマンド
	void SendHomeData();			// 帰還地点データ
	void SendSystemData();			// システムの内部状態を表すデータ
	void SendErrorData();			// UAVで発生したエラーデータ
	void SendActionPoint();			// アクション実行地点に関するデータ
	void SendActionPointAck();		// アクション実行地点に関するデータのACK【基地局S/Wでは受信しない】
	void SendMultiWayPointData();	// マルチウェイポイント転送データ【基地局S/Wでは受信しない】
	void SendMultiWayPointAck();	// マルチウェイポイント転送に対する応答データ
	void SendFirmwareInfo();		// ファームウェア情報を通知するためのパケット
	void SendPropoData();			// プロポデータ
	void SendEepromData();			// EEPROM書き込み要求／EEPROM取得データ【基地局S/Wでは受信しない】
	void SendEepromCommand();		// EEPROM操作コマンド【基地局S/Wでは受信しない】
	void SendDebugData_H();			// 上位制御MCUのデバッグ領域の値
	void SendDebugData_L();			// 下位制御MCUのデバッグ領域の値

	// デコード
	void DecodeCommand();			// UAV/GCSに対するコマンド
	void DecodeReply();				// COMMAND_PACKETに対する返答
	void DecodeActionPointAck();	// アクション実行地点データのACK
	void DecodeWayPointData();		// ウェイポイントのデータおよび設定コマンド
	void DecodeMultiWayPointData();	// マルチウェイポイント転送データ
};

extern CUAVSimApp theApp;