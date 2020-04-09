#ifndef INCLUDED_PROTOCOL_LEGACYDEF_H
#define INCLUDED_PROTOCOL_LEGACYDEF_H

#include "StandardDef.h"

//**************************************************
//	Higher-Level MCU <--> Ground Control Station  communication definition
//**************************************************

// �p�P�b�g����ID
enum PACKETID 
{
	PACKETID_PROPO                = 0x01, // PC  �� UAV  ~Ver0007a
	PACKETID_IMU                  = 0x02, // UAV �� PC   ~Ver0009a4
	PACKETID_GPS                  = 0x03, // UAV �� PC   ~Ver0009a4
	PACKETID_INSGPS               = 0x04, // UAV �� PC   ~Ver0007a
	PACKETID_INPUT                = 0x05, // UAV �� PC   ~Ver0007a
	PACKETID_ATTREF               = 0x06, // UAV �� PC   ~Ver0010
	PACKETID_VELREF               = 0x07, // UAV �� PC   ~Ver0010
	PACKETID_POSREF               = 0x08, // UAV �� PC   ~Ver0007a
	PACKETID_GPSREF               = 0x09, // PC  �� UAV  ~Ver0007a
	PACKETID_FLIGHT_MODE          = 0x0A, // PC  �� UAV
	PACKETID_LAND_COMMAND         = 0x0B, // PC  �� UAV  ~Ver3.2
	PACKETID_CURRENT_FLIGHTMODE   = 0x0C, // UAV �� PC 
	PACKETID_INSGPS_LITE          = 0x0D, // UAV �� PC   Ver0007b~        �y�ʔ�
	PACKETID_INPUT_LITE           = 0x0E, // UAV �� PC   Ver0007b~        �y�ʔ�
	PACKETID_POSREF_LITE          = 0x0F, // UAV �� PC   Ver0007b~        �y�ʔ�
	PACKETID_GPSREF_LITE          = 0x10, // PC  �� UAV  Ver0007b~Ver0010 �y�ʔ� 
	PACKETID_PROPO_LITE           = 0x11, // PC  �� UAV  Ver0007b~        �y�ʔ�
	PACKETID_COMMAND              = 0x12, // UAV �� PC   Ver0007b~
	PACKETID_COUNTERCOMMAND       = 0x13, // UAV �� PC   Ver0007b~
	PACKETID_IMU_ATTITUDERATE     = 0x14, // UAV �� PC   Ver0009a3~Ver0009a4
	PACKETID_GPS_LITE             = 0x15, // UAV �� PC   Ver0009a5~ �y�ʔ�
	PACKETID_IMU2                 = 0x16, // UAV �� PC   Ver0009a5~
	PACKETID_LOCAL_POSITION       = 0x17, // UAV �� PC   Ver0009a5~
	PACKETID_SYSTEM_CONDITION     = 0x18, // UAV �� PC   Ver0010~
	PACKETID_ATTREF2              = 0x19, // UAV �� PC   Ver1.0~ PACKETID_ATTREF �̃��W�A���P�ʔ�
	PACKETID_VELREF2              = 0x1A, // UAV �� PC   Ver1.0~ PACKETID_VELREF �̉��ǔ�
	PACKETID_GPSREF2              = 0x1B, // PC  �� UAV  Ver1.0~ PACKETID_GPSREF_LITE �̃��W�A���P�ʔ�
	PACKETID_PROPO_SIMPLE         = 0x1C, // PC  �� UAV  Ver1.0~ PACKETID_PROPO_LITE �̕ʌ`���i�ȒP�ȑ���Ƀp�P�b�g�T�C�Y��j
	PACKETID_GPS_INFO             = 0x1D, // UAV �� PC   Ver2.0~
	PACKETID_MULTIWAYPOINT        = 0x1E, // PC  �� UAV  Ver2.0~
	PACKETID_MULTIWAYPOINT_ACK    = 0x1F, // UAV �� PC   Ver2.0~
	PACKETID_USS                  = 0x20, // UAV �� PC 
	PACKETID_MAGNETIC             = 0x21, // UAV �� PC 
	PACKETID_LLC_INPUT_MINIMUM    = 0x22, // UAV �� PC 
	PACKETID_LLC_INPUT_4ROTOR     = 0x23, // UAV �� PC 
	PACKETID_LLC_INPUT_6ROTOR     = 0x24, // UAV �� PC 
	PACKETID_LLC_INPUT_8ROTOR     = 0x25, // UAV �� PC 
	PACKETID_LLC_INPUT_12ROTOR    = 0x26, // UAV �� PC 
	PACKETID_HEALTH               = 0x27, // UAV �� PC   Ver3.2~
	PACKETID_ACTION_POINT         = 0x28, // UAV �� PC   Ver3.2~
	PACKETID_EXTNAVSYS_PV         = 0x29, // PC  �� UAV  Ver3.3~ �O���i�r�Q�[�V�����V�X�e���ɂ��q�@�f�[�^ (�ʒu�E���x)
	PACKETID_EXTNAVSYS_PVH        = 0x30, // PC  �� UAV  Ver3.3~ �O���i�r�Q�[�V�����V�X�e���ɂ��q�@�f�[�^ (�ʒu�E���x�E����)
	PACKETID_EXTNAVSYS_PVQ        = 0x31, // PC  �� UAV  Ver3.3~ �O���i�r�Q�[�V�����V�X�e���ɂ��q�@�f�[�^ (�ʒu�E���x�E�N�H�[�^�j�I���p��)
	PACKETID_EEPROM_WRITE         = 0x32, // PC  �� UAV  Ver3.3~
	PACKETID_EEPROM_COMMAND       = 0x33, // PC  �� UAV  Ver3.3~
	PACKETID_EXTNAVSYS_PVH2       = 0x34, // PC  �� UAV  Ver3.3~ �O���i�r�Q�[�V�����V�X�e���ɂ��q�@�f�[�^ (�ʒu�E���x�E���ʁE�p���x��Βl)
	PACKETID_FIRMWAREINFO         = 0x35, // UAV �� PC   Ver3.3~ �t�@�[���E�F�A���iLow&High�j
	PACKETID_TRANS_MODE_CHANGE_REQ     = 0x36, // UAV �� GCS �f�[�^�]�����[�h�ύX�v��
	PACKETID_TRANS_MODE_CHANGE_RES     = 0x37, // UAV �� GCS �f�[�^�]�����[�h�ύX����
	PACKETID_SD_FILE_SEND_START_REQ    = 0x38, // UAV �� GCS �f�[�^�]���J�n�v��
	PACKETID_SD_FILE_SEND_START_NOTIFY = 0x39, // UAV �� GCS �f�[�^�]���J�n�ʒm
	PACKETID_SD_FILE_SEND_REQ          = 0x3A, // UAV �� GCS �ėp�v��
	PACKETID_SD_FILE_SEND_NOTIFY       = 0x3B, // UAV �� GCS �ėp�ʒm
	PACKETID_SD_FILE_USER_DATA         = 0x3C, // UAV �� GCS ���[�U�[�f�[�^
	PACKETID_SD_FILE_RESEND_REQ        = 0x3D, // UAV �� GCS �f�[�^�đ��v��
	PACKETID_SD_FILE_SEND_STOP_REQ     = 0x3E, // UAV �� GCS �f�[�^�]����~�v��
	PACKETID_SD_FILE_SEND_STOP_NOTIFY  = 0x3F, // UAV �� GCS �f�[�^�]����~�ʒm
// 201506xx TCS.Y.Nakamura ADD_S [�t�@�C���]���@�\�g���Ή�]
	PACKETID_SD_FILE_SEND_UPDATE_FILE_LIST_REQ = 0x40, // UAV �� GCS �t�@�C�����X�g�X�V�v��
	PACKETID_SD_FILE_SEND_UPDATE_FILE_LIST_RES = 0x41, // UAV �� GCS �t�@�C�����X�g�X�V����
	PACKETID_SD_FILE_LIST_DATA                 = 0x42, // �t�@�C���T�C�Y(4)�A�t�@�C��(�t�H���_)���(20)
// 201506xx TCS.Y.Nakamura ADD_E
	PACKETID_GENERALINT_4         = 0xE0, // UAV �� PC   Ver1.0~ ���[�U�[��`�f�[�^(4�o�C�g�o�b�t�@)
	PACKETID_GENERALINT_8         = 0xE1, // UAV �� PC   Ver1.0~ ���[�U�[��`�f�[�^(8�o�C�g�o�b�t�@)
	PACKETID_GENERALINT_12        = 0xE2, // UAV �� PC   Ver1.0~ ���[�U�[��`�f�[�^(12�o�C�g�o�b�t�@)
	PACKETID_GENERALINT_16        = 0xE3, // UAV �� PC   Ver1.0~ ���[�U�[��`�f�[�^(16�o�C�g�o�b�t�@)
	PACKETID_GENERALINT_32        = 0xE4, // UAV �� PC   Ver1.0~ ���[�U�[��`�f�[�^(32�o�C�g�o�b�t�@)
	PACKETID_GENERALREAL_4        = 0xE5, // UAV �� PC   Ver1.0~ ���[�U�[��`�f�[�^(4�o�C�g�o�b�t�@)
	PACKETID_GENERALREAL_8        = 0xE6, // UAV �� PC   Ver1.0~ ���[�U�[��`�f�[�^(8�o�C�g�o�b�t�@)
	PACKETID_GENERALREAL_12       = 0xE7, // UAV �� PC   Ver1.0~ ���[�U�[��`�f�[�^(12�o�C�g�o�b�t�@)
	PACKETID_GENERALREAL_16       = 0xE8, // UAV �� PC   Ver1.0~ ���[�U�[��`�f�[�^(16�o�C�g�o�b�t�@)
	PACKETID_GENERALREAL_32       = 0xE9, // UAV �� PC   Ver1.0~ ���[�U�[��`�f�[�^(32�o�C�g�o�b�t�@)
	PACKETID_USER1                = 0xF0, // PC  �� UAV  Ver0009a4~ ���[�U�[��`�i�p�P�b�g�T�C�Y����j
	PACKETID_USER2                = 0xF1, // PC  �� UAV  Ver0009a4~ ���[�U�[��`�i�p�P�b�g�T�C�Y����j
	PACKETID_USER3                = 0xF2, // PC  �� UAV  Ver0009a4~ ���[�U�[��`�i�p�P�b�g�T�C�Y����j
	PACKETID_USER4                = 0xF3, // PC  �� UAV  Ver0009a4~ ���[�U�[��`�i�p�P�b�g�T�C�Y����j
	PACKETID_USER5                = 0xF4, // PC  �� UAV  Ver0009a4~ ���[�U�[��`�i�p�P�b�g�T�C�Y����j
	PACKETID_DEBUG                = 0xFE, // UAV �� PC 
};

