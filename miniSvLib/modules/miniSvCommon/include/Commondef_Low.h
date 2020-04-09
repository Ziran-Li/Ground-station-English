///
///
/// @file       Commondef_Low.h
///
/// @brief      ミニサーベイヤープロジェクト共通定義
///
///             Lower-Level MCU (C), Higher-Level MCU (C++), GCS (C++) 共通
///             Lower-Level MCU (C)が主体となる定義
///
///

#ifndef INCLUDED_COMMONDEF_LOW_H
#define INCLUDED_COMMONDEF_LOW_H

#include "Common/BasicConfiguration.h"
#include "Common/StandardDef.h"
#include "Common/Health.h"

//**************************************************
//	Constant Values & Type Definition
//**************************************************

// ------------------------------
//	Machine (Drive System)
// ------------------------------

typedef enum 
{
	ROTORLAYOUT_QUAD_TYPE_A,
	ROTORLAYOUT_QUAD_TYPE_B,
	ROTORLAYOUT_HEX_TYPE_A,
	ROTORLAYOUT_HEX_TYPE_B,
} ROTORLAYOUT;

// ------------------------------
//	RC Signal Range Definition
// ------------------------------

typedef _UWORD RcScaleValue;  //!< 一般RCサーボ信号の型定義    1520 +/- 500   (nominal)
typedef _UWORD ErcScaleValue; //!< 拡張RCサーボ信号の型定義   32000 +/- 16000 (nominal)

///
/// 拡張RCサーボ信号
///
#define ERC_MID   32000 //!< 中点 （RCスケールの 1520 と等価）
#define ERC_MIN   16000 //!< 下限 （RCスケールの 1020 と等価）
#define ERC_MAX   48000 //!< 上限 （RCスケールの 2020 と等価）
#define ERC_RANGE 16000 //!< 中点と上下限の差
#define ERC_WIDTH 32000 //!< 上限と下限の差

///
/// 一般RCサーボ信号(Futaba)
///
#define RC_MID   1520 //!< 中点
#define RC_MIN   1020 //!< 下限
#define RC_MAX   2020 //!< 上限
#define RC_RANGE  500 //!< 中点と上下限の差
#define RC_WIDTH 1000 //!< 上限と下限の差

#define RC_LOW   RC_MAX //!< スロットル位置（ロー）
#define RC_HIGH  RC_MIN //!< スロットル位置（ハイ）

///
/// 値変換マクロ
///
#define RC2ERC(X) (ErcScaleValue)(((_SWORD)(X) - 520) << 5) //!< 変換演算 General rc scale -> Extended scale
#define ERC2RC(X) (RcScaleValue)(((X) >> 5) + 520)          //!< 変換演算 Extended scale -> General rc scale

#define RC2SW(X)  ((X) > 1270 ? ((X) > 1770 ? 2 : 1) : 0)   //!< スイッチ値(0/1/2)への変換演算

///
/// プロポデータの初期値
///
#define RC_THROTTLE_INIT RC_LOW //!< スロットルの初期値
#define RC_RPY_INIT      RC_MID //!< エルロン，エレベータ，ラダーの初期値
#define RC_CH5_INIT      RC_MIN //!< Ch5の初期値
#define RC_CH6_INIT      RC_MIN //!< Ch6の初期値
#define RC_CH7_INIT      RC_MIN //!< Ch7の初期値
#define RC_CH8_INIT      RC_MIN //!< Ch8の初期値
#define RC_CH9_INIT      RC_MID //!< Ch9の初期値
#define RC_CH10_INIT     RC_MID //!< Ch10の初期値
#define RC_CH11_INIT     RC_MID //!< Ch11の初期値
#define RC_CH12_INIT     RC_MID //!< Ch12の初期値
#define RC_CH13_INIT     RC_MID //!< Ch13の初期値
#define RC_CH14_INIT     RC_MID //!< Ch14の初期値
#define RC_CH15_INIT     RC_MID //!< Ch15の初期値
#define RC_CH16_INIT     RC_MID //!< Ch16の初期値
#define RC_CH17_INIT     0      //!< Ch17の初期値
#define RC_CH18_INIT     0      //!< Ch18の初期値

