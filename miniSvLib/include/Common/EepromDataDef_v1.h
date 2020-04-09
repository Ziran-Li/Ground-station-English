///
/// @file       EepromDataDef_v1.h
///
/// @brief      EEPROMで管理するデータに関する定義一覧（パラメータセットVer = 1）
///
/// @author     iwakura
///

//
// #define 定義名命名規則
//
// E2PDEF_構造体名_メンバ名_任意の名前
//

#ifndef INCLUDED_EEPROMDATADEF_V1_H
#define INCLUDED_EEPROMDATADEF_V1_H

#include "EepromStorageDef.h"

///
/// @brief  EEPROMブロックID
///
enum EEPROM_BLOCKID
{
	E2P_BID_BLANK              = 0,   //!< ブランク領域
	E2P_BID_EEPROMSYSTEMDATA   = 1,   //!< EEPROMストレージ管理情報
	E2P_BID_UNITINFO           = 2,   //!< 制御ユニット情報
	E2P_BID_SERIALNUMBER       = 100, //!< 製造番号
	E2P_BID_ASSEMBLER          = 101, //!< 機体組立業者
	E2P_BID_SELLER             = 102, //!< 販売者
	E2P_BID_AFFILIATION        = 103, //!< 所属情報
	E2P_BID_MANUFACTURINGFLAGS = 104, //!< 製造関連フラグ
	E2P_BID_USERINFO           = 105, //!< ユーザー情報
	E2P_BID_MAINTENANCEINFO    = 200, //!< 機体メンテナンス情報
	E2P_BID_MAINTENANCELOG1    = 201, //!< メンテナンスログ1
	E2P_BID_MAINTENANCELOG2    = 202, //!< メンテナンスログ2
	E2P_BID_MAINTENANCELOG3    = 203, //!< メンテナンスログ3
	E2P_BID_LIFEMANAGEMENT1    = 300, //!< 寿命管理情報1
	E2P_BID_LIFEMANAGEMENT2    = 301, //!< 寿命管理情報2
	E2P_BID_LIFEMANAGEMENT3    = 302, //!< 寿命管理情報3
	E2P_BID_LIFEMANAGEMENT4    = 303, //!< 寿命管理情報4
	E2P_BID_MACHINE_CONSTRUCT  = 400, //!< 機体構成情報
	E2P_BID_CIRCUIT_PARAMETER  = 401, //!< 回路パラメーター
	E2P_BID_GEOMETRICAL_PARAMETER = 402, //!< 重心・位置設定
	E2P_BID_HARD_IRON_CALIBRATION = 500, //!< ハードアイアン校正
	E2P_BID_CONTROLLER_GAIN_1  = 600, //!< 制御ゲイン（PRなど）
	E2P_BID_CONTROLLER_GAIN_2  = 601, //!< 制御ゲイン（Yなど）
	E2P_BID_FILTER_GAIN        = 602, //!< フィルタゲイン
	E2P_BID_CONTROLLER_RANGE   = 603, //!< レンジ設定
	E2P_BID_DATA_SEND_CYCLE    = 700, //!< データ送信周期
	E2P_BID_SAFETY_FUNCTION    = 800, //!< 安全機能パラメーター
	E2P_BID_GINBAL_SERVO_SETTING = 900, //!< ジンバルサーボ設定
	E2P_BID_REMOCON_NMEA_SETTING = 901, //!< リモコン・NMEA設定
	E2P_BID_SPECIAL_FUNCTION   = 1000,//!< 特殊機能パラメーター
	E2P_BID_CONTROLLER_LIMIT   = 1100,//!< 制御機能制限
	E2P_BID_FLYZONE_LIMIT      = 1101,//!< 飛行エリア制限
	E2P_BID_CUSTOM_PARAMETER1  = 1200,//!< カスタム対応パラメータ1
	E2P_BID_CUSTOM_PARAMETER2  = 1201 //!< カスタム対応パラメータ2
};

///
/// @brief  ユニットID
///
enum UNITINFO__UNIT_ID
{
	E2PDEF_UNITID_LEGACY  = 1, //!< MiniSurveyorAutoPilot Ver.0.6 までのユニットID
	E2PDEF_UNITID_MSAP_1  = 2, //!< MiniSurveyorAutoPilot Ver.1.0 のユニットID
};

///
/// @brief  製造業者ID
///
enum UNITINFO__MANUFACTURER_ID
{
	E2PDEF_MANUFACTURERID_ACSL = 1, // 自律制御システム研究所
	E2PDEF_MANUFACTURERID_CEC  = 2, // 中央電子
};

///
/// @brief  特殊用途対応フラグ
///
enum SPECIAL_FLAGS1
{
	E2PDEF_SPECIAL_FLAGS1_HIGHMCU_ROTORDRIVE_PERMISSION = 0x0001, //!< 上位制御MCU停止・始動許可
};

///
/// @brief  パルス生成周期
///
enum PULSE_GENE_CYCLE_ID
{
	E2PDEF_PULSE_GENE_CYCLE_NONE	= 0,	// 未設定
	E2PDEF_PULSE_GENE_CYCLE_50Hz	= 1,	// 50Hz  20ms
	E2PDEF_PULSE_GENE_CYCLE_100Hz	= 2,	// 100Hz 10ms
	E2PDEF_PULSE_GENE_CYCLE_200Hz	= 3,	// 200Hz 5ms
	E2PDEF_PULSE_GENE_CYCLE_400Hz	= 4 	// 400Hz 2.5ms
};

