///
///
/// @file       Commondef_Low.h
///
/// @brief      �~�j�T�[�x�C���[�v���W�F�N�g���ʒ�`
///
///             Lower-Level MCU (C), Higher-Level MCU (C++), GCS (C++) ����
///             Lower-Level MCU (C)����̂ƂȂ��`
///
///

#ifndef INCLUDED_COMMONDEF_LOW_H
#define INCLUDED_COMMONDEF_LOW_H

#include "Common/BasicConfiguration.h"
#include "Common/StandardDef.h"
#include "Common/Health.h"

//**************************************************
//	Constant Values & Type Definition
//**************************************************

// ------------------------------
//	Machine (Drive System)
// ------------------------------

typedef enum 
{
	ROTORLAYOUT_QUAD_TYPE_A,
	ROTORLAYOUT_QUAD_TYPE_B,
	ROTORLAYOUT_HEX_TYPE_A,
	ROTORLAYOUT_HEX_TYPE_B,
} ROTORLAYOUT;

// ------------------------------
//	RC Signal Range Definition
// ------------------------------

typedef _UWORD RcScaleValue;  //!< ���RC�T�[�{�M���̌^��`    1520 +/- 500   (nominal)
typedef _UWORD ErcScaleValue; //!< �g��RC�T�[�{�M���̌^��`   32000 +/- 16000 (nominal)

///
/// �g��RC�T�[�{�M��
///
#define ERC_MID   32000 //!< ���_ �iRC�X�P�[���� 1520 �Ɠ����j
#define ERC_MIN   16000 //!< ���� �iRC�X�P�[���� 1020 �Ɠ����j
#define ERC_MAX   48000 //!< ��� �iRC�X�P�[���� 2020 �Ɠ����j
#define ERC_RANGE 16000 //!< ���_�Ə㉺���̍�
#define ERC_WIDTH 32000 //!< ����Ɖ����̍�

///
/// ���RC�T�[�{�M��(Futaba)
///
#define RC_MID   1520 //!< ���_
#define RC_MIN   1020 //!< ����
#define RC_MAX   2020 //!< ���
#define RC_RANGE  500 //!< ���_�Ə㉺���̍�
#define RC_WIDTH 1000 //!< ����Ɖ����̍�

#define RC_LOW   RC_MAX //!< �X���b�g���ʒu�i���[�j
#define RC_HIGH  RC_MIN //!< �X���b�g���ʒu�i�n�C�j

///
/// �l�ϊ��}�N��
///
#define RC2ERC(X) (ErcScaleValue)(((_SWORD)(X) - 520) << 5) //!< �ϊ����Z General rc scale -> Extended scale
#define ERC2RC(X) (RcScaleValue)(((X) >> 5) + 520)          //!< �ϊ����Z Extended scale -> General rc scale

#define RC2SW(X)  ((X) > 1270 ? ((X) > 1770 ? 2 : 1) : 0)   //!< �X�C�b�`�l(0/1/2)�ւ̕ϊ����Z

///
/// �v���|�f�[�^�̏����l
///
#define RC_THROTTLE_INIT RC_LOW //!< �X���b�g���̏����l
#define RC_RPY_INIT      RC_MID //!< �G�������C�G���x�[�^�C���_�[�̏����l
#define RC_CH5_INIT      RC_MIN //!< Ch5�̏����l
#define RC_CH6_INIT      RC_MIN //!< Ch6�̏����l
#define RC_CH7_INIT      RC_MIN //!< Ch7�̏����l
#define RC_CH8_INIT      RC_MIN //!< Ch8�̏����l
#define RC_CH9_INIT      RC_MID //!< Ch9�̏����l
#define RC_CH10_INIT     RC_MID //!< Ch10�̏����l
#define RC_CH11_INIT     RC_MID //!< Ch11�̏����l
#define RC_CH12_INIT     RC_MID //!< Ch12�̏����l
#define RC_CH13_INIT     RC_MID //!< Ch13�̏����l
#define RC_CH14_INIT     RC_MID //!< Ch14�̏����l
#define RC_CH15_INIT     RC_MID //!< Ch15�̏����l
#define RC_CH16_INIT     RC_MID //!< Ch16�̏����l
#define RC_CH17_INIT     0      //!< Ch17�̏����l
#define RC_CH18_INIT     0      //!< Ch18�̏����l

