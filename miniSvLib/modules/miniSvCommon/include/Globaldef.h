//
//	    Globaldef.h
//
//		構造体・マクロ・型定義
//
//		'08/01/06	Ver 1.00	初版                                             by S-Suzuki
//		~08/10/08	Ver 1.01	定数の定義をいろいろと追加                       by iwakura
//		'08/10/16	Ver 1.02	フライトモード追加                               by iwakura
//		'08/10/28	Ver 1.02.1	デバッグ用変数追加                               by iwakura
//		'09/01/22	Ver 1.03	パケット情報追加                                 by iwakura
//		'09/02/27	Ver 1.04	フライトモード追加(USER1~5)                      by iwakura
//		         	        	パケットID追加(USER1~5)
//		'09/03/12	Ver 1.05	MAVInfoの追加                                    by iwakura
//		'09/03/15	Ver 1.06	MAVInfoのメンバ追加                              by iwakura
//		'09/04/02	Ver 1.07	EstDataの追加
//		'09/09/11	Ver 1.08	ファイルの場所変更に伴い微修正                   by iwakura
//		         	        	構造体名の変更(MavData)
//		         	        	パケットID追加(MAV_CONDITION)
//		'09/09/23	Ver 1.09	PosData にコンストラクタ追加                     by iwakura
//		         	        	PosData に値設定用の関数追加
//		'09/12/21	Ver 1.09.1	ファイル名・ファイルの場所変更に伴い微修正       by iwakura
//		'10/01/04	Ver 1.10	パケットID追加 (PACKETID_ATTREF2, PACKETID_VELREF2, PACKETID_GPSREF2)
//		         	        	                                                 by iwakura
//		'10/01/05	Ver 1.11	PI, EXP, PACKETID_PROPO_SIMPLE を追加            by iwakura
//		'10/01/30	Ver 1.12	PACKETID_GENERALINT, PACKETID_GENERALREAL を規定 by iwakura
//		'10/02/07	Ver 1.13	XUFO_COMMANDをMAV_COMMAND に改名                 by iwakura
//								COMMAND_PING を追加
//		'10/05/20	Ver 1.14	構造体名の変更(PPmdata -> CtrlData)              by iwakura
//		'10/06/20	Ver 1.15	構造体・PACKETID・PACKETSIZEの名前変更           by iwakura
//								PropoDataを初期化するようにした
//		'11/02/10	Ver 1.16	gDebugの初期化タイミングを変更                   by iwakura
//		∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽
//			ここから下のバージョンはプロジェクトのバージョン
//		∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽
//		'13/??/??	Ver 2.0.X	超いろいろと変更                                 by iwakura
//		'13/06/09	Ver 2.1.0	Data3D構造体を追加                               by iwakura
//		'12/07/06	Ver 2.1.1	EulerAngle, Quaternion構造体を追加               by iwakura
//		'13/08/15	Ver 3.2.0	Commondef_High に移動：
//		         	         	    "LowLevelMCUData"
//		         	         	Commondef_Low に移動：
//		         	         	    "QUADWORD"
//
//

#ifndef INCLUDED_GLOBALDEF_H
#define INCLUDED_GLOBALDEF_H

#include "Common/StandardDef.h"

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

// 目標値データ
/*
__STRUCT RefData {
	double RefRoll, RefPitch, RefYaw;  // Attitude Reference [rad]
	double RefVelY, RefVelX,  RefVelZ; // Velocity Reference [m/s]
	double RefLat,  RefLon,   RefAlt;  // Position Reference [deg, deg, m]
	
	// ウェイポイント飛行用
	double FlightSpeed; // 最大飛行速度 [m/s]
	double Radius;      // 到達判定半径 [m]
	
	RefData() : RefRoll(0), RefPitch(0), RefYaw(0), RefVelY(0), RefVelX(0), RefVelZ(0), RefLat(0), RefLon(0), RefAlt(0), FlightSpeed(0), Radius(0) {}
	RefData(const ::RefData& obj) : RefRoll(obj.RefRoll), RefPitch(obj.RefPitch), RefYaw(obj.RefYaw), RefVelY(obj.RefVelY), RefVelX(obj.RefVelX), RefVelZ(obj.RefVelZ),
		RefLat(obj.RefLat), RefLon(obj.RefLon), RefAlt(obj.RefAlt), FlightSpeed(obj.FlightSpeed), Radius(obj.Radius) {}
};
*/