///
/// @brief  パルス幅
///
enum PULSE_GENE_WIDTH_ID
{
	E2PDEF_PULSE_WIDTH_NONE		= 0,		// 未設定
	E2PDEF_PULSE_WIDTH_STANDARD	= 1,		// スタンダード
	E2PDEF_PULSE_WIDTH_WIDE		= 2 		// ワイド
};

///
/// @brief  プロポモード
///
enum PROPO_MODE_NUMBER
{
	E2PDEF_PROPO_MODE_1 = 0,		// MODE 1
	E2PDEF_PROPO_MODE_2 = 1,		// MODE 2
	E2PDEF_PROPO_MODE_3 = 2, 		// MODE 3
	E2PDEF_PROPO_MODE_4 = 3 		// MODE 4
};

///
/// @brief  通信プロトコル
///
enum COMMUNICATION_PROTOCOL
{
	E2PDEF_COMMUNICATION_PROTOCOL_LEGACY = 0,		// Legacy
	E2PDEF_COMMUNICATION_PROTOCOL_2G     = 1		// 2G
};

///
/// @brief  通信デバイス
///
enum COMMUNICATION_DEVICE
{
	E2PDEF_COMMUNICATION_DEVICE_TRANSPARENT = 0,	// Transparent
	E2PDEF_COMMUNICATION_DEVICE_FEP         = 1		// FEP
};

///
/// @brief  パケット送信周期
///
enum DATA_SEND_CYCLE
{
	E2PDEF_DATA_SEND_CYCLE_NOSEND = 0,	// 送信しない
	E2PDEF_DATA_SEND_CYCLE_50HZ   = 1,	// 50Hz
	E2PDEF_DATA_SEND_CYCLE_25HZ   = 2,	// 25Hz
	E2PDEF_DATA_SEND_CYCLE_12_5HZ = 4,	// 12.5Hz
	E2PDEF_DATA_SEND_CYCLE_10HZ   = 5,	// 10Hz
	E2PDEF_DATA_SEND_CYCLE_5HZ    = 10,	// 5Hz
	E2PDEF_DATA_SEND_CYCLE_2_5HZ  = 20,	// 2.5Hz
	E2PDEF_DATA_SEND_CYCLE_2HZ    = 25,	// 2Hz
	E2PDEF_DATA_SEND_CYCLE_1HZ    = 50,	// 1Hz
	E2PDEF_DATA_SEND_CYCLE_0_5HZ  = 100	// 0.5Hz
};

///
/// @brief  ユーザーID文字数最大
///
#define E2PDEF_USER_ID_MAX				14		// ユーザーID文字数最大

///
/// @brief  ユーザーパスワード文字数最大
///
#define E2PDEF_PASSWORD_MAX				15		// パスワード文字数最大

///
/// @brief  ローターの構成
///
#define ROTORARRANGEMENT_NONE 0
#define ROTORARRANGEMENT_X4_A 1
#define ROTORARRANGEMENT_X4_B 2
#define ROTORARRANGEMENT_X6_A 3
#define ROTORARRANGEMENT_X6_B 4

#define SWAP_ROTATIONAL_DIRECTION FALSE // If 1st rotor's propeller is negative pitch, please set TRUE

///
/// @brief  セル数
///
#define E2PDEF_NUM_OF_CELLS				6

///
/// @brief  パケット送信周期の総数
///
#define E2PDEF_SEND_PACKET_CYCLE_MAX	27		// パケット送信周期の総数

///
/// @brief  ゴーホーム条件
///
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_PROPO					0x01	//!< bit0-ﾌﾟﾛﾎﾟ操作
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_COMMAND					0x02	//!< bit1-無線ｺﾏﾝﾄﾞ
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_LOW_VOLTAGE				0x04	//!< bit2-電圧低下
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_DISCONNECT_PROPO		0x08	//!< bit3-ﾌﾟﾛﾎﾟ途絶
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_DISCONNECT_DATALINK		0x10	//!< bit4-ﾃﾞｰﾀﾘﾝｸ途絶
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_LESS_SATELITE			0x20	//!< bit5-衛星数低下
#define E2PDEF_SAFETYFUNCTION_GOHOME_REASON_REST_CAUSE				0x40	//!< bit6-その他ﾄﾘｶﾞを許可

///
/// @brief  安全機能フラグ
///
#define E2PDEF_SAFETYFUNCTION_FLAG_GPSLED_ENABLE		0x01	//!< bit0-GPSLED有効
#define E2PDEF_SAFETYFUNCTION_FLAG_BUZZER_ENABLE		0x02	//!< bit1-ﾌﾞｻﾞｰ有効

///
/// @brief  特殊機能
///
#define E2PDEF_BIT_NOFLYZONE_ENABLE						0x00000001UL	//!< 特殊機能・フラグ1 bit0-飛行禁止エリア機能有効
#define E2PDEF_BIT_HIGHMCU_STOPSTART_ENABLE					0x0001		//!< 特殊機能・フラグ2 bit0-上位制御MCU停止・始動許可 初期=0

///
/// @brief  飛行エリア制限
///
#define E2PDEF_FLYZONELIMIT_BIT_LIMIT_NONE	0x01	// ﾋﾞｯﾄﾌｨｰﾙﾄﾞ bit0~2 - 形状 bit0-無制限
#define E2PDEF_FLYZONELIMIT_BIT_ELLIPSE		0x02	// ﾋﾞｯﾄﾌｨｰﾙﾄﾞ bit0~2 - 形状 bit1-楕円形
#define E2PDEF_FLYZONELIMIT_BIT_RECTANGLE	0x04	// ﾋﾞｯﾄﾌｨｰﾙﾄﾞ bit0~2 - 形状 bit2-長方形
#define E2PDEF_FLYZONELIMIT_BIT_LL			0x08	// ﾋﾞｯﾄﾌｨｰﾙﾄﾞ bit3   - 緯度経度設定有効

