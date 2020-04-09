#include "stdafx.h"
#include "SCIDeviceBase.h"

//*******************************************************************************
//	�\�z�E�j�����̏���
//*******************************************************************************
CSciDeviceBase::CSciDeviceBase()
{
	m_pSci = NULL;
}

CSciDeviceBase::~CSciDeviceBase()
{
	if( m_pSci ) delete m_pSci;
}

//*******************************************************************************
//	�N���X�̏��������s���C�w�肵���V���A���|�[�g�֐ڑ�����
//
// Parameter:
//	sciport - �ڑ������� COM �|�[�g���w��
//	          ���ɐڑ��ς݂ŁC�ڑ����̃|�[�g�ƈ����������������ꍇ :
//	              �����̐ڑ�����Ă���C�V�����ڑ������݂�
//                �ڑ��Ɏ��s�����ꍇ�́C�ڑ��悪������ԂɂȂ�
//	          ���ɐڑ��ς݂ŁC�ڑ����̃|�[�g�ƈ������قȂ�ꍇ :
//	              �ڑ��ɐ��������ꍇ�́C�V�����ڑ��ɐ؂�ւ��
//	              �ڑ��Ɏ��s�����ꍇ�́C�Â��ڑ����ێ�����
//
//	br      - �ʐM���x���w�肷��
//*******************************************************************************
CRESULT CSciDeviceBase::InitializeSci(SCIPORT sciport, BAUDRATE br, BYTE Parity, BYTE StopBits)
{
	if( sciport == GetCurrentSciPort() ) {
		// �����V���A���|�[�g���w�肵�Ă����Ƃ�
		
		delete m_pSci; // m_pSci �̓C���X�^���X�����ς�
		m_pSci = NULL;
	}

	CSciBase* pSci_tmp = new CSciBase;
	
	if( ISOK(pSci_tmp->Initialize(sciport, br, Parity, StopBits)) ){
		// �������ɐ���

		if( m_pSci ) delete m_pSci; // �C���X�^���X�����݂���ꍇ�͊J��

		m_pSci = pSci_tmp;
		return C_OK;
	} else {
		// �������Ɏ��s

		delete pSci_tmp;

		return C_FAIL;
	}
}

//*******************************************************************************
//	COM �|�[�g�����
//*******************************************************************************
CRESULT CSciDeviceBase::CloseSci()
{
	if( m_pSci ) {
		//m_pSci->CloseComPort(); 
		delete m_pSci; 
		m_pSci = NULL;  
		return C_OK;
	} else {
		return C_OK;
	}
}

//*******************************************************************************
//	���݂� COM �|�[�g���L�������ׂ�
//*******************************************************************************
BOOL CSciDeviceBase::IsPortEnable() const
{
	if( m_pSci ) {
		return m_pSci->IsPortEnable();
	} else {
		return FALSE;
	}
}

//*******************************************************************************
//	���݂� COM �|�[�g�ԍ����擾
//*******************************************************************************
SCIPORT CSciDeviceBase::GetCurrentSciPort()
{
	if( m_pSci ) {
		return m_pSci->GetCurrentSciPort();
	} else {
		return PORTNO_NULL;
	}
}

//*******************************************************************************
//	���݂̃{�[���[�g���擾
//*******************************************************************************
BAUDRATE CSciDeviceBase::GetCurrentBaudrate()
{
	if( m_pSci ) {
		return m_pSci->GetCurrentBaudrate();
	} else {
		return (BAUDRATE)0;
	}
}

//*******************************************************************************
//	�]�������r�b�g�����擾
//*******************************************************************************
UINT CSciDeviceBase::GetTransmitBits()
{
	if( m_pSci ) {
		return m_pSci->GetTransmitBytes() * CSciBase::SCIBITS_PER_BYTE;
	} else {
		return 0;
	}
}

//*******************************************************************************
//	�]�������r�b�g�����N���A
//*******************************************************************************
void CSciDeviceBase::ClearTransmitBits()
{
	if( m_pSci ) {
		m_pSci->ClearTransmitBytes();
	}
}

//*******************************************************************************
//	��M�����r�b�g�����擾
//*******************************************************************************
UINT CSciDeviceBase::GetReceiveBits()
{
	if( m_pSci ) {
		return m_pSci->GetReceiveBytes() * CSciBase::SCIBITS_PER_BYTE;
	} else {
		return 0;
	}
}

//*******************************************************************************
//	��M�����r�b�g�����N���A
//*******************************************************************************
void CSciDeviceBase::ClearReceiveBits()
{
	if( m_pSci ) {
		m_pSci->ClearReceiveBytes();
	}
}