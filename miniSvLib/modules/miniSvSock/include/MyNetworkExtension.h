
//	UGV ネットワーク用にカスタマイズしたクラス群
//	X-Monitor 用

#pragma once

#include "CommBuffer.h"
#include "common/CommonMacro.h"

#include "ExclusiveObject.h"
#include "WinsockTCP2.h"

#include <queue>

// クラスのプロトタイプ宣言
class XMonitorServer;

// インテリセンスを有効活用するための名前空間
class MyRxBuffer;
class MyTxBuffer;

namespace exc {
	typedef CExclusivePointer<MyRxBuffer> RxBuffer;
	typedef CExclusivePointer<MyTxBuffer> TxBuffer;
}

struct XMLRecvData{
	unsigned int connectionID;  // 接続相手を識別するためのID
	std::string xmlData;        // 受信したXML文

	XMLRecvData() : connectionID(0), xmlData(){};
};

//*******************************************************************************
//	受信バッファの拡張版
//
//	説明：
//		リトルエンディアンアライメントの通信用受信バッファに，パケットの読み
//		出し機能を追加したもの．
//
//	追加機能：
//*******************************************************************************
class MyRxBuffer : public CommBuffer::LittleEndianProtocol::RxBuffer
{
protected:

	size_t m_AllowedPacketSize; // 許容する最大のパケットサイズ（この値以上のサイズが設定されているパケットは無視する）

public:
	MyRxBuffer(size_t nBlockSize, size_t nMaximumSize = NO_LIMIT, size_t nAllowedPacketSize = NO_LIMIT);

	BOOL       GetPacket(std::string &strPacket, const unsigned char delimiter);
};

//*******************************************************************************
//	送信バッファの拡張版
//
//	説明：
//		リトルエンディアンアライメントの通信用送信バッファに，パケットの検索と読み
//		出し機能を追加したもの．
//
//	追加機能：
//*******************************************************************************
class MyTxBuffer : public CommBuffer::LittleEndianProtocol::TxBuffer
{
public:
	MyTxBuffer(size_t nBlockSize, size_t nMaximumSize = NO_LIMIT);

	BOOL SetPacket(const std::string &strPacket);
};

//*******************************************************************************
//	拡張単位接続クラス（サーバー・クライアント共通部分）
//
//	説明：
//*******************************************************************************
class MyWSAConnection : public WSAConnection
{
protected:
	exc::RxBuffer m_RxBuff;
	exc::TxBuffer m_TxBuff;

public:
	MyWSAConnection(size_t RxBlockSize, size_t TxBlockSize);

	BOOL SendPacket(const std::string &strPacket);

protected:
	virtual void OnSend();
	virtual void OnReceive();
	virtual void OnClose();
};

//*******************************************************************************
//	サーバー用拡張単位接続クラス
//
//	説明：
//*******************************************************************************
class MyWSAConnection_For_Server : public MyWSAConnection
{
	friend class XMonitorServer;

private:
	enum {
		RXBUFFER_BLOCK_SIZE = 1024,
		TXBUFFER_BLOCK_SIZE = 51200,
	};
public:
	MyWSAConnection_For_Server() : MyWSAConnection(RXBUFFER_BLOCK_SIZE, TXBUFFER_BLOCK_SIZE) {}
};

//*******************************************************************************
//	クライアント用拡張単位接続クラス
//
//	説明：
//*******************************************************************************
class MyWSAConnection_For_Client : public MyWSAConnection
{
private:
	enum {
		RXBUFFER_BLOCK_SIZE = 51200,
		TXBUFFER_BLOCK_SIZE = 1024,
	};
public:
	MyWSAConnection_For_Client() : MyWSAConnection(RXBUFFER_BLOCK_SIZE, TXBUFFER_BLOCK_SIZE) {}
};

//*******************************************************************************
//	X-Monitor 用サーバークラス
//
//	説明：
//*******************************************************************************
class XMonitorServer : public WSATcpServer<MyWSAConnection_For_Server>
{
public:
	//enum RECEIVE_DATA_TYPE
	//{
	//	CHANGE_NAVDATA,
	//	CHANGE_IMUDATA,
	//	CHANGE_LOCALPOS,
	//	CHANGE_ANEMOMETERDATA,
	//	CHANGE_GCSPROPODATA,
	//	MAINLINK_DATA_TRANSMISSION,
	//};

	//typedef void (*ReceiveCallbackFunc)(XMonitorServer* pServer, RECEIVE_DATA_TYPE ReceiveDataType, void* pUser);

protected:
	// メッセージ関連（今のところ排他制御はしていない）
	DWORD m_ReceiveNoticeMessage_idThread; // 受信通知メッセージのポスト先スレッド
	UINT  m_ReceiveNoticeMessage_Msg;      // 受信通知時に使用するメッセージ

	DWORD m_AcceptNoticeMessage_idThread;  // 接続通知メッセージのポスト先スレッド
	UINT  m_AcceptNoticeMessage_Msg;       // 接続通知時に使用するメッセージ

	DWORD m_CloseNoticeMessage_idThread;   // 切断通知メッセージのポスト先スレッド
	UINT  m_CloseNoticeMessage_Msg;        // 切断通知時に使用するメッセージ

	// 受信データキュー
	CExclusiveObject< std::queue<XMLRecvData> >            m_XMLRecvDataQueue;

public:
	XMonitorServer();
	virtual ~XMonitorServer() { Destroy(); }

	// メッセージの登録
	BOOL RegisterReceiveNoticeMessage(DWORD idThread, UINT Msg); // 受信通知メッセージの送り先を登録する
	BOOL RegisterAcceptNoticeMessage (DWORD idThread, UINT Msg);
	BOOL RegisterCloseNoticeMessage  (DWORD idThread, UINT Msg);

