///
///
/// @file       Udp.h
///
/// @brief      UDP通信クラス定義
///
/// @author     iwakura
///
///

#pragma once

#include <basetsd.h>
#include "Common/IUdp.h"

class Udp : public IUdp
{
	static _UDWORD s_Count;

	typedef UINT_PTR SOCKET;
	typedef struct sockaddr_in SOCKADDR_IN;

	SOCKET*      m_pSock;
	SOCKADDR_IN* m_pAddrIn;
	SOCKADDR_IN* m_pAddrOut;

	_UBYTE m_RxBuffer[65536];
	_UWORD m_RxReaded; // 読み取り済バイト数
	_UWORD m_RxLength; // 全バイト数

public:
	Udp(const char* HostName, _UWORD PortSend, _UWORD PortRecv);
	virtual ~Udp();

	// 接続／切断
	virtual CRESULT Open();
	virtual void    Close();

	// 通信可能かチェック
	virtual bool    IsEnable() const;

	// 受信チェック
	virtual bool    IsReceiveData();

	// 送信可能チェック
	virtual _UDWORD IsTransmittable(void);

	// 入出力関数
	virtual	CRESULT WriteElement(_UBYTE chr);                   // 1Byte送信
	virtual	CRESULT ReadElement(_UBYTE* ptr);                   // 1Byte受信

	virtual CRESULT ReadString(void* ptr, _UWORD len);          // 連続データ送信(UDP専用)
	virtual	CRESULT WriteString(const _UBYTE* ptr, _UWORD len); // 連続データ送信

	// 統計情報
	virtual _UDWORD GetTxCount() const { return 0; }
	virtual _UDWORD GetRxCount() const { return 0; }

private:
	void _Receive();
};