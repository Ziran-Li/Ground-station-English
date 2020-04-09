///
/// @file       MSAP_Error.h
///
/// @brief      エラー定義
///

#ifndef INCLUDED_MSAP_ERROR_H
#define INCLUDED_MSAP_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAKE_ERR_CODE(class_no, err_level, time_cnt, err_info1, err_info2)	(((class_no) << 24) | ((err_level) << 22) | (((time_cnt) & 0x3F)<<16) | ((err_info1)<<8) | (err_info2))

/// MSAPエラーレベル
typedef enum {
	MSAP_ERR_LEVEL_0 = 0,				//!< 基地局表示のみ有効です
	MSAP_ERR_LEVEL_1 = 1,				//!< 機体は飛行禁止です。また、飛行中に発生した場合は直ちに着陸する必要があります。
	MSAP_ERR_LEVEL_2 = 2,				//!< バッテリー残量通知など、注意を促すレベルのエラーです。
	MSAP_ERR_LEVEL_3 = 3				//!< 特に問題ではありません。ユーザーが任意で出力します。
} MSAP_ERR_LEVEL_T;

/// MSAPエラー
typedef enum {
	MSAP_ERR_LOWER_MCU					= 0x01,				//!< 下位マイコン異常
	MSAP_ERR_SBUS						= 0x02,				//!< SBUS異常
	MSAP_ERR_DATALINK_TRANSPARENT		= 0x03,				//!< 汎用（直接接続／透過タイプ）Xbeeデバイス異常
	MSAP_ERR_DATALINK_FEP				= 0x04,				//!< Futaba FEPシリーズ デバイス異常
	MSAP_ERR_BATTERY_MONITOR_L			= 0x05,				//!< 下位マイコンバッテリ電圧異常
	MSAP_ERR_BATTERY_MONITOR_H			= 0x06,				//!< 上位マイコンバッテリ電圧異常
	MSAP_ERR_ESC_1						= 0x07,				//!< ESC1異常
	MSAP_ERR_ESC_2						= 0x08,				//!< ESC2異常
	MSAP_ERR_ESC_3						= 0x09,				//!< ESC3異常
	MSAP_ERR_ESC_4						= 0x0A,				//!< ESC4異常
	MSAP_ERR_ESC_5						= 0x0B,				//!< ESC5異常
	MSAP_ERR_ESC_6						= 0x0C,				//!< ESC6異常
	MSAP_ERR_SDCARD						= 0x0D,				//!< SDカード異常
	MSAP_ERR_CANBUS						= 0x0E,				//!< CAN Bus異常
	MSAP_ERR_SYSTEM						= 0x10,				//!< 上位マイコンシステム異常
	MSAP_ERR_HARDWARE_MINISVSIM			= 0x20,				//!< シミュレータ用擬似センサ異常
	MSAP_ERR_HARDWARE_MTI				= 0x21,				//!< Xsens MTi 異常                --- SCI2        ---  IMU
	MSAP_ERR_HARDWARE_IMU05				= 0x22,				//!< HIROBO IMU05 異常             --- SCI2        ---  IMU
	MSAP_ERR_HARDWARE_OEMV				= 0x23,				//!< Novatel OEMV 異常             --- SCI3        ---  GPS（未完成・フリーズする問題あり．回路？）
	MSAP_ERR_HARDWARE_UBLOX5			= 0x24,				//!< u-blox5 異常                  --- SCI3        ---  GPS
	MSAP_ERR_HARDWARE_UBLOX6			= 0x25,				//!< u-blox6 異常                  --- SCI3        ---  GPS
	MSAP_ERR_HARDWARE_MOTIONCAPTURE		= 0x26,				//!< ﾓｰｼｮﾝｷｬﾌﾟﾁｬｼｽﾃﾑ(専用回線) 異常--- SCI3        ---  GPS, (姿勢)
	MSAP_ERR_HARDWARE_NAV_VIA_MAINLINK	= 0x27,				//!< ﾓｰｼｮﾝｷｬﾌﾟﾁｬｼｽﾃﾑ(ﾃﾞｰﾀﾘﾝｸ)  異常--- MainLink    ---  GPS, (姿勢)，(加速度)
	MSAP_ERR_HARDWARE_SLAM_PC			= 0x28,				//!< SLAM-PC 異常                  --- SCI3        ---  GPS, 方位, 距離
	MSAP_ERR_HARDWARE_SCP1000_D01		= 0x29,				//!< VTI SCP1000-D01 異常          --- PD0~3       ---  気圧センサ
	MSAP_ERR_HARDWARE_MS5803_01BA		= 0x2A,				//!< m.s. MS5803-01BA 異常         --- PD0~3, CMT0 ---  気圧センサ
	MSAP_ERR_HARDWARE_LV_MAXSONAR_EZ1	= 0x2B,				//!< MaxBotix LV-MaxSonar-EZ1 異常 --- AN5         ---  外界センサ（距離）
	MSAP_ERR_DATALINK_PROTOCOL_LEGACY	= 0x40,				//!< 旧X-Monitor用プロトコル異常
	MSAP_ERR_DATALINK_PROTOCOL_2G		= 0x41,				//!< 第二世代プロトコル異常
	MSAP_ERR_FLIGHTLOGGER				= 0x42,				//!< 飛行ログ機能異常
	MSAP_ERR_MSAP_START                 = 0xFF				//!< オートパイロット開始時に出力
} MSAP_ERR_NUM_T;

