///
/// @file       WirelessDeviceCreator.h
///
/// @brief      無線デバイスオブジェクト生成クラス
///
/// @author     iwakura
///

#include "WirelessDevice.h"

enum WIRELESSDEVICE
{
	WIRELESSDEVICE_TRANSPARENT = 1,
	WIRELESSDEVICE_FEP_HEADERLESS = 2,
};

class WirelessDeviceCreator
{
private:
	_UBYTE m_PortNo;

public:
	WirelessDeviceCreator(_UBYTE PortNo) : m_PortNo(PortNo) {}

	WirelessDevice* Create(WIRELESSDEVICE id);
};