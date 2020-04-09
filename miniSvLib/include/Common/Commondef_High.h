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

// システムデータ
struct SysData
{
	float  BatteryVoltage;     // バッテリー電圧     [V]
	_UBYTE CpuUtilizationRate; // CPU稼働率          [%]
	_UBYTE reserved1;          // 予約
	_UBYTE reserved2;          // 予約
	_UBYTE reserved3;          // 予約
	size_t HeapSize;           // ヒープ領域サイズ
	size_t StackSize;          // スタック領域サイズ
	size_t HeapAllocated;      // ヒープ領域確保済みサイズ
	size_t StackAllocated;     // スタック領域確保済みサイズ

	SysData() : BatteryVoltage(0), CpuUtilizationRate(0), HeapSize(0), StackSize(0), HeapAllocated(0), StackAllocated(0) {}
	SysData(const ::SysData& obj) : BatteryVoltage(obj.BatteryVoltage), CpuUtilizationRate(obj.CpuUtilizationRate),
		HeapSize(obj.HeapSize), StackSize(obj.StackSize), HeapAllocated(obj.HeapAllocated), StackAllocated(obj.StackAllocated) {}
};

// 制御対象への入力（プロポの仕様に対応）
struct CtrlData
{
	double Roll;  // Aileron      - 1000(  ) ～ 2000(    ), Neutral : 1520
	double Pitch; // Elevator     - 1000(  ) ～ 2000(    ), Neutral : 1520
	double Yaw;   // Rudder       - 1000     ～ 2000      , Neutral : 1520
	double Power; // Thrust       - 1000(UP) ～ 2000(DOWN), Neutral : 1520
};

// 修正を受ける前の目標値データ
struct RawRefData
{
	double Lat;     //!< 緯度
	double Lon;     //!< 経度
	double Alt;     //!< 高度

	double VelX;    //!< 速度(X)
	double VelY;    //!< 速度(Y)
	double VelZ;    //!< 速度(Z)
	double Yaw;     //!< 方位
	double YawRate; //!< 角速度(Z)

	RawRefData() : Lat(0), Lon(0), Alt(0), VelX(0), VelY(0), VelZ(0), Yaw(0), YawRate(0) {}
};

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

	bool operator == (const Waypoint& obj)
	{
		return (lat == obj.lat) && (lon == obj.lon) && (alt == obj.alt) && (yaw == obj.yaw) && (spd == obj.spd);
	}
};

// 位置データ
struct PosData
{
	double lat; // 緯度 [deg]
	double lon; // 経度 [deg]
	double alt; // 高度 [m]   重力と逆の方向を正

	PosData() : lat(0), lon(0), alt(0) {}
	PosData(double latitude, double longitude, double altitude) : lat(latitude), lon(longitude), alt(altitude) {}
	PosData(const ::PosData& obj) : lat(obj.lat), lon(obj.lon), alt(obj.alt) {}

	void SetPosition(double latitude, double longitude, double altitude) { lat = latitude; lon = longitude; alt = altitude; }
};

// GPS情報
struct GPSInfo
{
	float  hPosAccuracy; // 水平面位置推定精度 [m]
	float  vPosAccuracy; // 高度位置推定精度   [m]
	float  hVelAccuracy; // 水平面速度推定精度 [m]
	float  vVelAccuracy; // 高度速度推定精度   [m]
	_UBYTE NavWorking;   // GPS有効(1) or 無効(0)
	_UBYTE nSat;         // 有効衛星数

	GPSInfo() : NavWorking(FALSE), nSat(0), hPosAccuracy(0), vPosAccuracy(0), hVelAccuracy(0), vVelAccuracy(0) {}
	GPSInfo(const ::GPSInfo& obj) : NavWorking(obj.NavWorking), nSat(obj.nSat), hPosAccuracy(obj.hPosAccuracy), vPosAccuracy(obj.vPosAccuracy),
		hVelAccuracy(obj.hVelAccuracy), vVelAccuracy(obj.vVelAccuracy) {}
};

#endif