///
/// �X�e�B�b�N�ʒu�ɂ�郂�[�h�����臒l�i�ݒ�l�j
///
#define RC_THROTTLEBOTTOM_THRESHOLD     1920 //!< �X���b�g�������[�̃|�W�V�����ɂ��邩�ǂ�����臒l
#define RC_RUDDERLEFT_THRESHOLD         1060 //!< ���_�[�����[�^�n���ʒu�ɂ��邩�ǂ�����臒l
#define RC_RUDDERRIGHT_THRESHOLD        1980 //!< ���_�[�����[�^��~�ʒu�ɂ��邩�ǂ�����臒l

#define RC_RUDDERMID_THRESHOLD_WIDTH     100 //!< ���_�[�������ʒu�ɂ��邩�ǂ�����臒l�i�����l����̑��Βl�j
#define RC_ELEVATORMID_THRESHOLD_WIDTH   100 //!< �G���x�[�^�������ʒu�ɂ��邩�ǂ�����臒l�i�����l����̑��Βl�j
#define RC_AILERONMID_THRESHOLD_WIDTH    100 //!< �G�������������ʒu�ɂ��邩�ǂ�����臒l�i�����l����̑��Βl�j

// ------------------------------
//	������̓f�[�^�Ɋ܂܂��t���O
// ------------------------------

#define CONTROLFLAG_TRANSIT_TO_IDLE  0x01 //!< ��~��Ԃ���A�C�h����ԂɑJ�ڂ�����i�v�F��ʐ���MCU��~�E�n�����j
#define CONTROLFLAG_TRANSIT_TO_STOP  0x02 //!< �A�C�h����Ԃ����~��ԂɑJ�ڂ�����i�v�F��ʐ���MCU��~�E�n�����j

// ------------------------------
//	Gain-Adjustment Data Definition
// ------------------------------

///
/// �p���p�ɂ��X���b�g���␳
///
#define THROTTLE_CORRECTION_BY_ATTITUDE_SCALE 32768U //!< �X�P�[���ϊ��萔 �i�␳�W�� = �����l / ���̒l�j
#define THROTTLE_CORRECTION_BY_ATTITUDE_MIN   32768U //!< �␳�����l�̍ŏ��l
#define THROTTLE_CORRECTION_BY_ATTITUDE_MAX   65535U //!< �␳�����l�̍ő�l
#define THROTTLE_CORRECTION_BY_ATTITUDE_INIT  32768U //!< �␳�����l�̏����l
#define THROTTLE_CORRECTION_BY_ATTITUDE_SHR   15     //!< �␳���Z�ɂ�����E�V�t�g�� (1/32768)

// ------------------------------
//	Remote EEPROM Operation
// ------------------------------

#define EEPROM_OPERATION_DATASIZE    29                    //!< EEPROM�̃f�[�^�̈�T�C�Y

///
/// EEPROM����R�}���h
///
#define EEPROM_OPERATION_COMMAND_ID_READ              0x01 //!< ID�w��f�[�^�v��
#define EEPROM_OPERATION_COMMAND_ID_READ_BY_BLOCK_NO  0x02 //!< �u���b�NNO�w��f�[�^�v��
#define EEPROM_OPERATION_COMMAND_ID_FORMAT            0x03 //!< �t�H�[�}�b�g
#define EEPROM_OPERATION_COMMAND_ID_BLOCK_ERACE       0x04 //!< �u���b�N����

///
/// EEPROM����R�}���h�͈�
///
#define EEPROM_OPERATION_COMMAND_ID_RANGE_LOW  0x01
#define EEPROM_OPERATION_COMMAND_ID_RANGE_HIGH 0x04

