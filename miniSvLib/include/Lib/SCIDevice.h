///
/// @file       SCIDevice.h
///
/// @brief      シリアル通信を使うデバイスの基本定義
///
/// @author     iwakura
///
/// @note       依存度を下げるためある時点でファクトリクラスに置き換える予定
///

#ifndef INCLUDED_SCIDEVICE_BASE_H
#define INCLUDED_SCIDEVICE_BASE_H

#include "Common/StandardDef.h"
#include "Common/ISci.h"

enum BAUDRATE 
{
	BR_4800   = 4800,
	BR_9600   = 9600,
	BR_19200  = 19200,
	BR_38400  = 38400,
	BR_57600  = 57600,
	BR_62500  = 62500,
	BR_115200 = 115200,
	BR_125000 = 125000,
};

//*******************************************************************************
/// @brief  シリアル通信を使うデバイスの基本クラス
//*******************************************************************************
class SciDevice
{
protected:
	// シリアル通信クラスのポインタ(InitializeSciで自動設定)
	ISci* m_pSci;

public:
	SciDevice();
	virtual ~SciDevice();

protected:
	CRESULT OpenSci(_UBYTE Port, _UDWORD Baudrate);
	CRESULT OpenSci(_UBYTE Port, _UDWORD Baudrate, PARITY Parity, STOPBIT StopBits);
};





#endif