///
/// @file       EepromDataDef_v1.h
///
/// @brief      EEPROM�ŊǗ�����f�[�^�Ɋւ����`�ꗗ�i�p�����[�^�Z�b�gVer = 1�j
///
/// @author     iwakura
///

//
// #define ��`�������K��
//
// E2PDEF_�\���̖�_�����o��_�C�ӂ̖��O
//

#ifndef INCLUDED_EEPROMDATADEF_V1_H
#define INCLUDED_EEPROMDATADEF_V1_H

#include "EepromStorageDef.h"

///
/// @brief  EEPROM�u���b�NID
///
enum EEPROM_BLOCKID
{
	E2P_BID_BLANK              = 0,   //!< �u�����N�̈�
	E2P_BID_EEPROMSYSTEMDATA   = 1,   //!< EEPROM�X�g���[�W�Ǘ����
	E2P_BID_UNITINFO           = 2,   //!< ���䃆�j�b�g���
	E2P_BID_SERIALNUMBER       = 100, //!< �����ԍ�
	E2P_BID_ASSEMBLER          = 101, //!< �@�̑g���Ǝ�
	E2P_BID_SELLER             = 102, //!< �̔���
	E2P_BID_AFFILIATION        = 103, //!< �������
	E2P_BID_MANUFACTURINGFLAGS = 104, //!< �����֘A�t���O
	E2P_BID_USERINFO           = 105, //!< ���[�U�[���
	E2P_BID_MAINTENANCEINFO    = 200, //!< �@�̃����e�i���X���
	E2P_BID_MAINTENANCELOG1    = 201, //!< �����e�i���X���O1
	E2P_BID_MAINTENANCELOG2    = 202, //!< �����e�i���X���O2
	E2P_BID_MAINTENANCELOG3    = 203, //!< �����e�i���X���O3
	E2P_BID_LIFEMANAGEMENT1    = 300, //!< �����Ǘ����1
	E2P_BID_LIFEMANAGEMENT2    = 301, //!< �����Ǘ����2
	E2P_BID_LIFEMANAGEMENT3    = 302, //!< �����Ǘ����3
	E2P_BID_LIFEMANAGEMENT4    = 303, //!< �����Ǘ����4
	E2P_BID_MACHINE_CONSTRUCT  = 400, //!< �@�̍\�����
	E2P_BID_CIRCUIT_PARAMETER  = 401, //!< ��H�p�����[�^�[
	E2P_BID_GEOMETRICAL_PARAMETER = 402, //!< �d�S�E�ʒu�ݒ�
	E2P_BID_HARD_IRON_CALIBRATION = 500, //!< �n�[�h�A�C�A���Z��
	E2P_BID_CONTROLLER_GAIN_1  = 600, //!< ����Q�C���iPR�Ȃǁj
	E2P_BID_CONTROLLER_GAIN_2  = 601, //!< ����Q�C���iY�Ȃǁj
	E2P_BID_FILTER_GAIN        = 602, //!< �t�B���^�Q�C��
	E2P_BID_CONTROLLER_RANGE   = 603, //!< �����W�ݒ�
	E2P_BID_DATA_SEND_CYCLE    = 700, //!< �f�[�^���M����
	E2P_BID_SAFETY_FUNCTION    = 800, //!< ���S�@�\�p�����[�^�[
	E2P_BID_GINBAL_SERVO_SETTING = 900, //!< �W���o���T�[�{�ݒ�
	E2P_BID_REMOCON_NMEA_SETTING = 901, //!< �����R���ENMEA�ݒ�
	E2P_BID_SPECIAL_FUNCTION   = 1000,//!< ����@�\�p�����[�^�[
	E2P_BID_CONTROLLER_LIMIT   = 1100,//!< ����@�\����
	E2P_BID_FLYZONE_LIMIT      = 1101,//!< ��s�G���A����
	E2P_BID_CUSTOM_PARAMETER1  = 1200,//!< �J�X�^���Ή��p�����[�^1
	E2P_BID_CUSTOM_PARAMETER2  = 1201 //!< �J�X�^���Ή��p�����[�^2
};

///
/// @brief  ���j�b�gID
///
enum UNITINFO__UNIT_ID
{
	E2PDEF_UNITID_LEGACY  = 1, //!< MiniSurveyorAutoPilot Ver.0.6 �܂ł̃��j�b�gID
	E2PDEF_UNITID_MSAP_1  = 2, //!< MiniSurveyorAutoPilot Ver.1.0 �̃��j�b�gID
};

///
/// @brief  �����Ǝ�ID
///
enum UNITINFO__MANUFACTURER_ID
{
	E2PDEF_MANUFACTURERID_ACSL = 1, // ��������V�X�e��������
	E2PDEF_MANUFACTURERID_CEC  = 2, // �����d�q
};

///
/// @brief  ����p�r�Ή��t���O
///
enum SPECIAL_FLAGS1
{
	E2PDEF_SPECIAL_FLAGS1_HIGHMCU_ROTORDRIVE_PERMISSION = 0x0001, //!< ��ʐ���MCU��~�E�n������
};

///
/// @brief  �p���X��������
///
enum PULSE_GENE_CYCLE_ID
{
	E2PDEF_PULSE_GENE_CYCLE_NONE	= 0,	// ���ݒ�
	E2PDEF_PULSE_GENE_CYCLE_50Hz	= 1,	// 50Hz  20ms
	E2PDEF_PULSE_GENE_CYCLE_100Hz	= 2,	// 100Hz 10ms
	E2PDEF_PULSE_GENE_CYCLE_200Hz	= 3,	// 200Hz 5ms
	E2PDEF_PULSE_GENE_CYCLE_400Hz	= 4 	// 400Hz 2.5ms
};