// 20150202 TCS.S.Kawanoue ADD_S [ATMega�u�[�g���[�_����]
// ------------------------------
//	Remote LOWFIRM Operation
// ------------------------------

#define USER_AREA_SIZE (120*1024)				//!< ���[�U�̈�̃o�C�g�� */
#define SPM_PAGESIZE (256)						//!< �t���b�V��1�y�[�W�̃T�C�Y (128���[�h=256�o�C�g) */
#define PAGE_SPLIT_NUMBER (8)					//!< �t���b�V��1�y�[�W�̕�����
#define LOWFIRM_OPERATION_DATASIZE (SPM_PAGESIZE/PAGE_SPLIT_NUMBER)		//!< LOWFIRM�̃f�[�^�̈�T�C�Y

// CRC32�v�Z�֘A
#define LOWFIRM_CRC32_POLYNOMIAL	(0xEDB88320UL)	// CRC32�v�ZUART�p(����������X32+X26+X23+X22+X16+X12+X11+X10+X8+X7+X5+X4+X2+X1+1)
#define LOWFIRM_CRC32_INITIAL_VAL	(0xFFFFFFFFUL)	// �����l
#define LOWFIRM_CRC32_OUT_XOR_VAL	(0xFFFFFFFFUL)	// �o��XOR

typedef enum 
{
	LOWFIRM_RES_OK   = 0,
	LOWFIRM_RES_FAIL = 1,
} LOWFIRM_RES;

///
/// LOWFIRM����R�}���h
///
#define LOWFIRM_OPERATION_COMMAND_ID_NOTIFY_START  0x01 //!< �J�n�ʒm           : High -> Low
#define LOWFIRM_OPERATION_COMMAND_ID_READY_START   0x02 //!< �J�n����           : Low  -> High
#define LOWFIRM_OPERATION_COMMAND_ID_DATA_END      0x03 //!< �f�[�^�I��         : High -> Low
#define LOWFIRM_OPERATION_COMMAND_ID_DATA_COMPLETE 0x04 //!< �f�[�^�������݊��� : Low  -> High

///
/// LOWFIRM����R�}���h�͈�
///
#define LOWFIRM_OPERATION_COMMAND_ID_RANGE_LOW  0x01
#define LOWFIRM_OPERATION_COMMAND_ID_RANGE_HIGH 0x04
// 20150202 TCS.S.Kawanoue ADD_E


// ------------------------------
//	System
// ------------------------------

///
/// @brief �N�����[�h�̒�`
///
typedef enum 
{
	BOOT_UNDEFINED      = 0, //!< ����`���[�h�i�G���[�C��s�s�j
	BOOT_NORMAL         = 1, //!< �ʏ탂�[�h�i��s�j
	BOOT_EEPROM         = 2, //!< EEPROM���샂�[�h�i��s�s�j
	BOOT_ESC_INITIALIZE = 3, //!< ESC���������[�h�i��s�s�j
} BOOTMODE;

///
/// @brief �쓮���[�h�i0�`7�j
///
typedef enum 
{
	SYS_STOP       = 0, //!< ��~���[�h�i�S���[�^��~�j
	SYS_IDLE       = 1, //!< �A�C�h�����[�h�i�A�C�h����]���Ń��[�^��]�j
	SYS_FLIGHT     = 2, //!< ��s���[�h�i��s��ԁD�t�B�[�h�o�b�N����ƃ~�L�V���O���Z���s���j
	SYS_Reserved1  = 3, //!< �\��
	SYS_Reserved2  = 4, //!< �\��
	SYS_TERMINATED = 5, //!< �^�[�~�l�[�V�������[�h�i�S���[�^���ً}��~�����ė������郂�[�h�j
	SYS_Reserved3  = 6, //!< �\��
	SYS_Reserved4  = 7, //!< �\��
} SYSTEMMODE;