// �p�P�b�g�̑傫��
enum PACKETSIZE 
{
	PACKETSIZE_CONSTANT           = 5,
	PACKETSIZE_PROPO              = PACKETSIZE_CONSTANT + 12,
	PACKETSIZE_IMU                = PACKETSIZE_CONSTANT + 12,
	PACKETSIZE_GPS                = PACKETSIZE_CONSTANT + 26,
	PACKETSIZE_INSGPS             = PACKETSIZE_CONSTANT + 26,
	PACKETSIZE_INPUT              = PACKETSIZE_CONSTANT + 12,
	PACKETSIZE_ATTREF             = PACKETSIZE_CONSTANT +  6,
	PACKETSIZE_VELREF             = PACKETSIZE_CONSTANT +  6,
	PACKETSIZE_POSREF             = PACKETSIZE_CONSTANT + 20,
	PACKETSIZE_GPSREF             = PACKETSIZE_CONSTANT + 22,
	PACKETSIZE_FLIGHT_MODE        = PACKETSIZE_CONSTANT +  1,
	PACKETSIZE_LAND_COMMAND       = PACKETSIZE_CONSTANT +  4,
	PACKETSIZE_CURRENT_FLIGHTMODE = PACKETSIZE_CONSTANT +  1,
	PACKETSIZE_INSGPS_LITE        = PACKETSIZE_CONSTANT + 19,
	PACKETSIZE_INPUT_LITE         = PACKETSIZE_CONSTANT +  5,
	PACKETSIZE_POSREF_LITE        = PACKETSIZE_CONSTANT + 13,
	PACKETSIZE_GPSREF_LITE        = PACKETSIZE_CONSTANT + 15,
	PACKETSIZE_PROPO_LITE         = PACKETSIZE_CONSTANT +  5,
	PACKETSIZE_COMMAND            = PACKETSIZE_CONSTANT +  5,
	PACKETSIZE_COUNTERCOMMAND     = PACKETSIZE_CONSTANT +  5,
	PACKETSIZE_IMU_ATTITUDERATE   = PACKETSIZE_CONSTANT +  6,
	PACKETSIZE_GPS_LITE           = PACKETSIZE_CONSTANT + 19,
	PACKETSIZE_IMU2               = PACKETSIZE_CONSTANT + 23,
	PACKETSIZE_LOCAL_POSITION     = PACKETSIZE_CONSTANT +  9,
	PACKETSIZE_SYSTEM_CONDITION   = PACKETSIZE_CONSTANT + 11,
	PACKETSIZE_ATTREF2            = PACKETSIZE_CONSTANT +  6,
	PACKETSIZE_VELREF2            = PACKETSIZE_CONSTANT +  6,
	PACKETSIZE_GPSREF2            = PACKETSIZE_CONSTANT + 15,
	PACKETSIZE_PROPO_SIMPLE       = PACKETSIZE_CONSTANT +  9,
	PACKETSIZE_GPS_INFO           = PACKETSIZE_CONSTANT + 10,
	PACKETSIZE_MULTIWAYPOINT      = PACKETSIZE_CONSTANT + 30,
	PACKETSIZE_MULTIWAYPOINT_ACK  = PACKETSIZE_MULTIWAYPOINT,
	PACKETSIZE_USS                = PACKETSIZE_CONSTANT + 10,
	PACKETSIZE_MAGNETIC           = PACKETSIZE_CONSTANT +  6,
	PACKETSIZE_LLC_INPUT_MINIMUM  = PACKETSIZE_CONSTANT +  8,
	PACKETSIZE_LLC_INPUT_4ROTOR   = PACKETSIZE_CONSTANT + 16,
	PACKETSIZE_LLC_INPUT_6ROTOR   = PACKETSIZE_CONSTANT + 20,
	PACKETSIZE_LLC_INPUT_8ROTOR   = PACKETSIZE_CONSTANT + 24,
	PACKETSIZE_LLC_INPUT_12ROTOR  = PACKETSIZE_CONSTANT + 32,
	PACKETSIZE_HEALTH             = PACKETSIZE_CONSTANT + 17,
	PACKETSIZE_ACTION_POINT       = PACKETSIZE_CONSTANT + 24,
	PACKETSIZE_EXTNAVSYS_PV       = PACKETSIZE_CONSTANT + 12,
	PACKETSIZE_EXTNAVSYS_PVH      = PACKETSIZE_CONSTANT + 14,
	PACKETSIZE_EXTNAVSYS_PVQ      = PACKETSIZE_CONSTANT + 20,
	PACKETSIZE_EEPROM_WRITE       = PACKETSIZE_CONSTANT + 32,
	PACKETSIZE_EEPROM_COMMAND     = PACKETSIZE_CONSTANT + 4,
	PACKETSIZE_EXTNAVSYS_PVH2     = PACKETSIZE_CONSTANT + 16,
	PACKETSIZE_FIRMWAREINFO       = PACKETSIZE_CONSTANT + 18,
	PACKETSIZE_FROMSECONDARY      = PACKETSIZE_CONSTANT + 12,
	PACKETSIZE_TRANS_MODE_CHANGE_REQ     = PACKETSIZE_CONSTANT + 1,         // ���[�h(1)
	PACKETSIZE_TRANS_MODE_CHANGE_RES     = PACKETSIZE_TRANS_MODE_CHANGE_REQ,// ����(1)
// 201506xx TCS.Y.Nakamura ADD_S [�t�@�C���]���@�\�g���Ή�]
	PACKETSIZE_SD_FILE_SEND_START_REQ    = PACKETSIZE_CONSTANT + 1+4+11+20,    // RW(1)�A�L���f�[�^��(4)�A�t�H���_��(11)�A�t�@�C�����(20)
	PACKETSIZE_SD_FILE_SEND_START_NOTIFY = PACKETSIZE_CONSTANT + 1+1+4+20,  // RW(1)�A����(1)�A�L���f�[�^��(4)�A�t�@�C�����(20)
// 201506xx TCS.Y.Nakamura ADD_E
	PACKETSIZE_SD_FILE_SEND_REQ          = PACKETSIZE_CONSTANT + 1+2+4,     // ���(1)�A�u���b�N�ԍ�(2)�ACRC32(4)
	PACKETSIZE_SD_FILE_SEND_NOTIFY       = PACKETSIZE_CONSTANT + 1+1+2,     // ���(1)�A����(1)�A�u���b�N�ԍ�(2)
	PACKETSIZE_SD_FILE_USER_DATA         = PACKETSIZE_CONSTANT + 1+32,      // �f�[�^�ԍ�(1)�A���[�U�[�f�[�^(32)
	PACKETSIZE_SD_FILE_RESEND_REQ        = PACKETSIZE_CONSTANT + 32,        // �đ��v���p�P�b�g���(32)
	PACKETSIZE_SD_FILE_SEND_STOP_REQ     = PACKETSIZE_CONSTANT + 1,         // �t���O(1)
	PACKETSIZE_SD_FILE_SEND_STOP_NOTIFY  = PACKETSIZE_SD_FILE_SEND_STOP_REQ,// ����(1)
// 201506xx TCS.Y.Nakamura ADD_S [�t�@�C���]���@�\�g���Ή�]
	PACKETSIZE_SD_FILE_SEND_UPDATE_FILE_LIST_REQ = PACKETSIZE_CONSTANT + 11,// �f�B���N�g����(11)
	PACKETSIZE_SD_FILE_SEND_UPDATE_FILE_LIST_RES = PACKETSIZE_CONSTANT + 1 + 2, // ����(1)�A�t�@�C����(2)
	PACKETSIZE_SD_FILE_LIST_DATA         = PACKETSIZE_CONSTANT + 1 + 2 + 4 + 20,  // ���[�h(1)�A�f�[�^�ԍ�(2)�A�t�@�C���T�C�Y(4)�A�t�@�C��(�t�H���_)���(20)
// 201506xx TCS.Y.Nakamura ADD_E
	PACKETSIZE_GENERALINT_4       = PACKETSIZE_CONSTANT + 1 + 4,  // �p�����[�^(1) + �o�b�t�@(4)
	PACKETSIZE_GENERALINT_8       = PACKETSIZE_CONSTANT + 1 + 8,
	PACKETSIZE_GENERALINT_12      = PACKETSIZE_CONSTANT + 1 + 12,
	PACKETSIZE_GENERALINT_16      = PACKETSIZE_CONSTANT + 1 + 16,
	PACKETSIZE_GENERALINT_32      = PACKETSIZE_CONSTANT + 1 + 32,
	PACKETSIZE_GENERALREAL_4      = PACKETSIZE_CONSTANT + 1 + 4,
	PACKETSIZE_GENERALREAL_8      = PACKETSIZE_CONSTANT + 1 + 8,
	PACKETSIZE_GENERALREAL_12     = PACKETSIZE_CONSTANT + 1 + 12,
	PACKETSIZE_GENERALREAL_16     = PACKETSIZE_CONSTANT + 1 + 16,
	PACKETSIZE_GENERALREAL_32     = PACKETSIZE_CONSTANT + 1 + 32,
	PACKETSIZE_DEBUG              = PACKETSIZE_CONSTANT + sizeof(::QUADWORD),
};