/// MSAPエラー出力タイミング(1カウント：20ms)
#define MSAP_ERR_COUNT_LOWER_MCU						1500		//!< 下位マイコン異常
#define MSAP_ERR_COUNT_SBUS								1500		//!< SBUS異常
#define MSAP_ERR_COUNT_DATALINK_TRANSPARENT				1500		//!< 汎用（直接接続／透過タイプ）Xbeeデバイス異常
#define MSAP_ERR_COUNT_DATALINK_FEP						1500		//!< Futaba FEPシリーズ デバイス異常
#define MSAP_ERR_COUNT_BATTERY_MONITOR_L				1500		//!< 下位マイコンバッテリ電圧異常
#define MSAP_ERR_COUNT_BATTERY_MONITOR_H				1500		//!< 上位マイコンバッテリ電圧異常
#define MSAP_ERR_COUNT_ESC_1							1500		//!< ESC1異常
#define MSAP_ERR_COUNT_ESC_2							1500		//!< ESC2異常
#define MSAP_ERR_COUNT_ESC_3							1500		//!< ESC3異常
#define MSAP_ERR_COUNT_ESC_4							1500		//!< ESC4異常
#define MSAP_ERR_COUNT_ESC_5							1500		//!< ESC5異常
#define MSAP_ERR_COUNT_ESC_6							1500		//!< ESC6異常
#define MSAP_ERR_COUNT_SDCARD							1500		//!< SDカード異常
#define MSAP_ERR_COUNT_CANBUS							1500		//!< CAN Bus異常
#define MSAP_ERR_COUNT_SYSTEM							1500		//!< 上位マイコンシステム異常
#define MSAP_ERR_COUNT_HARDWARE_MINISVSIM				1500		//!< シミュレータ用擬似センサ異常
#define MSAP_ERR_COUNT_HARDWARE_MTI						1500		//!< Xsens MTi 異常                --- SCI2        ---  IMU
#define MSAP_ERR_COUNT_HARDWARE_IMU05					1500		//!< HIROBO IMU05 異常             --- SCI2        ---  IMU
#define MSAP_ERR_COUNT_HARDWARE_OEMV					1500		//!< Novatel OEMV 異常             --- SCI3        ---  GPS（未完成・フリーズする問題あり．回路？）
#define MSAP_ERR_COUNT_HARDWARE_UBLOX5					1500		//!< u-blox5 異常                  --- SCI3        ---  GPS
#define MSAP_ERR_COUNT_HARDWARE_UBLOX6					1500		//!< u-blox6 異常                  --- SCI3        ---  GPS
#define MSAP_ERR_COUNT_HARDWARE_MOTIONCAPTURE			1500		//!< ﾓｰｼｮﾝｷｬﾌﾟﾁｬｼｽﾃﾑ(専用回線) 異常--- SCI3        ---  GPS, (姿勢)
#define MSAP_ERR_COUNT_HARDWARE_NAV_VIA_MAINLINK		1500		//!< ﾓｰｼｮﾝｷｬﾌﾟﾁｬｼｽﾃﾑ(ﾃﾞｰﾀﾘﾝｸ)  異常--- MainLink    ---  GPS, (姿勢)，(加速度)
#define MSAP_ERR_COUNT_HARDWARE_SLAM_PC					1500		//!< SLAM-PC 異常                  --- SCI3        ---  GPS, 方位, 距離
#define MSAP_ERR_COUNT_HARDWARE_SCP1000_D01				1500		//!< VTI SCP1000-D01 異常          --- PD0~3       ---  気圧センサ
#define MSAP_ERR_COUNT_HARDWARE_MS5803_01BA				1500		//!< m.s. MS5803-01BA 異常         --- PD0~3, CMT0 ---  気圧センサ
#define MSAP_ERR_COUNT_HARDWARE_LV_MAXSONAR_EZ1			1500		//!< MaxBotix LV-MaxSonar-EZ1 異常 --- AN5         ---  外界センサ（距離）
#define MSAP_ERR_COUNT_DATALINK_PROTOCOL_LEGACY			1500		//!< 旧X-Monitor用プロトコル異常
#define MSAP_ERR_COUNT_DATALINK_PROTOCOL_2G				1500		//!< 第二世代プロトコル異常
#define MSAP_ERR_COUNT_FLIGHTLOGGER						1500		//!< 飛行ログ機能異常
#define MSAP_ERR_COUNT_MSAP_START						 250		//!< オートパイロット開始時に出力

#ifdef __cplusplus
}
#endif

#endif	// INCLUDED_MSAP_ERROR_H