///
/// @brief  �p���X��
///
enum PULSE_GENE_WIDTH_ID
{
	E2PDEF_PULSE_WIDTH_NONE		= 0,		// ���ݒ�
	E2PDEF_PULSE_WIDTH_STANDARD	= 1,		// �X�^���_�[�h
	E2PDEF_PULSE_WIDTH_WIDE		= 2 		// ���C�h
};

///
/// @brief  �v���|���[�h
///
enum PROPO_MODE_NUMBER
{
	E2PDEF_PROPO_MODE_1 = 0,		// MODE 1
	E2PDEF_PROPO_MODE_2 = 1,		// MODE 2
	E2PDEF_PROPO_MODE_3 = 2, 		// MODE 3
	E2PDEF_PROPO_MODE_4 = 3 		// MODE 4
};

///
/// @brief  �ʐM�v���g�R��
///
enum COMMUNICATION_PROTOCOL
{
	E2PDEF_COMMUNICATION_PROTOCOL_LEGACY = 0,		// Legacy
	E2PDEF_COMMUNICATION_PROTOCOL_2G     = 1		// 2G
};

///
/// @brief  �ʐM�f�o�C�X
///
enum COMMUNICATION_DEVICE
{
	E2PDEF_COMMUNICATION_DEVICE_TRANSPARENT = 0,	// Transparent
	E2PDEF_COMMUNICATION_DEVICE_FEP         = 1		// FEP
};

///
/// @brief  �p�P�b�g���M����
///
enum DATA_SEND_CYCLE
{
	E2PDEF_DATA_SEND_CYCLE_NOSEND = 0,	// ���M���Ȃ�
	E2PDEF_DATA_SEND_CYCLE_50HZ   = 1,	// 50Hz
	E2PDEF_DATA_SEND_CYCLE_25HZ   = 2,	// 25Hz
	E2PDEF_DATA_SEND_CYCLE_12_5HZ = 4,	// 12.5Hz
	E2PDEF_DATA_SEND_CYCLE_10HZ   = 5,	// 10Hz
	E2PDEF_DATA_SEND_CYCLE_5HZ    = 10,	// 5Hz
	E2PDEF_DATA_SEND_CYCLE_2_5HZ  = 20,	// 2.5Hz
	E2PDEF_DATA_SEND_CYCLE_2HZ    = 25,	// 2Hz
	E2PDEF_DATA_SEND_CYCLE_1HZ    = 50,	// 1Hz
	E2PDEF_DATA_SEND_CYCLE_0_5HZ  = 100	// 0.5Hz
};

///
/// @brief  ���[�U�[ID�������ő�
///
#define E2PDEF_USER_ID_MAX				14		// ���[�U�[ID�������ő�

///
/// @brief  ���[�U�[�p�X���[�h�������ő�
///
#define E2PDEF_PASSWORD_MAX				15		// �p�X���[�h�������ő�

///
/// @brief  ���[�^�[�̍\��
///
#define ROTORARRANGEMENT_NONE 0
#define ROTORARRANGEMENT_X4_A 1
#define ROTORARRANGEMENT_X4_B 2
#define ROTORARRANGEMENT_X6_A 3
#define ROTORARRANGEMENT_X6_B 4

#define SWAP_ROTATIONAL_DIRECTION FALSE // If 1st rotor's propeller is negative pitch, please set TRUE

///
/// @brief  �Z����
///
#define E2PDEF_NUM_OF_CELLS				6

///
/// @brief  �p�P�b�g���M�����̑���
///
#define E2PDEF_SEND_PACKET_CYCLE_MAX	27		// �p�P�b�g���M�����̑���

///
/// @brief  �S�[�z�[������
///
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_PROPO					0x01	//!< bit0-����ߑ���
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_COMMAND					0x02	//!< bit1-���������
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_LOW_VOLTAGE				0x04	//!< bit2-�d���ቺ
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_DISCONNECT_PROPO		0x08	//!< bit3-����ߓr��
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_DISCONNECT_DATALINK		0x10	//!< bit4-�ް��ݸ�r��
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_LESS_SATELITE			0x20	//!< bit5-�q�����ቺ
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_REST_CAUSE				0x40	//!< bit6-���̑��ضނ�����

///
/// @brief  ���S�@�\�t���O
///
#define E2PDEF_SAFETYFUNCTION_FLAG_GPSLED_ENABLE		0x01	//!< bit0-GPSLED�L��
#define E2PDEF_SAFETYFUNCTION_FLAG_BUZZER_ENABLE		0x02	//!< bit1-�޻ް�L��

///
/// @brief  ����@�\
///
#define E2PDEF_BIT_NOFLYZONE_ENABLE						0x00000001UL	//!< ����@�\�E�t���O1 bit0-��s�֎~�G���A�@�\�L��
#define E2PDEF_BIT_HIGHMCU_STOPSTART_ENABLE					0x0001		//!< ����@�\�E�t���O2 bit0-��ʐ���MCU��~�E�n������ ����=0

///
/// @brief  ��s�G���A����
///
#define E2PDEF_FLYZONELIMIT_BIT_LIMIT_NONE	0x01	// �ޯ�̨���� bit0~2 - �`�� bit0-������
#define E2PDEF_FLYZONELIMIT_BIT_ELLIPSE		0x02	// �ޯ�̨���� bit0~2 - �`�� bit1-�ȉ~�`
#define E2PDEF_FLYZONELIMIT_BIT_RECTANGLE	0x04	// �ޯ�̨���� bit0~2 - �`�� bit2-�����`
#define E2PDEF_FLYZONELIMIT_BIT_LL			0x08	// �ޯ�̨���� bit3   - �ܓx�o�x�ݒ�L��

#if defined(WIN32) || defined(IAR)
#pragma pack(1)
#else
#pragma pack 1
#endif

//####################################################################################################
//	#0000	�Ǘ��f�[�^
//####################################################################################################

///
/// @brief  EEPROM�Ǘ����
///
typedef struct _tag_EepromSystemData
{
	_UWORD ParametersetVer;
	_UWORD ParametersetVerLowerCompatibleValue;
	_UWORD ParametersetVerHigherCompatibleValue;
	_UBYTE Reserved[23];

} E2PDEF_EepromSystemData;

///
/// @brief  ���j�b�g���
///
typedef struct _tag_UnitInfo
{
	_UBYTE UnitID;
	_UBYTE RevisionID;
	_UBYTE MenufacturerID;
	_UWORD LotInfo;
	_UBYTE UnitSN[16];
	_UBYTE EditionCode[4];
	_UBYTE Reserved[4];

} E2PDEF_UnitInfo;

//####################################################################################################
//	#0100	�@�̊Ǘ��E�������
//####################################################################################################

///
/// @brief  �����ԍ�
///
typedef struct _tag_SerialNumber
{
	_UBYTE SerialNumber[29];

} E2PDEF_SerialNumber;

///
/// @brief  �@�̑g���Ǝ�
///
typedef struct _tag_Assembler
{
	_UBYTE Assembler[29];

} E2PDEF_Assembler;

///
/// @brief  �̔���
///
typedef struct _tag_Seller
{
	_UBYTE Seller[29];

} E2PDEF_Seller;

///
/// @brief  �������
///
typedef struct _tag_Affiliation
{
	_UBYTE Affiliation[29];

} E2PDEF_Affiliation;

///
/// @brief  �����֘A�t���O
///
typedef struct _tag_ManufacturingFlags
{
	_UBYTE  Flag1;
	_UDWORD ProcessManegementFlag;
	_UBYTE  LifeCycle;
	_UBYTE  Reserved[23];

} E2PDEF_ManufacturingFlags;

///
/// @brief  ���[�U�[���
///
typedef struct _tag_UserInfo
{
	_UBYTE	USER_ID[E2PDEF_USER_ID_MAX];
	_UBYTE	PASSWORD[E2PDEF_PASSWORD_MAX];
} E2PDEF_UserInfo;

//####################################################################################################
//	#0200	�����e�i���X
//####################################################################################################

///
/// @brief  �@�̃����e�i���X���
///
typedef struct _tag_MaintenanceInfo
{
	_UWORD	TotalFlightCnt;				// �O�񃁃��e���̑���s��
	_UBYTE	TotalPowerOnTime[3];		// �O�񃁃��e���̑��N������
	_UBYTE	TotalFlightTime[3];			// �O�񃁃��e���̑���s����
	_UWORD	ShockDetectCnt;				// �O�񃁃��e���̏��Ռ����m��
	_UBYTE	StrongShockDetectCnt;		// �O�񃁃��e���̑�Ռ����m��
	_UBYTE	FallDetectCnt;				// �O�񃁃��e���̓]�|���m��
	_UWORD	StrongShockDetectDate;		// �O�񃁃��e���̑�Ռ����m�N����
	_UWORD	FlightTimeStrongShockDetect;// �O�񃁃��e���̑�Ռ����m���̑���s����
	_UWORD	FallDetectDate;				// �O�񃁃��e���̓]�|���m�N����
	_UWORD	FlightTimeFallDetect;		// �O�񃁃��e���̓]�|���m���̑���s����
	_UBYTE  Reserved[9];

} E2PDEF_MaintenanceInfo;

///
/// @brief  �����e�i���X���O1
///
typedef struct _tag_MaintenanceLog1
{
	_UWORD	MaintenanceLog1Date;		// �����e�i���X�N����1
	_UBYTE	MaintenanceLog1Comment[27];	// �����e�i���X���O1

} E2PDEF_MaintenanceLog1;

///
/// @brief  �����e�i���X���O2
///
typedef struct _tag_MaintenanceLog2
{
	_UWORD	MaintenanceLog2Date;		// �����e�i���X�N����2
	_UBYTE	MaintenanceLog2Comment[27]; // �����e�i���X���O2

} E2PDEF_MaintenanceLog2;

///
/// @brief  �����e�i���X���O3
///
typedef struct _tag_MaintenanceLog3
{
	_UWORD	MaintenanceLog3Date;		// �����e�i���X�N����3
	_UBYTE	MaintenanceLog3Comment[27]; // �����e�i���X���O3

} E2PDEF_MaintenanceLog3;

//####################################################################################################
//	#0300	�����Ǘ�
//####################################################################################################

///
/// @brief  �����Ǘ����1
///
typedef struct _tag_LifeManagement1
{
	_UWORD TotalFlightCnt;
	_UBYTE TotalPowerOnTime[3];
	_UBYTE TotalFlightTime[3];
	_UWORD ShockDetectCnt;
	_UBYTE StrongShockDetectCnt;
	_UBYTE FallDetectCnt;
	_UWORD StrongShockDetectDate;
	_UWORD FlightTimeStrongShockDetect;
	_UWORD FallDetectDate;
	_UWORD FlightTimeFallDetect;
	_UBYTE  Reserved[9];
} E2PDEF_LifeManagement1;