// PACKETID_GENERALINT_* �̃p�����[�^�i�f�[�^�̐擪�Ɏw�肷��l�D����5�r�b�g�̓f�[�^�������݈ʒu�̎w��Ɏg�p�j
#define GENERALINT_PARAMETER_UBYTE     (_UBYTE)0        // �o�b�t�@�̃f�[�^�͕�������8�r�b�g�^
#define GENERALINT_PARAMETER_SBYTE     (_UBYTE)(1 << 5) // �o�b�t�@�̃f�[�^�͕����t��8�r�b�g�^
#define GENERALINT_PARAMETER_UWORD     (_UBYTE)(2 << 5) // �o�b�t�@�̃f�[�^�͕�������16�r�b�g�^
#define GENERALINT_PARAMETER_SWORD     (_UBYTE)(3 << 5) // �o�b�t�@�̃f�[�^�͕����t��16�r�b�g�^
#define GENERALINT_PARAMETER_UDWORD    (_UBYTE)(4 << 5) // �o�b�t�@�̃f�[�^�͕�������32�r�b�g�^
#define GENERALINT_PARAMETER_SDWORD    (_UBYTE)(5 << 5) // �o�b�t�@�̃f�[�^�͕����t��32�r�b�g�^
#define GENERALINT_PARAMETER_SQWORD    (_UBYTE)(6 << 5) // �o�b�t�@�̃f�[�^�͕����t��64�r�b�g�^�i���������f�[�^�͑���Ȃ��D�󂯑���signed long long�j
#define GENERALINT_PARAMETER_RESERVED0 (_UBYTE)(7 << 5) // �\��
#define GENERALINT_PARAMETER_MASK      (_UBYTE)0x1F     // ����5�r�b�g�̃}�X�N

