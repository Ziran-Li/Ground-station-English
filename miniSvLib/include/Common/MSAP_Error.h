///
/// @file       MSAP_Error.h
///
/// @brief      �G���[��`
///

#ifndef INCLUDED_MSAP_ERROR_H
#define INCLUDED_MSAP_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAKE_ERR_CODE(class_no, err_level, time_cnt, err_info1, err_info2)	(((class_no) << 24) | ((err_level) << 22) | (((time_cnt) & 0x3F)<<16) | ((err_info1)<<8) | (err_info2))

/// MSAP�G���[���x��
typedef enum {
	MSAP_ERR_LEVEL_0 = 0,				//!< ��n�Ǖ\���̂ݗL���ł�
	MSAP_ERR_LEVEL_1 = 1,				//!< �@�͔̂�s�֎~�ł��B�܂��A��s���ɔ��������ꍇ�͒����ɒ�������K�v������܂��B
	MSAP_ERR_LEVEL_2 = 2,				//!< �o�b�e���[�c�ʒʒm�ȂǁA���ӂ𑣂����x���̃G���[�ł��B
	MSAP_ERR_LEVEL_3 = 3				//!< ���ɖ��ł͂���܂���B���[�U�[���C�ӂŏo�͂��܂��B
} MSAP_ERR_LEVEL_T;

/// MSAP�G���[
typedef enum {
	MSAP_ERR_LOWER_MCU					= 0x01,				//!< ���ʃ}�C�R���ُ�
	MSAP_ERR_SBUS						= 0x02,				//!< SBUS�ُ�
	MSAP_ERR_DATALINK_TRANSPARENT		= 0x03,				//!< �ėp�i���ڐڑ��^���߃^�C�v�jXbee�f�o�C�X�ُ�
	MSAP_ERR_DATALINK_FEP				= 0x04,				//!< Futaba FEP�V���[�Y �f�o�C�X�ُ�
	MSAP_ERR_BATTERY_MONITOR_L			= 0x05,				//!< ���ʃ}�C�R���o�b�e���d���ُ�
	MSAP_ERR_BATTERY_MONITOR_H			= 0x06,				//!< ��ʃ}�C�R���o�b�e���d���ُ�
	MSAP_ERR_ESC_1						= 0x07,				//!< ESC1�ُ�
	MSAP_ERR_ESC_2						= 0x08,				//!< ESC2�ُ�
	MSAP_ERR_ESC_3						= 0x09,				//!< ESC3�ُ�
	MSAP_ERR_ESC_4						= 0x0A,				//!< ESC4�ُ�
	MSAP_ERR_ESC_5						= 0x0B,				//!< ESC5�ُ�
	MSAP_ERR_ESC_6						= 0x0C,				//!< ESC6�ُ�
	MSAP_ERR_SDCARD						= 0x0D,				//!< SD�J�[�h�ُ�
	MSAP_ERR_CANBUS						= 0x0E,				//!< CAN Bus�ُ�
	MSAP_ERR_SYSTEM						= 0x10,				//!< ��ʃ}�C�R���V�X�e���ُ�
	MSAP_ERR_HARDWARE_MINISVSIM			= 0x20,				//!< �V�~�����[�^�p�[���Z���T�ُ�
	MSAP_ERR_HARDWARE_MTI				= 0x21,				//!< Xsens MTi �ُ�                --- SCI2        ---  IMU
	MSAP_ERR_HARDWARE_IMU05				= 0x22,				//!< HIROBO IMU05 �ُ�             --- SCI2        ---  IMU
	MSAP_ERR_HARDWARE_OEMV				= 0x23,				//!< Novatel OEMV �ُ�             --- SCI3        ---  GPS�i�������E�t���[�Y�����肠��D��H�H�j
	MSAP_ERR_HARDWARE_UBLOX5			= 0x24,				//!< u-blox5 �ُ�                  --- SCI3        ---  GPS
	MSAP_ERR_HARDWARE_UBLOX6			= 0x25,				//!< u-blox6 �ُ�                  --- SCI3        ---  GPS
	MSAP_ERR_HARDWARE_MOTIONCAPTURE		= 0x26,				//!< Ӱ��ݷ���������(��p���) �ُ�--- SCI3        ---  GPS, (�p��)
	MSAP_ERR_HARDWARE_NAV_VIA_MAINLINK	= 0x27,				//!< Ӱ��ݷ���������(�ް��ݸ)  �ُ�--- MainLink    ---  GPS, (�p��)�C(�����x)
	MSAP_ERR_HARDWARE_SLAM_PC			= 0x28,				//!< SLAM-PC �ُ�                  --- SCI3        ---  GPS, ����, ����
	MSAP_ERR_HARDWARE_SCP1000_D01		= 0x29,				//!< VTI SCP1000-D01 �ُ�          --- PD0~3       ---  �C���Z���T
	MSAP_ERR_HARDWARE_MS5803_01BA		= 0x2A,				//!< m.s. MS5803-01BA �ُ�         --- PD0~3, CMT0 ---  �C���Z���T
	MSAP_ERR_HARDWARE_LV_MAXSONAR_EZ1	= 0x2B,				//!< MaxBotix LV-MaxSonar-EZ1 �ُ� --- AN5         ---  �O�E�Z���T�i�����j
	MSAP_ERR_DATALINK_PROTOCOL_LEGACY	= 0x40,				//!< ��X-Monitor�p�v���g�R���ُ�
	MSAP_ERR_DATALINK_PROTOCOL_2G		= 0x41,				//!< ��񐢑�v���g�R���ُ�
	MSAP_ERR_FLIGHTLOGGER				= 0x42,				//!< ��s���O�@�\�ُ�
	MSAP_ERR_MSAP_START                 = 0xFF				//!< �I�[�g�p�C���b�g�J�n���ɏo��
} MSAP_ERR_NUM_T;