///
/// @brief  �����Ǘ����2
///
typedef struct _tag_LifeManagement2
{
	_UBYTE MotorTotalTime1[3];
	_UBYTE MotorTotalTime2[3];
	_UBYTE MotorTotalTime3[3];
	_UBYTE MotorTotalTime4[3];
	_UBYTE MotorTotalTime5[3];
	_UBYTE MotorTotalTime6[3];
	_UBYTE MotorTotalTime7[3];
	_UBYTE MotorTotalTime8[3];
	_UBYTE MotorTotalTime9[3];
	_UBYTE  Reserved[2];
} E2PDEF_LifeManagement2;

///
/// @brief  �����Ǘ����3
///
typedef struct _tag_LifeManagement3
{
	_UBYTE MotorTotalTime10[3];
	_UBYTE MotorTotalTime11[3];
	_UBYTE MotorTotalTime12[3];
	_UBYTE EscTotalTime1[3];
	_UBYTE EscTotalTime2[3];
	_UBYTE EscTotalTime3[3];
	_UBYTE EscTotalTime4[3];
	_UBYTE EscTotalTime5[3];
	_UBYTE EscTotalTime6[3];
	_UBYTE  Reserved[2];
} E2PDEF_LifeManagement3;

///
/// @brief  �����Ǘ����4
///
typedef struct _tag_LifeManagement4
{
	_UBYTE EscTotalTime7 [3];
	_UBYTE EscTotalTime8 [3];
	_UBYTE EscTotalTime9 [3];
	_UBYTE EscTotalTime10 [3];
	_UBYTE EscTotalTime11 [3];
	_UBYTE EscTotalTime12 [3];
	_UBYTE UserTotalTime1 [3];
	_UBYTE UserTotalTime2 [3];
	_UBYTE UserTotalTime3 [3];
	_UBYTE  Reserved[2];
} E2PDEF_LifeManagement4;

//####################################################################################################
//	#0400	�\��
//####################################################################################################

///
/// @brief  �@�̍\�����
///
typedef struct _tag_MachineConstruct
{
	_UBYTE RotorArrangement;		// �z�u 0:���ݒ� 1:4��A 2:4��B 3:6��A 4:6��B
	_UBYTE Flag;					// �t���O bit0:��]������ް�
	_UBYTE PulseGenerateCycle;		// �p���X�������� 0:���ݒ� 1:50Hz 2:100Hz 3:200Hz 4:400Hz
	_UBYTE PulseWidth;				// �p���X�� 0:���ݒ� 1:����ް�� 2:���C�h
	_UBYTE ButteryCellNum;			// �Z����
	_UBYTE PropoMode;				// �v���|���[�h
	_UBYTE reserved[23];
} E2PDEF_MachineConstruct;

///
/// @brief  ��H�p�����[�^�[
///
typedef struct _tag_CircuitParameter
{
	_UWORD Vmon_R1;		// [��] �d�����̕�����R
	_UWORD Vmon_R2;		// [��] GND���̕�����R
	_UWORD Vmon_VF;		// [mV]  �_�C�I�[�h�̏������~���d���D�����ꍇ��0���g�p
	_UBYTE reserved[23];
} E2PDEF_CircuitParameter;

///
/// @brief  �d�S�E�ʒu�ݒ�
///
typedef struct _tag_GeometricalParameter
{
	_UWORD DistanceBetweenBasePointAndCentroid_X;	// ��_�Əd�S�ʒu�̋���(X)
	_UWORD DistanceBetweenBasePointAndCentroid_Y;	// ��_�Əd�S�ʒu�̋���(Y)
	_UWORD DistanceBetweenBasePointAndCentroid_Z;	// ��_�Əd�S�ʒu�̋���(Z)
	_UWORD DistanceBetweenBasePointAndGPS_X;		// ��_��GPS�̋���(X)
	_UWORD DistanceBetweenBasePointAndGPS_Y;		// ��_��GPS�̋���(Y)
	_UWORD DistanceBetweenBasePointAndGPS_Z;		// ��_��GPS�̋���(Z)
	_UWORD BodyMass;								// �@�̎���(10�O�����P��)
	_UWORD RotorInertiaMoment;						// ���[�^����Ӱ���
	_UWORD PayloadMass;								// �y�C���[�h����(10�O�����P��)
	_SWORD PayloadPosition_X;						// �y�C���[�h�ʒu(X)
	_SWORD PayloadPosition_Y;						// �y�C���[�h�ʒu(Y)
	_SWORD PayloadPosition_Z;						// �y�C���[�h�ʒu(Z)
	_UBYTE reserved[5];
} E2PDEF_GeometricalParameter;

//####################################################################################################
//	#0500	�Z��
//####################################################################################################

///
/// @brief  �n�[�h�A�C�A���Z��
///
typedef struct _tag_HardIronCalibration
{
	_UWORD HardIron_X;			// ʰ�ޱ��ݍZ��X��
	_UWORD HardIron_Y;			// ʰ�ޱ��ݍZ��Y��
	_UWORD HardIron_Z;			// ʰ�ޱ��ݍZ��Z��
	_UWORD HardIronFlying_X;	// ʰ�ޱ��ݍZ��X��(��s��)
	_UWORD HardIronFlying_Y;	// ʰ�ޱ��ݍZ��Y��(��s��)
	_UWORD HardIronFlying_Z;	// ʰ�ޱ��ݍZ��Z��(��s��)
	_UWORD ThrottleValue;		// �X���b�g���l�i1020�`2020�j
	_SWORD MagneticAngleOffset;	// ���C�Ίp�I�t�Z�b�g�i0.01�x�P�� �Ƃ蓾��͈́F-127.00�`127.00�x�j
	_UBYTE reserved[13];
} E2PDEF_HardIronCalibration;