// Switch position �iCONTROLMODE�̎����ɂ���č폜���邩��)
typedef enum 
{
	SWITCH_OFF  = 0,
	SWITCH_ON_1 = 1,
	SWITCH_ON_2 = 2,
} SWITCHSTATE;

///
/// @brief ���ʐ���MCU�̓���t���O
///
typedef enum 
{
	SYSFLAG_NULL                   = 0x00, //!< �t���O�Ȃ�
	SYSFLAG_PROPODATAAVAILABLE     = 0x01, //!< �v���|�f�[�^��M��
	SYSFLAG_THROTTLEBOTTOM         = 0x02, //!< �X���b�g�������[�̃|�W�V�����ɂ���
	SYSFLAG_MCUCOMMTRANSMITENABLE  = 0x04, //!< ���ʐ���MCU�����ʐ���MCU�ւ̃f�[�^���o���s��
	SYSFLAG_HIGHERMCUDATAAVAILABLE = 0x08, //!< ��ʐ���MCU����̐�����̓f�[�^��M��
	SYSFLAG_HIGHERMCUDATAUSING     = 0x10, //!< ��ʐ���MCU����̐�����̓f�[�^���g�p��
} SYSFLAG;

///
/// ���ʐ���MCU�̃G���[�t���O�i���䉉�Z���̃G���[�͕ʒ�`�j
///
typedef enum 
{
	SEF_NONE = 0,                       //!< �G���[�t���O�Ȃ�
	SEF_TIMEOUT_CONTROLCALCULATION = 1, //!< ���䉉�Z�^�C���I�[�o�[
} SOFTWARE_ERRORFLAG;

///
/// ���ʐ���MCU�̌x���t���O
///
typedef enum 
{
	SWF_NONE = 0,       //!< �x���t���O�Ȃ�
} SOFTWARE_WARNINGFLAG;

///
/// ���ʐ���MCU�̐��䉉�Z�G���[�t���O
///
typedef enum 
{
  	CEF_NONE = 0,                   //!< �G���[�t���O�Ȃ�
	CEF_SATURATION_THROTTLE = 0x01, //!< �X���b�g�����Z�ɂĖO�a������
	CEF_SATURATION_ROLL     = 0x02, //!< ���[���p���x����ɂĖO�a������
	CEF_SATURATION_PITCH    = 0x04, //!< �s�b�`�p���x����ɂĖO�a������
	CEF_SATURATION_YAW      = 0x08, //!< ���[�p���x����ɂĖO�a������
	CEF_SATURATION_MIXING   = 0x10, //!< �~�L�V���O���Z�ɂĖO�a������
} CONTROL_ERRORFLAG;

// Error flags on lower-Level MCU hardware system
typedef enum 
{
	HEF_NONE = 0,
} HARDWARE_ERRORFLAG;

//**************************************************
//	Data type
//**************************************************

//
// @brief ���ʐ���MCU�̏��
//

// for C
typedef struct 
{
	SYSTEMMODE   SysMode;      // ���䃂�[�h
	SWITCHSTATE  FailsafeSW;   // �t�F�C���Z�[�t�X�C�b�`
	SWITCHSTATE  AssistSW;     // �A�V�X�g���[�h�X�C�b�`
	SWITCHSTATE  AutonomousSW; // �������[�h�X�C�b�`
	RcScaleValue Func1;        // �t�@���N�V�����P�Ɋ��蓖�Ă�ꂽ�l
	SYSFLAG      Flag;         // �V�X�e����ԃt���O
} SysState;

// for C++
#ifdef __cplusplus
struct CSysState : SysState
{
	CSysState()
	{
		SysMode      = SYS_STOP;
		FailsafeSW   = SWITCH_OFF;
		AssistSW     = SWITCH_OFF;
		AutonomousSW = SWITCH_OFF;
		Func1        = RC_MIN;
		Flag         = SYSFLAG_NULL;
	}	
};
#endif

//
// @brief ���ʐ���MCU�̐�����i�p���x���䃋�[�v�j
//