///
/// スティック位置によるモード判定の閾値（設定値）
///
#define RC_THROTTLEBOTTOM_THRESHOLD     1920 //!< スロットルがローのポジションにあるかどうかの閾値
#define RC_RUDDERLEFT_THRESHOLD         1060 //!< ラダーがロータ始動位置にあるかどうかの閾値
#define RC_RUDDERRIGHT_THRESHOLD        1980 //!< ラダーがロータ停止位置にあるかどうかの閾値

#define RC_RUDDERMID_THRESHOLD_WIDTH     100 //!< ラダーが中央位置にあるかどうかの閾値（中央値からの相対値）
#define RC_ELEVATORMID_THRESHOLD_WIDTH   100 //!< エレベータが中央位置にあるかどうかの閾値（中央値からの相対値）
#define RC_AILERONMID_THRESHOLD_WIDTH    100 //!< エルロンが中央位置にあるかどうかの閾値（中央値からの相対値）

// ------------------------------
//	制御入力データに含まれるフラグ
// ------------------------------

#define CONTROLFLAG_TRANSIT_TO_IDLE  0x01 //!< 停止状態からアイドル状態に遷移させる（要：上位制御MCU停止・始動許可）
#define CONTROLFLAG_TRANSIT_TO_STOP  0x02 //!< アイドル状態から停止状態に遷移させる（要：上位制御MCU停止・始動許可）

// ------------------------------
//	Gain-Adjustment Data Definition
// ------------------------------

///
/// 姿勢角によるスロットル補正
///
#define THROTTLE_CORRECTION_BY_ATTITUDE_SCALE 32768U //!< スケール変換定数 （補正係数 = 整数値 / この値）
#define THROTTLE_CORRECTION_BY_ATTITUDE_MIN   32768U //!< 補正整数値の最小値
#define THROTTLE_CORRECTION_BY_ATTITUDE_MAX   65535U //!< 補正整数値の最大値
#define THROTTLE_CORRECTION_BY_ATTITUDE_INIT  32768U //!< 補正整数値の初期値
#define THROTTLE_CORRECTION_BY_ATTITUDE_SHR   15     //!< 補正演算における右シフト量 (1/32768)

// ------------------------------
//	Remote EEPROM Operation
// ------------------------------

#define EEPROM_OPERATION_DATASIZE    29                    //!< EEPROMのデータ領域サイズ

///
/// EEPROM操作コマンド
///
#define EEPROM_OPERATION_COMMAND_ID_READ              0x01 //!< ID指定データ要求
#define EEPROM_OPERATION_COMMAND_ID_READ_BY_BLOCK_NO  0x02 //!< ブロックNO指定データ要求
#define EEPROM_OPERATION_COMMAND_ID_FORMAT            0x03 //!< フォーマット
#define EEPROM_OPERATION_COMMAND_ID_BLOCK_ERACE       0x04 //!< ブロック消去

///
/// EEPROM操作コマンド範囲
///
#define EEPROM_OPERATION_COMMAND_ID_RANGE_LOW  0x01
#define EEPROM_OPERATION_COMMAND_ID_RANGE_HIGH 0x04

// 20150202 TCS.S.Kawanoue ADD_S [ATMegaブートローダ実装]
// ------------------------------
//	Remote LOWFIRM Operation
// ------------------------------

#define USER_AREA_SIZE (120*1024)				//!< ユーザ領域のバイト数 */
#define SPM_PAGESIZE (256)						//!< フラッシュ1ページのサイズ (128ワード=256バイト) */
#define PAGE_SPLIT_NUMBER (8)					//!< フラッシュ1ページの分割数
#define LOWFIRM_OPERATION_DATASIZE (SPM_PAGESIZE/PAGE_SPLIT_NUMBER)		//!< LOWFIRMのデータ領域サイズ

// CRC32計算関連
#define LOWFIRM_CRC32_POLYNOMIAL	(0xEDB88320UL)	// CRC32計算UART用(生成多項式X32+X26+X23+X22+X16+X12+X11+X10+X8+X7+X5+X4+X2+X1+1)
#define LOWFIRM_CRC32_INITIAL_VAL	(0xFFFFFFFFUL)	// 初期値
#define LOWFIRM_CRC32_OUT_XOR_VAL	(0xFFFFFFFFUL)	// 出力XOR

