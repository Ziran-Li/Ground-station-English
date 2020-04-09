///
/// @file       Protocol.h
///
/// @brief      �v���g�R���N���X�̊�{��`
///
///             �v���g�R���N���X�̖��
///             ���M�F
///                 �y�C���[�h�ɑ΂��A�w�b�_�ƃt�b�^��t�����Ė����f�o�C�X�I�u�W�F�N�g�Ƀf�[�^��n��
///             ��M�F
///                 �����f�o�C�X�I�u�W�F�N�g����擾�����f�[�^����p�P�b�g�����o���A�y�C���[�h���𒊏o����
///                 ���o�����f�[�^��Communicator�I�u�W�F�N�g�ɓn�����
///
/// @author     iwakura
///

#ifndef INCLUDED_PROTOCOL_H
#define INCLUDED_PROTOCOL_H

#include "Common/StandardDef.h"

class WirelessDevice;
class ICommunicator;

///
/// @brief  �v���g�R����{�N���X
///
class Protocol
{
protected:
	WirelessDevice& m_WirelessDevice; //!< �����f�o�C�X�N���X�ւ̎Q��
	ICommunicator&  m_Communicator;   //!< �y�C���[�h�����E�W�J�N���X�ւ̎Q��

public:

	///
	/// @brief  �R���X�g���N�^
	///
	/// @param[in] device        �����f�o�C�X�N���X�ւ̎Q��
	/// @param[in] communicator  �y�C���[�h�����E�W�J�N���X�ւ̎Q��
	///
	Protocol(WirelessDevice& device,
	         ICommunicator&  communicator) : 
			 m_WirelessDevice(device),
			 m_Communicator(communicator) {}

	///
	/// @brief  ������
	///
	/// @return  C_OK - ���� / C_FAIL - ���s
	///
	virtual CRESULT Initialize() = 0;

	///
	/// @brief  �f�[�^�̑��M
	///         �h���N���X�ł͎󂯎�����y�C���[�h���̃f�[�^���v���g�R�����Ƃ̃t�H�[�}�b�g�Ńp�P�b�g������R�[�h�����������
	///
	/// @param[in] ID      �p�P�b�gID
	/// @param[in] pBuff   �f�[�^�擪�̃|�C���^
	/// @param[in] length  ���M�f�[�^�̒���
	///
	/// @return  C_OK - ���� / C_FAIL - ���s
	///
	virtual CRESULT Send(unsigned int ID, const void* pBuff, size_t length) = 0;
	
	///
	/// @brief  �f�[�^�̎�M
	///         �h���N���X�ł̓p�P�b�g�̒��o���s���AICommunicator::Decode ���Ă�Ńy�C���[�h����W�J������R�[�h�����������
	///
	/// @return  C_OK - ���� / C_FAIL - ���s
	///
	virtual CRESULT Receive() = 0;
	
	///
	/// @brief  �p�P�b�g�̍ŏ��T�C�Y�iID���܂ރw�b�_�̒����{�t�b�^�̒����j��Ԃ�
	///
	/// @return  �p�P�b�g�̍ŏ��T�C�Y
	///
	virtual size_t  GetRxBufferMinimumRequirement() const = 0;
};

#endif