///
///
/// @file       SBusData.h
///
/// @brief      S.Busデータ構造定義
///
/// Lower-Level MCU (C), Higher-Level MCU (C++), GCS (C++) 共通
///
/// プロポデータの標準構造の定義
/// 値のスケールは取得元のデバイスに依存してよい
/// ただし，デバイス依存のスケールはカプセル化されたモジュール内のみにとどめ，
/// モジュール同士のデータ交換は必ずRCスケールで行うこと
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

#define SBUS_CH_STICK_START  1 // スティックに割り当てられるチャンネルの開始番号
#define SBUS_CH_STICK_END    4 // スティックに割り当てられるチャンネルの終了番号
#define SBUS_CH_AUX_START    5 // AUXチャンネルの開始番号

#define SBUS_CH_STICK_START_IDX  (SBUS_CH_STICK_START-1)
#define SBUS_CH_STICK_END_IDX    (SBUS_CH_STICK_END-1)
#define SBUS_CH_AUX_START_IDX    (SBUS_CH_AUX_START-1)

#define SBUS_VALUE_MAX   1000 // 仮
#define SBUS_VALUE_MID    750 // 仮
#define SBUS_VALUE_MIN    500 // 仮
#define SBUS_VALUE_WIDTH  500 // 仮


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