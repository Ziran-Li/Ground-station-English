
//	原発向けシステムのネットワーク用にカスタマイズしたクラス群

#pragma once
#include "MyNetworkExtension.h"

#include "ExclusiveObject.h"
#include "WinsockTCP2.h"

#include <queue>


// 
//*******************************************************************************
//	原発向けシステム用通信データ用構造体および定義
//
//	説明：
//*******************************************************************************

// 定数
enum {
	CAM_DATA_MAX_SIZE = 50000,	// 上方向/下方向カメラJPEGデータの最大サイズ　temporary
	CH1_4_SIZE = 5,				// UDP5のCh1～Ch4のサイズ[byte]
	LRF_DATA_H_SIZE = 416,		// LRFデータ(水平)のサイズ[byte]
	LRF_DATA_V_SIZE = 169,		// LRFデータ(垂直)のサイズ[byte]
	CLT_MATRIX_SIZE = 3,		// 乱雑度行列のサイズ
	DEBUG_SIZE = 8,				// デバッグ用データのサイズ
	EXCHG_STATUS_SIZE = 16,		// バッテリ交換機用データのサイズ
};

// UDPパケットID
enum UDP_PACKET_ID{
	PACKET_ID_UDP1 = 0x0102,
	PACKET_ID_UDP2 = 0x0201,
	PACKET_ID_UDP3 = 0x0301,
	PACKET_ID_UDP4 = 0x0401,
	PACKET_ID_UDP5 = 0x0501,
	PACKET_ID_UDP6 = 0x0601,
};


#pragma pack(1) // 構造体のアライメントを1バイトにする

// UDPパケットのヘッダ
struct UdpPacketHeader {
	BYTE preamble1;		// HEADER1
	BYTE preamble2;		// HEADER2
	USHORT packetId;	// パケットID
	UINT payloadLen;	// ペイロード長

	UdpPacketHeader() : preamble1(0xAA), preamble2(0xAA), packetId(0x0000), payloadLen(0) {}
};

// UDPパケットのフッタ
struct UdpPacketFooter {
	BYTE postamble1;	// FOOTTER1
	BYTE postamble2;	// FOOTTER2

	UdpPacketFooter() : postamble1(0x55), postamble2(0x55) {}
};


