//
//		SCI.h
//
//		Windows�p�V���A���ʐM�N���X
//

#ifndef INCLUDED_SCI_WIN_H
#define INCLUDED_SCI_WIN_H

#include <windows.h>
#include "Common/ISci.h"

#define PORTNO_NULL  -1

class CSciBase : public ISci
{
public:
	typedef void*         HANDLE;
	typedef unsigned long DWORD;

	enum 
	{
		RXBUFFSIZE = 1024,
	};

private:
	HANDLE  m_hComm;      // COM�|�[�g�̃n���h��
	DCB     m_CurrentDCB;
	DCB     m_DefaultDCB; // ���̐ݒ�̃o�b�N�A�b�v
	int     m_Port;
	_UDWORD m_Baudrate;
	_UBYTE  m_Parity;
	_UBYTE  m_StopBits;
	
	struct 
	{
		UINT nTransmitBytes;
	} m_TxInfo;

	struct 
	{
		unsigned char Data[RXBUFFSIZE];
		int           wp;
		int           rp;
	} m_Rx;

	struct 
	{
		UINT nReceiveBytes;
	} m_RxInfo;

	void          _RxAdd(unsigned char var); // ��M�o�b�t�@�Ƀf�[�^��ǉ��i�f�o�b�O�p�j
	unsigned char _RxPop();

public:
	CSciBase(_UBYTE PortNo);
	virtual ~CSciBase();

	virtual int     GetPortNumber() const;

	virtual CRESULT SetBaudrate(_UDWORD Baudrate);
	virtual CRESULT SetParity(PARITY Parity);
	virtual CRESULT SetStopBits(STOPBIT StopBits);

	virtual CRESULT Open();
	virtual void    Close();

	virtual bool    IsEnable() const;

	// �f�[�^���M�֐�
	virtual CRESULT WriteElement(_UBYTE chr);                   // 1Byte���M
	virtual CRESULT WriteString(const _UBYTE* ptr, _UWORD len); // �A���f�[�^���M

	// �f�[�^��M�֐�
	virtual bool    IsReceiveData();                            // �f�[�^����M���Ă��邩
	virtual CRESULT ReadElement(_UBYTE* ptr);                   // 1Byte��M

	virtual _UDWORD IsTransmittable(void);						// ���M�\�`�F�b�N

	// ���v���
	virtual _UDWORD GetTxCount() const;
	virtual _UDWORD GetRxCount() const;
	virtual _UDWORD GetParityErrors() const { return 0; }
	virtual _UDWORD GetFramingErrors() const { return 0; }
	virtual _UDWORD GetOverrunErrors() const { return 0; }

protected:
	// Windows����p
	CRESULT _OpenComPort(_UBYTE PortNo, _UDWORD Baudrate, _UBYTE Parity, _UBYTE StopBits);
	CRESULT _Update();

	CRESULT _CloseComPort(); // �ڑ������
};

#endif