#include "SCI_Win.h"
#include <string>

using namespace std;

string _GetComName(unsigned char PortNo);

CSciBase::CSciBase(_UBYTE PortNo)
{
	m_hComm = NULL;
	ZeroMemory(&m_CurrentDCB, sizeof(m_CurrentDCB));
	ZeroMemory(&m_DefaultDCB, sizeof(m_DefaultDCB));
	m_Port     = PortNo;
	m_Baudrate = CBR_9600;
	m_Parity   = SCI_PARITY_NONE;
	m_StopBits = SCI_STOPBIT_ONE;

	m_TxInfo.nTransmitBytes = 0;
	m_RxInfo.nReceiveBytes  = 0;

	m_Rx.wp = m_Rx.rp = 0;
}

CSciBase::~CSciBase()
{
	// 開いているCOMポートを閉じる
	_CloseComPort();
}

int CSciBase::GetPortNumber() const
{
	return m_Port;
}

CRESULT CSciBase::SetBaudrate(_UDWORD Baudrate)
{
	if (m_hComm) return C_FAIL;

	m_Baudrate = Baudrate;

	return C_OK;
}

CRESULT CSciBase::SetParity(PARITY Parity)
{
	if (m_hComm) return C_FAIL;

	m_Parity = Parity;

	return C_OK;
}

CRESULT CSciBase::SetStopBits(STOPBIT StopBits)
{
	if (m_hComm) return C_FAIL;

	m_StopBits = StopBits;

	return C_OK;
}

CRESULT CSciBase::Open()
{
	CRESULT res = _CloseComPort(); // 開いているComポートを閉じる

	if (ISOK(res)) 
	{
		return _OpenComPort(m_Port, m_Baudrate, m_Parity, m_StopBits);
	}
	else 
	{
		return C_FAIL;
	}
}

void CSciBase::Close()
{
	_CloseComPort();
}

bool CSciBase::IsEnable() const
{
	if (m_hComm) 
	{
		DCB dcbDummy;
		return GetCommState(m_hComm, &dcbDummy) ? true : false;
	}
	else 
	{
		return FALSE;
	}
}

bool CSciBase::IsReceiveData()
{
	_Update();
	return m_Rx.wp != m_Rx.rp;
}

//*******************************************************************************
//	1バイト送信する
//*******************************************************************************
CRESULT CSciBase::WriteElement(_UBYTE chr)
{
	if (m_hComm == NULL) return C_FAIL;

	DWORD nWritten;

	BOOL ret = WriteFile(m_hComm, &chr, 1, &nWritten, NULL);

	m_TxInfo.nTransmitBytes += nWritten;

	if (ret && (nWritten == 1)) {
		return C_OK;
	}
	else {
		return C_FAIL;
	}
}

//*******************************************************************************
//	連続したデータを送信する
//*******************************************************************************
CRESULT CSciBase::WriteString(const _UBYTE* ptr, _UWORD len)
{
	if( m_hComm == NULL ) return C_FAIL;

	DWORD nWritten;

	BOOL ret = WriteFile(m_hComm, ptr, len, &nWritten, NULL);

	m_TxInfo.nTransmitBytes += nWritten;

	if( ret && (nWritten == len) ) {
		return C_OK;
	} else {
		return C_FAIL;
	}
}

CRESULT CSciBase::_Update()
{
	DWORD   dwErrors;
	COMSTAT ComStat;
	DWORD   nToRead;
	DWORD   nReceived;

	if( !m_hComm ) return C_FAIL;

	// 受信済みバイト数を取得
	if( !ClearCommError(m_hComm, &dwErrors, &ComStat) ) return C_FAIL;

	nToRead = ComStat.cbInQue;
	if( nToRead == 0 ) return C_OK;
	
	// バッファを生成
	unsigned char* Buff = new unsigned char[nToRead];
	
	// データ読み込み
	if( !ReadFile(m_hComm, Buff, nToRead, &nReceived, NULL) ) {
		delete[] Buff;
		return C_FAIL;
	}
	m_RxInfo.nReceiveBytes += nReceived;

	// データを格納
	if( nReceived >= RXBUFFSIZE ) {
		CopyMemory(m_Rx.Data, Buff + nReceived - RXBUFFSIZE, RXBUFFSIZE);
		m_Rx.rp = 1;
		m_Rx.wp = 0;
	} else {
		DWORD nRxDataCount = (m_Rx.wp - m_Rx.rp + RXBUFFSIZE) % RXBUFFSIZE;

		DWORD nWpToEnd = RXBUFFSIZE - m_Rx.wp;
		if( nWpToEnd > nReceived ) {
			CopyMemory(m_Rx.Data + m_Rx.wp, Buff, nReceived);
			m_Rx.wp += nReceived;
		} else {
			CopyMemory(m_Rx.Data + m_Rx.wp, Buff, nWpToEnd);
			CopyMemory(m_Rx.Data, Buff + nWpToEnd, nReceived - nWpToEnd);
			m_Rx.wp = nReceived - nWpToEnd;
		}

		// データを幾つか上書きした
		if( nRxDataCount + nReceived > RXBUFFSIZE ) {
			m_Rx.rp = (m_Rx.wp + 1) % RXBUFFSIZE;
		}
	}

	delete[] Buff;

	return C_OK;
}