// UDP1のペイロード
struct Payload_UDP1 {
	ULONG msapTime;					// MSAP時刻（起動時から）
	float uavAvel_X;				// 角速度（X）
	float uavAvel_Y;				// 角速度（Y）
	float uavAvel_Z;				// 角速度（Z）
	float uavAcc_X;					// 加速度（X）
	float uavAcc_Y;					// 加速度（Y）
	float uavAcc_Z;					// 加速度（Z）
	float uavAtt_Roll;				// 姿勢角（ロール）
	float uavAtt_Pitch;				// 姿勢角（ピッチ）
	float presAlt;					// 気圧高度
	float uavTgtPos_X;				// 位置目標値（X）
	float uavTgtPos_Y;				// 位置目標値（Y）
	float uavTgtPos_Z;				// 位置目標値（Z）
	short uavTgtVel_X;				// 速度目標値（X）
	short uavTgtVel_Y;				// 速度目標値（Y）
	short uavTgtVel_Z;				// 速度目標値（Z）
	short uavTgtAtt_Roll;			// 姿勢目標値（ロール）
	short uavTgtAtt_Pitch;			// 姿勢目標値（ピッチ）
	short uavTgtAtt_Yaw;			// 姿勢目標値（ヨー）
	USHORT uavCtrlInH_Th;			// 上位制御入力（スロットル）
	USHORT uavCtrlInH_Roll;			// 上位制御入力（ロール）
	USHORT uavCtrlInH_Pitch;		// 上位制御入力（ピッチ）
	USHORT uavCtrlInH_Yaw;			// 上位制御入力（ヨー）
	USHORT uavCtrlInL_Th;			// 下位制御入力（スロットル）
	short uavCtrlInL_Roll;			// 下位制御入力（ロール）
	short uavCtrlInL_Pitch;			// 下位制御入力（ピッチ）
	short uavCtrlInL_Yaw;			// 下位制御入力（ヨー）
	USHORT distUs_Back;				// 後方超音波距離
	USHORT distUs_Nad;				// 下方向超音波距離
	USHORT distUs_Zen;				// 上方向超音波距離
	float voltage;					// バッテリ電圧
	BYTE flightMode;				// 飛行モード
	USHORT msapStatus;				// MSAPステータス
	BYTE debug[DEBUG_SIZE];			// デバッグ用データ
	ULONG slamPcTime;				// SLAM-PC時刻（演算開始から）
	LONGLONG unixTime_Sec;			// UNIX時刻（UTC）
	int unixTime_uSec;				// UNIX時刻（マイクロ秒成分）
	float uavPos_X;					// 位置（X）
	float uavPos_Y;					// 位置（Y）
	float uavPos_Z;					// 位置（Z）
	float uavVel_X;					// 速度（X）
	float uavVel_Y;					// 速度（Y）
	float uavVel_Z;					// 速度（Z）
	float uavAtt_Yaw;				// 姿勢角（ヨー）
	USHORT dist_Fwd;				// 前方距離
	USHORT dist_FwdRight;			// 右前方距離
	USHORT dist_Right;				// 右方向距離
	USHORT dist_RightBack;			// 右後方距離
	USHORT dist_LeftBack;			// 左後方距離
	USHORT dist_Left;				// 左方向距離
	USHORT dist_FwdLeft;			// 左前方距離
	USHORT dist_Nad;				// 下方向距離
	USHORT slamDataStatus;			// SLAMデータステータス
	USHORT lrfData[LRF_DATA_H_SIZE];	// レーザスキャンデータ配列（水平）
	USHORT lrfDataV[LRF_DATA_V_SIZE];	// レーザスキャンデータ配列（垂直）

	Payload_UDP1() : msapTime(0), uavAvel_X(0.f), uavAvel_Y(0.f), uavAvel_Z(0.f),
		uavAcc_X(0.f), uavAcc_Y(0.f), uavAcc_Z(0.f),
		uavAtt_Roll(0.f), uavAtt_Pitch(0.f), presAlt(0.f),
		uavTgtPos_X(0.f), uavTgtPos_Y(0.f), uavTgtPos_Z(0.f),
		uavTgtVel_X(0), uavTgtVel_Y(0), uavTgtVel_Z(0),
		uavTgtAtt_Roll(0), uavTgtAtt_Pitch(0), uavTgtAtt_Yaw(0),
		uavCtrlInH_Th(0), uavCtrlInH_Roll(0), uavCtrlInH_Pitch(0), uavCtrlInH_Yaw(0),
		uavCtrlInL_Th(0), uavCtrlInL_Roll(0), uavCtrlInL_Pitch(0), uavCtrlInL_Yaw(0),
		distUs_Back(0), distUs_Nad(0), distUs_Zen(0),
		voltage(0.f), flightMode(0), msapStatus(0), slamPcTime(0),
		unixTime_Sec(0), unixTime_uSec(0),
		uavPos_X(0.f), uavPos_Y(0.f), uavPos_Z(0.f), uavVel_X(0.f), uavVel_Y(0.f), uavVel_Z(0.f), uavAtt_Yaw(0.f),
		dist_Fwd(0), dist_FwdRight(0), dist_Right(0), dist_RightBack(0),
		dist_LeftBack(0), dist_Left(0), dist_FwdLeft(0), dist_Nad(0), slamDataStatus(0)
	{
		ZeroMemory(debug, DEBUG_SIZE);
		ZeroMemory(lrfData, LRF_DATA_H_SIZE);
		ZeroMemory(lrfDataV, LRF_DATA_V_SIZE);
	}
};