#if defined(WIN32) || defined(IAR)
#pragma pack(1)
#else
#pragma pack 1
#endif

//####################################################################################################
//	#0000	管理データ
//####################################################################################################

///
/// @brief  EEPROM管理情報
///
typedef struct _tag_EepromSystemData
{
	_UWORD ParametersetVer;
	_UWORD ParametersetVerLowerCompatibleValue;
	_UWORD ParametersetVerHigherCompatibleValue;
	_UBYTE Reserved[23];

} E2PDEF_EepromSystemData;

///
/// @brief  ユニット情報
///
typedef struct _tag_UnitInfo
{
	_UBYTE UnitID;
	_UBYTE RevisionID;
	_UBYTE MenufacturerID;
	_UWORD LotInfo;
	_UBYTE UnitSN[16];
	_UBYTE EditionCode[4];
	_UBYTE Reserved[4];

} E2PDEF_UnitInfo;

//####################################################################################################
//	#0100	機体管理・製造情報
//####################################################################################################

///
/// @brief  製造番号
///
typedef struct _tag_SerialNumber
{
	_UBYTE SerialNumber[29];

} E2PDEF_SerialNumber;

///
/// @brief  機体組立業者
///
typedef struct _tag_Assembler
{
	_UBYTE Assembler[29];

} E2PDEF_Assembler;

///
/// @brief  販売者
///
typedef struct _tag_Seller
{
	_UBYTE Seller[29];

} E2PDEF_Seller;

///
/// @brief  所属情報
///
typedef struct _tag_Affiliation
{
	_UBYTE Affiliation[29];

} E2PDEF_Affiliation;

///
/// @brief  製造関連フラグ
///
typedef struct _tag_ManufacturingFlags
{
	_UBYTE  Flag1;
	_UDWORD ProcessManegementFlag;
	_UBYTE  LifeCycle;
	_UBYTE  Reserved[23];

} E2PDEF_ManufacturingFlags;

///
/// @brief  ユーザー情報
///
typedef struct _tag_UserInfo
{
	_UBYTE	USER_ID[E2PDEF_USER_ID_MAX];
	_UBYTE	PASSWORD[E2PDEF_PASSWORD_MAX];
} E2PDEF_UserInfo;

//####################################################################################################
//	#0200	メンテナンス
//####################################################################################################

///
/// @brief  機体メンテナンス情報
///
typedef struct _tag_MaintenanceInfo
{
	_UWORD	TotalFlightCnt;				// 前回メンテ時の総飛行回数
	_UBYTE	TotalPowerOnTime[3];		// 前回メンテ時の総起動時間
	_UBYTE	TotalFlightTime[3];			// 前回メンテ時の総飛行時間
	_UWORD	ShockDetectCnt;				// 前回メンテ時の小衝撃検知回数
	_UBYTE	StrongShockDetectCnt;		// 前回メンテ時の大衝撃検知回数
	_UBYTE	FallDetectCnt;				// 前回メンテ時の転倒検知回数
	_UWORD	StrongShockDetectDate;		// 前回メンテ時の大衝撃検知年月日
	_UWORD	FlightTimeStrongShockDetect;// 前回メンテ時の大衝撃検知時の総飛行時間
	_UWORD	FallDetectDate;				// 前回メンテ時の転倒検知年月日
	_UWORD	FlightTimeFallDetect;		// 前回メンテ時の転倒検知時の総飛行時間
	_UBYTE  Reserved[9];

} E2PDEF_MaintenanceInfo;

///
/// @brief  メンテナンスログ1
///
typedef struct _tag_MaintenanceLog1
{
	_UWORD	MaintenanceLog1Date;		// メンテナンス年月日1
	_UBYTE	MaintenanceLog1Comment[27];	// メンテナンスログ1

} E2PDEF_MaintenanceLog1;

///
/// @brief  メンテナンスログ2
///
typedef struct _tag_MaintenanceLog2
{
	_UWORD	MaintenanceLog2Date;		// メンテナンス年月日2
	_UBYTE	MaintenanceLog2Comment[27]; // メンテナンスログ2

} E2PDEF_MaintenanceLog2;

///
/// @brief  メンテナンスログ3
///
typedef struct _tag_MaintenanceLog3
{
	_UWORD	MaintenanceLog3Date;		// メンテナンス年月日3
	_UBYTE	MaintenanceLog3Comment[27]; // メンテナンスログ3

} E2PDEF_MaintenanceLog3;

//####################################################################################################
//	#0300	寿命管理
//####################################################################################################

///
/// @brief  寿命管理情報1
///
typedef struct _tag_LifeManagement1
{
	_UWORD TotalFlightCnt;
	_UBYTE TotalPowerOnTime[3];
	_UBYTE TotalFlightTime[3];
	_UWORD ShockDetectCnt;
	_UBYTE StrongShockDetectCnt;
	_UBYTE FallDetectCnt;
	_UWORD StrongShockDetectDate;
	_UWORD FlightTimeStrongShockDetect;
	_UWORD FallDetectDate;
	_UWORD FlightTimeFallDetect;
	_UBYTE  Reserved[9];
} E2PDEF_LifeManagement1;

