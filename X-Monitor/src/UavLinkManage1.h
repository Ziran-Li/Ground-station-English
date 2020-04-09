#pragma once

#include <vector>
#include "SCI.h"

//#import "msxml3.dll" named_guids
#import "msxml6.dll" named_guids

class CUavLink;
class dataManage;

class UavLinkManage
{
	//// 定数

	// RSSIのHealth値の計算用
	enum{
		PSSI_PARAM_A		= -100,		// 計算結果が0になるときのRSSI値
		PSSI_PARAM_B		= -80,		// 計算結果が15になるときのRSSI値
		PSSI_PARAM_TIMEOUT	= 1000,		// タイムアウト時間[ms]
	};

	// コマンドバッファの定義（コマンドを送る順番に上から並べる）
	enum CmdBufId{
		CMD_BUF_SLEEP_REPLY	= 0,		// COMMAND_SLEEPコマンドのリプライ
		CMD_BUF_GO_HOME,				// GO HOMEコマンド
		CMD_BUF_LAND,					// LANDコマンド
		CMD_BUF_PAUSE,					// PAUSEコマンド
		CMD_BUF_RESUME,					// RESUMEコマンド
		CMD_BUF_WAYPOINT,				// ウェイポイントコマンド
		CMD_BUF_ACTIONOINT_ACK,			// アクション実行地点データのACK
		CMD_BUF_MULTIWAYPOINT,			// マルチウェイポイントコマンド
		CMD_BUF_REQ_TIME_REPLY,			// COMMAND_REQUEST_TIMEコマンドのリプライ
		CMD_BUF_NUM						// コマンド用バッファの要素数	
	};

	//// 構造体

	// マルチウェイポイントコマンドに対するACK
public:
	struct MultiwayAck{
		unsigned long wptId;			// ウェイポイントID
		double lat;						// 緯度
		double lon;						// 経度
		double alt;						// 高度
		double yaw;						// 方位
		double spd;						// 飛行速度
		double tol;						// 到達判定円半径
		double wait;					// 待機時間
		unsigned long  funcId;			// 機能ID

		MultiwayAck():wptId(0),lat(0),lon(0),alt(0),yaw(0),spd(0),tol(0),wait(0),funcId(0){}
	};

private:
	// マルチウェイポイント情報
	struct MultiWayInfo{
		unsigned long nextWptId;		// 次に送るべきウェイポイントID
		unsigned long prevSendTime;		// 直近にコマンドを送信した時刻[ms]
		unsigned long randVal;			// 受信したランダム値
		BOOL isTransferring;			// 転送中かどうか
		std::vector<MultiwayAck> ack;	// 受信したACKのパラメータ(通常、要素数は0個か1個)
		unsigned short retryInterval;	// コマンドを再送する間隔[ms]
		unsigned int dbID;				// コマンドパラメータを格納したDBのID
		MultiWayInfo():nextWptId(0),prevSendTime(0),randVal(0),isTransferring(FALSE),ack(),retryInterval(0),dbID(0){}
	};

	//// メンバ変数
	CUavLink	*m_mainLink;			// メインリンク
	dataManage	*m_xmlDataDb;			// XMLデータ管理
	std::vector<_bstr_t> m_commonPath;	// XMLデータ管理部に登録する際のパス（タグ）の共通部分
	DWORD m_recvTime;					// 有効なパケットを受け取った時刻（RSSIのHealth値の計算で使用）[ms]
	MultiWayInfo m_multiwayInfo;		// マルチウェイポイント情報

	// COMMAND_SLEEPに対する送信関数実行予約
	bool m_flagSendReady;

public:
	CString m_dbgMsg;	// デバッグ用


	//// メンバ関数
public:
	UavLinkManage();
	~UavLinkManage();

	// 初期化
	BOOL Initialize(CUavLink *wl, dataManage *dm);

	// MainLinkへ接続する
	BOOL Connect(SCIPORT comPortNo);

	// 接続チェック
	BOOL IsConnected();

	// 受信データをデコードしてXMLデータ管理部に登録する
	int DecodeAndRegister();

	// ローカル時刻をXMLデータ管理部に登録する
	void RegisterLocalTime();

	// MultiWayPointDataをXMLデータ管理部に登録する
	void RegisterMultiWayPointData(bool flag, BSTR value);
	void RegisterData(MSXML2::IXMLDOMDocument2Ptr& pDoc);