// PACKETID_GENERALREAL_* �̃p�����[�^�i�f�[�^�̐擪�Ɏw�肷��l�D����5�r�b�g�̓f�[�^�������݈ʒu�̎w��Ɏg�p�j
#define GENERALREAL_PARAMETER_FLOAT     (_UBYTE)0        // �o�b�t�@�̃f�[�^��float�^
#define GENERALREAL_PARAMETER_DOUBLE    (_UBYTE)(1 << 5) // �o�b�t�@�̃f�[�^��double�^
#define GENERALREAL_PARAMETER_100TO2    (_UBYTE)(2 << 5) // �o�b�t�@�̃f�[�^�͕����t��16�r�b�g�^�iDec2ByteTo100�Ńf�R�[�h����j
#define GENERALREAL_PARAMETER_RADTO2    (_UBYTE)(3 << 5) // �o�b�t�@�̃f�[�^�͕����t��16�r�b�g�^�iDec2ByteToRad�Ńf�R�[�h����j
#define GENERALREAL_PARAMETER_1TO2      (_UBYTE)(4 << 5) // �o�b�t�@�̃f�[�^�͕����t��16�r�b�g�^�iDec2ByteTo1�Ńf�R�[�h����j
#define GENERALREAL_PARAMETER_RESERVED1 (_UBYTE)(5 << 5) // �\��
#define GENERALREAL_PARAMETER_RESERVED2 (_UBYTE)(6 << 5) // �\��
#define GENERALREAL_PARAMETER_RESERVED3 (_UBYTE)(7 << 5) // �\��
#define GENERALREAL_PARAMETER_MASK      (_UBYTE)0x1F     // ����5�r�b�g�̃}�X�N