///
/// @brief  寿命管理情報2
///
typedef struct _tag_LifeManagement2
{
	_UBYTE MotorTotalTime1[3];
	_UBYTE MotorTotalTime2[3];
	_UBYTE MotorTotalTime3[3];
	_UBYTE MotorTotalTime4[3];
	_UBYTE MotorTotalTime5[3];
	_UBYTE MotorTotalTime6[3];
	_UBYTE MotorTotalTime7[3];
	_UBYTE MotorTotalTime8[3];
	_UBYTE MotorTotalTime9[3];
	_UBYTE  Reserved[2];
} E2PDEF_LifeManagement2;

///
/// @brief  寿命管理情報3
///
typedef struct _tag_LifeManagement3
{
	_UBYTE MotorTotalTime10[3];
	_UBYTE MotorTotalTime11[3];
	_UBYTE MotorTotalTime12[3];
	_UBYTE EscTotalTime1[3];
	_UBYTE EscTotalTime2[3];
	_UBYTE EscTotalTime3[3];
	_UBYTE EscTotalTime4[3];
	_UBYTE EscTotalTime5[3];
	_UBYTE EscTotalTime6[3];
	_UBYTE  Reserved[2];
} E2PDEF_LifeManagement3;

///
/// @brief  寿命管理情報4
///
typedef struct _tag_LifeManagement4
{
	_UBYTE EscTotalTime7 [3];
	_UBYTE EscTotalTime8 [3];
	_UBYTE EscTotalTime9 [3];
	_UBYTE EscTotalTime10 [3];
	_UBYTE EscTotalTime11 [3];
	_UBYTE EscTotalTime12 [3];
	_UBYTE UserTotalTime1 [3];
	_UBYTE UserTotalTime2 [3];
	_UBYTE UserTotalTime3 [3];
	_UBYTE  Reserved[2];
} E2PDEF_LifeManagement4;

//####################################################################################################
//	#0400	構成
//####################################################################################################

///
/// @brief  機体構成情報
///
typedef struct _tag_MachineConstruct
{
	_UBYTE RotorArrangement;		// 配置 0:未設定 1:4発A 2:4発B 3:6発A 4:6発B
	_UBYTE Flag;					// フラグ bit0:回転方向ﾘﾊﾞｰｽ
	_UBYTE PulseGenerateCycle;		// パルス生成周期 0:未設定 1:50Hz 2:100Hz 3:200Hz 4:400Hz
	_UBYTE PulseWidth;				// パルス幅 0:未設定 1:ｽﾀﾝﾀﾞｰﾄﾞ 2:ワイド
	_UBYTE ButteryCellNum;			// セル数
	_UBYTE PropoMode;				// プロポモード
	_UBYTE reserved[23];
} E2PDEF_MachineConstruct;

///
/// @brief  回路パラメーター
///
typedef struct _tag_CircuitParameter
{
	_UWORD Vmon_R1;		// [Ω] 電源側の分圧抵抗
	_UWORD Vmon_R2;		// [Ω] GND側の分圧抵抗
	_UWORD Vmon_VF;		// [mV]  ダイオードの順方向降下電圧．無い場合は0を使用
	_UBYTE reserved[23];
} E2PDEF_CircuitParameter;

///
/// @brief  重心・位置設定
///
typedef struct _tag_GeometricalParameter
{
	_UWORD DistanceBetweenBasePointAndCentroid_X;	// 基準点と重心位置の距離(X)
	_UWORD DistanceBetweenBasePointAndCentroid_Y;	// 基準点と重心位置の距離(Y)
	_UWORD DistanceBetweenBasePointAndCentroid_Z;	// 基準点と重心位置の距離(Z)
	_UWORD DistanceBetweenBasePointAndGPS_X;		// 基準点とGPSの距離(X)
	_UWORD DistanceBetweenBasePointAndGPS_Y;		// 基準点とGPSの距離(Y)
	_UWORD DistanceBetweenBasePointAndGPS_Z;		// 基準点とGPSの距離(Z)
	_UWORD BodyMass;								// 機体質量(10グラム単位)
	_UWORD RotorInertiaMoment;						// ロータ慣性ﾓｰﾒﾝﾄ
	_UWORD PayloadMass;								// ペイロード質量(10グラム単位)
	_SWORD PayloadPosition_X;						// ペイロード位置(X)
	_SWORD PayloadPosition_Y;						// ペイロード位置(Y)
	_SWORD PayloadPosition_Z;						// ペイロード位置(Z)
	_UBYTE reserved[5];
} E2PDEF_GeometricalParameter;

//####################################################################################################
//	#0500	校正
//####################################################################################################

///
/// @brief  ハードアイアン校正
///
typedef struct _tag_HardIronCalibration
{
	_UWORD HardIron_X;			// ﾊｰﾄﾞｱｲｱﾝ校正X軸
	_UWORD HardIron_Y;			// ﾊｰﾄﾞｱｲｱﾝ校正Y軸
	_UWORD HardIron_Z;			// ﾊｰﾄﾞｱｲｱﾝ校正Z軸
	_UWORD HardIronFlying_X;	// ﾊｰﾄﾞｱｲｱﾝ校正X軸(飛行中)
	_UWORD HardIronFlying_Y;	// ﾊｰﾄﾞｱｲｱﾝ校正Y軸(飛行中)
	_UWORD HardIronFlying_Z;	// ﾊｰﾄﾞｱｲｱﾝ校正Z軸(飛行中)
	_UWORD ThrottleValue;		// スロットル値（1020～2020）
	_SWORD MagneticAngleOffset;	// 磁気偏角オフセット（0.01度単位 とり得る範囲：-127.00～127.00度）
	_UBYTE reserved[13];
} E2PDEF_HardIronCalibration;

//####################################################################################################
//	#0600	制御パラメータ
//####################################################################################################

