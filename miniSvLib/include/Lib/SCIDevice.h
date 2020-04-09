///
/// @file       SCIDevice.h
///
/// @brief      �V���A���ʐM���g���f�o�C�X�̊�{��`
///
/// @author     iwakura
///
/// @note       �ˑ��x�������邽�߂��鎞�_�Ńt�@�N�g���N���X�ɒu��������\��
///

#ifndef INCLUDED_SCIDEVICE_BASE_H
#define INCLUDED_SCIDEVICE_BASE_H

#include "Common/StandardDef.h"
#include "Common/ISci.h"

enum BAUDRATE 
{
	BR_4800   = 4800,
	BR_9600   = 9600,
	BR_19200  = 19200,
	BR_38400  = 38400,
	BR_57600  = 57600,
	BR_62500  = 62500,
	BR_115200 = 115200,
	BR_125000 = 125000,
};

//*******************************************************************************
/// @brief  �V���A���ʐM���g���f�o�C�X�̊�{�N���X
//*******************************************************************************
class SciDevice
{
protected:
	// �V���A���ʐM�N���X�̃|�C���^(InitializeSci�Ŏ����ݒ�)
	ISci* m_pSci;

public:
	SciDevice();
	virtual ~SciDevice();

protected:
	CRESULT OpenSci(_UBYTE Port, _UDWORD Baudrate);
	CRESULT OpenSci(_UBYTE Port, _UDWORD Baudrate, PARITY Parity, STOPBIT StopBits);
};





#endif