	// データの送信
	BOOL SendPacket(const std::string &packet, UINT connectionID); // 汎用パケット送信関数
	//BOOL SendPacket(const PacketBase* pPacket); // 汎用パケット送信関数
	//BOOL SendMAVData(const CMavState& Mav);
	//BOOL SendGCSData(const CGcsState& Gcs);
	//BOOL SendSync();

	// 受信データの取得
	BOOL   PopRecvData(std::string& recvData, unsigned int &connectionID); // 受信パケットの中身、接続ID
	//BOOL   PopPayload_ChangeData(IMUData& Imu);
	//BOOL   PopPayload_ChangeData(LocalPos& Loc);
	//BOOL   PopPayload_ChangeData(AnemometerData& Anemo);
	//BOOL   PopPayload_ChangeGCSPropoData(PropoData& Propo);
	//size_t GetSize_FrontOfMainLinkData() const;
	//BOOL   PopPayload_MainLinkData(PACKETID& PacketID, BYTE* pData, DWORD DataSize);
	//BOOL   PopPayload_MainLinkData(); // 破棄

protected:
	virtual void OnReceive(MyWSAConnection_For_Server* pClient);
	virtual void OnAccept (MyWSAConnection_For_Server* pClient);
	virtual void OnClose  (MyWSAConnection_For_Server* pClient);
};

//*******************************************************************************
//	X-Monitor クライアントクラス（同期）
//
//	説明：
//		本クラスは，同期パケットを利用することで X-Monitor 内の時系列に正確な 
//		MavState と GcsState を提供します。
//		厳密な MavState と GcsState の時系列が必要な場合に本クラスを使用してくだ
//		さい。
//
//	注意：
//		バッファにデータがたまらないようにするため，できるだけ 20ms 以下の周期で
//		データ取得を行ってください．
//*******************************************************************************
class XMonitorSyncClient : public WSATcpClient<MyWSAConnection_For_Client>
{
protected:
	HANDLE m_hSyncEvent;

	typedef CExclusiveObject< std::queue<std::string> > RecvDataQueue;

	RecvDataQueue m_RecvDataQueue;

	//// OnReceive のみアクセスする
	//MavStatePacket m_MavStatePacket;
	//GcsStatePacket m_GcsStatePacket;
	//BOOL           m_flagMavStatePacket;
	//BOOL           m_flagGcsStatePacket;

public:
	XMonitorSyncClient() 
	{
		m_hSyncEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);	// 自動リセット
		//m_flagMavStatePacket = FALSE;
		//m_flagGcsStatePacket = FALSE;
	}
	virtual ~XMonitorSyncClient() 
	{ 
		::CloseHandle(m_hSyncEvent);
		Destroy(); 
	}

	//BOOL GetData(CMavState& Mav, CGcsState& Gcs, DWORD msecTimeout = 2000);
	BOOL GetData(std::vector<std::string>& recvData, DWORD msecTimeout = 2000);
	BOOL SendData(const std::string &sendData);

protected:
	virtual void OnReceive();
	virtual void OnClose();
	virtual void OnSync() {}
};

//
////*******************************************************************************
////	X-Monitor クライアントクラス（非同期）
////
////	説明：
////		本クラスは，X-Monitor から受信した最新のデータを提供します．
////		X-Monitor との同期は行われないため，データの欠落や重複したデータの取得が
////		起こり得ます。
////		データのモニタリングなどの用途に使用してください。
////		
////	注意：
////		X-Monitor 側でログデータの再生を行う場合，本クラスを用いたプログラムでは
////		動作の再現性が保証されません。
////*******************************************************************************
//class XMonitorAsyncClient : public WSATcpClient<MyWSAConnection_For_Client>
//{
//protected:
//	typedef CExclusiveObject< CMavState > MavState;
//	typedef CExclusiveObject< CGcsState > GcsState;
//
//	MavState m_MavState;
//	GcsState m_GcsState;
//
//	BOOL m_flagMavStateReceived; // 受信フラグ
//	BOOL m_flagGcsStateReceived; // 受信フラグ
//
//public:
//	XMonitorAsyncClient() 
//	{
//		m_flagMavStateReceived = FALSE;
//		m_flagGcsStateReceived = FALSE;
//	}
//	virtual ~XMonitorAsyncClient() 
//	{
//		Destroy(); 
//	}
//
//	BOOL GetData(CMavState& Mav);                 // 受信した MAV 関連データを取得する
//	BOOL GetData(CGcsState& Gcs);                 // 受信した GCS 関連データを取得する
//	BOOL GetData(CMavState& Mav, CGcsState& Gcs); // 受信した MAV, GCS 関連データを取得する
//
//	BOOL ChangeData(const NAVData& Nav);          // X-Monitor に NAVData を送って内部データを変更する
//	BOOL ChangeData(const IMUData& Imu);          // X-Monitor に IMUData を送って内部データを変更する
//	BOOL ChangeData(const LocalPos& Loc);         // X-Monitor に LocalPos を送って内部データを変更する 
//	BOOL ChangeData(const AnemometerData& Anemo); // X-Monitor に AnemometerData を送って内部データを変更する
//	BOOL ChangeGCSPropoData(const PropoData& Propo);
//
//	BOOL TransmitPacketToMAV(PACKETID PacketID, const void* pData, size_t nDataLength); // X-Monitor 経由で MAV にパケットを送信する
//
//protected:
//	virtual void OnConnect();
//	virtual void OnReceive();
//	virtual void OnClose();
//};