/// MSAP�G���[�o�̓^�C�~���O(1�J�E���g�F20ms)
#define MSAP_ERR_COUNT_LOWER_MCU						1500		//!< ���ʃ}�C�R���ُ�
#define MSAP_ERR_COUNT_SBUS								1500		//!< SBUS�ُ�
#define MSAP_ERR_COUNT_DATALINK_TRANSPARENT				1500		//!< �ėp�i���ڐڑ��^���߃^�C�v�jXbee�f�o�C�X�ُ�
#define MSAP_ERR_COUNT_DATALINK_FEP						1500		//!< Futaba FEP�V���[�Y �f�o�C�X�ُ�
#define MSAP_ERR_COUNT_BATTERY_MONITOR_L				1500		//!< ���ʃ}�C�R���o�b�e���d���ُ�
#define MSAP_ERR_COUNT_BATTERY_MONITOR_H				1500		//!< ��ʃ}�C�R���o�b�e���d���ُ�
#define MSAP_ERR_COUNT_ESC_1							1500		//!< ESC1�ُ�
#define MSAP_ERR_COUNT_ESC_2							1500		//!< ESC2�ُ�
#define MSAP_ERR_COUNT_ESC_3							1500		//!< ESC3�ُ�
#define MSAP_ERR_COUNT_ESC_4							1500		//!< ESC4�ُ�
#define MSAP_ERR_COUNT_ESC_5							1500		//!< ESC5�ُ�
#define MSAP_ERR_COUNT_ESC_6							1500		//!< ESC6�ُ�
#define MSAP_ERR_COUNT_SDCARD							1500		//!< SD�J�[�h�ُ�
#define MSAP_ERR_COUNT_CANBUS							1500		//!< CAN Bus�ُ�
#define MSAP_ERR_COUNT_SYSTEM							1500		//!< ��ʃ}�C�R���V�X�e���ُ�
#define MSAP_ERR_COUNT_HARDWARE_MINISVSIM				1500		//!< �V�~�����[�^�p�[���Z���T�ُ�
#define MSAP_ERR_COUNT_HARDWARE_MTI						1500		//!< Xsens MTi �ُ�                --- SCI2        ---  IMU
#define MSAP_ERR_COUNT_HARDWARE_IMU05					1500		//!< HIROBO IMU05 �ُ�             --- SCI2        ---  IMU
#define MSAP_ERR_COUNT_HARDWARE_OEMV					1500		//!< Novatel OEMV �ُ�             --- SCI3        ---  GPS�i�������E�t���[�Y�����肠��D��H�H�j
#define MSAP_ERR_COUNT_HARDWARE_UBLOX5					1500		//!< u-blox5 �ُ�                  --- SCI3        ---  GPS
#define MSAP_ERR_COUNT_HARDWARE_UBLOX6					1500		//!< u-blox6 �ُ�                  --- SCI3        ---  GPS
#define MSAP_ERR_COUNT_HARDWARE_MOTIONCAPTURE			1500		//!< Ӱ��ݷ���������(��p���) �ُ�--- SCI3        ---  GPS, (�p��)
#define MSAP_ERR_COUNT_HARDWARE_NAV_VIA_MAINLINK		1500		//!< Ӱ��ݷ���������(�ް��ݸ)  �ُ�--- MainLink    ---  GPS, (�p��)�C(�����x)
#define MSAP_ERR_COUNT_HARDWARE_SLAM_PC					1500		//!< SLAM-PC �ُ�                  --- SCI3        ---  GPS, ����, ����
#define MSAP_ERR_COUNT_HARDWARE_SCP1000_D01				1500		//!< VTI SCP1000-D01 �ُ�          --- PD0~3       ---  �C���Z���T
#define MSAP_ERR_COUNT_HARDWARE_MS5803_01BA				1500		//!< m.s. MS5803-01BA �ُ�         --- PD0~3, CMT0 ---  �C���Z���T
#define MSAP_ERR_COUNT_HARDWARE_LV_MAXSONAR_EZ1			1500		//!< MaxBotix LV-MaxSonar-EZ1 �ُ� --- AN5         ---  �O�E�Z���T�i�����j
#define MSAP_ERR_COUNT_DATALINK_PROTOCOL_LEGACY			1500		//!< ��X-Monitor�p�v���g�R���ُ�
#define MSAP_ERR_COUNT_DATALINK_PROTOCOL_2G				1500		//!< ��񐢑�v���g�R���ُ�
#define MSAP_ERR_COUNT_FLIGHTLOGGER						1500		//!< ��s���O�@�\�ُ�
#define MSAP_ERR_COUNT_MSAP_START						 250		//!< �I�[�g�p�C���b�g�J�n���ɏo��

#ifdef __cplusplus
}
#endif

#endif	// INCLUDED_MSAP_ERROR_H

