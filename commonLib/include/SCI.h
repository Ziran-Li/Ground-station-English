//
//		SCI.h
//
//		Windows�p�V���A���ʐM�N���X
//
//		'09/01/25	Ver 0.01	�v���O���~���O�J�n                  by iwakura
//		'09/02/15	Ver 1.00	�ЂƂ܂�����                        by iwakura
//		'09/09/11	Ver 1.01	��M�o�C�g�����擾�\�ɂ���        by iwakura
//		'09/12/21	Ver 1.01.1	include�t�@�C�����̕ύX�ɔ������C�� by iwakura
//		'11/06/16	Ver 1.02	�|�[�g����֐��� Public �ɕύX      by iwakura
//								�ڑ����ɂ������̕ϐ������Z�b�g����Ă��Ȃ����������C��
//

#ifndef __SCI_H__
#define __SCI_H__

#include <windows.h>
#include "common/StandardDef.h"

enum SCIPORT {
	PORTNO_NULL = -1,  // ���݂��Ȃ��|�[�g�ԍ�
	PORTNO_MIN  = 0,   // �|�[�g�ԍ��̉���
	PORTNO_MAX  = 256, // �|�[�g�ԍ��̏��
	SCI0 = 0, SCI1 = 1, SCI2 = 2, SCI3 = 3, SCI4 = 4,
	SCI5 = 5, SCI6 = 6, SCI7 = 7, SCI8 = 8, SCI9 = 9,
};

enum BAUDRATE { // �ݒ�\�ȃ{�[���[�g
	BR_110    = CBR_110,    BR_300    = CBR_300,    BR_600    = CBR_600,   
	BR_1200   = CBR_1200,   BR_2400   = CBR_2400,   BR_4800   = CBR_4800,
	BR_9600   = CBR_9600,   BR_14400  = CBR_14400,  BR_19200  = CBR_19200,
	BR_38400  = CBR_38400,  BR_56000  = CBR_56000,  BR_57600  = CBR_57600,        
	BR_115200 = CBR_115200, BR_128000 = CBR_128000, BR_256000 = CBR_256000    
};

class CSciBase
{
public:
	typedef void*         HANDLE;
	typedef unsigned long DWORD;

	enum {
		RXBUFFSIZE = 1024,
		SCIBITS_PER_BYTE = 10, // STB, DATAB*8, EDB
	};

private:
	HANDLE   m_hComm;      // COM�|�[�g�̃n���h��
	DCB      m_CurrentDCB;
	DCB      m_DefaultDCB; // ���̐ݒ�̃o�b�N�A�b�v
	SCIPORT  m_CurrentSciPort;
	
	struct {
		UINT nTransmitBytes;
		
	} m_TxInfo;

	struct {
		unsigned char Data[RXBUFFSIZE];
		int           wp;
		int           rp;
	} m_Rx;

	struct {
		UINT nReceiveBytes;
	} m_RxInfo;

	void          _RxAdd(unsigned char var); // ��M�o�b�t�@�Ƀf�[�^��ǉ��i�f�o�b�O�p�j
	unsigned char _RxPop();

public:
	CSciBase();
	virtual ~CSciBase();

	// �������i������COM�|�[�g���I�[�v���j
	virtual CRESULT Initialize(SCIPORT sciport, BAUDRATE br, BYTE Parity = NOPARITY, BYTE StopBits = ONESTOPBIT);

	// �f�[�^���M�֐�
	CRESULT WriteString(unsigned char* str, unsigned short nBytes); // �A�������f�[�^�𑗐M����

	// �f�[�^��M�֐�
	bool    IsReceiveData() { _Update(); return m_Rx.wp != m_Rx.rp; } // �f�[�^����M���Ă��邩
	CRESULT ReadElement(unsigned char* pData);                        // �o�b�t�@���� 1 �����ǂݏo��


	// �e����擾
	BOOL     IsPortEnable() const; // �ʐM�\���ǂ������ׂ�
	SCIPORT  GetCurrentSciPort();  // ���݂� COM �|�[�g�ԍ����擾
	BAUDRATE GetCurrentBaudrate(); // ���݂̃{�[���[�g���擾
	UINT     GetTransmitBytes();   // ���M�����o�C�g�����擾
	void     ClearTransmitBytes(); // �N���A
	UINT     GetReceiveBytes();    // ��M�����o�C�g�����擾
	void     ClearReceiveBytes();  // �N���A

protected:
	// Windows����p
	CRESULT _OpenComPort(unsigned char PortNo, DWORD BaudRate, BYTE Parity = NOPARITY, BYTE StopBits = ONESTOPBIT);
	CRESULT _Update();

public:
	CRESULT CloseComPort(); // �ڑ������
};

#endif