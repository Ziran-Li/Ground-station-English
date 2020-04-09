//
//		ControlInfo.h
//
//		制御情報格納用クラスの定義
//
//		'09/01/17	Ver 0.01	プログラミング開始
//
//		X-UFO 組み込み用の ControlInfo とは別物なので注意！
//
//

#ifndef INCLUDED_CONTROLINFO_H
#define INCLUDED_CONTROLINFO_H

#include <windows.h>
//#include "Core/Common/Globaldef.h"

#ifdef CPP_CLI
#undef INCLUDED_GLOBALDEF_H
#include "Globaldef.h"
#else
#include "Globaldef.h"
#endif

//*******************************************************************************
//	C++, C++/CLI 両対応にするためのプリプロセッサ定義
//
//	Note:
//		本ヘッダファイルをインクルードする前に
//
//		#define CPP_CLI
//
//		とすると，クラス・構造体定義がC++/CLI用（マネージドコード）になります
//
//		__CLASS__, __STRUCT__ で定義されるクラス/構造体は必ずコンストラクタと
//		クラス名(const ::クラス名& obj) の形式のコピーコンストラクタを有して
//		いなければなりません．後者のコードは C++ のコードでは単なるコピーコンスト
//		ラクタとなります．C++/CLI では C++ 定義のインスタンスから C++/CLI 
//		定義のインスタンスを生成する引数付きコンストラクタとなります．
//		グローバル名前空間からの参照子"::"は忘れずに付けてください．
//*******************************************************************************
#ifdef CPP_CLI

// C++/CLI 版
#define __CLASS     public ref class  // C++/CLI クラス定義
#define __STRUCT    public ref struct // C++/CLI 構造体定義
#define __ENUM      enum class        // C++/CLI 列挙体定義（クラス内）
#define __CH(TYPE)  TYPE^             // クラスハンドラ
#define __CHR(TYPE) TYPE^
#define __IND(PTR)  PTR
#define __GCN       gcnew             // インスタンス化キーワード gcnew

#else

// C++ 版
#define __CLASS     class             // 通常のクラス定義
#define __STRUCT    struct            // 通常の構造体定義
#define __ENUM      enum              // 通常の列挙体定義
#define __CH(TYPE)  TYPE              // そのまま型を返す （C++/CLI版では ^ が型名の後ろに付加される）
#define __CHR(TYPE) TYPE&
#define __IND(PTR)  *(PTR)
#define __GCN                         // 空文字に置換     （C++/CLI版では gcnew になる）

#endif

__STRUCT SYSTEMTIME_T
{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;

	SYSTEMTIME_T() : wYear(0), wMonth(0), wDayOfWeek(0), wDay(0), wHour(0), wMinute(0), wSecond(0), wMilliseconds(0) {}
	SYSTEMTIME_T(const ::SYSTEMTIME_T& obj) : wYear(obj.wYear), wMonth(obj.wMonth), wDayOfWeek(obj.wDayOfWeek), wHour(obj.wHour), wMinute(obj.wMinute), wSecond(obj.wSecond),
		wMilliseconds(obj.wMilliseconds) {}
	SYSTEMTIME_T(const ::SYSTEMTIME& obj) : wYear(obj.wYear), wMonth(obj.wMonth), wDayOfWeek(obj.wDayOfWeek), wHour(obj.wHour), wMinute(obj.wMinute), wSecond(obj.wSecond),
		wMilliseconds(obj.wMilliseconds) {}
	__CHR(SYSTEMTIME_T) operator = (const ::SYSTEMTIME& obj)
	{
		wYear         = obj.wYear;
		wMonth        = obj.wMonth;
		wDayOfWeek    = obj.wDayOfWeek;
		wDay          = obj.wDay;
		wHour         = obj.wHour;
		wMinute       = obj.wMinute;
		wSecond       = obj.wSecond;
		wMilliseconds = obj.wMilliseconds;
		return __IND(this);
	}
};

