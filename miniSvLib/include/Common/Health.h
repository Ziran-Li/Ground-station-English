///
/// @file       Health.h
///
/// @brief      ヘルスチェック関連の定義
///
///             健全度の値はすべてのデバイス・モジュールで共通仕様とし，0～15の
///             範囲で変化するものとする．値の意味は次の通り：
///             0 は機能が停止している状態
///             1～4 は機能に深刻な異常が生じている状態
///             5～9 は機能が正常の範囲外にあり注意が必要な状態
///             10～15 は機能が正常に動作している状態
///
///             具体的な値の計算は以上のルールを守った上でデバイス毎に任意とする
///
/// @author     iwakura
///

#ifndef INCLUDED_HEALTHCHECKSYSTEM_H
#define INCLUDED_HEALTHCHECKSYSTEM_H

#include "Common/StandardDef.h"

// ------------------------------
//	Health Data
// ------------------------------
typedef _UBYTE HEALTH_VALUE; //!< 健全度データの型（値：0～15）

typedef enum 
{
	HEALTH_VALUE_MIN                = 0,  //!< 健全度の最小値             
	HEALTH_VALUE_MAX                = 15, //!< 健全度の最大値
	HEALTH_VALUE_WARNINGLEVEL       = 9,  //!< この値以下は＜注意＞を意味
	HEALTH_VALUE_EMERGENCYLEVEL     = 4,  //!< この値以下は＜警告・緊急＞を意味
	HEALTH_CPUUSEAGE_WARNINGLEVEL   = 90, //!< CPU使用率はこの値以上で＜注意＞
	HEALTH_CPUUSEAGE_EMERGENCYLEVEL = 95, //!< CPU使用率はこの値以上で＜警告＞
} HEALTHDATA_DEFINITION;


///
/// @brief 下位制御MCUの健全度情報
///
typedef struct _tag_LowLvMcuHealthData
{
	HEALTH_VALUE CpuUseage;                 // CPU使用率 [%]
	HEALTH_VALUE RcReceiverCommHealth;      // RCレシーバ-MCU間通信健全度        (0～15)
	HEALTH_VALUE RcReceiverReceptionHealth; // RCレシーバ受信健全度              (0～15)
	HEALTH_VALUE GyroCommHealth;            // ジャイロセンサ-MCU間通信健全度    (0～15)
	HEALTH_VALUE GyroMeasurementHealth;     // ジャイロセンサ計測健全度          (0～15)
	HEALTH_VALUE McuCommHealth;             // MCU-MCU間通信健全度（Low <- High）(0～15)

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
/// @brief 上位制御MCUの健全度情報
///
#ifdef __cplusplus
struct HighLvMcuHealthData
{
	float  BatteryVoltage;           // バッテリー電圧（全電圧）
	_UWORD HeapAllocated;            // ヒープ領域確保済みサイズ
	_UWORD StackAllocated;           // スタック領域確保済みサイズ
	_UBYTE CpuUseage;                // CPU使用率
	_UBYTE ImuCommHealth;            // IMUセンサ-MCU間通信健全度              (0～15)
	_UBYTE ImuGyroMeasurementHealth; // ジャイロセンサ計測健全度               (0～15)
	_UBYTE ImuAccMeasurementHealth;  // 加速度センサ計測健全度                 (0～15)
	_UBYTE ImuMagMeasurementHealth;  // 磁気センサ計測健全度                   (0～15)
	_UBYTE PsCommHealth;             // 気圧センサ-MCU間通信健全度             (0～15)
	_UBYTE PsMeasurementHealth;      // 気圧センサ計測健全度                   (0～15)
	_UBYTE GpsCommHealth;            // GPSセンサ-MCU間通信健全度              (0～15)
	_UBYTE GpsMeasurementHealth;     // GPS計測健全度                          (0～15)
	_UBYTE McuCommHealth;            // MCU-MCU間通信健全度（Low -> High）     (0～15)
	_UBYTE MainLinkCommHealth;       // UAV-基地局間通信健全度（UAV <- 基地局）(0～15)

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