typedef enum 
{
	LOWFIRM_RES_OK   = 0,
	LOWFIRM_RES_FAIL = 1,
} LOWFIRM_RES;

///
/// LOWFIRM操作コマンド
///
#define LOWFIRM_OPERATION_COMMAND_ID_NOTIFY_START  0x01 //!< 開始通知           : High -> Low
#define LOWFIRM_OPERATION_COMMAND_ID_READY_START   0x02 //!< 開始応答           : Low  -> High
#define LOWFIRM_OPERATION_COMMAND_ID_DATA_END      0x03 //!< データ終了         : High -> Low
#define LOWFIRM_OPERATION_COMMAND_ID_DATA_COMPLETE 0x04 //!< データ書き込み完了 : Low  -> High

///
/// LOWFIRM操作コマンド範囲
///
#define LOWFIRM_OPERATION_COMMAND_ID_RANGE_LOW  0x01
#define LOWFIRM_OPERATION_COMMAND_ID_RANGE_HIGH 0x04
// 20150202 TCS.S.Kawanoue ADD_E


// ------------------------------
//	System
// ------------------------------

///
/// @brief 起動モードの定義
///
typedef enum 
{
	BOOT_UNDEFINED      = 0, //!< 未定義モード（エラー，飛行不可）
	BOOT_NORMAL         = 1, //!< 通常モード（飛行可）
	BOOT_EEPROM         = 2, //!< EEPROM操作モード（飛行不可）
	BOOT_ESC_INITIALIZE = 3, //!< ESC初期化モード（飛行不可）
} BOOTMODE;

///
/// @brief 駆動モード（0～7）
///
typedef enum 
{
	SYS_STOP       = 0, //!< 停止モード（全ロータ停止）
	SYS_IDLE       = 1, //!< アイドルモード（アイドル回転数でロータ回転）
	SYS_FLIGHT     = 2, //!< 飛行モード（飛行状態．フィードバック制御とミキシング演算を行う）
	SYS_Reserved1  = 3, //!< 予約
	SYS_Reserved2  = 4, //!< 予約
	SYS_TERMINATED = 5, //!< ターミネーションモード（全ロータを緊急停止させ墜落させるモード）
	SYS_Reserved3  = 6, //!< 予約
	SYS_Reserved4  = 7, //!< 予約
} SYSTEMMODE;

// Switch position （CONTROLMODEの実装によって削除するかも)
typedef enum 
{
	SWITCH_OFF  = 0,
	SWITCH_ON_1 = 1,
	SWITCH_ON_2 = 2,
} SWITCHSTATE;

///
/// @brief 下位制御MCUの動作フラグ
///
typedef enum 
{
	SYSFLAG_NULL                   = 0x00, //!< フラグなし
	SYSFLAG_PROPODATAAVAILABLE     = 0x01, //!< プロポデータ受信済
	SYSFLAG_THROTTLEBOTTOM         = 0x02, //!< スロットルがローのポジションにある
	SYSFLAG_MCUCOMMTRANSMITENABLE  = 0x04, //!< 下位制御MCUから上位制御MCUへのデータ送出を行う
	SYSFLAG_HIGHERMCUDATAAVAILABLE = 0x08, //!< 上位制御MCUからの制御入力データ受信済
	SYSFLAG_HIGHERMCUDATAUSING     = 0x10, //!< 上位制御MCUからの制御入力データを使用中
} SYSFLAG;

///
/// 下位制御MCUのエラーフラグ（制御演算部のエラーは別定義）
///
typedef enum 
{
	SEF_NONE = 0,                       //!< エラーフラグなし
	SEF_TIMEOUT_CONTROLCALCULATION = 1, //!< 制御演算タイムオーバー
} SOFTWARE_ERRORFLAG;

///
/// 下位制御MCUの警告フラグ
///
typedef enum 
{
	SWF_NONE = 0,       //!< 警告フラグなし
} SOFTWARE_WARNINGFLAG;

