///
/// @file       IConnection.h
///
/// @brief      1対1通信共通インターフェース定義
///
/// @author     iwakura
///

#ifndef INCLUDED_ICONNECTION_H
#define INCLUDED_ICONNECTION_H

#include "Common/StandardDef.h"

//*******************************************************************************
/// @brief  1対1通信共通インターフェース
//*******************************************************************************
class IConnection
{
public:
	virtual ~IConnection() {}

	// 
	virtual CRESULT Open() = 0;
	virtual void    Close() = 0;

	// 通信ができる状態か否か調べる
	virtual bool    IsEnable() const = 0;

	// 受信チェック
	virtual bool    IsReceiveData() = 0;

	// 送信可能チェック
	virtual _UDWORD IsTransmittable(void) = 0;

	// 入出力関数
	virtual	CRESULT WriteElement(_UBYTE chr) = 0;                   // 1Byte送信
	virtual	CRESULT ReadElement(_UBYTE* ptr) = 0;                   // 1Byte受信

	virtual	CRESULT WriteString(const _UBYTE* ptr, _UWORD len) = 0; // 連続データ送信

	// 統計情報
	virtual _UDWORD GetTxCount() const = 0;
	virtual _UDWORD GetRxCount() const = 0;
};

#endif