//####################################################################################################
//	#0600	����p�����[�^
//####################################################################################################

///
/// @brief  ����Q�C���iPR�j
///
typedef struct _tag_ControllerGain1
{
	_UWORD  P_Propo_Roll;
	_UWORD  I_Propo_Roll;
	_UWORD  D_Propo_Roll;
	_UWORD  P_Gyro_Roll;
	_UWORD  I_Gyro_Roll;
	_UWORD  D_Gyro_Roll;
	_UWORD  P_Propo_Pitch;
	_UWORD  I_Propo_Pitch;
	_UWORD  D_Propo_Pitch;
	_UWORD  P_Gyro_Pitch;
	_UWORD  I_Gyro_Pitch;
	_UWORD  D_Gyro_Pitch;
	_UWORD  DC_Adj_Gain_R;	// ���g�p
	_UWORD  DC_Adj_Gain_P;	// ���g�p
	_UBYTE  Reserved;
} E2PDEF_ControllerGain1;


///
/// @brief  ����Q�C���iY�j
///
typedef struct _tag_ControllerGain2
{
	_UWORD  P_Propo_Yaw;
	_UWORD  I_Propo_Yaw;
	_UWORD  D_Propo_Yaw;
	_UWORD  P_Gyro_Yaw;
	_UWORD  I_Gyro_Yaw;
	_UWORD  D_Gyro_Yaw;
	_UWORD  P_Propo_THR;
	_UWORD  Untiwindup_Gain_PR;
	_UWORD  Untiwindup_Gain_Y;
	_UBYTE  Reserved[11];
} E2PDEF_ControllerGain2;


///
/// @brief  �t�B���^
///
typedef struct _tag_FilterGain
{
	_UWORD  EnableFlags;
	_UBYTE  Proportional_Filter_Gain_R;
	_UBYTE  Proportional_Filter_Gain_P;
	_UBYTE  Proportional_Filter_Gain_Y;
	_UBYTE  Pseudo_Differential_Gain_R;
	_UBYTE  Pseudo_Differential_Gain_P;
	_UBYTE  Pseudo_Differential_Gain_Y;
	_UBYTE  Differential_HighFreq_Filter_Gain_R;
	_UBYTE  Differential_HighFreq_Filter_Gain_P;
	_UBYTE  Differential_HighFreq_Filter_Gain_Y;
	_UBYTE  Reserved[18];
} E2PDEF_FilterGain;

#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_ROLL_CONTROL                     0x0001  //!< �p���x�����L���ɂ���i���[���j
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_PITCH_CONTROL                    0x0002  //!< �p���x�����L���ɂ���i�s�b�`�j
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_YAW_CONTROL                      0x0004  //!< �p���x�����L���ɂ���i���[�j

#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_1_R                       0x0008  //!< ��ᐬ���Ƀ��[�p�X�t�B���^��K�p����i���[���j
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_1_P                       0x0010  //!< ��ᐬ���Ƀ��[�p�X�t�B���^��K�p����i�s�b�`�j
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_1_Y                       0x0020  //!< ��ᐬ���Ƀ��[�p�X�t�B���^��K�p����i���[�j

#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_2_R                       0x0040  //!< �[��������K�p����i���[���j
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_2_P                       0x0080  //!< �[��������K�p����i�s�b�`�j
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_2_Y                       0x0100  //!< �[��������K�p����i���[�j

#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_3_R                       0x0200  //!< ���������Ƀ��[�p�X�t�B���^��K�p����i���[���j
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_3_P                       0x0400  //!< ���������Ƀ��[�p�X�t�B���^��K�p����i�s�b�`�j
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_3_Y                       0x0800  //!< ���������Ƀ��[�p�X�t�B���^��K�p����i���[�j

#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_THROTTLE_CORRECTION_BY_ATTITUDE  0x1000  // �p���p�ɉ������X���b�g���l�␳���s��

///
/// @brief  �����W�ݒ�
///
typedef struct _tag_ControllerRange
{
	_UWORD  RateRange_RP;
	_UWORD  RateRange_Y;
	_UWORD  PwmDriver_ScaledInput_Stop;
	_UWORD  PwmDriver_ScaledInput_Idle;
	_UWORD  PwmDriver_ScaledInput_Min;
	_UWORD  PwmDriver_ScaledInput_Max;
	_UBYTE  Reserved[17];
} E2PDEF_ControlRange;

//####################################################################################################
//	#0700	�ʐM
//####################################################################################################
///
/// @brief  �f�[�^���M����
///
typedef struct _tag_DataSendCycle
{
	_UBYTE  CommunicationProtocol;
	_UBYTE  CommunicationDevice;
	_UBYTE  PacketSendCycle[E2PDEF_SEND_PACKET_CYCLE_MAX];
} E2PDEF_DataSendCycle;

//####################################################################################################
//	#0800	���S�@�\
//####################################################################################################
///
/// @brief  ���S�@�\
///
typedef struct _tag_SafetyFunction
{
	_UBYTE  GoHomeConditions;
	_UBYTE  SafetyFunctionFlags;
	_UBYTE  JudgementAlartBadSateliteNum;
	_UBYTE  JudgementAlartLessSateliteNum;
	_UBYTE  JudgementAlartLowVoltage;
	_UBYTE  JudgementAlartHalfVoltage;
	_UBYTE  JudgementStartGoHomeVoltage;
	_UBYTE  Reserved[22];
} E2PDEF_SafetyFunction;

//####################################################################################################
//	#0900	�t���f�o�C�X�\��
//####################################################################################################
///
/// @brief  �W���o���T�[�{�ݒ�
///
typedef struct _tag_GinbalServoSetting
{
	_UBYTE  Reserved[29];			// ���聙
} E2PDEF_GinbalServoSetting;

