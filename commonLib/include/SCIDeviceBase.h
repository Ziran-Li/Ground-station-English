//
//	    SCIDeviceBase.h
//
//		RS232(EIA232)�f�o�C�X�p�R�~���j�P�[�V�����N���X�̊�{�N���X
//
//		'08/10/23	Ver 0.01	�b���                              by iwakura
//		'08/11/26	Ver 0.02	IsInit() �ǉ�                       by iwakura
//		'09/12/21	Ver 0.02.1	include�t�@�C�����̕ύX�ɔ������C�� by iwakura
//
//		CSci�̎����̍��ق��z�����C�p����̃N���X�ɋ��ʂ̋@�\��񋟂���
//
//		�݌v�������c
//		�ڑ��|�[�g�������F������̂��x�X�g�H
//
//		
//

#ifndef __SCIDEVICE_BASE_H__
#define __SCIDEVICE_BASE_H__

#include "SCI.h"
#include "common/StandardDef.h"

class CSciDeviceBase
{
protected:
	// �V���A���ʐM�N���X�̃|�C���^(InitializeSci�Ŏ����ݒ�)
	CSciBase* m_pSci;
	
	// �V���A���ʐM�N���X�̏�����(�K���������ŌĂяo������)
	CRESULT InitializeSci(SCIPORT sciport, BAUDRATE br, BYTE Parity = NOPARITY, BYTE StopBits = ONESTOPBIT);
	
public:
	// �\�z�E����
	CSciDeviceBase();
	virtual ~CSciDeviceBase();
	
	// �����������֐�
	//virtual CRESULT Initialize(SCIPORT sciport) = 0;
	
	// �ؒf
	CRESULT CloseSci();

	// �e���Ԏ擾
	bool     IsInit() { return m_pSci != NULL; } // ���������ꂽ���ǂ���
	BOOL     IsPortEnable() const; // ���݂� COM �|�[�g���L�����ǂ���
	SCIPORT  GetCurrentSciPort();  // ���݂� COM �|�[�g�ԍ����擾
	BAUDRATE GetCurrentBaudrate(); // ���݂̃{�[���[�g���擾
	UINT     GetTransmitBits();    // ���݂̓]���r�b�g�����擾
	void     ClearTransmitBits();
	UINT     GetReceiveBits();
	void     ClearReceiveBits();
};

#endif