// �R�}���hID
enum MAV_COMMAND 
{
	COMMAND_UNKNOWNCOMMAND       = 0x00, // UAV �� PC   Ver0007b~ | �J�E���^�[�R�}���h��p �����F�R�}���hID
	COMMAND_BACKTOLAUNCHPOINT    = 0x01, // PC  �� UAV  Ver0007b~ | �����n�_�ւ̋A�� �����F�Ȃ�
	COMMAND_NEXTWAYPOINT         = 0x02, // PC  �� UAV  Ver2.0~   | ���̃E�F�C�|�C���g�ֈړ��i�}���`�E�F�C�|�C���g��s���j �����F�Ȃ�
	COMMAND_CALIBRATION_START     = 0x03, // UAV �� PC �L�����u���[�V�����J�n�v��
	COMMAND_CALIBRATION_STOP      = 0x04, // UAV �� PC �L�����u���[�V������~�v��
	COMMAND_CALIBRATION_RESTART   = 0x05, // UAV �� PC �L�����u���[�V������蒼���v��
	COMMAND_CALIBRATION_INDICATION = 0x06, // UAV �� PC ���[�U�[�I�y���[�V�����w��
	COMMAND_LASERPOINTING_ANGLE  = 0x20, // UGV�̃��[�U�[�R�}���h | 0x00, �f�o�C�XID, PWM�p���X��
	COMMAND_LASERPOINTING_SHOOT  = 0x21, // UGV�̃��[�U�[�R�}���h | 0x00, 0x00, �f�o�C�XID, ���[�UON/OFF
	COMMAND_PING                 = 0xFF, // PC  �� UAV  Ver1.0~
	COMMAND_REQUEST_FIRMWAREINFO = 0x30, // PC  �� UAV  Ver3.3.0~ | �t�@�[���E�F�A���̖₢���킹 �����F�Ȃ�
};

#endif