///
/// @file       Commondef_MSTS.h
///
/// @brief      ミニサーベイヤー＆トータルステーションシステム共通定義
///
/// Higher-Level MCU (C++) と MSTS Monitor で共有する定義
///

#ifndef INCLUDED_COMMONDEF_MSTS_H
#define INCLUDED_COMMONDEF_MSTS_H

#include "Common/StandardDef.h"

namespace MSTS
{
	enum
	{
		PACKETSIZE_CONSTANT = 5,
		PACKETSIZE_USER1    = 33 + PACKETSIZE_CONSTANT,
	};

	///
	/// @brief  MSTS Monitor から UAV に送るデータの構造体
	///
	struct GcsData
	{
		double Distance;     //!< 計測距離 [m]
		double Pitch;        //!< トータルステーションの仰角 [rad] 
		double Yaw;          //!< トータルステーションの方位 [rad]（磁気方位）
		double Latitude;     //!< 緯度 [deg]
		double Longitude;    //!< 経度 [deg]
		double Altitude;     //!< 高度 [m]
		double DistanceRate; //!< 計測距離の時間変化速度 [m/s]
		double PitchRate;    //!< ピッチ角の時間変化速度 [rad/s]
		double YawRate;      //!< ヨー角の時間変化速度 [rad/s]

		_UWORD Status;
	};

	///
	/// @brief  フラグ（GCS->UAV)
	///
	enum GCSSTATUS
	{
		STATUS_TRACKING_OK         = 0x0001,
		STATUS_GCS_GPS_ACCURACY_OK = 0x0002,
	};

} // end of namespace MSTS

#endif