///
/// @file       IUdp.h
///
/// @brief      UDP�ʐM�C���^�[�t�F�[�X��`
///
/// @author     iwakura
///

#pragma once

#include "Common/IConnection.h"

class IUdp : public IConnection
{
public:
	virtual CRESULT ReadString(void* ptr, _UWORD len) = 0; // �A���f�[�^���M(UDP��p)
};
