///
/// @file       IConnection.h
///
/// @brief      1��1�ʐM���ʃC���^�[�t�F�[�X��`
///
/// @author     iwakura
///

#ifndef INCLUDED_ICONNECTION_H
#define INCLUDED_ICONNECTION_H

#include "Common/StandardDef.h"

//*******************************************************************************
/// @brief  1��1�ʐM���ʃC���^�[�t�F�[�X
//*******************************************************************************
class IConnection
{
public:
	virtual ~IConnection() {}

	// 
	virtual CRESULT Open() = 0;
	virtual void    Close() = 0;

	// �ʐM���ł����Ԃ��ۂ����ׂ�
	virtual bool    IsEnable() const = 0;

	// ��M�`�F�b�N
	virtual bool    IsReceiveData() = 0;

	// ���M�\�`�F�b�N
	virtual _UDWORD IsTransmittable(void) = 0;

	// ���o�͊֐�
	virtual	CRESULT WriteElement(_UBYTE chr) = 0;                   // 1Byte���M
	virtual	CRESULT ReadElement(_UBYTE* ptr) = 0;                   // 1Byte��M

	virtual	CRESULT WriteString(const _UBYTE* ptr, _UWORD len) = 0; // �A���f�[�^���M

	// ���v���
	virtual _UDWORD GetTxCount() const = 0;
	virtual _UDWORD GetRxCount() const = 0;
};

#endif