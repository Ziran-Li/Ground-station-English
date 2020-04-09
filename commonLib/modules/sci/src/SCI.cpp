#include "stdafx.h"
//#include <windows.h>
#include <string>
#include "SCI.h"

using namespace std;

string _GetComName(unsigned char PortNo);

CSciBase::CSciBase()
{
	m_hComm = NULL;
	ZeroMemory(&m_CurrentDCB, sizeof(m_CurrentDCB));
	ZeroMemory(&m_DefaultDCB, sizeof(m_DefaultDCB));
	m_CurrentSciPort = PORTNO_NULL;

	m_TxInfo.nTransmitBytes = 0;
	m_RxInfo.nReceiveBytes  = 0;

	m_Rx.wp = m_Rx.rp = 0;
}

CSciBase::~CSciBase()
{
	// 关闭打开的com口
	CloseComPort();
}

CRESULT CSciBase::Initialize(SCIPORT sciport, BAUDRATE br, BYTE Parity, BYTE StopBits)
{
	CRESULT res = CloseComPort(); // 关闭端口
	
	if( ISOK(res) ) {
		return _OpenComPort(sciport, br, Parity, StopBits);
	} else {
		return C_FAIL;
	}
}

//*******************************************************************************
//	发送连续的数据
//*******************************************************************************
CRESULT CSciBase::WriteString(unsigned char* str, unsigned short nBytes)
{
	if( m_hComm == NULL ) return C_FAIL;

	DWORD nWritten;

	BOOL ret = WriteFile(m_hComm, str, nBytes, &nWritten, NULL);

	m_TxInfo.nTransmitBytes += nWritten;

	if( ret && (nWritten == nBytes) ) {
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

	// 已经接收的字节数
	if( !ClearCommError(m_hComm, &dwErrors, &ComStat) ) return C_FAIL;

	nToRead = ComStat.cbInQue;
	if( nToRead == 0 ) return C_OK;
	
	// 产生缓冲
	unsigned char* Buff = new unsigned char[nToRead];
	
	// 数据读取
	if( !ReadFile(m_hComm, Buff, nToRead, &nReceived, NULL) ) {
		delete[] Buff;
		return C_FAIL;
	}
	m_RxInfo.nReceiveBytes += nReceived;

	// 数据保存
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

		// 数据覆盖
		if( nRxDataCount + nReceived > RXBUFFSIZE ) {
			m_Rx.rp = (m_Rx.wp + 1) % RXBUFFSIZE;
		}
	}

	delete[] Buff;

	return C_OK;
}

CRESULT CSciBase::ReadElement(unsigned char* pData) 
{
	if( IsReceiveData() ) {
		*(pData) = _RxPop();
		return C_OK;
	} else {
		return C_FAIL;
	}
}

//*******************************************************************************
//	是否可通信
//*******************************************************************************
BOOL CSciBase::IsPortEnable() const
{
	if( m_hComm ) {
		DCB dcbDummy;
		return GetCommState(m_hComm, &dcbDummy);
	} else {
		return FALSE;
	}
}

//*******************************************************************************
//	取得目前的COM端口号
//*******************************************************************************
SCIPORT CSciBase::GetCurrentSciPort()
{
	if( m_hComm ) {
		return m_CurrentSciPort;
	} else {
		return PORTNO_NULL;
	}
}

//*******************************************************************************
//	取得目前的COM端口波特率
//*******************************************************************************
BAUDRATE CSciBase::GetCurrentBaudrate()
{
	if( m_hComm ) {
		return (BAUDRATE)m_CurrentDCB.BaudRate;
	} else {
		return (BAUDRATE)0;
	}
}

//*******************************************************************************
//	传输字节数
//*******************************************************************************
UINT CSciBase::GetTransmitBytes()
{
	return m_TxInfo.nTransmitBytes;
}

//*******************************************************************************
//	转送了的字节数的记录清除
//*******************************************************************************
void CSciBase::ClearTransmitBytes()
{
	m_TxInfo.nTransmitBytes = 0;
}