///
/// 下位制御MCUの制御演算エラーフラグ
///
typedef enum 
{
  	CEF_NONE = 0,                   //!< エラーフラグなし
	CEF_SATURATION_THROTTLE = 0x01, //!< スロットル演算にて飽和が発生
	CEF_SATURATION_ROLL     = 0x02, //!< ロール角速度制御にて飽和が発生
	CEF_SATURATION_PITCH    = 0x04, //!< ピッチ角速度制御にて飽和が発生
	CEF_SATURATION_YAW      = 0x08, //!< ヨー角速度制御にて飽和が発生
	CEF_SATURATION_MIXING   = 0x10, //!< ミキシング演算にて飽和が発生
} CONTROL_ERRORFLAG;

// Error flags on lower-Level MCU hardware system
typedef enum 
{
	HEF_NONE = 0,
} HARDWARE_ERRORFLAG;

//**************************************************
//	Data type
//**************************************************

//
// @brief 下位制御MCUの情報
//

// for C
typedef struct 
{
	SYSTEMMODE   SysMode;      // 制御モード
	SWITCHSTATE  FailsafeSW;   // フェイルセーフスイッチ
	SWITCHSTATE  AssistSW;     // アシストモードスイッチ
	SWITCHSTATE  AutonomousSW; // 自律モードスイッチ
	RcScaleValue Func1;        // ファンクション１に割り当てられた値
	SYSFLAG      Flag;         // システム状態フラグ
} SysState;

// for C++
#ifdef __cplusplus
struct CSysState : SysState
{
	CSysState()
	{
		SysMode      = SYS_STOP;
		FailsafeSW   = SWITCH_OFF;
		AssistSW     = SWITCH_OFF;
		AutonomousSW = SWITCH_OFF;
		Func1        = RC_MIN;
		Flag         = SYSFLAG_NULL;
	}	
};
#endif

//
// @brief 下位制御MCUの制御情報（角速度制御ループ）
//

// for C
typedef struct 
{
	// ミキシング前の制御入力
	_UWORD ControlInput_T; // Throttle (     0～65535)
	_SWORD ControlInput_R; // Roll     (-32767～32768)
	_SWORD ControlInput_P; // Pitch    (-32767～32768)
	_SWORD ControlInput_Y; // Yaw      (-32767～32768)
	// ミキシング後の制御入力
	_UWORD RotorInput[MAXIMUM_NUMBER_OF_ROTORS]; // (     0～65535)
} ScaledControlData;

// for C++
#ifdef __cplusplus
struct CScaledControlData : ScaledControlData
{
	CScaledControlData()
	{
		ControlInput_T = 0;
		ControlInput_R = 0;
		ControlInput_P = 0;
		ControlInput_Y = 0;
		for( int i = 0; i < MAXIMUM_NUMBER_OF_ROTORS; i++ ) 
		{
			RotorInput[i] = 0;	
		}
	}
};
#endif

typedef struct {
	CONTROL_ERRORFLAG Control;
} Error;


typedef struct _tag_FirmwareInfo 
{
	_UBYTE Major;          //!< メジャー
	_UBYTE Minor;          //!< マイナ
	_UBYTE Rivision;       //!< リビジョン
	_UWORD CompiledDate;   //!< コンパイル年月日 (Bit:YYYYYYYMMMMDDDDD)
	_UBYTE EditionCode[4]; //!< エディションコード
} FirmwareInfo;


//**************************************************
//	Lower-Level <--> Higher-Level communication definition
//**************************************************

#define MCUCOMM_PREAMBLE1 0xAA
#define MCUCOMM_PREAMBLE2 0xAB

#define MCUCOMM_LEN_PREAMBLE 2
#define MCUCOMM_LEN_PACKETID 1
#define MCUCOMM_LEN_HEADER   (MCUCOMM_LEN_PREAMBLE + MCUCOMM_LEN_PACKETID)
#define MCUCOMM_LEN_CHECKSUM 1