CRESULT CSciBase::ReadElement(_UBYTE* ptr)
{
	if( IsReceiveData() ) {
		*(ptr) = _RxPop();
		return C_OK;
	} else {
		return C_FAIL;
	}
}

//*******************************************************************************
/// @brief  送信可能チェック
///
/// @param[in] chr  送信したいデータ
/// @return バッファの空きサイズ
//*******************************************************************************
_UDWORD CSciBase::IsTransmittable(void)
{
	// デバイス側のFIFOに空きがある。且つ、送信バッファにデータがある場合
	// POP * 送信データ
	// マスターライト実施
	// デバイス側FIFOの空きをチェック
	// 送信バッファの空きをチェック

	// 戻り値で送信バッファの空きを返す予定
	return 0;
}

//*******************************************************************************
//	転送したバイト数を取得
//*******************************************************************************
_UDWORD CSciBase::GetTxCount() const
{
	return m_TxInfo.nTransmitBytes;
}

//*******************************************************************************
//	受信したバイト数を取得
//*******************************************************************************
_UDWORD CSciBase::GetRxCount() const
{
	return m_RxInfo.nReceiveBytes;
}

void CSciBase::_RxAdd(unsigned char var)
{
	m_Rx.Data[m_Rx.wp++] = var;
	m_Rx.wp %= RXBUFFSIZE;
	if( m_Rx.wp == m_Rx.rp ) {
		m_Rx.rp = ++m_Rx.rp % RXBUFFSIZE;
	}
}

unsigned char CSciBase::_RxPop()
{
	unsigned char ret;

	if( m_Rx.rp != m_Rx.wp ) {
		ret = m_Rx.Data[m_Rx.rp++];
		m_Rx.rp %= RXBUFFSIZE;

	} else {
		ret = 0; // 通常は呼ばれない
	}

	return ret;
}

CRESULT CSciBase::_OpenComPort(_UBYTE PortNo, _UDWORD Baudrate, _UBYTE Parity, _UBYTE StopBits)
{
	CRESULT res = C_OK;

	HANDLE hNewComm = NULL;
	DCB new_dcb, old_dcb;

	if( m_hComm ) res = C_FAIL; // COMポートオープン済
	
	// COMポートを開く
	if (ISOK(res))
	{
		hNewComm = CreateFileA(_GetComName(PortNo).c_str(), //ポート名
			GENERIC_READ | GENERIC_WRITE,                   //アクセスモード
			0,                                              //非共有
			NULL,
			OPEN_EXISTING,                                  //COMポートにアクセスする時はこれに設定
			FILE_ATTRIBUTE_NORMAL,                          //ファイル属性無し
			NULL);                                          //テンプレートファイル無し

		if( hNewComm == INVALID_HANDLE_VALUE ) res = C_FAIL;
	}

	if (ISOK(res))
	{
		// COMポートの状態を取得する
		if (!GetCommState(hNewComm, &old_dcb)) {
			CloseHandle(hNewComm);
			res = C_FAIL;
		}
	}

	if (ISOK(res))
	{
		// COMポートの設定
		new_dcb = old_dcb;
		new_dcb.BaudRate = Baudrate;
		new_dcb.ByteSize = 8;
		new_dcb.Parity = Parity;
		new_dcb.StopBits = StopBits;
		if (!SetCommState(hNewComm, &new_dcb)) {
			CloseHandle(hNewComm);
			res = C_FAIL;
		}
	}

	if (ISOK(res))
	{
		// バッファの初期化
		if (!PurgeComm(hNewComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR)) {
			SetCommState(hNewComm, &old_dcb);
			CloseHandle(hNewComm);
			res = C_FAIL;
		}
	}

	if (ISOK(res))
	{
		// デバイスのエラーフラグをクリア
		DWORD errors;
		if (!ClearCommError(hNewComm, &errors, NULL)) {
			SetCommState(hNewComm, &old_dcb);
			CloseHandle(hNewComm);
			res = C_FAIL;
		}
	}

	if (ISOK(res))
	{
		// 成功
		m_hComm = hNewComm;
		m_CurrentDCB = new_dcb;
		m_DefaultDCB = old_dcb;
		m_TxInfo.nTransmitBytes = 0;
		m_RxInfo.nReceiveBytes = 0;
		m_Rx.wp = m_Rx.rp = 0;

		debug_printf("COM%d Open Succeeded\n", PortNo);
	}
	else
	{
		// 失敗
		debug_printf("COM%d Open Failed\n", PortNo);
	}

	return C_OK;
}

CRESULT CSciBase::_CloseComPort()
{
	if( m_hComm == NULL ) return C_OK;

	SetCommState(m_hComm, &m_DefaultDCB);

	if( CloseHandle(m_hComm) ) {
		m_hComm = NULL;
		return C_OK;
	} else {
		SetCommState(m_hComm, &m_CurrentDCB);
		return C_FAIL;
	}
}



string _GetComName(unsigned char PortNo)
{
	char temp[12];

#if _MSC_VER >= 1400
	sprintf_s(temp, sizeof(temp), "\\\\.\\COM%d", PortNo);
#else
	_snprintf(temp, sizeof(temp), "\\\\.\\COM%d", PortNo);
#endif
	return temp;
};
