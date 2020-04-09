#include "WirelessDeviceCreator.h"
#include "WirelessDevice_Transparent.h"
#include "WirelessDevice_FEP.h"

WirelessDevice* WirelessDeviceCreator::Create(WIRELESSDEVICE id)
{
	switch (id)
	{
	case WIRELESSDEVICE_TRANSPARENT:
		return new Wireless_Transparent(m_PortNo);
		break;
	case WIRELESSDEVICE_FEP_HEADERLESS:
		return new Wireless_FEP(m_PortNo);
		break;
	default:
		return NULL;
		break;
	}
}