//*******************************************************************************
//	获取接收到的字节数
//*******************************************************************************
UINT CSciBase::GetReceiveBytes()
{
	return m_RxInfo.nReceiveBytes;
}

//*******************************************************************************
//	清除完成的字节数的记录
//*******************************************************************************
void CSciBase::ClearReceiveBytes()
{
	m_RxInfo.nReceiveBytes = 0;
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
		ret = 0; // 返回 0 
	}

	return ret;
}

CRESULT CSciBase::_OpenComPort(unsigned char PortNo, CSciBase::DWORD BaudRate, BYTE Parity, BYTE StopBits)
{
	DCB new_dcb, old_dcb;

	if( m_hComm ) return C_FAIL; // COM億乕僩僆乕僾儞嵪
	
	// COM億乕僩傪奐偔
	HANDLE hNewComm = CreateFileA(_GetComName(PortNo).c_str() , //億乕僩柤
	                              GENERIC_READ | GENERIC_WRITE, //傾僋僙僗儌乕僪
	                              0,                            //旕嫟桳
	                              NULL,
	                              OPEN_EXISTING,                //COM億乕僩偵傾僋僙僗偡傞帪偼偙傟偵愝掕
	                              FILE_ATTRIBUTE_NORMAL,        //僼傽僀儖懏惈柍偟
	                              NULL);                        //僥儞僾儗乕僩僼傽僀儖柍偟

	if( !hNewComm ) return C_FAIL;
	//if( hNewComm == INVALID_HANDLE_VALUE ) return C_FAIL;

	// COM億乕僩偺忬懺傪庢摼偡傞
	if( !GetCommState(hNewComm, &old_dcb) ) {
		CloseHandle(hNewComm);
		return C_FAIL;
	}

	// COM億乕僩偺愝掕
	new_dcb = old_dcb;
	new_dcb.BaudRate = BaudRate;
	new_dcb.ByteSize = 8;
	new_dcb.Parity   = Parity;
	new_dcb.StopBits = StopBits;
	if( !SetCommState(hNewComm, &new_dcb) ) {
		CloseHandle(hNewComm);
		return C_FAIL;
	}

	// 僶僢僼傽偺弶婜壔
	if( !PurgeComm(hNewComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) ) {
		SetCommState(hNewComm, &old_dcb);
		CloseHandle(hNewComm);
		return C_FAIL;
	}

	// 僨僶僀僗偺僄儔乕僼儔僌傪僋儕傾
	DWORD errors;
	if( !ClearCommError(hNewComm, &errors, NULL) ) {
		SetCommState(hNewComm, &old_dcb);
		CloseHandle(hNewComm);
		return C_FAIL;
	}

	// 惉岟
	m_hComm = hNewComm;
	m_CurrentDCB = new_dcb;
	m_DefaultDCB = old_dcb;
	m_CurrentSciPort = (SCIPORT)PortNo;
	m_TxInfo.nTransmitBytes = 0;
	m_RxInfo.nReceiveBytes  = 0;
	m_Rx.wp = m_Rx.rp = 0;

	return C_OK;
}

CRESULT CSciBase::CloseComPort()
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

/*
CSci0::CSci0()
{
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // 捠忢
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // 2楢懕僨乕僞
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // 3楢懕僨乕僞
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xCA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // 搑拞偑攋夡偝傟偨3楢懕僨乕僞
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // 1僶僀僩儘僗僩偟偨3楢懕僨乕僞
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // 0xFF偑梋寁側3楢懕僨乕僞
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x02,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // 枹搊榐僷働僢僩傪娷傓3楢懕僨乕僞
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x02,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // 枹搊榐 + 梋寁側0xFF

	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0xFF,0xFF,0x01}; // 晄惓側僨乕僞 + 懠偺僷働僢僩3僶僀僩
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0xFF,0xFF}; // 晄惓側僨乕僞 + 懠偺僷働僢僩2僶僀僩
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x01,0xFF}; // 晄惓側僨乕僞 + 懠偺僷働僢僩1僶僀僩

	for( int i = 0; i < sizeof(s); i++ ) {
		AddData(s[i]);
	}
}
*/

