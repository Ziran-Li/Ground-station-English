///
///
/// @file       SBusData.h
///
/// @brief      S.Bus�f�[�^�\����`
///
/// Lower-Level MCU (C), Higher-Level MCU (C++), GCS (C++) ����
///
/// �v���|�f�[�^�̕W���\���̒�`
/// �l�̃X�P�[���͎擾���̃f�o�C�X�Ɉˑ����Ă悢
/// �������C�f�o�C�X�ˑ��̃X�P�[���̓J�v�Z�������ꂽ���W���[�����݂̂ɂƂǂ߁C
/// ���W���[�����m�̃f�[�^�����͕K��RC�X�P�[���ōs������
///
///

#ifndef INCLUDED_SBUSDATA_H
#define INCLUDED_SBUSDATA_H

#include "Common/StandardDef.h"

#define SBUS_PROPORTIONAL_CHANNELS  16 // Number of S.Bus protocol proportional channel
#define SBUS_DIGITAL_CHANNELS        2 // Number of S.Bus protocol digital channel

#define SBUS_FLAG_CH17       0x01
#define SBUS_FLAG_CH18       0x02
#define SBUS_FLAG_FRAMELOST  0x04
#define SBUS_FLAG_FAILSAFE   0x08
#define SBUS_FLAG_RESERVED1  0x10
#define SBUS_FLAG_RESERVED2  0x20
#define SBUS_FLAG_RESERVED3  0x40
#define SBUS_FLAG_RESERVED4  0x80

#define SBUS_GET_CH17(X)      (((X) >> 0) & 0x1)
#define SBUS_GET_CH18(X)      (((X) >> 1) & 0x1)
#define SBUS_GET_FLAMELOST(X) (((X) >> 2) & 0x1)
#define SBUS_GET_FAILSAFE(X)  (((X) >> 3) & 0x1)

#define SBUS_CH_STICK_START  1 // �X�e�B�b�N�Ɋ��蓖�Ă���`�����l���̊J�n�ԍ�
#define SBUS_CH_STICK_END    4 // �X�e�B�b�N�Ɋ��蓖�Ă���`�����l���̏I���ԍ�
#define SBUS_CH_AUX_START    5 // AUX�`�����l���̊J�n�ԍ�

#define SBUS_CH_STICK_START_IDX  (SBUS_CH_STICK_START-1)
#define SBUS_CH_STICK_END_IDX    (SBUS_CH_STICK_END-1)
#define SBUS_CH_AUX_START_IDX    (SBUS_CH_AUX_START-1)

#define SBUS_VALUE_MAX   1000 // ��
#define SBUS_VALUE_MID    750 // ��
#define SBUS_VALUE_MIN    500 // ��
#define SBUS_VALUE_WIDTH  500 // ��


typedef struct _tag_SBusData 
{
	_UWORD Prop[SBUS_PROPORTIONAL_CHANNELS];
	_UBYTE Flags;
	
#ifdef __cplusplus
	_tag_SBusData()
	{
		for( int i = 0; i < SBUS_PROPORTIONAL_CHANNELS; i++ ) Prop[i] = 0;
		Flags = 0;
	}
#endif
} SBusData;

#endif