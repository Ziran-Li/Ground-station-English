///
/// @file       Protocol.h
///
/// @brief      プロトコルクラスの基本定義
///
///             プロトコルクラスの役目
///             送信：
///                 ペイロードに対し、ヘッダとフッタを付加して無線デバイスオブジェクトにデータを渡す
///             受信：
///                 無線デバイスオブジェクトから取得したデータからパケットを検出し、ペイロード部を抽出する
///                 抽出したデータはCommunicatorオブジェクトに渡される
///
/// @author     iwakura
///

#ifndef INCLUDED_PROTOCOL_H
#define INCLUDED_PROTOCOL_H

#include "Common/StandardDef.h"

class WirelessDevice;
class ICommunicator;

///
/// @brief  プロトコル基本クラス
///
class Protocol
{
protected:
	WirelessDevice& m_WirelessDevice; //!< 無線デバイスクラスへの参照
	ICommunicator&  m_Communicator;   //!< ペイロード生成・展開クラスへの参照

public:

	///
	/// @brief  コンストラクタ
	///
	/// @param[in] device        無線デバイスクラスへの参照
	/// @param[in] communicator  ペイロード生成・展開クラスへの参照
	///
	Protocol(WirelessDevice& device,
	         ICommunicator&  communicator) : 
			 m_WirelessDevice(device),
			 m_Communicator(communicator) {}

	///
	/// @brief  初期化
	///
	/// @return  C_OK - 成功 / C_FAIL - 失敗
	///
	virtual CRESULT Initialize() = 0;

	///
	/// @brief  データの送信
	///         派生クラスでは受け取ったペイロード部のデータをプロトコルごとのフォーマットでパケット化するコードが実装される
	///
	/// @param[in] ID      パケットID
	/// @param[in] pBuff   データ先頭のポインタ
	/// @param[in] length  送信データの長さ
	///
	/// @return  C_OK - 成功 / C_FAIL - 失敗
	///
	virtual CRESULT Send(unsigned int ID, const void* pBuff, size_t length) = 0;
	
	///
	/// @brief  データの受信
	///         派生クラスではパケットの抽出を行い、ICommunicator::Decode を呼んでペイロード部を展開させるコードが実装される
	///
	/// @return  C_OK - 成功 / C_FAIL - 失敗
	///
	virtual CRESULT Receive() = 0;
	
	///
	/// @brief  パケットの最小サイズ（IDを含むヘッダの長さ＋フッタの長さ）を返す
	///
	/// @return  パケットの最小サイズ
	///
	virtual size_t  GetRxBufferMinimumRequirement() const = 0;
};

#endif