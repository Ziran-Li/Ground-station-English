///
///
/// @file       Commondef_Sim.h
///
/// @brief      ミニサーベイヤープロジェクト共通定義
///
///             Lower-Level MCU (C), Higher-Level MCU (C++), MiniSvSim (C++) 共通
///
///

#ifndef INCLUDED_COMMONDEF_SIM_H
#define INCLUDED_COMMONDEF_SIM_H

#include "BasicConfiguration.h"
#include <wchar.h>

#ifndef INCLUDED_TYPEDEFINE_H
#	include "CompatibleDefine/typedefine.h"
#endif


///
/// アドレスとUDPポート
///
#define ADDR_WINMINISV    "127.0.0.1"   //!< IPアドレス （WinMinisurveyor）
#define ADDR_SOLVER       "127.0.0.1"   //!< IPアドレス （ソルバ）
//#define ADDR_GUI          "10.39.22.71" //!< IPアドレス （GUI）
#define ADDR_GUI          "169.254.98.194" //!< IPアドレス （GUI）
//#define ADDR_GUI          "127.0.0.1" //!< IPアドレス （GUI）
#define PORT_LOW2SOLVER   53901         //!< UDPポート（下位MCU → ソルバ ）
#define PORT_SOLVER2LOW   53902         //!< UDPポート（ソルバ  → 下位MCU）
#define PORT_HIGH2SOLVER  53910         //!< UDPポート（上位MCU → ソルバ ）
#define PORT_SOLVER2HIGH  53907         //!< UDPポート（ソルバ  → 上位MCU）
#define PORT_GUI2SOLVER   53908         //!< UDPポート（GUI     → ソルバ ）
#define PORT_SOLVER2GUI   53909         //!< UDPポート（ソルバ  → GUI    ）


///
/// パケットのヘッダとフッタ
///
#define MINISVSIMPACKET_HEADER 0xAAAA //!< パケット共通ヘッダ
#define MINISVSIMPACKET_FOOTER 0x5555 //!< パケット共通フッタ


///
/// コマンド（GUI→ソルバ）
///
#define COMMAND_END_SIMULATION  1 //!< シミュレータを終了する

///
/// @brief  ESCデータパケット（下位MCU→ソルバ）
///
#pragma pack(1)
typedef struct _tag_EscPacket
{
	uint16_t header;
	double  pwm[MAXIMUM_NUMBER_OF_ROTORS];
	uint16_t footer;

} EscPacket;
#pragma pack()


///
/// @brief  センサデータパケット（ソルバ→下位MCU）
///
#pragma pack(1)
typedef struct _tag_GyroPacket
{
	uint16_t header;
	double  wx;
	double  wy;
	double  wz;
	uint16_t footer;

} GyroPacket;
#pragma pack()


///
/// @brief  センサデータパケットデータ更新フラグ（ソルバ→上位MCU）
///
typedef enum
{
	MINISVSIM_FLAG_GYRO     = 0x0001, //!< ジャイロセンサ更新フラグ
	MINISVSIM_FLAG_ACC      = 0x0002, //!< 加速度センサ更新フラグ
	MINISVSIM_FLAG_MAG      = 0x0004, //!< 磁気センサ更新フラグ
	MINISVSIM_FLAG_ATT      = 0x0008, //!< 姿勢センサ更新フラグ
	MINISVSIM_FLAG_GPS      = 0x0010, //!< GPSセンサ更新フラグ
	MINISVSIM_FLAG_PS       = 0x0020, //!< 気圧センサ更新フラグ
	MINISVSIM_FLAG_GPSINFO  = 0x0040, //!< GPS情報更新フラグ
	MINISVSIM_FLAG_DISTANCE = 0x0080, //!< 距離データ更新フラグ
	MINISVSIM_FLAG_VOLTAGE  = 0x0100, //!< 電圧データ更新フラグ

} MINISVSIM_UPDATE_FLAG;


///
/// @brief  センサデータパケット（ソルバ→上位MCU）
///
#pragma pack(1)
typedef struct
{
	_UWORD  header;
	double  wX, wY, wZ;          //!< 3軸角速度
	double  aX, aY, aZ;          //!< 3軸加速度
	double  mX, mY, mZ;          //!< 3軸磁場
	double  phi, the, psi;       //!< 3軸姿勢
	double  q0, q1, q2, q3;      //!< クォータニオン
	double  vN, vE, vD;          //!< GPS速度
	double  lat, lon, alt;       //!< GPS位置
	double  Ps;                  //!< 気圧高度
	_UBYTE  NavWorking;          //!< GPS測位中
	_UBYTE  nSats;               //!< GPS衛星補足数
	double  hPosAcc, vPosAcc;    //!< GPS位置精度
	double  hVelAcc, vVelAcc;    //!< GPS速度精度
	_UBYTE  dist_Layout;         //!< 距離センサレイアウト
	_UWORD  dist[10];            //!< 下, 上, 水平8方向 距離データ
	_UWORD  dist_offset[10];     //!< 下, 上, 水平8方向 距離データ オフセット
	_UBYTE  dist_InRange[10];    //!< 下, 上, 水平8方向 距離データ 有効レンジ内フラグ
	float   voltage;             //!< セル平均電圧
	_UDWORD flagUpdated;         //!< 更新フラグ
	float   GpsDelay;            //!< GPS時間遅れ特性
	float   Imu_X, Imu_Y, Imu_Z; //!< IMU設置位置
	float   Mag_Pose;            //!< 磁気センサ設置姿勢
	float   Gps_X, Gps_Y, Gps_Z; //!< GPS設置位置
	float   Ps_X, Ps_Y, Ps_Z;    //!< 気圧センサ設置位置
	float   Cog_X, Cog_Y, Cog_Z; //!< 重心位置
	_UWORD  footer;

} HighSensorDataPacket;
#pragma pack()


