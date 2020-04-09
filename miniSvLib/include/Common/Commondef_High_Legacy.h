//
//		Commondef_High_Legacy.h
//
//		旧システム用ミニサーベイヤープロジェクト共通定義
//

#ifndef INCLUDED_COMMONDEF_HIGH_LEGACY_H
#define INCLUDED_COMMONDEF_HIGH_LEGACY_H

// プロポの情報（プロポの仕様に対応）
struct PropoData
{
	unsigned int PRoll;  // Aileron      - 1000(  ) ～ 2000(    ), Neutral : 1520
	unsigned int PPitch; // Elevator     - 1000(  ) ～ 2000(    ), Neutral : 1520
	unsigned int PPower; // Thrust       - 1000(UP) ～ 2000(DOWN), Neutral : 1520
	unsigned int PYaw;   // Rudder       - 1000     ～ 2000      , Neutral : 1520
	int Sw1, Sw2, Sw3, Sw4; // Switch State - 0:OFF / 1:ON

	PropoData() : PRoll(1520), PPitch(1520), PPower(1520), PYaw(1520), Sw1(0), Sw2(0), Sw3(0), Sw4(0) {}
	PropoData(const ::PropoData& obj) : PRoll(obj.PRoll), PPitch(obj.PPitch), PPower(obj.PPower), PYaw(obj.PYaw), Sw1(obj.Sw1), Sw2(obj.Sw2), Sw3(obj.Sw3), Sw4(obj.Sw4) {}
};

//*******************************************************************************
//	各種定数の定義
//*******************************************************************************

// 制御モード (0～7)
enum CONTROLMODE
{
	CONTROLMODE_UNDEFINED         = 0, // 未定義状態
	CONTROLMODE_MANUAL            = 1, // マニュアル操縦
	CONTROLMODE_ASSISTEDMANUAL    = 2, // 姿勢アシストマニュアル
	CONTROLMODE_AUTOMATICCONTROL  = 3, // 自律制御（フライトモードで飛行）
	CONTROLMODE_EMERGENCY_LANDING = 6, // 緊急着陸モード
	CONTROLMODE_FAILSAFE          = 7, // 通信フェイルセーフ
};

// フライトモード (0～31)
enum FLIGHTMODE 
{
	FLIGHTMODE_UNDEFINED          = 0,  // 未定義/不明
	FLIGHTMODE_AUTOMATIC          = 1,  // 完全自律
	FLIGHTMODE_VELOCITY_CONTROL   = 2,  // 速度制御（一部マニュアル）
	FLIGHTMODE_AUTOMATIC_TAKEOFF  = 3,  // 自動離陸 (→AUTOMATIC)
	FLIGHTMODE_AUTOMATIC_LANDING  = 4,  // 自動着陸
	FLIGHTMODE_START_STOP         = 6,  // プロペラ始動/停止 (→NEUTRAL)
	FLIGHTMODE_NEUTRAL            = 7,  // 待機
	FLIGHTMODE_MULTIWAYPOINT      = 8,  // マルチウェイポイント飛行
	FLIGHTMODE_TEST               = 9,  // テストモード
	FLIGHTMODE_OBSTACLE_AVOIDANCE = 20, // 障害物回避
	FLIGHTMODE_USER1              = 27, // ユーザー定義1
	FLIGHTMODE_USER2              = 28, // ユーザー定義2
	FLIGHTMODE_USER3              = 29, // ユーザー定義3
	FLIGHTMODE_USER4              = 30, // ユーザー定義4
	FLIGHTMODE_USER5              = 31, // ユーザー定義5
};
#define IS_USERFLIGHTMODE(FMODE) ((FMODE) >= (FLIGHTMODE)20) // ユーザー予約かどうか

#define FLIGHTMODETEXT_UNKNOWN           "不明"
#define FLIGHTMODETEXT_UNDEFINED         "未定義"
#define FLIGHTMODETEXT_AUTOMATIC         "完全自律"
#define FLIGHTMODETEXT_VELOCITY_CONTROL  "速度制御"
#define FLIGHTMODETEXT_AUTOMATIC_TAKEOFF "自動離陸"
#define FLIGHTMODETEXT_AUTOMATIC_LANDING "自動着陸"
#define FLIGHTMODETEXT_START_STOP        "プロペラ始動／停止"
#define FLIGHTMODETEXT_NEUTRAL           "待機"
#define FLIGHTMODETEXT_MULTIWAYPOINT     "マルチウェイポイント"
#define FLIGHTMODETEXT_TEST              "開発用"
#define FLIGHTMODETEXT_USER1             "ユーザー定義1"
#define FLIGHTMODETEXT_USER2             "ユーザー定義2"
#define FLIGHTMODETEXT_USER3             "ユーザー定義3"
#define FLIGHTMODETEXT_USER4             "ユーザー定義4"
#define FLIGHTMODETEXT_USER5             "ユーザー定義5"

#endif