	// データ送信(データの中身に従って、適切な送信関数をコール)
	BOOL SendData(UINT dbID, const _bstr_t &sendMsg);

	// マルチウェイポイントコマンド転送中か？
	BOOL IsMultiwayTransferring() const { return m_multiwayInfo.isTransferring; }

	// 受信した"マルチウェイポイントコマンドのACK"の中身を取り出す
	BOOL PopMultiwayAckData(MultiwayAck &ack);

	// マルチウェイポイントコマンドのリトライをするか判断する
	BOOL DecideMultiwayCmdRetry();

	BOOL Send();

private:
	// デコード
	void DecodeCommand();			// UAV/GCSに対するコマンド
	void DecodeReply();				// COMMAND_PACKETに対する返答
	void DecodeInputData();			// 制御系の制御入力データ
	void DecodeInputData_6R();		// 6個のロータに対する入力データが付属した制御系の制御入力データ
	void DecodeInputData_12R();		// 12個のロータに対する入力データが付属した制御系の制御入力データ
	void DecodeInputData_Min();		// 最小限の制御系の制御入力データ
	void DecodeImuPsData();			// IMUセンサ・気圧センサのデータ
	void DecodeMagData();			// IMUの磁場データ
	void DecodeGpsData();			// GPS生データ
	void DecodeGpsInfo();			// GPSの付加情報データ
	void DecodeNavData();			// 航法演算データ
	void DecodeNavDataEx();			// 航法演算データ・姿勢データ付き
	void DecodeRefData();			// 制御目標値のデータ
	void DecodeWayPointData();		// ウェイポイントのデータおよび設定コマンド
	void DecodeWayPointData2();		// ウェイポイントの番号付きデータおよび設定コマンド
	void DecodeHomeData();			// 帰還地点データ
	void DecodeSystemData();		// システムの内部状態を表すデータ
	void DecodeErrorData();			// UAVで発生したエラーデータ
	void DecodeActionPoint();		// アクション実行地点に関するデータ
	void DecodeActionPointAck();	// アクション実行地点に関するデータのACK【基地局S/Wでは受信しない】
	void DecodeMultiWayPointData();	// マルチウェイポイント転送データ【基地局S/Wでは受信しない】
	void DecodeMultiWayPointAck();	// マルチウェイポイント転送に対する応答データ
	void DecodeFirmwareInfo();		// ファームウェア情報を通知するためのパケット
	void DecodePropoData();			// プロポデータ
	void DecodeEepromData();		// EEPROM書き込み要求／EEPROM取得データ【基地局S/Wでは受信しない】
	void DecodeEepromCommand();		// EEPROM操作コマンド【基地局S/Wでは受信しない】
	void DecodeDebugData_H();		// 上位制御MCUのデバッグ領域の値
	void DecodeDebugData_L();		// 下位制御MCUのデバッグ領域の値
	void DecodeGeneralPurposeData();// 汎用データ

	// アクションタイプの文字列を取得
	std::string GetActionString(_UBYTE type) const;

	// データ送信
	BOOL SendWayPointData(UINT dbID);
	BOOL SendMultiWayPointData(UINT dbID, BOOL forcedFlush = FALSE);
	BOOL SendCmdRequestFirmwareinfo(UINT dbID);
	BOOL SendCmdRequestSendCmdRequestSerialNum(UINT dbID);
	BOOL SendCmdGoHome(UINT dbID);
	BOOL SendCmdLand(UINT dbID);
	BOOL SendCmdPause(UINT dbID);
	BOOL SendCmdResume(UINT dbID);
	BOOL SendActionPointAck(_UBYTE actionType, _UWORD count);

	BOOL SendCmdRequestSerialNumber(UINT dbID);
	void DecodeSerialNumber();

	BOOL SendCmdPlannedFlight(UINT dbID);
	BOOL SendError(UINT dbID);

	// UAVからのコマンド対応
	BOOL ProcCmdSleep();
	BOOL ProcCmdReqTime();

	// 電界強度をXMLデータ管理部に登録する
	BOOL RegisterRssi(_UBYTE rssi);

	// ユーティリティ
	std::vector<_bstr_t> GetXmlPath(_bstr_t path1, _bstr_t path2 = L"", _bstr_t path3 = L"");	// XMLパスを生成
};

