//
//
//		Commondef_High.h
//
//		ミニサーベイヤープロジェクト共通定義
//		Higher-Level MCU (C++), GCS (C++) 共通
//
//

#ifndef INCLUDED_COMMONDEF_HIGH_H
#define INCLUDED_COMMONDEF_HIGH_H

#include "Commondef_Low.h"

//**************************************************
//	Constant Values
//**************************************************

// ------------------------------
//	RC Signal Range Definition
// ------------------------------


// ------------------------------
//	System
// ------------------------------


//**************************************************
//	Data type
//**************************************************

// 下位制御用MCUの情報
#ifndef CPP_CLI
struct LowLevelMCUData 
{
public:	
	_UBYTE              NumOfRotors;       // ロータ数
	CScaledControlData  ScaledControlData; // 制御入力データ
	
	CSysState           System;            // システム状態データ

	LowLevelMCUData() : NumOfRotors(0)
	{
	}
};
#else
pubric ref struct LowLevelMCUData
{
public:
	_UBYTE              NumOfRotors;       // ロータ数
	CScaledControlData  ScaledControlData; // 制御入力データ
	
	CSysState           System;            // システム状態データ

	LowLevelMCUData() : NumOfRotors(0)
	{
	}
	LowLevelMCUData(const ::LowLevelMCUData& obj) {}
};

#endif





// 目標値データ
struct RefData 
{
	double RefRoll, RefPitch, RefYaw;  // Attitude Reference [rad]
	double RefVelY, RefVelX,  RefVelZ; // Velocity Reference [m/s]
	double RefLat,  RefLon,   RefAlt;  // Position Reference [deg, deg, m]
	double RefX, RefY;
	
	// ウェイポイント飛行用
	double FlightSpeed; // 最大飛行速度 [m/s]
	double Radius;      // 到達判定半径 [m]
	
	RefData() : RefRoll(0), RefPitch(0), RefYaw(0), RefVelY(0), RefVelX(0), RefVelZ(0), RefLat(0), RefLon(0), RefAlt(0), FlightSpeed(0), Radius(0) {}
	RefData(const RefData& obj) : RefRoll(obj.RefRoll), RefPitch(obj.RefPitch), RefYaw(obj.RefYaw), RefVelY(obj.RefVelY), RefVelX(obj.RefVelX), RefVelZ(obj.RefVelZ),
		RefLat(obj.RefLat), RefLon(obj.RefLon), RefAlt(obj.RefAlt), FlightSpeed(obj.FlightSpeed), Radius(obj.Radius) {}
};

// ウェイポイントデータ
struct Waypoint
{
	double lat; //!< 緯度 [deg]
	double lon; //!< 経度 [deg]
	double alt; //!< 高度 [m]
	double yaw; //!< 方位 [rad]
	double spd; //!< 飛行速度 [m/s]

	Waypoint() : lat(0), lon(0), alt(0), yaw(0), spd(0) {}
	Waypoint(double lat, double lon, double alt, double yaw, double spd) : lat(lat), lon(lon), alt(alt), yaw(yaw), spd(spd) {}
};

#endif