///
/// @brief  制御ゲイン（PR）
///
typedef struct _tag_ControllerGain1
{
	_UWORD  P_Propo_Roll;
	_UWORD  I_Propo_Roll;
	_UWORD  D_Propo_Roll;
	_UWORD  P_Gyro_Roll;
	_UWORD  I_Gyro_Roll;
	_UWORD  D_Gyro_Roll;
	_UWORD  P_Propo_Pitch;
	_UWORD  I_Propo_Pitch;
	_UWORD  D_Propo_Pitch;
	_UWORD  P_Gyro_Pitch;
	_UWORD  I_Gyro_Pitch;
	_UWORD  D_Gyro_Pitch;
	_UWORD  DC_Adj_Gain_R;	// 未使用
	_UWORD  DC_Adj_Gain_P;	// 未使用
	_UBYTE  Reserved;
} E2PDEF_ControllerGain1;


///
/// @brief  制御ゲイン（Y）
///
typedef struct _tag_ControllerGain2
{
	_UWORD  P_Propo_Yaw;
	_UWORD  I_Propo_Yaw;
	_UWORD  D_Propo_Yaw;
	_UWORD  P_Gyro_Yaw;
	_UWORD  I_Gyro_Yaw;
	_UWORD  D_Gyro_Yaw;
	_UWORD  P_Propo_THR;
	_UWORD  Untiwindup_Gain_PR;
	_UWORD  Untiwindup_Gain_Y;
	_UBYTE  Reserved[11];
} E2PDEF_ControllerGain2;


///
/// @brief  フィルタ
///
typedef struct _tag_FilterGain
{
	_UWORD  EnableFlags;
	_UBYTE  Proportional_Filter_Gain_R;
	_UBYTE  Proportional_Filter_Gain_P;
	_UBYTE  Proportional_Filter_Gain_Y;
	_UBYTE  Pseudo_Differential_Gain_R;
	_UBYTE  Pseudo_Differential_Gain_P;
	_UBYTE  Pseudo_Differential_Gain_Y;
	_UBYTE  Differential_HighFreq_Filter_Gain_R;
	_UBYTE  Differential_HighFreq_Filter_Gain_P;
	_UBYTE  Differential_HighFreq_Filter_Gain_Y;
	_UBYTE  Reserved[18];
} E2PDEF_FilterGain;

#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_ROLL_CONTROL                     0x0001  //!< 角速度制御を有効にする（ロール）
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_PITCH_CONTROL                    0x0002  //!< 角速度制御を有効にする（ピッチ）
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_YAW_CONTROL                      0x0004  //!< 角速度制御を有効にする（ヨー）

#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_1_R                       0x0008  //!< 比例成分にローパスフィルタを適用する（ロール）
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_1_P                       0x0010  //!< 比例成分にローパスフィルタを適用する（ピッチ）
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_1_Y                       0x0020  //!< 比例成分にローパスフィルタを適用する（ヨー）

#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_2_R                       0x0040  //!< 擬似微分を適用する（ロール）
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_2_P                       0x0080  //!< 擬似微分を適用する（ピッチ）
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_2_Y                       0x0100  //!< 擬似微分を適用する（ヨー）

#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_3_R                       0x0200  //!< 微分成分にローパスフィルタを適用する（ロール）
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_3_P                       0x0400  //!< 微分成分にローパスフィルタを適用する（ピッチ）
#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_FILTER_3_Y                       0x0800  //!< 微分成分にローパスフィルタを適用する（ヨー）

#define E2PDEF_FILTERGAIN_ENABLEFLAGS_ENABLE_THROTTLE_CORRECTION_BY_ATTITUDE  0x1000  // 姿勢角に応じたスロットル値補正を行う

///
/// @brief  レンジ設定
///
typedef struct _tag_ControllerRange
{
	_UWORD  RateRange_RP;
	_UWORD  RateRange_Y;
	_UWORD  PwmDriver_ScaledInput_Stop;
	_UWORD  PwmDriver_ScaledInput_Idle;
	_UWORD  PwmDriver_ScaledInput_Min;
	_UWORD  PwmDriver_ScaledInput_Max;
	_UBYTE  Reserved[17];
} E2PDEF_ControlRange;

//####################################################################################################
//	#0700	通信
//####################################################################################################
///
/// @brief  データ送信周期
///
typedef struct _tag_DataSendCycle
{
	_UBYTE  CommunicationProtocol;
	_UBYTE  CommunicationDevice;
	_UBYTE  PacketSendCycle[E2PDEF_SEND_PACKET_CYCLE_MAX];
} E2PDEF_DataSendCycle;

//####################################################################################################
//	#0800	安全機能
//####################################################################################################
///
/// @brief  安全機能
///
typedef struct _tag_SafetyFunction
{
	_UBYTE  GoHomeConditions;
	_UBYTE  SafetyFunctionFlags;
	_UBYTE  JudgementAlartBadSateliteNum;
	_UBYTE  JudgementAlartLessSateliteNum;
	_UBYTE  JudgementAlartLowVoltage;
	_UBYTE  JudgementAlartHalfVoltage;
	_UBYTE  JudgementStartGoHomeVoltage;
	_UBYTE  Reserved[22];
} E2PDEF_SafetyFunction;

//####################################################################################################
//	#0900	付加デバイス構成
//####################################################################################################
///
/// @brief  ジンバルサーボ設定
///
typedef struct _tag_GinbalServoSetting
{
	_UBYTE  Reserved[29];			// 未定☆
} E2PDEF_GinbalServoSetting;

///
/// @brief  リモコン・NMEA設定
///
typedef struct _tag_RemoconNmeaSetting
{
	_UBYTE  Reserved[29];			// 未定☆
} E2PDEF_RemoconNmeaSetting;