// UDP2のペイロード
struct Payload_UDP2_wo_DownCam {
	ULONG syncFlag;			// フラグ
	ULONG slamPcTime;		// SLAM-PC時刻（演算開始から）
	LONGLONG unixTime_Sec;	// UNIX時刻（UTC）
	int unixTime_uSec;		// UNIX時刻（マイクロ秒成分）
	char clutterData_cols;	// 乱雑度データの列数（u）
	char clutterData_rows;	// 乱雑度データの行数（v）
	float clutterData[CLT_MATRIX_SIZE][CLT_MATRIX_SIZE];	// 乱雑度
	UINT jpegDataSize;		// JPEGデータサイズ

	Payload_UDP2_wo_DownCam() : syncFlag(0), slamPcTime(0), unixTime_Sec(0), unixTime_uSec(0),
							  clutterData_cols(0), clutterData_rows(0), jpegDataSize(0)
	{
		for (UINT ui = 0; ui < CLT_MATRIX_SIZE; ++ui){
			for (UINT uj = 0; uj < CLT_MATRIX_SIZE; ++uj){
				clutterData[ui][uj] = 0.;
			}
		}
	}
};

struct Payload_UDP2_DownCam {
	BYTE downCamData[CAM_DATA_MAX_SIZE];	// 下方向カメラJPEGデータ

	Payload_UDP2_DownCam() { ::ZeroMemory(downCamData, CAM_DATA_MAX_SIZE); }
};

// UDP3のペイロード(上カメラJPEGデータ(可変長)を除く)
struct Payload_UDP3_wo_UpCam {
	ULONG syncFlag;			// フラグ
	ULONG slamPcTime;		// SLAM-PC時刻（演算開始から）
	LONGLONG unixTime_Sec;	// UNIX時刻（UTC）
	int unixTime_uSec;		// UNIX時刻（マイクロ秒成分）
	UINT jpegDataSize;		// JPEGデータサイズ

	Payload_UDP3_wo_UpCam() : syncFlag(0), slamPcTime(0), unixTime_Sec(0), unixTime_uSec(0), jpegDataSize(0) {}
};

struct Payload_UDP3_UpCam {
	BYTE upCamData[CAM_DATA_MAX_SIZE];	// 上方向カメラJPEGデータ

	Payload_UDP3_UpCam() { ::ZeroMemory(upCamData, CAM_DATA_MAX_SIZE); }
};

// UDP4のペイロード
struct Payload_UDP4 {
	ULONG syncFlag;			// フラグ
	ULONG slamPcTime;		// SLAM-PC時刻（演算開始から）
	LONGLONG unixTime_Sec;	// UNIX時刻（UTC）
	int unixTime_uSec;		// UNIX時刻（マイクロ秒成分）
	UCHAR statisErr;		// 統計誤差
	float equivDose;		// 等価線量率

	Payload_UDP4() : syncFlag(0), slamPcTime(0), unixTime_Sec(0), unixTime_uSec(0), statisErr(0), equivDose(0.) {}
};

// UDP5のペイロード
struct Payload_UDP5 {
	BYTE  axis[CH1_4_SIZE];		// Ch1～Ch4
	BYTE  buttonA;				// Ch5～Ch12
	BYTE  buttonB;				// Ch13～Ch20

	Payload_UDP5() : buttonA(0), buttonB(0) { ::ZeroMemory(axis, CH1_4_SIZE); }
};

// UDP6のペイロード
struct Payload_UDP6 {
	float uavAcc_X;					// 加速度（X）
	float uavAcc_Y;					// 加速度（Y）
	float uavAcc_Z;					// 加速度（Z）
	float uavAtt_Roll;				// 姿勢角（ロール）
	float uavAtt_Pitch;				// 姿勢角（ピッチ）
	float presAlt;					// 気圧高度
	float uavPos_X;					// 位置（X）
	float uavPos_Y;					// 位置（Y）
	float uavPos_Z;					// 位置（Z）
	float uavAtt_Yaw;				// 姿勢角（ヨー）
	USHORT lrfData[LRF_DATA_H_SIZE];	// レーザスキャンデータ配列（水平）
	USHORT lrfDataV[LRF_DATA_V_SIZE];	// レーザスキャンデータ配列（垂直）

