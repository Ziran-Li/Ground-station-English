///
/// @file       ISci.h
///
/// @brief      シリアル通信共通インターフェース定義
///
/// @author     iwakura
///

#ifndef INCLUDED_ISCI_H
#define INCLUDED_ISCI_H

#include "Common/IConnection.h"

enum PARITY 
{
	SCI_PARITY_NONE    = 0,   //!< パリティなし
	SCI_PARITY_ODD     = 1,   //!< 奇数パリティ
	SCI_PARITY_EVEN    = 2,   //!< 偶数パリティ
	SCI_PARITY_DEFAULT = 255, //!< オブジェクトの規定値
};

enum STOPBIT
{
	SCI_STOPBIT_ONE     = 0,   //!< 1ストップビット
	SCI_STOPBIT_ONE5    = 1,   //!< 1.5ストップビット
	SCI_STOPBIT_TWO     = 2,   //!< 2ストップビット
	SCI_STOPBIT_DEFAULT = 255, //!< オブジェクトの規定値
};

//*******************************************************************************
/// @brief  シリアル通信共通インターフェース
//*******************************************************************************
class ISci : public IConnection
{
public:
	virtual ~ISci() {}

	virtual int     GetPortNumber() const = 0;

	virtual CRESULT SetBaudrate(_UDWORD Baudrate) = 0; // Openしている間に変更することはできない
	virtual CRESULT SetParity(PARITY Parity)      = 0; // Openしている間に変更することはできない
	virtual CRESULT SetStopBits(STOPBIT StopBits) = 0; // Openしている間に変更することはできない
	
	virtual _UDWORD GetParityErrors() const = 0;
	virtual _UDWORD GetFramingErrors() const = 0;
	virtual _UDWORD GetOverrunErrors() const = 0;
};

#endif