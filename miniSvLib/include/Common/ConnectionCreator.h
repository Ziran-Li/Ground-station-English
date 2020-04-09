#ifndef INCLUDED_CONNECTIONCREATOR_H
#define INCLUDED_CONNECTIONCREATOR_H

#include "Common/ISci.h"
#include "Common/IUdp.h"

struct PUdp
{
	const char* HostName; //!< 送信先ホスト名
	_UWORD      PortRecv; //!< 送信先ポート番号
	_UWORD      PortSend; //!< 受信ポート番号

	PUdp(const char* HostName, _UWORD PortSend, _UWORD PortRecv) : HostName(HostName), PortSend(PortSend), PortRecv(PortRecv) {}
};

struct PSci
{
	_UBYTE  Port;
	_UWORD  Baudrate;
	PARITY  Parity;
	STOPBIT StopBits;

	PSci(_UBYTE Port) : Port(Port), Baudrate(0), Parity(SCI_PARITY_DEFAULT), StopBits(SCI_STOPBIT_DEFAULT) {}
	PSci(_UBYTE Port, _UWORD Baudrate, PARITY Parity, STOPBIT StopBits) : Port(Port), Baudrate(Baudrate), Parity(Parity), StopBits(StopBits) {}

	operator _UBYTE ()
	{
		return Port;
	}
};

struct PSciFromXml : PSci
{
	const char* FileName;
	const char* XmlNode;

	PSciFromXml(const char* FileName, const char* XmlNode);
};

struct PSciNull
{
	PSciNull() {}
};

class ConnectionCreator
{
public:
	static ISci* Create(_UBYTE Port) { return Create(PSci(Port)); }
	static ISci* Create(const PSci& param);
	static ISci* Create(const PSciNull& param);
	static IUdp* Create(const PUdp& param);

	static void Release(IConnection* pConnection);
};


#endif