// for C
typedef struct 
{
	// �~�L�V���O�O�̐������
	_UWORD ControlInput_T; // Throttle (     0�`65535)
	_SWORD ControlInput_R; // Roll     (-32767�`32768)
	_SWORD ControlInput_P; // Pitch    (-32767�`32768)
	_SWORD ControlInput_Y; // Yaw      (-32767�`32768)
	// �~�L�V���O��̐������
	_UWORD RotorInput[MAXIMUM_NUMBER_OF_ROTORS]; // (     0�`65535)
} ScaledControlData;

// for C++
#ifdef __cplusplus
struct CScaledControlData : ScaledControlData
{
	CScaledControlData()
	{
		ControlInput_T = 0;
		ControlInput_R = 0;
		ControlInput_P = 0;
		ControlInput_Y = 0;
		for( int i = 0; i < MAXIMUM_NUMBER_OF_ROTORS; i++ ) 
		{
			RotorInput[i] = 0;	
		}
	}
};
#endif

typedef struct {
	CONTROL_ERRORFLAG Control;
} Error;


typedef struct _tag_FirmwareInfo 
{
	_UBYTE Major;          //!< ���W���[
	_UBYTE Minor;          //!< �}�C�i
	_UBYTE Rivision;       //!< ���r�W����
	_UWORD CompiledDate;   //!< �R���p�C���N���� (Bit:YYYYYYYMMMMDDDDD)
	_UBYTE EditionCode[4]; //!< �G�f�B�V�����R�[�h
} FirmwareInfo;


//**************************************************
//	Lower-Level <--> Higher-Level communication definition
//**************************************************

#define MCUCOMM_PREAMBLE1 0xAA
#define MCUCOMM_PREAMBLE2 0xAB

#define MCUCOMM_LEN_PREAMBLE 2
#define MCUCOMM_LEN_PACKETID 1
#define MCUCOMM_LEN_HEADER   (MCUCOMM_LEN_PREAMBLE + MCUCOMM_LEN_PACKETID)
#define MCUCOMM_LEN_CHECKSUM 1

typedef enum 
{
	MCUCOMM_PACKETID_CONTROLINPUT      = 0x00, // High -> Low  (old format)
	MCUCOMM_PACKETID_PROPODATA         = 0x01, // Low  -> High (old format)
	MCUCOMM_PACKETID_CONTROLINPUT_VER2 = 0x02, // High -> Low  (old format)
	MCUCOMM_PACKETID_PROPODATA_VER2    = 0x03, // Low  -> High (old format)
	MCUCOMM_PACKETID_CONTROLINPUT_VER3 = 0x04, // High -> Low  (old format)
	MCUCOMM_PACKETID_PROPODATA_VER3    = 0x05, // Low  -> High
	MCUCOMM_PACKETID_GYRODATA          = 0x06, // Low  -> High
	MCUCOMM_PACKETID_CONTROLDATA       = 0x07, // Low  -> High
	MCUCOMM_PACKETID_SYSTEMDATA        = 0x08, // Low  -> High (old format)
	MCUCOMM_PACKETID_DATAREQUEST       = 0x09, // High -> Low
	MCUCOMM_PACKETID_HEALTHDATA        = 0x10, // Low  -> High
	MCUCOMM_PACKETID_RCRECEIVERSTAT    = 0x11, // Low  -> High
	MCUCOMM_PACKETID_SYSTEMDATA_VER2   = 0x12, // Low  -> High
	MCUCOMM_PACKETID_GAINADJUSTMENT    = 0x13, // High -> Low
	MCUCOMM_PACKETID_EEPROM            = 0x14, // Low <-> High
	MCUCOMM_PACKETID_EEPROMCOMMAND     = 0x15, // High -> Low
	MCUCOMM_PACKETID_PROPODATA_VER4    = 0x16, // Low  -> High
	MCUCOMM_PACKETID_SYSTEMDATA_VER3   = 0x17, // Low  -> High
	MCUCOMM_PACKETID_CONTROLINPUT_VER4 = 0x18, // High -> Low
	MCUCOMM_PACKETID_TRANSFERMODECHANGE= 0x19, // High -> Low
	MCUCOMM_PACKETID_SHOCKDETECTIONINFO= 0x1A, // High -> Low
// 20150202 TCS.S.Kawanoue ADD_S [ATMega�u�[�g���[�_����]
	MCUCOMM_PACKETID_LOWFIRMDATA       = 0x1B, // High -> Low
	MCUCOMM_PACKETID_LOWFIRMDATARSP    = 0x1C, // Low  -> High
	MCUCOMM_PACKETID_LOWFIRMCOMMAND    = 0x1D, // High <-> Low
// 20150202 TCS.S.Kawanoue ADD_E
	MCUCOMM_PACKETID_FIRMWAREINFO      = 0xF0, // Low  -> High
	MCUCOMM_PACKETID_DEBUG             = 0xFF, // Low  -> High
} MCUCOMM_PACKETID;