//*******************************************************************************
//	機体データ
//*******************************************************************************
__CLASS CMavState
{
public:

	__ENUM CONST_VALUES {
		LENGTH_MAVNAME   = 32,
		SIZE_GENERALAREA = 32,
	};

public:

	// 機体名
#ifdef CPP_CLI
	array<WCHAR>^ m_MavName;
#else
	WCHAR m_MavName[LENGTH_MAVNAME];
#endif

	// 制御モード
	CONTROLMODE m_nControlMode;
	
	// 飛行モード
	FLIGHTMODE  m_nFlightMode;

	// 下位制御用MCUに関するデータ
#ifdef CPP_CLI
	
#else
	LowLevelMCUData m_LowLevelMCUData;
#endif

	// システム健全度データ
#ifndef CPP_CLI
	SystemHealthData m_HealthData;
#endif

	// ファンクション実行地点データ
#ifndef CPP_CLI
	ActionPoint m_ActionPoint;
#endif

	// 各種センサデータ
	__CH(IMUData) m_IMUData;
	__CH(GPSData) m_GPSData;
	__CH(GPSInfo) m_GPSInfo;
	__CH(NAVData) m_NAVData;
	__CH(RefData) m_RefData;

	// 座標データ
	__CH(PosData) m_OrgData;     // GPS原点情報
	__CH(LocalPos) m_LocalPos;    // GPS原点を基準とする局所座標
	
	// マルチウェイポイント最後尾データ
	__CH(MultiWaypointElement) m_MultiWaypointEnd;

	// 機体データ
	__CH(SysData) m_SysData;

	// 汎用的に使える記録エリア
#ifdef CPP_CLI
	array<long long>^ m_UndefinedInt;
	array<double>^    m_UndefinedReal;
#else
	long long m_UndefinedInt [SIZE_GENERALAREA]; // 内容を規定していない整数データ（MAVはどんなデータを送ってもよい．
	double    m_UndefinedReal[SIZE_GENERALAREA]; // 内容を規定していない実数データ  研究毎にデータの内容を規定する．）
#endif

	// PPM制御入力データ
	__CH(PropoData) m_InputData;

	// デバッグデータ
#ifndef CPP_CLI
	__CH(CQUADWORD) m_Debug;
#endif

public:
	CMavState() :
		m_nControlMode(CONTROLMODE_UNDEFINED),
		m_nFlightMode(FLIGHTMODE_UNDEFINED),
		m_IMUData(__GCN IMUData()),
		m_GPSData(__GCN GPSData()),
		m_GPSInfo(__GCN GPSInfo()),
		m_NAVData(__GCN NAVData()),
		m_RefData(__GCN RefData()),
		m_OrgData(__GCN PosData()),
		m_LocalPos(__GCN LocalPos()),
		m_MultiWaypointEnd(__GCN MultiWaypointElement()),
		m_SysData(__GCN SysData()),
		m_InputData(__GCN PropoData())
	{
#ifndef CPP_CLI
		ZeroMemory(m_MavName,       sizeof(m_MavName));
		ZeroMemory(m_UndefinedInt,  sizeof(m_UndefinedInt));
		ZeroMemory(m_UndefinedReal, sizeof(m_UndefinedReal));
#else
		m_MavName       = gcnew array<WCHAR>((int)CONST_VALUES::LENGTH_MAVNAME);
		m_UndefinedInt  = gcnew array<long long>((int)CONST_VALUES::SIZE_GENERALAREA);
		m_UndefinedReal = gcnew array<double>   ((int)CONST_VALUES::SIZE_GENERALAREA);
#endif
	}

	CMavState(const ::CMavState& obj) :
		m_nControlMode((CONTROLMODE)obj.m_nControlMode),
		m_nFlightMode((FLIGHTMODE)obj.m_nFlightMode),
		m_IMUData(__GCN IMUData(obj.m_IMUData)),
		m_GPSData(__GCN GPSData(obj.m_GPSData)),
		m_GPSInfo(__GCN GPSInfo(obj.m_GPSInfo)),
		m_NAVData(__GCN NAVData(obj.m_NAVData)),
		m_RefData(__GCN RefData(obj.m_RefData)),
		m_OrgData(__GCN PosData(obj.m_OrgData)),
		m_LocalPos(__GCN LocalPos(obj.m_LocalPos)),
		m_MultiWaypointEnd(__GCN MultiWaypointElement(obj.m_MultiWaypointEnd)),
		m_SysData(__GCN SysData(obj.m_SysData)),
		m_InputData(__GCN PropoData(obj.m_InputData))
	{
#ifndef CPP_CLI
		CopyMemory(m_MavName,       obj.m_MavName,       sizeof(m_MavName));
		CopyMemory(m_UndefinedInt,  obj.m_UndefinedInt,  sizeof(m_UndefinedInt));
		CopyMemory(m_UndefinedReal, obj.m_UndefinedReal, sizeof(m_UndefinedReal));
		m_LowLevelMCUData = obj.m_LowLevelMCUData;
		m_ActionPoint = obj.m_ActionPoint;
		m_Debug = obj.m_Debug;
#else
		m_MavName = gcnew array<WCHAR>((int)CONST_VALUES::LENGTH_MAVNAME);
		for( int i = 0; i < (int)CONST_VALUES::LENGTH_MAVNAME; i++ ) {
			m_MavName[i] = obj.m_MavName[i];
		}

		m_UndefinedInt  = gcnew array<long long>((int)CONST_VALUES::SIZE_GENERALAREA);
		m_UndefinedReal = gcnew array<double>   ((int)CONST_VALUES::SIZE_GENERALAREA);
		for( int i = 0; i < (int)CONST_VALUES::SIZE_GENERALAREA; i++ ) {
			m_UndefinedInt[i]  = obj.m_UndefinedInt[i];
			m_UndefinedReal[i] = obj.m_UndefinedReal[i];
		}
#endif
	}
};

//*******************************************************************************
//	地上局データ
//*******************************************************************************
__CLASS CGcsState
{

public:
	// システムタイム
	__CH(SYSTEMTIME_T)   m_SystemTime;

	// プロポデータ
	__CH(PropoData)      m_PropoData;
	LONG                 m_Volume;    // 暫定．そのうち PropoData と統合する

	// 気圧計データ
	__CH(AnemometerData) m_AnemometerData;

	// 通信状態
	UINT      m_Tx_bps;          // 現在の転送トラフィック
	UINT      m_Rx_bps;          // 現在の受信トラフィック
	double    m_Ping_Reply_Rate; // PING コマンドの返信率

public:
	CGcsState() :
		m_PropoData(__GCN PropoData()), 
		m_Volume(0), 
		m_Tx_bps(0), 
		m_Rx_bps(0),
		m_Ping_Reply_Rate(0.0)
	{}

	CGcsState(const ::CGcsState& obj) : 
		m_SystemTime(__GCN SYSTEMTIME_T(obj.m_SystemTime)), 
		m_PropoData(__GCN PropoData(obj.m_PropoData)),  
		m_Volume(obj.m_Volume),
		m_AnemometerData(__GCN AnemometerData(obj.m_AnemometerData)),
		m_Tx_bps(obj.m_Tx_bps), 
		m_Rx_bps(obj.m_Rx_bps),
		m_Ping_Reply_Rate(obj.m_Ping_Reply_Rate)
	{}
};

#undef __CLASS
#undef __STRUCT
#undef __ENUM
#undef __CH
#undef __CHR
#undef __IND
#undef __GCN

#endif