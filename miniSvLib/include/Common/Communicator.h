///
/// @file       Communicator.h
///
/// @brief      �y�C���[�h�����E�W�J�N���X�̊�{��`
///
///             Communicator�N���X�̖��
///             ���M�F
///                 ���C���N���X��������擾���A�y�C���[�h���쐬���ăv���g�R���I�u�W�F�N�g�̑��M�֐��ɓn��
///             ��M�F
///                 �v���g�R���I�u�W�F�N�g����y�C���[�h�����擾���A�f�R�[�h���ă��C���N���X�ɓn��
///
/// @author     iwakura
///

#ifndef INCLUDED_COMMUNICATOR_H
#define INCLUDED_COMMUNICATOR_H

#include "WirelessDevice.h"

class ICommunicator
{
protected:
	WirelessDevice& m_WirelessDevice;

public:
	///
	/// @brief  �R���X�g���N�^
	///
	/// @param[in] rWirelessDevice  �����f�o�C�X�N���X�ւ̎Q��
	///
	ICommunicator(WirelessDevice& rWirelessDevice) : m_WirelessDevice(rWirelessDevice) {}

	virtual CRESULT Initialize() = 0;
	virtual bool Startup() = 0;

	virtual CRESULT Receive() = 0;	
	virtual CRESULT Send() = 0;
	
	virtual CRESULT Decode(unsigned int ID, const void* pBuff, size_t BufferLength) { return C_OK; }
	
	virtual size_t GetPayloadLengthByID(unsigned int ID) const {return 0;}
	
	virtual CRESULT GetReceiveBuffer(void** BufferPtr, size_t* pBufferLength) = 0;
};

#endif