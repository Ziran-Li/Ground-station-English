///
/// @file       WirelessDevice_Transparent.h
///
/// @brief      �ėp�����f�o�C�X�i���߃��[�h�j
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
	/// @brief  �p�P�b�g�̉����ɗv���鎞�Ԃ��~���b�P�ʂŎ擾���܂�
	///         �������A��M���͎�M����ɑ��M����Ɖ��肵�܂�
	///
	virtual _UWORD GetRoundTripTime(size_t TxBytes = (size_t)-1, size_t RxBytes = (size_t)-1) const;
};



#endif