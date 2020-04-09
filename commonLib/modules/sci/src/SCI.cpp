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
	// �رմ򿪵�com��
	CloseComPort();
}

CRESULT CSciBase::Initialize(SCIPORT sciport, BAUDRATE br, BYTE Parity, BYTE StopBits)
{
	CRESULT res = CloseComPort(); // �رն˿�
	
	if( ISOK(res) ) {
		return _OpenComPort(sciport, br, Parity, StopBits);
	} else {
		return C_FAIL;
	}
}

//*******************************************************************************
//	��������������
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

	// �Ѿ����յ��ֽ���
	if( !ClearCommError(m_hComm, &dwErrors, &ComStat) ) return C_FAIL;

	nToRead = ComStat.cbInQue;
	if( nToRead == 0 ) return C_OK;
	
	// ��������
	unsigned char* Buff = new unsigned char[nToRead];
	
	// ���ݶ�ȡ
	if( !ReadFile(m_hComm, Buff, nToRead, &nReceived, NULL) ) {
		delete[] Buff;
		return C_FAIL;
	}
	m_RxInfo.nReceiveBytes += nReceived;

	// ���ݱ���
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

		// ���ݸ���
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
//	�Ƿ��ͨ��
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
//	ȡ��Ŀǰ��COM�˿ں�
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
//	ȡ��Ŀǰ��COM�˿ڲ�����
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
//	�����ֽ���
//*******************************************************************************
UINT CSciBase::GetTransmitBytes()
{
	return m_TxInfo.nTransmitBytes;
}

//*******************************************************************************
//	ת���˵��ֽ����ļ�¼���
//*******************************************************************************
void CSciBase::ClearTransmitBytes()
{
	m_TxInfo.nTransmitBytes = 0;
}

//*******************************************************************************
//	��ȡ���յ����ֽ���
//*******************************************************************************
UINT CSciBase::GetReceiveBytes()
{
	return m_RxInfo.nReceiveBytes;
}

//*******************************************************************************
//	�����ɵ��ֽ����ļ�¼
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
		ret = 0; // ���� 0 
	}

	return ret;
}

CRESULT CSciBase::_OpenComPort(unsigned char PortNo, CSciBase::DWORD BaudRate, BYTE Parity, BYTE StopBits)
{
	DCB new_dcb, old_dcb;

	if( m_hComm ) return C_FAIL; // COM�|�[�g�I�[�v����
	
	// COM�|�[�g���J��
	HANDLE hNewComm = CreateFileA(_GetComName(PortNo).c_str() , //�|�[�g��
	                              GENERIC_READ | GENERIC_WRITE, //�A�N�Z�X���[�h
	                              0,                            //�񋤗L
	                              NULL,
	                              OPEN_EXISTING,                //COM�|�[�g�ɃA�N�Z�X���鎞�͂���ɐݒ�
	                              FILE_ATTRIBUTE_NORMAL,        //�t�@�C����������
	                              NULL);                        //�e���v���[�g�t�@�C������

	if( !hNewComm ) return C_FAIL;
	//if( hNewComm == INVALID_HANDLE_VALUE ) return C_FAIL;

	// COM�|�[�g�̏�Ԃ��擾����
	if( !GetCommState(hNewComm, &old_dcb) ) {
		CloseHandle(hNewComm);
		return C_FAIL;
	}

	// COM�|�[�g�̐ݒ�
	new_dcb = old_dcb;
	new_dcb.BaudRate = BaudRate;
	new_dcb.ByteSize = 8;
	new_dcb.Parity   = Parity;
	new_dcb.StopBits = StopBits;
	if( !SetCommState(hNewComm, &new_dcb) ) {
		CloseHandle(hNewComm);
		return C_FAIL;
	}

	// �o�b�t�@�̏�����
	if( !PurgeComm(hNewComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) ) {
		SetCommState(hNewComm, &old_dcb);
		CloseHandle(hNewComm);
		return C_FAIL;
	}

	// �f�o�C�X�̃G���[�t���O���N���A
	DWORD errors;
	if( !ClearCommError(hNewComm, &errors, NULL) ) {
		SetCommState(hNewComm, &old_dcb);
		CloseHandle(hNewComm);
		return C_FAIL;
	}

	// ����
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
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // �ʏ�
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // 2�A���f�[�^
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // 3�A���f�[�^
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xCA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // �r�����j�󂳂ꂽ3�A���f�[�^
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // 1�o�C�g���X�g����3�A���f�[�^
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // 0xFF���]�v��3�A���f�[�^
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x02,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // ���o�^�p�P�b�g���܂�3�A���f�[�^
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0x02,0x11,0xAA,0x00,0xBC,0xFF,0xFF,0xFF,0x01,0x11,0xAA,0x00,0xBC}; // ���o�^ + �]�v��0xFF

	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0xFF,0xFF,0x01}; // �s���ȃf�[�^ + ���̃p�P�b�g3�o�C�g
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0xFF,0xFF}; // �s���ȃf�[�^ + ���̃p�P�b�g2�o�C�g
	//unsigned char s[] = {0xFF,0xFF,0x01,0x11,0xAA,0x01,0xFF}; // �s���ȃf�[�^ + ���̃p�P�b�g1�o�C�g

	for( int i = 0; i < sizeof(s); i++ ) {
		AddData(s[i]);
	}
}
*/