///
/// @brief  �����R���ENMEA�ݒ�
///
typedef struct _tag_RemoconNmeaSetting
{
	_UBYTE  Reserved[29];			// ���聙
} E2PDEF_RemoconNmeaSetting;

//####################################################################################################
//	#1000	����@�\
//####################################################################################################
///
/// @brief  ����@�\ ��
///
typedef struct _tag_SpecialFunctions
{
	_UDWORD SpecialFunctionFlag1;			// �t���O1 bit0-��s�֎~�G���A�@�\�L�� ����=1
	_UWORD  SpecialFunctionFlag2;			// �t���O2 bit0-��ʐ���MCU��~�E�n������ ����=0
	_UBYTE  TerminationChannel;				// ����4�r�b�g�F���Ȱ��ݗpCh�ԍ� (0�͖���, 1�`18:Ch�ԍ�)
	_UBYTE  TerminationThreshold;			// ���Ȱ���臒l (RC�X�P�[���l/8, �ŏ�ʃr�b�g�͗\��)
	_UBYTE  Reserved[21];
} E2PDEF_SpecialFunctions;

//####################################################################################################
//	#1100	����
//####################################################################################################
///
/// @brief  ����@�\���� ��
///
typedef struct _tag_ControllerLimit
{
	_UBYTE  MaxHorizontalSpeedForAutonomous;  // ������s �ő吅����s���x 0.1m/s = 1
	_UBYTE  MaxVerticalSpeedForAutonomous;    // ������s �ő司�~���x     0.1m/s = 1
	_UBYTE  MaxYawRateForAutonomous;          // ������s �ő���񑬓x     1deg/s = 1
	_UBYTE  MaxHorizontalSpeedForUserControl; // հ�ޑ��� �ő吅����s���x 0.1m/s = 1
	_UBYTE  MaxVerticalSpeedForUserControl;   // հ�ޑ��� �ő司�~���x     0.1m/s = 1
	_UBYTE  MaxYawRateForUserControl;         // հ�ޑ��� �ő���񑬓x     1deg/s = 1
	_UBYTE  MaxRollPitchAngleForUserControl;  // հ�ޑ��� �ő�p���p       0.5deg = 1
	_UBYTE  Reserved[22];
} E2PDEF_ControllerLimit;

///
/// @brief  ��s�G���A���� �� 
///
typedef struct _tag_FlyZoneLimit
{
	_UBYTE  BitFieldInfo;			// �ޯ�̨���� bit0~2 - �`�� 0-������ 1-�ȉ~�` 2-�����` bit3 - �ܓx�o�x�ݒ�L�� ����=1
	_UBYTE  HorizontalWidth[3];		// ���� 0.1m=1 ����=10000
	_UBYTE  VerticalWidth[3];		// �c�� 0.1m=1 ����=10000
	_UWORD  MaxHightAboveTheGround;	// �ő�Βn���x 0.1m=1 ����=1500
	_UWORD  MinHightAboveTheGround;	// �ŏ��Βn���x -0.1m=1 ����=1500
	_UBYTE  Angle;					// �p�x 2deg=1 ����=0
	_SDWORD CentralLatitude;		// ���S�ܓx 1�x=10000000 ����=0
	_SDWORD CentralLongitude;		// ���S�o�x 1�x=10000000 ����=0
	_UBYTE  Reserved[9];
} E2PDEF_FlyZoneLimit;

//####################################################################################################
//	#1200	�J�X�^��
//####################################################################################################
///
/// @brief  �J�X�^���Ή��p�����[�^ �� 
///
typedef struct _tag_CustomParameter1
{
	double  FreeParameter1;			// ���R1(double) ����0
	double  FreeParameter2;			// ���R2(double) ����0
	double  FreeParameter3;			// ���R3(double) ����0
	float   FreeParameter7;			// ���R7(float)  ����0
	_UBYTE  FreeParameter9;			// ���R9 ����0
} E2PDEF_CustomParameter1;

///
/// @brief  �J�X�^���Ή��p�����[�^ �� 
///
typedef struct _tag_CustomParameter2
{
	double  FreeParameter4;			// ���R4(double) ����0
	double  FreeParameter5;			// ���R5(double) ����0
	double  FreeParameter6;			// ���R6(double) ����0
	float   FreeParameter8;			// ���R8(float)  ����0
	_UBYTE  FreeParameter10;		// ���R10 ����0
} E2PDEF_CustomParameter2;

#if defined(WIN32) || defined(IAR)
#pragma pack()
#else
#pragma unpack
#endif

#ifdef __cplusplus
class E2PDEF_Operator
{
private:
	static void _Copy(EepromData& dest, const void* pSrc, _UWORD BlockID);
	static void _Copy(void* pDest, const EepromData& src, size_t nSize);
	static bool _IsSame(const void* pA, const void* pB, size_t nSize);

public:
	//---------------------------------
	// �R�s�[(EEPROM�Ǘ��\���^�ւ̕ϊ�)
	//---------------------------------
	static void Copy(EepromData& dest, const E2PDEF_EepromSystemData& src) { _Copy(dest, &src, E2P_BID_EEPROMSYSTEMDATA); }
	static void Copy(EepromData& dest, const E2PDEF_UnitInfo& src) {_Copy(dest, &src, E2P_BID_UNITINFO);}
	