//####################################################################################################
//	#1000	特殊機能
//####################################################################################################
///
/// @brief  特殊機能 ☆
///
typedef struct _tag_SpecialFunctions
{
	_UDWORD SpecialFunctionFlag1;			// フラグ1 bit0-飛行禁止エリア機能有効 初期=1
	_UWORD  SpecialFunctionFlag2;			// フラグ2 bit0-上位制御MCU停止・始動許可 初期=0
	_UBYTE  TerminationChannel;				// 下位4ビット：ﾀｰﾐﾈｰｼｮﾝ用Ch番号 (0は無効, 1～18:Ch番号)
	_UBYTE  TerminationThreshold;			// ﾀｰﾐﾈｰｼｮﾝ閾値 (RCスケール値/8, 最上位ビットは予約)
	_UBYTE  Reserved[21];
} E2PDEF_SpecialFunctions;

//####################################################################################################
//	#1100	制限
//####################################################################################################
///
/// @brief  制御機能制限 ☆
///
typedef struct _tag_ControllerLimit
{
	_UBYTE  MaxHorizontalSpeedForAutonomous;  // 自律飛行 最大水平飛行速度 0.1m/s = 1
	_UBYTE  MaxVerticalSpeedForAutonomous;    // 自律飛行 最大昇降速度     0.1m/s = 1
	_UBYTE  MaxYawRateForAutonomous;          // 自律飛行 最大旋回速度     1deg/s = 1
	_UBYTE  MaxHorizontalSpeedForUserControl; // ﾕｰｻﾞ操作 最大水平飛行速度 0.1m/s = 1
	_UBYTE  MaxVerticalSpeedForUserControl;   // ﾕｰｻﾞ操作 最大昇降速度     0.1m/s = 1
	_UBYTE  MaxYawRateForUserControl;         // ﾕｰｻﾞ操作 最大旋回速度     1deg/s = 1
	_UBYTE  MaxRollPitchAngleForUserControl;  // ﾕｰｻﾞ操作 最大姿勢角       0.5deg = 1
	_UBYTE  Reserved[22];
} E2PDEF_ControllerLimit;

///
/// @brief  飛行エリア制限 ☆ 
///
typedef struct _tag_FlyZoneLimit
{
	_UBYTE  BitFieldInfo;			// ﾋﾞｯﾄﾌｨｰﾙﾄﾞ bit0~2 - 形状 0-無制限 1-楕円形 2-長方形 bit3 - 緯度経度設定有効 初期=1
	_UBYTE  HorizontalWidth[3];		// 横幅 0.1m=1 初期=10000
	_UBYTE  VerticalWidth[3];		// 縦幅 0.1m=1 初期=10000
	_UWORD  MaxHightAboveTheGround;	// 最大対地高度 0.1m=1 初期=1500
	_UWORD  MinHightAboveTheGround;	// 最小対地高度 -0.1m=1 初期=1500
	_UBYTE  Angle;					// 角度 2deg=1 初期=0
	_SDWORD CentralLatitude;		// 中心緯度 1度=10000000 初期=0
	_SDWORD CentralLongitude;		// 中心経度 1度=10000000 初期=0
	_UBYTE  Reserved[9];
} E2PDEF_FlyZoneLimit;

//####################################################################################################
//	#1200	カスタム
//####################################################################################################
///
/// @brief  カスタム対応パラメータ ☆ 
///
typedef struct _tag_CustomParameter1
{
	double  FreeParameter1;			// 自由1(double) 初期0
	double  FreeParameter2;			// 自由2(double) 初期0
	double  FreeParameter3;			// 自由3(double) 初期0
	float   FreeParameter7;			// 自由7(float)  初期0
	_UBYTE  FreeParameter9;			// 自由9 初期0
} E2PDEF_CustomParameter1;

///
/// @brief  カスタム対応パラメータ ☆ 
///
typedef struct _tag_CustomParameter2
{
	double  FreeParameter4;			// 自由4(double) 初期0
	double  FreeParameter5;			// 自由5(double) 初期0
	double  FreeParameter6;			// 自由6(double) 初期0
	float   FreeParameter8;			// 自由8(float)  初期0
	_UBYTE  FreeParameter10;		// 自由10 初期0
} E2PDEF_CustomParameter2;

#if defined(WIN32) || defined(IAR)
#pragma pack()
#else
#pragma unpack
#endif

#ifdef __cplusplus
class E2PDEF_Operator
{
private:
	static void _Copy(EepromData& dest, const void* pSrc, _UWORD BlockID);
	static void _Copy(void* pDest, const EepromData& src, size_t nSize);
	static bool _IsSame(const void* pA, const void* pB, size_t nSize);

public:
	//---------------------------------
	// コピー(EEPROM管理構造型への変換)
	//---------------------------------
	static void Copy(EepromData& dest, const E2PDEF_EepromSystemData& src) { _Copy(dest, &src, E2P_BID_EEPROMSYSTEMDATA); }
	static void Copy(EepromData& dest, const E2PDEF_UnitInfo& src) {_Copy(dest, &src, E2P_BID_UNITINFO);}
	