typedef enum 
{
	MCUCOMM_PACKETID_CONTROLINPUT      = 0x00, // High -> Low  (old format)
	MCUCOMM_PACKETID_PROPODATA         = 0x01, // Low  -> High (old format)
	MCUCOMM_PACKETID_CONTROLINPUT_VER2 = 0x02, // High -> Low  (old format)
	MCUCOMM_PACKETID_PROPODATA_VER2    = 0x03, // Low  -> High (old format)
	MCUCOMM_PACKETID_CONTROLINPUT_VER3 = 0x04, // High -> Low  (old format)
	MCUCOMM_PACKETID_PROPODATA_VER3    = 0x05, // Low  -> High
	MCUCOMM_PACKETID_GYRODATA          = 0x06, // Low  -> High
	MCUCOMM_PACKETID_CONTROLDATA       = 0x07, // Low  -> High
	MCUCOMM_PACKETID_SYSTEMDATA        = 0x08, // Low  -> High (old format)
	MCUCOMM_PACKETID_DATAREQUEST       = 0x09, // High -> Low
	MCUCOMM_PACKETID_HEALTHDATA        = 0x10, // Low  -> High
	MCUCOMM_PACKETID_RCRECEIVERSTAT    = 0x11, // Low  -> High
	MCUCOMM_PACKETID_SYSTEMDATA_VER2   = 0x12, // Low  -> High
	MCUCOMM_PACKETID_GAINADJUSTMENT    = 0x13, // High -> Low
	MCUCOMM_PACKETID_EEPROM            = 0x14, // Low <-> High
	MCUCOMM_PACKETID_EEPROMCOMMAND     = 0x15, // High -> Low
	MCUCOMM_PACKETID_PROPODATA_VER4    = 0x16, // Low  -> High
	MCUCOMM_PACKETID_SYSTEMDATA_VER3   = 0x17, // Low  -> High
	MCUCOMM_PACKETID_CONTROLINPUT_VER4 = 0x18, // High -> Low
	MCUCOMM_PACKETID_TRANSFERMODECHANGE= 0x19, // High -> Low
	MCUCOMM_PACKETID_SHOCKDETECTIONINFO= 0x1A, // High -> Low
// 20150202 TCS.S.Kawanoue ADD_S [ATMegaブートローダ実装]
	MCUCOMM_PACKETID_LOWFIRMDATA       = 0x1B, // High -> Low
	MCUCOMM_PACKETID_LOWFIRMDATARSP    = 0x1C, // Low  -> High
	MCUCOMM_PACKETID_LOWFIRMCOMMAND    = 0x1D, // High <-> Low
// 20150202 TCS.S.Kawanoue ADD_E
	MCUCOMM_PACKETID_FIRMWAREINFO      = 0xF0, // Low  -> High
	MCUCOMM_PACKETID_DEBUG             = 0xFF, // Low  -> High
} MCUCOMM_PACKETID;

typedef enum 
{
	MCUCOMM_PACKETSIZE_CONSTANT          = 4,
	MCUCOMM_PACKETSIZE_CONTROLINPUT_VER3 = MCUCOMM_PACKETSIZE_CONSTANT + 8,
	MCUCOMM_PACKETSIZE_PROPODATA_VER3    = MCUCOMM_PACKETSIZE_CONSTANT + 25,
	MCUCOMM_PACKETSIZE_GYRODATA          = MCUCOMM_PACKETSIZE_CONSTANT + 6,
	MCUCOMM_PACKETSIZE_CONTROLDATA       = MCUCOMM_PACKETSIZE_CONSTANT + 32,
	MCUCOMM_PACKETSIZE_SYSTEMDATA        = MCUCOMM_PACKETSIZE_CONSTANT + 10,
	MCUCOMM_PACKETSIZE_DATAREQUEST       = MCUCOMM_PACKETSIZE_CONSTANT + 8,
	MCUCOMM_PACKETSIZE_HEALTHDATA        = MCUCOMM_PACKETSIZE_CONSTANT + 6,
	MCUCOMM_PACKETSIZE_RCRECEIVERSTAT    = MCUCOMM_PACKETSIZE_CONSTANT,
	MCUCOMM_PACKETSIZE_SYSTEMDATA_VER2   = MCUCOMM_PACKETSIZE_CONSTANT + 12,
	MCUCOMM_PACKETSIZE_GAINADJUSTMENT    = MCUCOMM_PACKETSIZE_CONSTANT + 8,
	MCUCOMM_PACKETSIZE_EEPROM            = MCUCOMM_PACKETSIZE_CONSTANT + 31,
	MCUCOMM_PACKETSIZE_EEPROMCOMMAND     = MCUCOMM_PACKETSIZE_CONSTANT + 3,
	MCUCOMM_PACKETSIZE_PROPODATA_VER4    = MCUCOMM_PACKETSIZE_CONSTANT + 21,
	MCUCOMM_PACKETSIZE_SYSTEMDATA_VER3   = MCUCOMM_PACKETSIZE_CONSTANT + 14,
	MCUCOMM_PACKETSIZE_CONTROLINPUT_VER4 = MCUCOMM_PACKETSIZE_CONSTANT + 9,
	MCUCOMM_PACKETSIZE_TRANSFERMODECHANGE= MCUCOMM_PACKETSIZE_CONSTANT,
	MCUCOMM_PACKETSIZE_SHOCKDETECTIONINFO= MCUCOMM_PACKETSIZE_CONSTANT + 3,
// 20150202 TCS.S.Kawanoue ADD_S [ATMegaブートローダ実装]
	MCUCOMM_PACKETSIZE_LOWFIRMDATA       = MCUCOMM_PACKETSIZE_CONSTANT + (4+SPM_PAGESIZE/PAGE_SPLIT_NUMBER), 
	MCUCOMM_PACKETSIZE_LOWFIRMDATARSP    = MCUCOMM_PACKETSIZE_CONSTANT + 4,
	MCUCOMM_PACKETSIZE_LOWFIRMCOMMAND    = MCUCOMM_PACKETSIZE_CONSTANT + 5,
// 20150202 TCS.S.Kawanoue ADD_E
	MCUCOMM_PACKETSIZE_FIRMWAREINFO      = MCUCOMM_PACKETSIZE_CONSTANT + 9, 
	MCUCOMM_PACKETSIZE_DEBUG             = MCUCOMM_PACKETSIZE_CONSTANT + 8,
} MCUCOMM_PACKETSIZE;