	// "�������"�^�u
	static void Copy(EepromData& dest, const E2PDEF_SerialNumber& src) {_Copy(dest, &src, E2P_BID_SERIALNUMBER);}
	static void Copy(EepromData& dest, const E2PDEF_Assembler& src) {_Copy(dest, &src, E2P_BID_ASSEMBLER);}
	static void Copy(EepromData& dest, const E2PDEF_Seller& src) {_Copy(dest, &src, E2P_BID_SELLER);}
	static void Copy(EepromData& dest, const E2PDEF_Affiliation& src) {_Copy(dest, &src, E2P_BID_AFFILIATION);}
	static void Copy(EepromData& dest, const E2PDEF_ManufacturingFlags& src) {_Copy(dest, &src, E2P_BID_MANUFACTURINGFLAGS);}
	static void Copy(EepromData& dest, const E2PDEF_UserInfo& src) { _Copy(dest, &src, E2P_BID_USERINFO); }
	// "�����e"�^�u
	static void Copy(EepromData& dest, const E2PDEF_MaintenanceInfo& src) {_Copy(dest, &src, E2P_BID_MAINTENANCEINFO);}
	static void Copy(EepromData& dest, const E2PDEF_MaintenanceLog1& src) {_Copy(dest, &src, E2P_BID_MAINTENANCELOG1);}
	static void Copy(EepromData& dest, const E2PDEF_MaintenanceLog2& src) {_Copy(dest, &src, E2P_BID_MAINTENANCELOG2);}
	static void Copy(EepromData& dest, const E2PDEF_MaintenanceLog3& src) {_Copy(dest, &src, E2P_BID_MAINTENANCELOG3);}
	// "�����Ǘ�"�^�u
	static void Copy(EepromData& dest, const E2PDEF_LifeManagement1& src) { _Copy(dest, &src, E2P_BID_LIFEMANAGEMENT1); }
	static void Copy(EepromData& dest, const E2PDEF_LifeManagement2& src) { _Copy(dest, &src, E2P_BID_LIFEMANAGEMENT2); }
	static void Copy(EepromData& dest, const E2PDEF_LifeManagement3& src) { _Copy(dest, &src, E2P_BID_LIFEMANAGEMENT3); }
	static void Copy(EepromData& dest, const E2PDEF_LifeManagement4& src) { _Copy(dest, &src, E2P_BID_LIFEMANAGEMENT4); }
	// "�@�̍\��"�^�u
	static void Copy(EepromData& dest, const E2PDEF_MachineConstruct& src) { _Copy(dest, &src, E2P_BID_MACHINE_CONSTRUCT); }
	static void Copy(EepromData& dest, const E2PDEF_CircuitParameter& src) { _Copy(dest, &src, E2P_BID_CIRCUIT_PARAMETER); }
	static void Copy(EepromData& dest, const E2PDEF_GeometricalParameter& src) { _Copy(dest, &src, E2P_BID_GEOMETRICAL_PARAMETER); }
	// "�Z��"�^�u
	static void Copy(EepromData& dest, const E2PDEF_HardIronCalibration& src) { _Copy(dest, &src, E2P_BID_HARD_IRON_CALIBRATION); }
	// "�����"�^�u
	static void Copy(EepromData& dest, const E2PDEF_ControllerGain1& src) { _Copy(dest, &src, E2P_BID_CONTROLLER_GAIN_1); }
	static void Copy(EepromData& dest, const E2PDEF_ControllerGain2& src) { _Copy(dest, &src, E2P_BID_CONTROLLER_GAIN_2); }
	static void Copy(EepromData& dest, const E2PDEF_FilterGain& src) { _Copy(dest, &src, E2P_BID_FILTER_GAIN); }
	static void Copy(EepromData& dest, const E2PDEF_ControlRange& src) { _Copy(dest, &src, E2P_BID_CONTROLLER_RANGE); }
	// "�ʐM�^�u"
	static void Copy(EepromData& dest, const E2PDEF_DataSendCycle& src) { _Copy(dest, &src, E2P_BID_DATA_SEND_CYCLE); }
#if 1	// 2015�N4���ȍ~
	// "���S�@�\"�^�u
	static void Copy(EepromData& dest, const E2PDEF_SafetyFunction& src) { _Copy(dest, &src, E2P_BID_SAFETY_FUNCTION); }
	// "�f�o�C�X"�^�u
	static void Copy(EepromData& dest, const E2PDEF_GinbalServoSetting& src) { _Copy(dest, &src, E2P_BID_GINBAL_SERVO_SETTING); }
	static void Copy(EepromData& dest, const E2PDEF_RemoconNmeaSetting& src) { _Copy(dest, &src, E2P_BID_REMOCON_NMEA_SETTING); }
	// "����@�\"�^�u
	static void Copy(EepromData& dest, const E2PDEF_SpecialFunctions& src) { _Copy(dest, &src, E2P_BID_SPECIAL_FUNCTION); }
#endif
	// "����"�^�u
	static void Copy(EepromData& dest, const E2PDEF_ControllerLimit& src) { _Copy(dest, &src, E2P_BID_CONTROLLER_LIMIT); }
	static void Copy(EepromData& dest, const E2PDEF_FlyZoneLimit& src) { _Copy(dest, &src, E2P_BID_FLYZONE_LIMIT); }
	// "�J�X�^��"�^�u
	static void Copy(EepromData& dest, const E2PDEF_CustomParameter1& src) { _Copy(dest, &src, E2P_BID_CUSTOM_PARAMETER1); }
	static void Copy(EepromData& dest, const E2PDEF_CustomParameter2& src) { _Copy(dest, &src, E2P_BID_CUSTOM_PARAMETER2); }