	// "製造情報"タブ
	static void Copy(EepromData& dest, const E2PDEF_SerialNumber& src) {_Copy(dest, &src, E2P_BID_SERIALNUMBER);}
	static void Copy(EepromData& dest, const E2PDEF_Assembler& src) {_Copy(dest, &src, E2P_BID_ASSEMBLER);}
	static void Copy(EepromData& dest, const E2PDEF_Seller& src) {_Copy(dest, &src, E2P_BID_SELLER);}
	static void Copy(EepromData& dest, const E2PDEF_Affiliation& src) {_Copy(dest, &src, E2P_BID_AFFILIATION);}
	static void Copy(EepromData& dest, const E2PDEF_ManufacturingFlags& src) {_Copy(dest, &src, E2P_BID_MANUFACTURINGFLAGS);}
	static void Copy(EepromData& dest, const E2PDEF_UserInfo& src) { _Copy(dest, &src, E2P_BID_USERINFO); }
	// "メンテ"タブ
	static void Copy(EepromData& dest, const E2PDEF_MaintenanceInfo& src) {_Copy(dest, &src, E2P_BID_MAINTENANCEINFO);}
	static void Copy(EepromData& dest, const E2PDEF_MaintenanceLog1& src) {_Copy(dest, &src, E2P_BID_MAINTENANCELOG1);}
	static void Copy(EepromData& dest, const E2PDEF_MaintenanceLog2& src) {_Copy(dest, &src, E2P_BID_MAINTENANCELOG2);}
	static void Copy(EepromData& dest, const E2PDEF_MaintenanceLog3& src) {_Copy(dest, &src, E2P_BID_MAINTENANCELOG3);}
	// "寿命管理"タブ
	static void Copy(EepromData& dest, const E2PDEF_LifeManagement1& src) { _Copy(dest, &src, E2P_BID_LIFEMANAGEMENT1); }
	static void Copy(EepromData& dest, const E2PDEF_LifeManagement2& src) { _Copy(dest, &src, E2P_BID_LIFEMANAGEMENT2); }
	static void Copy(EepromData& dest, const E2PDEF_LifeManagement3& src) { _Copy(dest, &src, E2P_BID_LIFEMANAGEMENT3); }
	static void Copy(EepromData& dest, const E2PDEF_LifeManagement4& src) { _Copy(dest, &src, E2P_BID_LIFEMANAGEMENT4); }
	// "機体構成"タブ
	static void Copy(EepromData& dest, const E2PDEF_MachineConstruct& src) { _Copy(dest, &src, E2P_BID_MACHINE_CONSTRUCT); }
	static void Copy(EepromData& dest, const E2PDEF_CircuitParameter& src) { _Copy(dest, &src, E2P_BID_CIRCUIT_PARAMETER); }
	static void Copy(EepromData& dest, const E2PDEF_GeometricalParameter& src) { _Copy(dest, &src, E2P_BID_GEOMETRICAL_PARAMETER); }
	// "校正"タブ
	static void Copy(EepromData& dest, const E2PDEF_HardIronCalibration& src) { _Copy(dest, &src, E2P_BID_HARD_IRON_CALIBRATION); }
	// "制御器"タブ
	static void Copy(EepromData& dest, const E2PDEF_ControllerGain1& src) { _Copy(dest, &src, E2P_BID_CONTROLLER_GAIN_1); }
	static void Copy(EepromData& dest, const E2PDEF_ControllerGain2& src) { _Copy(dest, &src, E2P_BID_CONTROLLER_GAIN_2); }
	static void Copy(EepromData& dest, const E2PDEF_FilterGain& src) { _Copy(dest, &src, E2P_BID_FILTER_GAIN); }
	static void Copy(EepromData& dest, const E2PDEF_ControlRange& src) { _Copy(dest, &src, E2P_BID_CONTROLLER_RANGE); }
	// "通信タブ"
	static void Copy(EepromData& dest, const E2PDEF_DataSendCycle& src) { _Copy(dest, &src, E2P_BID_DATA_SEND_CYCLE); }
#if 1	// 2015年4月以降
	// "安全機能"タブ
	static void Copy(EepromData& dest, const E2PDEF_SafetyFunction& src) { _Copy(dest, &src, E2P_BID_SAFETY_FUNCTION); }
	// "デバイス"タブ
	static void Copy(EepromData& dest, const E2PDEF_GinbalServoSetting& src) { _Copy(dest, &src, E2P_BID_GINBAL_SERVO_SETTING); }
	static void Copy(EepromData& dest, const E2PDEF_RemoconNmeaSetting& src) { _Copy(dest, &src, E2P_BID_REMOCON_NMEA_SETTING); }
	// "特殊機能"タブ
	static void Copy(EepromData& dest, const E2PDEF_SpecialFunctions& src) { _Copy(dest, &src, E2P_BID_SPECIAL_FUNCTION); }
#endif
	// "制限"タブ
	static void Copy(EepromData& dest, const E2PDEF_ControllerLimit& src) { _Copy(dest, &src, E2P_BID_CONTROLLER_LIMIT); }
	static void Copy(EepromData& dest, const E2PDEF_FlyZoneLimit& src) { _Copy(dest, &src, E2P_BID_FLYZONE_LIMIT); }
	// "カスタム"タブ
	static void Copy(EepromData& dest, const E2PDEF_CustomParameter1& src) { _Copy(dest, &src, E2P_BID_CUSTOM_PARAMETER1); }
	static void Copy(EepromData& dest, const E2PDEF_CustomParameter2& src) { _Copy(dest, &src, E2P_BID_CUSTOM_PARAMETER2); }

