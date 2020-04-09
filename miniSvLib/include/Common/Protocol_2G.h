///
/// @file       Protocol_2G.h
///
/// @brief      ��񐢑�v���g�R��
///
/// @author     iwakura
///

#ifndef INCLUDED_PROTOCOL_2G_H
#define INCLUDED_PROTOCOL_2G_H

#include "Protocol.h"

///
/// @brief  ��񐢑㖳���ʐM�v���g�R���N���X
///
class Protocol_2G : public Protocol
{
	enum
	{
		PREAMBLE1 = 0xA5,            //!< �����o�C�g1
		PREAMBLE2 = 0x5A,            //!< �����o�C�g2
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
	Protocol_2G(WirelessDevice& device, ICommunicator& communicator);
	
	///
	/// @brief  ������
	///
	/// @return  C_OK - ���� / C_FAIL - ���s
	///
	virtual CRESULT Initialize();
	
	///
	/// @brief  �f�[�^�̑��M
	///         �󂯎�����f�[�^���񐢑�V�X�e���̃t�H�[�}�b�g�ő��M����
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
	///          ��񐢑�V�X�e���̃t�H�[�}�b�g�ɍ��v����p�P�b�g�̒��o���s���A�f�R�[�h�֐�(ICommunicator::Decode)���Ă�
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
	void   _ClearRxBuffer();   //!< �f�R�[�h��Ɏ��̃p�P�b�g����M�ł���悤�Ƀo�b�t�@���N���A����
	void   _EraseJunkData();   //!< �j�����ꂽ�p�P�b�g���疳���������폜����
	bool   _VerifyCRC() const; //!< ��M�f�[�^�̃`�F�b�N�T�������؂���
	void   _ClearMemory();     //!< �I�u�W�F�N�g�̏�Ԃ�������Ԃɖ߂�

	size_t _GetPacketLengthByID(_UBYTE ID) const;

	static unsigned short _CRC16Value(unsigned char b);
	static unsigned short _CalculateBlockCRC16(unsigned char Preamble2, unsigned char ID, const void* pSrc, unsigned long count);
};



#endif