///
/// @brief  制御システムデータパケット（上位MCU→ソルバ）
///
#pragma pack(1)
typedef struct
{
	_UWORD header;
	double CtrlT, CtrlR, CtrlP, CtrlY; //!< 4軸制御入力
	double RefRoll, RefPitch, RefYaw;  //!< 3軸姿勢目標値
	double RefVelX, RefVelY,  RefVelZ; //!< 3軸速度目標値
	double RefYawRate;                 //!< ヨー角速度目標値
	double RefX,    RefY,     RefZ;    //!< 3軸位置目標値
	_UWORD footer;

} HighMcuControlSystemDataPacket;
#pragma pack()


///
/// @brief  対GUI状態データパケット（ソルバ→GUI）
///
#pragma pack(1)
typedef struct
{
	_UWORD  header;
	_SDWORD PacketID;                    //!< ID
	float   q0, q1, q2, q3;              //!< クォータニオン
	float   Wx, Wy, Wz;                  //!< 3軸角速度
	float   x,  y,  z;                   //!< 3軸位置
	float   Vx, Vy, Vz;                  //!< 3軸速度
	float   N[MAXIMUM_NUMBER_OF_ROTORS]; //!< ロータ回転数
    wchar_t Message[64];                 //!< メッセージ
    float   GinbalAtt0_Phi;              //!< ジンバル角度0 θ
    float   GinbalAtt0_The;              //!< ジンバル角度0 φ
    float   GinbalAtt0_Psi;              //!< ジンバル角度0 ψ
    float   GinbalAtt1_Phi;              //!< ジンバル角度1 θ
    float   GinbalAtt1_The;              //!< ジンバル角度1 φ
    float   GinbalAtt1_Psi;              //!< ジンバル角度1 ψ
    float   GinbalAtt2_Phi;              //!< ジンバル角度2 θ
    float   GinbalAtt2_The;              //!< ジンバル角度2 φ
    float   GinbalAtt2_Psi;              //!< ジンバル角度2 ψ
    float   GinbalAtt3_Phi;              //!< ジンバル角度3 θ
    float   GinbalAtt3_The;              //!< ジンバル角度3 φ
    float   GinbalAtt3_Psi;              //!< ジンバル角度3 ψ
    float   GinbalAtt4_Phi;              //!< ジンバル角度4 θ
    float   GinbalAtt4_The;              //!< ジンバル角度4 φ
    float   GinbalAtt4_Psi;              //!< ジンバル角度4 ψ
	_UWORD  footer;
} StateDataPacket;
#pragma pack()
#define STATEDATAPACKET_ID  1


///
/// @brief  衝突データアイテム
///
#pragma pack(1)
typedef struct
{
	float Xb;  //!< 衝突ポイント(x)
	float Yb;  //!< 衝突ポイント(y)
	float Zb;  //!< 衝突ポイント(z)
	float Vxb; //!< 衝突速度 (x)
	float Vyb; //!< 衝突速度 (y)
	float Vzb; //!< 衝突速度 (z)
	float nx;  //!< 法線ベクトル (x)
	float ny;  //!< 法線ベクトル (y)
	float nz;  //!< 法線ベクトル (z)
	float d;   //!< めりこんだ距離
	float F1;  //!< 静摩擦係数
	float F2;  //!< 動摩擦係数

} CollisionPoint;
#pragma pack()


///
/// @brief  対GUI状態衝突データパケット（GUI→ソルバ）
///
#pragma pack(1)
typedef struct
{
	_UWORD  header;
	_SDWORD PacketID;       //!< ID
    _UDWORD Command;        //!< コマンド
	_UBYTE  nPoint;         //!< 衝突点の数
	CollisionPoint* pPoint; //!< 衝突点データ（nPointの数だけある）
	_UBYTE  PropellerCollisionState[MAXIMUM_NUMBER_OF_ROTORS]; //!< プロペラに対する衝突状態（0:正常 255:完全破損）
	_UWORD  footer;

} CollisionDataPacket;
#pragma pack()
#define COLLISIONDATAPACKET_ID  1

#endif