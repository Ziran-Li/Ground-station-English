///
/// @file       Protocol_2G.h
///
/// @brief      第二世代プロトコル
///
/// @author     iwakura
///

#ifndef INCLUDED_PROTOCOL_2G_H
#define INCLUDED_PROTOCOL_2G_H

#include "Protocol.h"

///
/// @brief  第二世代無線通信プロトコルクラス
///
class Protocol_2G : public Protocol
{
	enum
	{
		PREAMBLE1 = 0xA5,            //!< 同期バイト1
		PREAMBLE2 = 0x5A,            //!< 同期バイト2
		PREAMBLE_LENGTH = 2,         //!< プリアンブルの長さ
		HEADER_LENGTH = 3,           //!< プリアンブル+IDの長さ
		FOOTER_LENGTH = 2,	         //!< チェックサムの長さ
		OFFSET_ID = PREAMBLE_LENGTH, //!< IDの位置
	};

	struct //!< パケットのデコードに用いる一時領域
	{
		unsigned char  BufferLength;        //!< バッファ長
		unsigned char* pBuff;               //!< バッファ
		unsigned char  DataCount;           //!< 受信バイト数
		unsigned char  CurrentPacketLength; //!< 受信中のパケットの長さ
	} m_Rx;
	
public:
	
	///
	/// @brief  コンストラクタ
	///
	/// @param[in] device        無線デバイスクラスへの参照
	/// @param[in] communicator  パケット生成・展開クラスへの参照
	///
	Protocol_2G(WirelessDevice& device, ICommunicator& communicator);
	
	///
	/// @brief  初期化
	///
	/// @return  C_OK - 成功 / C_FAIL - 失敗
	///
	virtual CRESULT Initialize();
	
	///
	/// @brief  データの送信
	///         受け取ったデータを第二世代システムのフォーマットで送信する
	///
	/// @param[in] ID      パケットID
	/// @param[in] pBuff   データ先頭のポインタ
	/// @param[in] length  送信データの長さ
	///
	/// @return  C_OK - 成功 / C_FAIL - 失敗
	///
	virtual CRESULT Send(unsigned int ID, const void* pBuff, size_t length);
	
	///
	/// @brief   データの受信
	///          第二世代システムのフォーマットに合致するパケットの抽出を行い、デコード関数(ICommunicator::Decode)を呼ぶ
	///
	/// @return  C_OK - 成功 / C_FAIL - 失敗
	///
	virtual CRESULT Receive();

	///
	/// @brief   パケットの最小サイズ（IDを含むヘッダの長さ＋フッタの長さ）を返す
	///
	/// @return  パケットの最小サイズ
	///
	virtual size_t  GetRxBufferMinimumRequirement() const;

private:
	void   _ClearRxBuffer();   //!< デコード後に次のパケットを受信できるようにバッファをクリアする
	void   _EraseJunkData();   //!< 破棄されたパケットから無効部分を削除する
	bool   _VerifyCRC() const; //!< 受信データのチェックサムを検証する
	void   _ClearMemory();     //!< オブジェクトの状態を初期状態に戻す

	size_t _GetPacketLengthByID(_UBYTE ID) const;

	static unsigned short _CRC16Value(unsigned char b);
	static unsigned short _CalculateBlockCRC16(unsigned char Preamble2, unsigned char ID, const void* pSrc, unsigned long count);
};



#endif