//**************************************************
//	Packet definition for Lower-Level <--> Higher-Level communication
//**************************************************
#ifndef __cplusplus

#pragma pack(1)

// 操舵入力データ
typedef struct 
{
	ErcScaleValue Throttle;
	ErcScaleValue Roll;
	ErcScaleValue Pitch;
	ErcScaleValue Yaw;
} McuComm_ControlInputPacket_Ver3;

// 操舵入力データ
typedef struct 
{
	ErcScaleValue Throttle;
	ErcScaleValue Roll;
	ErcScaleValue Pitch;
	ErcScaleValue Yaw;
	_UBYTE        Flags;
} McuComm_ControlInputPacket_Ver4;

// プロポデータ (Ver3)
typedef struct 
{
	RcScaleValue Throttle;
	RcScaleValue Roll;
	RcScaleValue Pitch;
	RcScaleValue Yaw;
	RcScaleValue Ch5;
	RcScaleValue Ch6;
	RcScaleValue Ch7;
	RcScaleValue Ch8;
	RcScaleValue Ch9;
	RcScaleValue Ch10;
	RcScaleValue Ch11;
	RcScaleValue Ch12;
	_UBYTE SignalExist;
} McuComm_PropoDataPacket_Ver3;

// プロポデータ (Ver4)
typedef struct 
{
	_UBYTE ProportionalData[20]; // 1Ch = 10ビット * 16Ch
	_UBYTE DigitalData;
} McuComm_PropoDataPacket_Ver4;

// ジャイロデータ
typedef struct 
{
	_UWORD RollRate;    // p [deg/s] = 0.01 * value;
	_UWORD PitchRate;   // q [deg/s] = 0.01 * value;
	_UWORD YawRate;     // r [deg/s] = 0.01 * value;
} McuComm_GyroDataPacket;

// 角速度制御ループ制御入力データ
typedef ScaledControlData McuComm_ControlDataPacket;

// システム状態データ (Ver2)
typedef struct 
{
	_UBYTE       SysMode;
	_UBYTE       Switches;          // Failsafe(0, 1), Assist(2, 3), Autonomous(4, 5)
	RcScaleValue Func1;    
	_UWORD       SysFlag;
	_UWORD       SoftwareErrorFlag;
	_UWORD       ControlErrorFlag;
	_UWORD       HardwareErrorFlag; // Currently, Reserved
} McuComm_SystemDataPacket_Ver2;

// システム状態データ (Ver3)
typedef struct 
{
	_UBYTE       SysMode;
	_UBYTE       Switches;          // Failsafe(0, 1), Assist(2, 3), Autonomous(4, 5)
	RcScaleValue Func1;    
	_UWORD       SysFlag;
	_UWORD       SoftwareErrorFlag;
	_UWORD       ControlErrorFlag;
	_UWORD       HardwareErrorFlag; // Currently, Reserved
	_UWORD       CellVoltage;
} McuComm_SystemDataPacket_Ver3;

