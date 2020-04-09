///
/// @file       MultiwaypointData.h
///
/// @brief      マルチウェイポイント関連データ定義
///
/// マルチウェイポイント飛行を行う際に扱うデータの定義
///


#ifndef INCLUDED_MULTIWAYPOINTDATA_H
#define INCLUDED_MULTIWAYPOINTDATA_H

#include "Common/StandardDef.h"

///
/// @brief ウェイポイント到達/出発時にコールバックする関数の定義
///
typedef void (*WaypointCallbackFunc)(void* pUser, int WaypointIndex, _UDWORD func_id);

///
/// @brief マルチウェイポイント要素データ
///
struct MultiWaypointElement
{
	_UDWORD WaypointID; //!< 重複受信を防ぐためのID．ただし，WaypointID = 0 のパケットを受信したらウェイポイントリストをクリアする
	double  RefLat;     //!< ウェイポイントの緯度
	double  RefLon;     //!< ウェイポイントの経度
	float   RefAlt;     //!< ウェイポイントの高度
	float   RefYaw;     //!< ウェイポイント到達時の方位
	float   speed;      //!< ウェイポイントへ向かう際の飛行速度
	float   wait;       //!< ウェイポイントでの待機時間
	float   accuracy;   //!< ウェイポイント到達判定円半径
	_UDWORD func_id;    //!< コマンドID

	MultiWaypointElement() : WaypointID(0), RefLat(0.0), RefLon(0.0), RefAlt(0.0), RefYaw(0.0), speed(0.0), wait(0.0), accuracy(0.0), func_id(0) {}
	MultiWaypointElement(const ::MultiWaypointElement& obj) : WaypointID(obj.WaypointID), RefLat(obj.RefLat), RefLon(obj.RefLon), RefAlt(obj.RefAlt), RefYaw(obj.RefYaw),
		speed(obj.speed), wait(obj.wait), accuracy(obj.accuracy), func_id(obj.func_id) {}
};



#endif