typedef enum 
{
	MCUCOMM_PACKETSIZE_CONSTANT          = 4,
	MCUCOMM_PACKETSIZE_CONTROLINPUT_VER3 = MCUCOMM_PACKETSIZE_CONSTANT + 8,
	MCUCOMM_PACKETSIZE_PROPODATA_VER3    = MCUCOMM_PACKETSIZE_CONSTANT + 25,
	MCUCOMM_PACKETSIZE_GYRODATA          = MCUCOMM_PACKETSIZE_CONSTANT + 6,
	MCUCOMM_PACKETSIZE_CONTROLDATA       = MCUCOMM_PACKETSIZE_CONSTANT + 32,
	MCUCOMM_PACKETSIZE_SYSTEMDATA        = MCUCOMM_PACKETSIZE_CONSTANT + 10,
	MCUCOMM_PACKETSIZE_DATAREQUEST       = MCUCOMM_PACKETSIZE_CONSTANT + 8,
	MCUCOMM_PACKETSIZE_HEALTHDATA        = MCUCOMM_PACKETSIZE_CONSTANT + 6,
	MCUCOMM_PACKETSIZE_RCRECEIVERSTAT    = MCUCOMM_PACKETSIZE_CONSTANT,
	MCUCOMM_PACKETSIZE_SYSTEMDATA_VER2   = MCUCOMM_PACKETSIZE_CONSTANT + 12,
	MCUCOMM_PACKETSIZE_GAINADJUSTMENT    = MCUCOMM_PACKETSIZE_CONSTANT + 8,
	MCUCOMM_PACKETSIZE_EEPROM            = MCUCOMM_PACKETSIZE_CONSTANT + 31,
	MCUCOMM_PACKETSIZE_EEPROMCOMMAND     = MCUCOMM_PACKETSIZE_CONSTANT + 3,
	MCUCOMM_PACKETSIZE_PROPODATA_VER4    = MCUCOMM_PACKETSIZE_CONSTANT + 21,
	MCUCOMM_PACKETSIZE_SYSTEMDATA_VER3   = MCUCOMM_PACKETSIZE_CONSTANT + 14,
	MCUCOMM_PACKETSIZE_CONTROLINPUT_VER4 = MCUCOMM_PACKETSIZE_CONSTANT + 9,
	MCUCOMM_PACKETSIZE_TRANSFERMODECHANGE= MCUCOMM_PACKETSIZE_CONSTANT,
	MCUCOMM_PACKETSIZE_SHOCKDETECTIONINFO= MCUCOMM_PACKETSIZE_CONSTANT + 3,
// 20150202 TCS.S.Kawanoue ADD_S [ATMega�u�[�g���[�_����]
	MCUCOMM_PACKETSIZE_LOWFIRMDATA       = MCUCOMM_PACKETSIZE_CONSTANT + (4+SPM_PAGESIZE/PAGE_SPLIT_NUMBER), 
	MCUCOMM_PACKETSIZE_LOWFIRMDATARSP    = MCUCOMM_PACKETSIZE_CONSTANT + 4,
	MCUCOMM_PACKETSIZE_LOWFIRMCOMMAND    = MCUCOMM_PACKETSIZE_CONSTANT + 5,
// 20150202 TCS.S.Kawanoue ADD_E
	MCUCOMM_PACKETSIZE_FIRMWAREINFO      = MCUCOMM_PACKETSIZE_CONSTANT + 9, 
	MCUCOMM_PACKETSIZE_DEBUG             = MCUCOMM_PACKETSIZE_CONSTANT + 8,
} MCUCOMM_PACKETSIZE;