// システム健全度データ
typedef LowLvMcuHealthData McuComm_HealthDataPacket;

// データリクエストパケット
typedef struct 
{
	_UBYTE  Reserved1;
	_UBYTE  Reserved2;
	_UBYTE  Reserved3;
	_UBYTE  Reserved4;
	_UBYTE  Reserved5;
	_UBYTE  Reserved6;
	_UBYTE  Reserved7;
	_UBYTE  Reserved8;
} McuComm_DataRequestPacket;

// 入力補正データ（実装途中）
typedef struct 
{
	_UWORD ThrottleCorrectionByAttitude;  // 姿勢角によるスロットル補正(32768～65565)
	_UWORD Reserved1;
	_UWORD Reserved2;
	_UWORD Reserved3;
} McuComm_GainAdjustmentPacket;

// EEPROMデータ（ATMega⇔SH 間は送信番号つけない）
typedef struct 
{
	_UWORD BlockID;
	_UBYTE Data[EEPROM_OPERATION_DATASIZE];
} McuComm_EepromPacket;

// EEPROM操作コマンド（ATMega⇔SH 間は送信番号つけない）
typedef struct 
{
	_UBYTE CommandID;
	_UWORD CommandParameter;
} McuComm_EepromCommandPacket;

// 衝撃検知情報データ
typedef struct {
	_UBYTE NotifykKind;		// 通知種別(0x01:小衝撃、0x02:大衝撃、0x04:転倒のOR値)
	_UWORD Date;			// 検知年月日
} McuComm_ShockDetectionInfo;

// 20150202 TCS.S.Kawanoue ADD_S [ATMegaブートローダ実装]
// LowFirm操作コマンド
typedef struct {
	_UBYTE CommandID;
	_UWORD Param1;		// 20150618 TCS.S.Kanai ADD [ファイルによるEEPROM初期化実装]
} McuComm_LowFirmCommandPacket;
// 20150202 TCS.S.Kawanoue ADD_E

// ファームウェア情報
typedef struct 
{
	_UBYTE Major;          //!< メジャー
	_UBYTE Minor;          //!< マイナ
	_UBYTE Rivision;       //!< リビジョン
	_UWORD CompiledDate;   //!< コンパイル年月日 (Bit:YYYYYYYMMMMDDDDD)
	_UBYTE EditionCode[4]; //!< エディションコード
} McuComm_FirmwareInfoPacket;

#pragma pack()

#endif

//**************************************************
// S.Bus Channel Assignment
//**************************************************

// 1-base definition!
#define SBUS_CH_ASSIGN_THROTTLE    3
#define SBUS_CH_ASSIGN_ROLL        1
#define SBUS_CH_ASSIGN_PITCH       2
#define SBUS_CH_ASSIGN_YAW         4
#define SBUS_CH_ASSIGN_FAILSAFE    8
#define SBUS_CH_ASSIGN_ATTCONTROL  5
#define SBUS_CH_ASSIGN_NAVIGATION  7
#define SBUS_CH_ASSIGN_FUNC1       6

// array-index (0-base) definition
#define SBUS_CH_INDEX_THROTTLE   (SBUS_CH_ASSIGN_THROTTLE-1)
#define SBUS_CH_INDEX_ROLL       (SBUS_CH_ASSIGN_ROLL-1)
#define SBUS_CH_INDEX_PITCH      (SBUS_CH_ASSIGN_PITCH-1)
#define SBUS_CH_INDEX_YAW        (SBUS_CH_ASSIGN_YAW-1)
#define SBUS_CH_INDEX_FAILSAFE   (SBUS_CH_ASSIGN_FAILSAFE-1)
#define SBUS_CH_INDEX_ATTCONTROL (SBUS_CH_ASSIGN_ATTCONTROL-1)
#define SBUS_CH_INDEX_NAVIGATION (SBUS_CH_ASSIGN_NAVIGATION-1)
#define SBUS_CH_INDEX_FUNC1      (SBUS_CH_ASSIGN_FUNC1-1)

#endif