// 修正を受ける前の目標値データ
struct RawRefData 
{
	double RefYaw; 
	double RefAlt;
	double RefVelX;
	double RefVelY;
	double RefVelZ;
	
	double RefLat;
	double RefLon;
	
	
	RawRefData() : RefYaw(0), RefAlt(0), RefVelX(0), RefVelY(0), RefVelZ(0) {}
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

// XYZ位置データ
struct XYZData 
{
	double x;
	double y;
	double z;

	XYZData() : x(0), y(0), z(0) {}
	XYZData(const ::XYZData& obj) : x(obj.x), y(obj.y), z(obj.z) {}
};

struct LocalPos : public XYZData
{
	LocalPos() {}
	LocalPos(const ::LocalPos& obj) : XYZData(obj) {}
};

// 制御対象への入力（プロポの仕様に対応）
struct CtrlData {
	double Roll;  // Aileron      - 1000(  ) ～ 2000(    ), Neutral : 1520
	double Pitch; // Elevator     - 1000(  ) ～ 2000(    ), Neutral : 1520
	double Yaw;   // Rudder       - 1000     ～ 2000      , Neutral : 1520
	double Power; // Thrust       - 1000(UP) ～ 2000(DOWN), Neutral : 1520
};

// 汎用状態ベクトル
class StateVector
{
protected:
	size_t  m_Length;
	
	double* m_pX;

public:
	StateVector();
	~StateVector();
	
	CRESULT SetLength(size_t length);                      // 領域確保用．
	size_t  GetLength() const;                             // ベクトル長の取得．
	double* GetPointer(unsigned int index, size_t length); // ポインタの取得．length が一致しない場合 NULL が返る．
	double* GetPointer(size_t length) { return GetPointer(0, length); }
};

// テストモードの構成データ
struct TestModeConfig
{
	enum {
		ENABLED_ROLL_PITCH_CONTROL = 0x01, // 姿勢制御ON             ※
		ENABLED_VELOCITY_CONTROL   = 0x02, // 速度・姿勢制御ON       ※どれか一つのみ設定可能
		ENABLED_POSITION_CONTROL   = 0x03, // 位置・速度・姿勢制御ON ※
		ENABLED_ALTITUDE_CONTROL   = 0x04, // 高度制御ON
		ENABLED_YAW_CONTROL        = 0x08, // 方位制御ON
		MASK_HORIZONTAL_CONTROL    = 0x03, // モード（姿勢/速度/位置）抽出用マスク
	};
	
	_UBYTE Mode;
	inline bool IsRollPitchControlEnabled() { return (Mode & MASK_HORIZONTAL_CONTROL) >= ENABLED_ROLL_PITCH_CONTROL ? true : false; }
	inline bool IsVelocityControlEnabled()  { return (Mode & MASK_HORIZONTAL_CONTROL) >= ENABLED_VELOCITY_CONTROL   ? true : false; }
	inline bool IsPositionControlEnabled()  { return (Mode & MASK_HORIZONTAL_CONTROL) >= ENABLED_POSITION_CONTROL   ? true : false; }
	inline bool IsAltitudeControlEnabled()  { return Mode & ENABLED_ALTITUDE_CONTROL ? true : false; }
	inline bool IsYawControlEnabled()       { return Mode & ENABLED_YAW_CONTROL  ? true : false; }
	
	TestModeConfig() : Mode(0) {}
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

#endif
