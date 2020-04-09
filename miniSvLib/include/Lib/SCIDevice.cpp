#include "Lib/SCIDevice.h"
#include "Common/ConnectionCreator.h"

//*******************************************************************************
//	\’zE”jŠüŽž‚Ìˆ—
//*******************************************************************************

//*******************************************************************************
//	ƒNƒ‰ƒX‚Ì‰Šú‰»
//*******************************************************************************


SciDevice::SciDevice()
{
	m_pSci = ConnectionCreator::Create(PSciNull());
}

SciDevice::~SciDevice()
{
	ConnectionCreator::Release(m_pSci);
}

CRESULT SciDevice::OpenSci(_UBYTE Port, _UDWORD Baudrate)
{
	return OpenSci(Port, Baudrate, SCI_PARITY_NONE, SCI_STOPBIT_ONE);
}

CRESULT SciDevice::OpenSci(_UBYTE Port, _UDWORD Baudrate, PARITY Parity, STOPBIT StopBits)
{
	CRESULT res = C_OK;

	ISci* pSci = ConnectionCreator::Create(PSci(Port));
	if (ISOK(res)) res = pSci->SetBaudrate(Baudrate);
	if (ISOK(res)) res = pSci->SetParity(Parity);
	if (ISOK(res)) res = pSci->SetStopBits(StopBits);
	if (ISOK(res)) res = pSci->Open();

	if (ISOK(res))
	{
		ConnectionCreator::Release(m_pSci);
		m_pSci = pSci;
	}

	return res;
}