	Payload_UDP6() : uavAcc_X(0.f), uavAcc_Y(0.f), uavAcc_Z(0.f),
		uavAtt_Roll(0.f), uavAtt_Pitch(0.f), presAlt(0.f),
		uavPos_X(0.f), uavPos_Y(0.f), uavPos_Z(0.f), uavAtt_Yaw(0.f)
	{
		ZeroMemory(lrfData, LRF_DATA_H_SIZE);
		ZeroMemory(lrfDataV, LRF_DATA_V_SIZE);
	}
};

#pragma pack() // 構造体のアライメントをデフォルトに戻す

// TCP1の受信ペイロード
struct Payload_TCP1_Recv {
	double uavSlamvel_X;
	double uavSlamvel_Y;
	double uavRoll;
	double uavPitch;
	double uavYaw;
	double uavPos_X;
	double uavPos_Y;
	USHORT dist_Fwd;
	USHORT dist_FwdRight;
	USHORT dist_Right;
	USHORT dist_Left;
	USHORT dist_FwdLeft;
	USHORT distUs_Back;
	USHORT distUs_Nad;
	USHORT distUs_Zen;
	double voltage;
	BYTE flightMode;
	USHORT msapStatus;
	USHORT slampcStatus;
	USHORT uavCtrlInH_Th;
	USHORT uavCtrlInH_Roll;
	USHORT uavCtrlInH_Pitch;
	USHORT uavCtrlInH_Yaw;
	double uavViconvel_X;
	double uavViconvel_Y;
	double uavViconvel_Z;
	double uavViconpos_X;
	double uavViconpos_Y;
	double uavViconpos_Z;
	double uavViconatt_X;
	double uavViconatt_Y;
	double uavViconatt_Z;
	BYTE uavViconStatus;
	BYTE exchgStatus[EXCHG_STATUS_SIZE];

	Payload_TCP1_Recv() : uavSlamvel_X(0.0), uavSlamvel_Y(0.0),
		uavRoll(0.0), uavPitch(0.0), uavYaw(0.0),
		uavPos_X(0.0), uavPos_Y(0.0),
		dist_Fwd(0), dist_FwdRight(0), dist_Right(0), dist_Left(0), dist_FwdLeft(0),
		distUs_Nad(0), distUs_Zen(0), voltage(0.0), flightMode(0),
		msapStatus(0), slampcStatus(0),
		uavCtrlInH_Th(0), uavCtrlInH_Roll(0), uavCtrlInH_Pitch(0), uavCtrlInH_Yaw(0),
		uavViconvel_X(0.0), uavViconvel_Y(0.0), uavViconvel_Z(0.0),
		uavViconpos_X(0.0), uavViconpos_Y(0.0), uavViconpos_Z(0.0),
		uavViconatt_X(0.0), uavViconatt_Y(0.0), uavViconatt_Z(0.0),
		uavViconStatus(0)
	{
		::ZeroMemory(exchgStatus, sizeof(exchgStatus));
	}
};

// TCP1の送信ペイロード
struct Payload_TCP1_Send {
	char fpvRecv;

	Payload_TCP1_Send() : fpvRecv(NULL)
	{
	}
};

//*******************************************************************************
//	原発向けシステム用UDP基本パケットクラス
//
//	説明：
//*******************************************************************************
class UdpPacket
{
protected:
	std::vector<UCHAR> m_buf;

public:
	UdpPacket() {};
	virtual ~UdpPacket() {};

	size_t GetBufSize() const { return m_buf.size(); }
	UCHAR *GetBufPtr() { return &m_buf[0]; }
	void Clear() { m_buf.clear(); }
	void ZeroClear() { std::fill(m_buf.begin(), m_buf.end(), 0); }	// すべての要素を0で埋める
};

//*******************************************************************************
//	原発向けシステム用UDP送信パケットクラス
//
//	説明：
//*******************************************************************************
class UdpSendPacket : public UdpPacket
{
protected:

public:
	UdpSendPacket() {};
	virtual ~UdpSendPacket() {};

