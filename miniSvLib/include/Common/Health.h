///
/// @file       Health.h
///
/// @brief      �w���X�`�F�b�N�֘A�̒�`
///
///             ���S�x�̒l�͂��ׂẴf�o�C�X�E���W���[���ŋ��ʎd�l�Ƃ��C0�`15��
///             �͈͂ŕω�������̂Ƃ���D�l�̈Ӗ��͎��̒ʂ�F
///             0 �͋@�\����~���Ă�����
///             1�`4 �͋@�\�ɐ[���Ȉُ킪�����Ă�����
///             5�`9 �͋@�\������͈̔͊O�ɂ��蒍�ӂ��K�v�ȏ��
///             10�`15 �͋@�\������ɓ��삵�Ă�����
///
///             ��̓I�Ȓl�̌v�Z�͈ȏ�̃��[�����������Ńf�o�C�X���ɔC�ӂƂ���
///
/// @author     iwakura
///

#ifndef INCLUDED_HEALTHCHECKSYSTEM_H
#define INCLUDED_HEALTHCHECKSYSTEM_H

#include "Common/StandardDef.h"

// ------------------------------
//	Health Data
// ------------------------------
typedef _UBYTE HEALTH_VALUE; //!< ���S�x�f�[�^�̌^�i�l�F0�`15�j

typedef enum 
{
	HEALTH_VALUE_MIN                = 0,  //!< ���S�x�̍ŏ��l             
	HEALTH_VALUE_MAX                = 15, //!< ���S�x�̍ő�l
	HEALTH_VALUE_WARNINGLEVEL       = 9,  //!< ���̒l�ȉ��́����Ӂ����Ӗ�
	HEALTH_VALUE_EMERGENCYLEVEL     = 4,  //!< ���̒l�ȉ��́��x���E�ً}�����Ӗ�
	HEALTH_CPUUSEAGE_WARNINGLEVEL   = 90, //!< CPU�g�p���͂��̒l�ȏ�Ł����Ӂ�
	HEALTH_CPUUSEAGE_EMERGENCYLEVEL = 95, //!< CPU�g�p���͂��̒l�ȏ�Ł��x����
} HEALTHDATA_DEFINITION;


///
/// @brief ���ʐ���MCU�̌��S�x���
///
typedef struct _tag_LowLvMcuHealthData
{
	HEALTH_VALUE CpuUseage;                 // CPU�g�p�� [%]
	HEALTH_VALUE RcReceiverCommHealth;      // RC���V�[�o-MCU�ԒʐM���S�x        (0�`15)
	HEALTH_VALUE RcReceiverReceptionHealth; // RC���V�[�o��M���S�x              (0�`15)
	HEALTH_VALUE GyroCommHealth;            // �W���C���Z���T-MCU�ԒʐM���S�x    (0�`15)
	HEALTH_VALUE GyroMeasurementHealth;     // �W���C���Z���T�v�����S�x          (0�`15)
	HEALTH_VALUE McuCommHealth;             // MCU-MCU�ԒʐM���S�x�iLow <- High�j(0�`15)

#ifdef __cplusplus
	_tag_LowLvMcuHealthData() :
		CpuUseage(0),
		RcReceiverCommHealth(0),
		RcReceiverReceptionHealth(0),
		GyroCommHealth(0),
		GyroMeasurementHealth(0),
		McuCommHealth(0) {}
#endif

} LowLvMcuHealthData;


///
/// @brief ��ʐ���MCU�̌��S�x���
///
#ifdef __cplusplus
struct HighLvMcuHealthData
{
	float  BatteryVoltage;           // �o�b�e���[�d���i�S�d���j
	_UWORD HeapAllocated;            // �q�[�v�̈�m�ۍς݃T�C�Y
	_UWORD StackAllocated;           // �X�^�b�N�̈�m�ۍς݃T�C�Y
	_UBYTE CpuUseage;                // CPU�g�p��
	_UBYTE ImuCommHealth;            // IMU�Z���T-MCU�ԒʐM���S�x              (0�`15)
	_UBYTE ImuGyroMeasurementHealth; // �W���C���Z���T�v�����S�x               (0�`15)
	_UBYTE ImuAccMeasurementHealth;  // �����x�Z���T�v�����S�x                 (0�`15)
	_UBYTE ImuMagMeasurementHealth;  // ���C�Z���T�v�����S�x                   (0�`15)
	_UBYTE PsCommHealth;             // �C���Z���T-MCU�ԒʐM���S�x             (0�`15)
	_UBYTE PsMeasurementHealth;      // �C���Z���T�v�����S�x                   (0�`15)
	_UBYTE GpsCommHealth;            // GPS�Z���T-MCU�ԒʐM���S�x              (0�`15)
	_UBYTE GpsMeasurementHealth;     // GPS�v�����S�x                          (0�`15)
	_UBYTE McuCommHealth;            // MCU-MCU�ԒʐM���S�x�iLow -> High�j     (0�`15)
	_UBYTE MainLinkCommHealth;       // UAV-��n�ǊԒʐM���S�x�iUAV <- ��n�ǁj(0�`15)

	HighLvMcuHealthData() :
		BatteryVoltage(0),
		HeapAllocated(0),
		StackAllocated(0),
		CpuUseage(0),
		ImuCommHealth(0),
		ImuGyroMeasurementHealth(0),
		ImuAccMeasurementHealth(0),
		ImuMagMeasurementHealth(0),
		PsCommHealth(0),
		PsMeasurementHealth(0),
		GpsCommHealth(0),
		GpsMeasurementHealth(0),
		McuCommHealth(0),
		MainLinkCommHealth(0) {}
};
#endif

#endif