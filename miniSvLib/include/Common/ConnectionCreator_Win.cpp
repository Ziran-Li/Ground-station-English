#include "ConnectionCreator.h"
#include "Lib/SCI_Win.h"

#include "Common/Udp.h"
#include "Common/XmlReader.h"
#include <string>

/*
PSciFromXml::PSciFromXml(const char* FileName, const char* XmlNode) : PSci(0)
{
	bool ok = true;
	XmlReader reader;
	int Port = 0;

	// XMLファイルオープン
	if (ok) ok = reader.Open(FileName);

	// ノードの情報を取得
	if (ok) ok = reader.Read((std::string("root.") + XmlNode + ".Port").c_str(), Port);

	// 取得した値の範囲チェック
	if (ok && 0 <= Port && Port <= 0xFF)
	{
		this->Port = static_cast<_UBYTE>(Port);
	}
}]
*/

///
/// @brief  何の機能も持たないシリアル通信クラス
///
class CSciNull : public ISci
{
	virtual int     GetPortNumber() const         { return PORTNO_NULL; }

	virtual CRESULT SetBaudrate(_UDWORD Baudrate) { return C_FAIL; }
	virtual CRESULT SetParity(PARITY Parity)      { return C_FAIL; }
	virtual CRESULT SetStopBits(STOPBIT StopBits) { return C_FAIL; }

	virtual CRESULT Open()  { return C_FAIL; }
	virtual void    Close() {}

	virtual bool    IsEnable() const { return true; }

	// 初期化（ここでCOMポートをオープン）
	virtual CRESULT Initialize(_UDWORD Baudrate, _UBYTE Parity, _UBYTE StopBits) { return C_FAIL; }

	// データ送信関数
	virtual CRESULT WriteElement(_UBYTE chr)                   { return C_FAIL; }
	virtual CRESULT WriteString(const _UBYTE* ptr, _UWORD len) { return C_FAIL; }

	// データ受信関数
	virtual bool    IsReceiveData()          { return false; }
	virtual CRESULT ReadElement(_UBYTE* ptr) { return C_FAIL; }

	virtual _UDWORD IsTransmittable(void)    { return 0; }

	// 統計情報
	virtual _UDWORD GetTxCount() const       { return 0; }
	virtual _UDWORD GetRxCount() const       { return 0; }
	virtual _UDWORD GetParityErrors() const  { return 0; }
	virtual _UDWORD GetFramingErrors() const { return 0; }
	virtual _UDWORD GetOverrunErrors() const { return 0; }
};

static CSciNull SciNull; // エラー時にNULLポインタを返さないことを保障するためのインスタンス

ISci* ConnectionCreator::Create(const PSci& param)
{
	ISci* pSci = new CSciBase(param.Port);

	if (!isAllocationOk(pSci)) pSci = &SciNull;

	pSci->SetBaudrate(param.Baudrate);
	pSci->SetParity(param.Parity);
	pSci->SetStopBits(param.StopBits);

	return pSci;
}

ISci* ConnectionCreator::Create(const PSciNull& param)
{
	return &SciNull;
}

/*
IUdp* ConnectionCreator::Create(const PUdp& param)
{
	return new Udp(param.HostName, param.PortSend, param.PortRecv);
}
*/

void ConnectionCreator::Release(IConnection* pConnection)
{
	if (pConnection == &SciNull) return; // deleteしない

	if (pConnection) delete pConnection;
}