	BOOL PushHeader(USHORT packetId, UINT payloadLen);
	BOOL PushFooter();
	BOOL PushData(const void *src, UINT srcSize);
};

//*******************************************************************************
//	原発向けシステム用UDP受信パケットクラス
//
//	説明：
//*******************************************************************************
class UdpRecvPacket : public UdpPacket
{
protected:

public:
	UdpRecvPacket(size_t bufSize) { m_buf.resize(bufSize); }
	virtual ~UdpRecvPacket() {};

	USHORT GetPacketId() const;
	UINT GetPayloadLen() const;
	BOOL GetPayloadData(void *dest, UINT destSize, UINT offset = 0);
};


//*******************************************************************************
//	原発向けシステム用UDPサーバークラス
//
//	説明：
//*******************************************************************************
class NpUdpServer : public WSAUdpServer
{
private:
	typedef CExclusiveObject< std::queue<Payload_UDP1> > DataQueueUDP1;
	typedef CExclusiveObject< std::queue<Payload_UDP2_wo_DownCam> > DataQueueUDP2_wo_DownCam;
	typedef CExclusiveObject< std::queue<Payload_UDP2_DownCam> > DataQueueUDP2_DownCam;
	typedef CExclusiveObject< std::queue<Payload_UDP3_wo_UpCam> > DataQueueUDP3_wo_UpCam;
	typedef CExclusiveObject< std::queue<Payload_UDP3_UpCam> > DataQueueUDP3_UpCam;
	typedef CExclusiveObject< std::queue<Payload_UDP4> > DataQueueUDP4;
	typedef CExclusiveObject< std::queue<Payload_UDP5> > DataQueueUDP5;
	typedef CExclusiveObject< std::queue<Payload_UDP6> > DataQueueUDP6;

	DataQueueUDP1 m_dataUDP1;					// 「UDP1」パケットデータ(排他制御あり)
	DataQueueUDP2_wo_DownCam m_dataUDP2_wo_DownCam;	// 「UDP2」パケットデータ：下方向カメラデータを除く(排他制御あり)
	DataQueueUDP2_DownCam m_dataUDP2_DownCam;		// 「UDP2」パケットデータ：下方向カメラデータ(排他制御あり)
	DataQueueUDP3_wo_UpCam m_dataUDP3_wo_UpCam;	// 「UDP3」パケットデータ：上方向カメラデータを除く(排他制御あり)
	DataQueueUDP3_UpCam m_dataUDP3_UpCam;		// 「UDP3」パケットデータ：上方向カメラデータ(排他制御あり)
	DataQueueUDP4 m_dataUDP4;					// 「UDP4」パケットデータ(排他制御あり)
	DataQueueUDP5 m_dataUDP5;					// 「UDP5」パケットデータ(排他制御あり)
	DataQueueUDP6 m_dataUDP6;					// 「UDP6」パケットデータ(排他制御あり)
	DWORD m_ReceiveNoticeMessage_idThread;		// 受信通知メッセージのポスト先スレッド
	UINT  m_ReceiveNoticeMessage_Msg;			// 受信通知時に使用するメッセージ

public:
	NpUdpServer();
	virtual ~NpUdpServer() {}

	virtual void OnReceive();
	virtual void OnClose() {}	// temporary

	// 「UDP1」のペイロードデータを取得する
	BOOL GetDataUDP1(Payload_UDP1 &payload_udp1);
	// 「UDP2」のペイロードデータを取得する
	BOOL GetDataUDP2(Payload_UDP2_wo_DownCam &payload_udp2_wo_DownCam, Payload_UDP2_DownCam &payload_udp2_DownCam);
	// 「UDP3」のペイロードデータを取得する
	BOOL GetDataUDP3(Payload_UDP3_wo_UpCam &payload_udp3_wo_UpCam, Payload_UDP3_UpCam &payload_udp3_UpCam);	
	// 「UDP4」のペイロードデータを取得する
	BOOL GetDataUDP4(Payload_UDP4 &payload_udp4);
	// 「UDP5」のペイロードデータを取得する
	BOOL GetDataUDP5(Payload_UDP5 &payload_udp5);
	// 「UDP6」のペイロードデータを取得する
	BOOL GetDataUDP6(Payload_UDP6 &payload_udp6);
	// 受信通知メッセージの送り先を登録する
	BOOL RegisterReceiveNoticeMessage(DWORD idThread, UINT Msg);	
};

