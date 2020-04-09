///
/// @file       Protocol_Legacy.h
///
/// @brief      ���~�j�T�[�x�C���[�v���g�R��
///
/// @author     iwakura
///

#ifndef INCLUDED_PROTOCOL_LEGACY_H
#define INCLUDED_PROTOCOL_LEGACY_H

#include "Protocol.h"

///
/// @brief  ���~�j�T�[�x�C���[�V�X�e�������ʐM�v���g�R���N���X
///
class Protocol_Legacy : public Protocol
{
	enum
	{
		PREAMBLE1 = 0xFF,            //!< �����o�C�g1
		PREAMBLE2 = 0xFF,            //!< �����o�C�g2
		PREAMBLE_LENGTH = 2,         //!< �v���A���u���̒���
		HEADER_LENGTH = 3,           //!< �v���A���u��+ID�̒���
		FOOTER_LENGTH = 2,	         //!< �`�F�b�N�T���̒���
		OFFSET_ID = PREAMBLE_LENGTH, //!< ID�̈ʒu
	};

	struct //!< �p�P�b�g�̃f�R�[�h�ɗp����ꎞ�̈�
	{
		unsigned char  BufferLength;        //!< �o�b�t�@��
		unsigned char* pBuff;               //!< �o�b�t�@
		unsigned char  DataCount;           //!< ��M�o�C�g��
		unsigned char  CurrentPacketLength; //!< ��M���̃p�P�b�g�̒���
	} m_Rx;
	
public:
	
	///
	/// @brief  �R���X�g���N�^
	///
	/// @param[in] device        �����f�o�C�X�N���X�ւ̎Q��
	/// @param[in] communicator  �p�P�b�g�����E�W�J�N���X�ւ̎Q��
	///
	Protocol_Legacy(WirelessDevice& device, ICommunicator& communicator);
	
	///
	/// @brief  ������
	///
	/// @return  C_OK - ���� / C_FAIL - ���s
	///
	virtual CRESULT Initialize();
	
	///
	/// @brief  �f�[�^�̑��M
	///         �󂯎�����f�[�^�����~�j�T�[�x�C���[�V�X�e���̃t�H�[�}�b�g�ő��M����
	///
	/// @param[in] ID      �p�P�b�gID
	/// @param[in] pBuff   �f�[�^�擪�̃|�C���^
	/// @param[in] length  ���M�f�[�^�̒���
	///
	/// @return  C_OK - ���� / C_FAIL - ���s
	///
	virtual CRESULT Send(unsigned int ID, const void* pBuff, size_t length);
	
	///
	/// @brief   �f�[�^�̎�M
	///          ���~�j�T�[�x�C���[�V�X�e���̃t�H�[�}�b�g�ɍ��v����p�P�b�g�̒��o���s���A�f�R�[�h�֐�(ICommunicator::Decode)���Ă�
	///
	/// @return  C_OK - ���� / C_FAIL - ���s
	///
	virtual CRESULT Receive();

	///
	/// @brief   �p�P�b�g�̍ŏ��T�C�Y�iID���܂ރw�b�_�̒����{�t�b�^�̒����j��Ԃ�
	///
	/// @return  �p�P�b�g�̍ŏ��T�C�Y
	///
	virtual size_t  GetRxBufferMinimumRequirement() const;

private:
	void   _ClearRxBuffer();        //!< �f�R�[�h��Ɏ��̃p�P�b�g����M�ł���悤�Ƀo�b�t�@���N���A����
	void   _EraseJunkData();        //!< �j�����ꂽ�p�P�b�g���疳���������폜����
	bool   _VerifyChecksum() const; //!< ��M�f�[�^�̃`�F�b�N�T�������؂���
	void   _ClearMemory();          //!< �I�u�W�F�N�g�̏�Ԃ�������Ԃɖ߂�
	
	static _UWORD _CalculateChecksum(unsigned char ID, const void* pData, size_t length); //!< �`�F�b�N�T�����v�Z����ÓI�֐�
};



#endif