//**************************************************
//	Packet definition for Lower-Level <--> Higher-Level communication
//**************************************************
#ifndef __cplusplus

#pragma pack(1)

// ���Ǔ��̓f�[�^
typedef struct 
{
	ErcScaleValue Throttle;
	ErcScaleValue Roll;
	ErcScaleValue Pitch;
	ErcScaleValue Yaw;
} McuComm_ControlInputPacket_Ver3;

// ���Ǔ��̓f�[�^
typedef struct 
{
	ErcScaleValue Throttle;
	ErcScaleValue Roll;
	ErcScaleValue Pitch;
	ErcScaleValue Yaw;
	_UBYTE        Flags;
} McuComm_ControlInputPacket_Ver4;

// �v���|�f�[�^ (Ver3)
typedef struct 
{
	RcScaleValue Throttle;
	RcScaleValue Roll;
	RcScaleValue Pitch;
	RcScaleValue Yaw;
	RcScaleValue Ch5;
	RcScaleValue Ch6;
	RcScaleValue Ch7;
	RcScaleValue Ch8;
	RcScaleValue Ch9;
	RcScaleValue Ch10;
	RcScaleValue Ch11;
	RcScaleValue Ch12;
	_UBYTE SignalExist;
} McuComm_PropoDataPacket_Ver3;

// �v���|�f�[�^ (Ver4)
typedef struct 
{
	_UBYTE ProportionalData[20]; // 1Ch = 10�r�b�g * 16Ch
	_UBYTE DigitalData;
} McuComm_PropoDataPacket_Ver4;

// �W���C���f�[�^
typedef struct 
{
	_UWORD RollRate;    // p [deg/s] = 0.01 * value;
	_UWORD PitchRate;   // q [deg/s] = 0.01 * value;
	_UWORD YawRate;     // r [deg/s] = 0.01 * value;
} McuComm_GyroDataPacket;

// �p���x���䃋�[�v������̓f�[�^
typedef ScaledControlData McuComm_ControlDataPacket;

// �V�X�e����ԃf�[�^ (Ver2)
typedef struct 
{
	_UBYTE       SysMode;
	_UBYTE       Switches;          // Failsafe(0, 1), Assist(2, 3), Autonomous(4, 5)
	RcScaleValue Func1;    
	_UWORD       SysFlag;
	_UWORD       SoftwareErrorFlag;
	_UWORD       ControlErrorFlag;
	_UWORD       HardwareErrorFlag; // Currently, Reserved
} McuComm_SystemDataPacket_Ver2;

// �V�X�e����ԃf�[�^ (Ver3)
typedef struct 
{
	_UBYTE       SysMode;
	_UBYTE       Switches;          // Failsafe(0, 1), Assist(2, 3), Autonomous(4, 5)
	RcScaleValue Func1;    
	_UWORD       SysFlag;
	_UWORD       SoftwareErrorFlag;
	_UWORD       ControlErrorFlag;
	_UWORD       HardwareErrorFlag; // Currently, Reserved
	_UWORD       CellVoltage;
} McuComm_SystemDataPacket_Ver3;

// �V�X�e�����S�x�f�[�^
typedef LowLvMcuHealthData McuComm_HealthDataPacket;

