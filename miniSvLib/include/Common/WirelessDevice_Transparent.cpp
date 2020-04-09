#include "WirelessDevice_Transparent.h"
#include "Common/ConnectionCreator.h"

#define DEFAULT_BAUDRATE  57600

Wireless_Transparent::Wireless_Transparent(_UBYTE PortNo) : WirelessDevice(PortNo)
{
	m_pSci = ConnectionCreator::Create(PSci(PortNo)); // インスタンス生成保障
}

Wireless_Transparent::~Wireless_Transparent()
{
	ConnectionCreator::Release(m_pSci);
}


CRESULT Wireless_Transparent::Initialize(_UDWORD Baudrate)
{
	if (Baudrate == 0) Baudrate = DEFAULT_BAUDRATE;

	CRESULT res = C_OK;

	if (ISOK(res)) res = m_pSci->SetBaudrate(Baudrate);
	if (ISOK(res)) res = m_pSci->SetParity(SCI_PARITY_NONE);
	if (ISOK(res)) res = m_pSci->SetStopBits(SCI_STOPBIT_ONE);
	if (ISOK(res)) res = m_pSci->Open();
	
	return res;	
}

void Wireless_Transparent::Update()
{
}

bool Wireless_Transparent::IsReady()
{
	return true;
}

bool Wireless_Transparent::IsTxBusy()
{
	return false;
}

CRESULT Wireless_Transparent::SendBegin(size_t TotalLength)
{
	return C_OK;
}

CRESULT Wireless_Transparent::Send(_UBYTE data)
{
	return m_pSci->WriteElement(data);	
}

CRESULT Wireless_Transparent::Send(const void* pData, size_t length)
{
	return m_pSci->WriteString(reinterpret_cast<const unsigned char*>(pData), length);
}

CRESULT Wireless_Transparent::SendEnd()
{
	return C_OK;
}

CRESULT Wireless_Transparent::Flush()
{
	return C_UNSUPPORTED;
}
	
CRESULT Wireless_Transparent::Receive(_UBYTE* pData)
{
	return m_pSci->ReadElement(pData);	
}
	
bool Wireless_Transparent::IsDataReceived()
{
	return m_pSci->IsReceiveData();	
}
	
void Wireless_Transparent::ProcessUnusedCharacter(_UBYTE c)
{
}
	
CRESULT Wireless_Transparent::GetRssi(_UBYTE* pRssi) const
{
	return C_UNSUPPORTED;
}

_UWORD Wireless_Transparent::GetRoundTripTime(size_t TxBytes, size_t RxBytes) const
{
	return 0;
}