///
/// @file       Communicator.h
///
/// @brief      ペイロード生成・展開クラスの基本定義
///
///             Communicatorクラスの役目
///             送信：
///                 メインクラスから情報を取得し、ペイロードを作成してプロトコルオブジェクトの送信関数に渡す
///             受信：
///                 プロトコルオブジェクトからペイロード部を取得し、デコードしてメインクラスに渡す
///
/// @author     iwakura
///

#ifndef INCLUDED_COMMUNICATOR_H
#define INCLUDED_COMMUNICATOR_H

#include "WirelessDevice.h"

class ICommunicator
{
protected:
	WirelessDevice& m_WirelessDevice;

public:
	///
	/// @brief  コンストラクタ
	///
	/// @param[in] rWirelessDevice  無線デバイスクラスへの参照
	///
	ICommunicator(WirelessDevice& rWirelessDevice) : m_WirelessDevice(rWirelessDevice) {}

	virtual CRESULT Initialize() = 0;
	virtual bool Startup() = 0;

	virtual CRESULT Receive() = 0;	
	virtual CRESULT Send() = 0;
	
	virtual CRESULT Decode(unsigned int ID, const void* pBuff, size_t BufferLength) { return C_OK; }
	
	virtual size_t GetPayloadLengthByID(unsigned int ID) const {return 0;}
	
	virtual CRESULT GetReceiveBuffer(void** BufferPtr, size_t* pBufferLength) = 0;
};

#endif