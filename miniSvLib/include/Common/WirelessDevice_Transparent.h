///
/// @file       WirelessDevice_Transparent.h
///
/// @brief      汎用無線デバイス（透過モード）
///
/// @author     iwakura
///

#ifndef INCLUDED_WIRELESSDEVICE_TRANSPARENT_H
#define INCLUDED_WIRELESSDEVICE_TRANSPARENT_H

#include "WirelessDevice.h"

class ISci;

class Wireless_Transparent : public WirelessDevice
{
	ISci* m_pSci;

public:
	Wireless_Transparent(_UBYTE PortNo);
	virtual ~Wireless_Transparent();

	virtual CRESULT Initialize(_UDWORD Baudrate = 0);

	virtual void Update();

	virtual bool IsReady();

	virtual bool IsTxBusy();

	virtual CRESULT SendBegin(size_t TotalLength);

	virtual CRESULT Send(_UBYTE data);

	virtual CRESULT Send(const void* pData, size_t length);

	virtual CRESULT SendEnd();

	virtual CRESULT Flush();
	
	virtual CRESULT Receive(_UBYTE* pData);
	
	virtual bool IsDataReceived();
	
	virtual void ProcessUnusedCharacter(_UBYTE c);
	
	virtual CRESULT GetRssi(_UBYTE* pRssi) const;

	///
	/// @brief  パケットの往復に要する時間をミリ秒単位で取得します
	///         ただし、受信側は受信直後に送信すると仮定します
	///
	virtual _UWORD GetRoundTripTime(size_t TxBytes = (size_t)-1, size_t RxBytes = (size_t)-1) const;
};



#endif