	//-----------------------------------
	// �R�s�[(EEPROM�Ǘ��\���^����̕ϊ�)
	//-----------------------------------
	// ""
	static void Copy(E2PDEF_EepromSystemData& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "MSAP"�^�u
	static void Copy(E2PDEF_UnitInfo& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	// "�������"�^�u
	static void Copy(E2PDEF_SerialNumber& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_Assembler& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_Seller& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_Affiliation& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_ManufacturingFlags& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_UserInfo& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "�����e"�^�u
	static void Copy(E2PDEF_MaintenanceInfo& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_MaintenanceLog1& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_MaintenanceLog2& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_MaintenanceLog3& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	// "�����Ǘ�"�^�u
	static void Copy(E2PDEF_LifeManagement1& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_LifeManagement2& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_LifeManagement3& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_LifeManagement4& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "�@�̍\��"�^�u
	static void Copy(E2PDEF_MachineConstruct& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_CircuitParameter& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_GeometricalParameter& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "�Z��"�^�u
	static void Copy(E2PDEF_HardIronCalibration& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "�����"�^�u
	static void Copy(E2PDEF_ControllerGain1& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_ControllerGain2& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_FilterGain& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_ControlRange& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "�ʐM�^�u"
	static void Copy(E2PDEF_DataSendCycle& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
#if 1	// 2015�N4���ȍ~
	// "���S�@�\"�^�u
	static void Copy(E2PDEF_SafetyFunction& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "�f�o�C�X"�^�u
	static void Copy(E2PDEF_GinbalServoSetting& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_RemoconNmeaSetting& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "����@�\"�^�u
	static void Copy(E2PDEF_SpecialFunctions& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
#endif
	// "����"�^�u
	static void Copy(E2PDEF_ControllerLimit& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_FlyZoneLimit& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "�J�X�^��"�^�u
	static void Copy(E2PDEF_CustomParameter1& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_CustomParameter2& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }

	//-------------------
	// �x���t�@�C�`�F�b�N
	//-------------------
	// "�������݃f�[�^�Ɠǂݍ��݃f�[�^�̔�r"
	static bool IsSame(const E2PDEF_EepromSystemData& a, E2PDEF_EepromSystemData& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "MSAP"�^�u
	static bool IsSame(const E2PDEF_UnitInfo& a, const E2PDEF_UnitInfo& b) {return _IsSame(&a, &b, sizeof(a));}
	// "�������"�^�u
	static bool IsSame(const E2PDEF_SerialNumber& a, const E2PDEF_SerialNumber& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_Assembler& a, const E2PDEF_Assembler& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_Seller& a, const E2PDEF_Seller& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_Affiliation& a, const E2PDEF_Affiliation& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_ManufacturingFlags& a, const E2PDEF_ManufacturingFlags& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_UserInfo& a, const E2PDEF_UserInfo& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "�����e"�^�u
	static bool IsSame(const E2PDEF_MaintenanceInfo& a, const E2PDEF_MaintenanceInfo& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_MaintenanceLog1& a, const E2PDEF_MaintenanceLog1& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_MaintenanceLog2& a, const E2PDEF_MaintenanceLog2& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_MaintenanceLog3& a, const E2PDEF_MaintenanceLog3& b) {return _IsSame(&a, &b, sizeof(a));}
	// "�����Ǘ�"�^�u
	static bool IsSame(const E2PDEF_LifeManagement1& a, const E2PDEF_LifeManagement1& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_LifeManagement2& a, const E2PDEF_LifeManagement2& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_LifeManagement3& a, const E2PDEF_LifeManagement3& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_LifeManagement4& a, const E2PDEF_LifeManagement4& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "�@�̍\��"�^�u
	static bool IsSame(const E2PDEF_MachineConstruct& a, const E2PDEF_MachineConstruct& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_CircuitParameter& a, const E2PDEF_CircuitParameter& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_GeometricalParameter& a, const E2PDEF_GeometricalParameter& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "�Z��"�^�u
	static bool IsSame(const E2PDEF_HardIronCalibration& a, const E2PDEF_HardIronCalibration& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "�����"�^�u
	static bool IsSame(const E2PDEF_ControllerGain1& a, const E2PDEF_ControllerGain1& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_ControllerGain2& a, const E2PDEF_ControllerGain2& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_FilterGain& a, const E2PDEF_FilterGain& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_ControlRange& a, const E2PDEF_ControlRange& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "�ʐM"�^�u
	static bool IsSame(const E2PDEF_DataSendCycle& a, const E2PDEF_DataSendCycle& b) { return _IsSame(&a, &b, sizeof(a)); }
#if 1	// 2015�N4���ȍ~
	// "���S�@�\"�^�u
	static bool IsSame(const E2PDEF_SafetyFunction& a, const E2PDEF_SafetyFunction& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "�f�o�C�X"�^�u
	static bool IsSame(const E2PDEF_GinbalServoSetting& a, const E2PDEF_GinbalServoSetting& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_RemoconNmeaSetting& a, const E2PDEF_RemoconNmeaSetting& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "����@�\"�^�u
	static bool IsSame(const E2PDEF_SpecialFunctions& a, const E2PDEF_SpecialFunctions& b) { return _IsSame(&a, &b, sizeof(a)); }
#endif
	// "����"�^�u
	static bool IsSame(const E2PDEF_ControllerLimit& a, const E2PDEF_ControllerLimit& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_FlyZoneLimit& a, const E2PDEF_FlyZoneLimit& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "�J�X�^��"�^�u
	static bool IsSame(const E2PDEF_CustomParameter1& a, const E2PDEF_CustomParameter1& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_CustomParameter2& a, const E2PDEF_CustomParameter2& b) { return _IsSame(&a, &b, sizeof(a)); }
};
#endif

#endif