	//-----------------------------------
	// コピー(EEPROM管理構造型からの変換)
	//-----------------------------------
	// ""
	static void Copy(E2PDEF_EepromSystemData& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "MSAP"タブ
	static void Copy(E2PDEF_UnitInfo& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	// "製造情報"タブ
	static void Copy(E2PDEF_SerialNumber& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_Assembler& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_Seller& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_Affiliation& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_ManufacturingFlags& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_UserInfo& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "メンテ"タブ
	static void Copy(E2PDEF_MaintenanceInfo& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_MaintenanceLog1& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_MaintenanceLog2& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	static void Copy(E2PDEF_MaintenanceLog3& dest, const EepromData& src) {_Copy(&dest, src, sizeof(dest));}
	// "寿命管理"タブ
	static void Copy(E2PDEF_LifeManagement1& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_LifeManagement2& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_LifeManagement3& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_LifeManagement4& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "機体構成"タブ
	static void Copy(E2PDEF_MachineConstruct& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_CircuitParameter& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_GeometricalParameter& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "校正"タブ
	static void Copy(E2PDEF_HardIronCalibration& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "制御器"タブ
	static void Copy(E2PDEF_ControllerGain1& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_ControllerGain2& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_FilterGain& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_ControlRange& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "通信タブ"
	static void Copy(E2PDEF_DataSendCycle& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
#if 1	// 2015年4月以降
	// "安全機能"タブ
	static void Copy(E2PDEF_SafetyFunction& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "デバイス"タブ
	static void Copy(E2PDEF_GinbalServoSetting& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_RemoconNmeaSetting& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "特殊機能"タブ
	static void Copy(E2PDEF_SpecialFunctions& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
#endif
	// "制限"タブ
	static void Copy(E2PDEF_ControllerLimit& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_FlyZoneLimit& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	// "カスタム"タブ
	static void Copy(E2PDEF_CustomParameter1& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }
	static void Copy(E2PDEF_CustomParameter2& dest, const EepromData& src) { _Copy(&dest, src, sizeof(dest)); }

	//-------------------
	// ベリファイチェック
	//-------------------
	// "書き込みデータと読み込みデータの比較"
	static bool IsSame(const E2PDEF_EepromSystemData& a, E2PDEF_EepromSystemData& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "MSAP"タブ
	static bool IsSame(const E2PDEF_UnitInfo& a, const E2PDEF_UnitInfo& b) {return _IsSame(&a, &b, sizeof(a));}
	// "製造情報"タブ
	static bool IsSame(const E2PDEF_SerialNumber& a, const E2PDEF_SerialNumber& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_Assembler& a, const E2PDEF_Assembler& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_Seller& a, const E2PDEF_Seller& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_Affiliation& a, const E2PDEF_Affiliation& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_ManufacturingFlags& a, const E2PDEF_ManufacturingFlags& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_UserInfo& a, const E2PDEF_UserInfo& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "メンテ"タブ
	static bool IsSame(const E2PDEF_MaintenanceInfo& a, const E2PDEF_MaintenanceInfo& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_MaintenanceLog1& a, const E2PDEF_MaintenanceLog1& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_MaintenanceLog2& a, const E2PDEF_MaintenanceLog2& b) {return _IsSame(&a, &b, sizeof(a));}
	static bool IsSame(const E2PDEF_MaintenanceLog3& a, const E2PDEF_MaintenanceLog3& b) {return _IsSame(&a, &b, sizeof(a));}
	// "寿命管理"タブ
	static bool IsSame(const E2PDEF_LifeManagement1& a, const E2PDEF_LifeManagement1& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_LifeManagement2& a, const E2PDEF_LifeManagement2& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_LifeManagement3& a, const E2PDEF_LifeManagement3& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_LifeManagement4& a, const E2PDEF_LifeManagement4& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "機体構成"タブ
	static bool IsSame(const E2PDEF_MachineConstruct& a, const E2PDEF_MachineConstruct& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_CircuitParameter& a, const E2PDEF_CircuitParameter& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_GeometricalParameter& a, const E2PDEF_GeometricalParameter& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "校正"タブ
	static bool IsSame(const E2PDEF_HardIronCalibration& a, const E2PDEF_HardIronCalibration& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "制御器"タブ
	static bool IsSame(const E2PDEF_ControllerGain1& a, const E2PDEF_ControllerGain1& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_ControllerGain2& a, const E2PDEF_ControllerGain2& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_FilterGain& a, const E2PDEF_FilterGain& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_ControlRange& a, const E2PDEF_ControlRange& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "通信"タブ
	static bool IsSame(const E2PDEF_DataSendCycle& a, const E2PDEF_DataSendCycle& b) { return _IsSame(&a, &b, sizeof(a)); }
#if 1	// 2015年4月以降
	// "安全機能"タブ
	static bool IsSame(const E2PDEF_SafetyFunction& a, const E2PDEF_SafetyFunction& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "デバイス"タブ
	static bool IsSame(const E2PDEF_GinbalServoSetting& a, const E2PDEF_GinbalServoSetting& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_RemoconNmeaSetting& a, const E2PDEF_RemoconNmeaSetting& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "特殊機能"タブ
	static bool IsSame(const E2PDEF_SpecialFunctions& a, const E2PDEF_SpecialFunctions& b) { return _IsSame(&a, &b, sizeof(a)); }
#endif
	// "制限"タブ
	static bool IsSame(const E2PDEF_ControllerLimit& a, const E2PDEF_ControllerLimit& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_FlyZoneLimit& a, const E2PDEF_FlyZoneLimit& b) { return _IsSame(&a, &b, sizeof(a)); }
	// "カスタム"タブ
	static bool IsSame(const E2PDEF_CustomParameter1& a, const E2PDEF_CustomParameter1& b) { return _IsSame(&a, &b, sizeof(a)); }
	static bool IsSame(const E2PDEF_CustomParameter2& a, const E2PDEF_CustomParameter2& b) { return _IsSame(&a, &b, sizeof(a)); }
};
#endif

#endif