//*******************************************************************************
//	原発向けシステム用UDPクライアントクラス
//
//	説明：
//*******************************************************************************
class NpUdpClient : public WSAUdpClient
{
protected:
	HANDLE m_hSyncEvent;

	Payload_UDP1 m_dataUDP1;					// 「UDP1」パケットデータ
	Payload_UDP2_wo_DownCam m_dataUDP2_wo_DownCam;	// 「UDP2」パケットデータ：下方向カメラデータを除く
	Payload_UDP2_DownCam m_dataUDP2_DownCam;		// 「UDP2」パケットデータ：下方向カメラデータ
	Payload_UDP3_wo_UpCam m_dataUDP3_wo_UpCam;	// 「UDP3」パケットデータ：上方向カメラデータを除く
	Payload_UDP3_UpCam m_dataUDP3_UpCam;		// 「UDP3」パケットデータ：上方向カメラデータ
	Payload_UDP4 m_dataUDP4;					// 「UDP4」パケットデータ
	Payload_UDP5 m_dataUDP5;					// 「UDP5」パケットデータ
	Payload_UDP6 m_dataUDP6;					// 「UDP6」パケットデータ

public:
	NpUdpClient()
	{
		m_hSyncEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);	// 自動リセット
	}
	virtual ~NpUdpClient()
	{ 
		::CloseHandle(m_hSyncEvent);
		Destroy(); 
	}

	// 「UDP1」のペイロードデータを送信する
	BOOL SendDataUDP1(const Payload_UDP1 &payload_udp1);
	// 「UDP2」のペイロードデータを送信する
	BOOL SendDataUDP2(const Payload_UDP2_wo_DownCam &payload_udp2_wo_DownCam, const Payload_UDP2_DownCam payload_udp2_DownCam);
	// 「UDP3」のペイロードデータを送信する
	BOOL SendDataUDP3(const Payload_UDP3_wo_UpCam &payload_udp3_wo_UpCam, const Payload_UDP3_UpCam payload_udp3_UpCam);
	// 「UDP4」のペイロードデータを送信する
	BOOL SendDataUDP4(const Payload_UDP4 &payload_udp4);
	// 「UDP5」のペイロードデータを送信する
	BOOL SendDataUDP5(const Payload_UDP5 &payload_udp5);
	// 「UDP6」のペイロードデータを送信する
	BOOL SendDataUDP6(const Payload_UDP6 &payload_udp6);

protected:
	virtual void OnReceive();
	virtual void OnClose() {}	// temporary
	virtual void OnSync() {}
};

//*******************************************************************************
//	原発向けシステム用TCPクライアントクラス
//
//	説明：
//*******************************************************************************
class NpTcpClient : public XMonitorSyncClient
{
private:
	DWORD m_ReceiveNoticeMessage_idThread;		// 受信通知メッセージのポスト先スレッド
	UINT  m_ReceiveNoticeMessage_Msg;			// 受信通知時に使用するメッセージ
	BOOL m_RequestData;							// データ要求フラグ

public:
	NpTcpClient();
	virtual ~NpTcpClient() {}

	// 受信通知メッセージの送り先を登録する
	BOOL RegisterReceiveNoticeMessage(DWORD idThread, UINT Msg);

	// データ要求取得
	BOOL IsRequested() { return m_RequestData; }
	// データ要求設定
	void SetRequest(BOOL req) { m_RequestData = req; }

protected:
	virtual void OnSync();
};