// �f�[�^���N�G�X�g�p�P�b�g
typedef struct 
{
	_UBYTE  Reserved1;
	_UBYTE  Reserved2;
	_UBYTE  Reserved3;
	_UBYTE  Reserved4;
	_UBYTE  Reserved5;
	_UBYTE  Reserved6;
	_UBYTE  Reserved7;
	_UBYTE  Reserved8;
} McuComm_DataRequestPacket;

// ���͕␳�f�[�^�i�����r���j
typedef struct 
{
	_UWORD ThrottleCorrectionByAttitude;  // �p���p�ɂ��X���b�g���␳(32768�`65565)
	_UWORD Reserved1;
	_UWORD Reserved2;
	_UWORD Reserved3;
} McuComm_GainAdjustmentPacket;

// EEPROM�f�[�^�iATMega��SH �Ԃ͑��M�ԍ����Ȃ��j
typedef struct 
{
	_UWORD BlockID;
	_UBYTE Data[EEPROM_OPERATION_DATASIZE];
} McuComm_EepromPacket;

// EEPROM����R�}���h�iATMega��SH �Ԃ͑��M�ԍ����Ȃ��j
typedef struct 
{
	_UBYTE CommandID;
	_UWORD CommandParameter;
} McuComm_EepromCommandPacket;

// �Ռ����m���f�[�^
typedef struct {
	_UBYTE NotifykKind;		// �ʒm���(0x01:���Ռ��A0x02:��Ռ��A0x04:�]�|��OR�l)
	_UWORD Date;			// ���m�N����
} McuComm_ShockDetectionInfo;

// 20150202 TCS.S.Kawanoue ADD_S [ATMega�u�[�g���[�_����]
// LowFirm����R�}���h
typedef struct {
	_UBYTE CommandID;
	_UWORD Param1;		// 20150618 TCS.S.Kanai ADD [�t�@�C���ɂ��EEPROM����������]
} McuComm_LowFirmCommandPacket;
// 20150202 TCS.S.Kawanoue ADD_E

// �t�@�[���E�F�A���
typedef struct 
{
	_UBYTE Major;          //!< ���W���[
	_UBYTE Minor;          //!< �}�C�i
	_UBYTE Rivision;       //!< ���r�W����
	_UWORD CompiledDate;   //!< �R���p�C���N���� (Bit:YYYYYYYMMMMDDDDD)
	_UBYTE EditionCode[4]; //!< �G�f�B�V�����R�[�h
} McuComm_FirmwareInfoPacket;

#pragma pack()

#endif

//**************************************************
// S.Bus Channel Assignment
//**************************************************

// 1-base definition!
#define SBUS_CH_ASSIGN_THROTTLE    3
#define SBUS_CH_ASSIGN_ROLL        1
#define SBUS_CH_ASSIGN_PITCH       2
#define SBUS_CH_ASSIGN_YAW         4
#define SBUS_CH_ASSIGN_FAILSAFE    8
#define SBUS_CH_ASSIGN_ATTCONTROL  5
#define SBUS_CH_ASSIGN_NAVIGATION  7
#define SBUS_CH_ASSIGN_FUNC1       6

// array-index (0-base) definition
#define SBUS_CH_INDEX_THROTTLE   (SBUS_CH_ASSIGN_THROTTLE-1)
#define SBUS_CH_INDEX_ROLL       (SBUS_CH_ASSIGN_ROLL-1)
#define SBUS_CH_INDEX_PITCH      (SBUS_CH_ASSIGN_PITCH-1)
#define SBUS_CH_INDEX_YAW        (SBUS_CH_ASSIGN_YAW-1)
#define SBUS_CH_INDEX_FAILSAFE   (SBUS_CH_ASSIGN_FAILSAFE-1)
#define SBUS_CH_INDEX_ATTCONTROL (SBUS_CH_ASSIGN_ATTCONTROL-1)
#define SBUS_CH_INDEX_NAVIGATION (SBUS_CH_ASSIGN_NAVIGATION-1)
#define SBUS_CH_INDEX_FUNC1      (SBUS_CH_ASSIGN_FUNC1-1)

#endif