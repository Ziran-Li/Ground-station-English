///
/// @file       StandardDef.h
///
/// @brief      プロジェクト共通定義
///

#ifndef INCLUDED_STANDARDDEF_H
#define INCLUDED_STANDARDDEF_H

// C言語環境専用インクルードファイル
#ifndef __cplusplus
#include <stdbool.h>
#endif

// 環境毎に実装の異なるインクルードファイル
#include "CompatibleDefine/_math.h"
#include "CompatibleDefine/typedefine.h"
#include "CompatibleDefine/heap.h"

// デバッグ用モジュール
#include "Debug.h"

// VisualStudio + c でのインライン展開
#if !defined(__cplusplus) && defined(WIN32)
#define inline _inline
#endif

// NULLポインタの定義(WinDef.h と同じ)
#ifndef NULL

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#endif

// BOOL型の定義(WinDef.h と同じ)
#ifndef BOOL

typedef int BOOL;
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif

// 1バイトブール型 (TRUE, FALSE 指定可能)
typedef _UBYTE CBOOL;

// 1バイトエラー型
typedef char CRESULT;
#define C_OK             (CRESULT)0   //!< 成功
#define C_FAIL           (CRESULT)1   //!< 失敗（メッセージ：エラーです。原因は特定できないか、定義されていません）
#define C_UNSUPPORTED    (CRESULT)2   //!< 失敗（メッセージ：その機能はサポートされていません。）
#define C_NOTIMPLIMENTED (CRESULT)3   //!< 失敗（メッセージ：その機能は実装されていません。）
#define C_INVALIDARG     (CRESULT)4   //!< 失敗（メッセージ：引数が指定が間違っています。）
#define C_INVALIDRANGE   (CRESULT)5   //!< 失敗（メッセージ：引数の範囲が間違っています。）
#define C_INVALIDPOINTER (CRESULT)6   //!< 失敗（メッセージ：ポインタが不正です。）
#define C_OUTOFMEMORY    (CRESULT)7   //!< 失敗（メッセージ：この操作を完了するのに十分な記憶域がありません。）
#define C_OUTOFBUFFER    (CRESULT)8   //!< 失敗（メッセージ：バッファサイズの制限に達したため、この操作を完了できませんでした。）
#define C_NODATA         (CRESULT)9   //!< 失敗（メッセージ：データが存在しません。）
#define C_BUSY           (CRESULT)10  //!< 失敗（メッセージ：ビジー状態のため、操作を完了できませんでした。）
#define C_NOTACCEPTED    (CRESULT)11  //!< 失敗（メッセージ：その操作は受け付けられませんでした。）
#define C_NOTINITIALIZED (CRESULT)12  //!< 失敗（メッセージ：初期化されていないため、この操作を完了できませんでした。）
#define C_TIMEOUT        (CRESULT)13  //!< 失敗（メッセージ：所定の時間内に処理を終えられなかったため、この操作を完了できませんでした。）
#define C_INVALIDDATA    (CRESULT)14  //!< 失敗（メッセージ：データが無効です。）
#define C_UNEXPECTED     (CRESULT)127 //!< 失敗（メッセージ：想定外です。／致命的なエラーが発生しました。）
#define C_USER           (CRESULT)128 //!< 128～255はモジュール毎に自由に定義できるエラー値

#define ISSUCCESS(X) ((CRESULT)(X) == 0)
#define ISOK(X)      ((CRESULT)(X) == 0) // 短縮版
#define ISFAIL(X)    ((CRESULT)(X) != 0)

// メモリ





#ifdef __cplusplus
template<typename FUNC_PTR> struct Callback
{
	FUNC_PTR pFunc;
	void*    pUser;
	
	Callback() : pFunc(NULL), pUser(NULL) {}
};
#endif


//
// 8バイト共用体
//

typedef union _tag_QUADWORD
{
	double  DOUBLE[1];
	float   FLOAT[2];
	_SQWORD SQWORD[1];
	_UQWORD UQWORD[1];
	_SDWORD SDWORD[2];
	_UDWORD UDWORD[2];
	_SWORD  SWORD[4];
	_UWORD  UWORD[4];
	_SBYTE  SBYTE[8];
	_UBYTE  UBYTE[8];
	
#ifdef __cplusplus
	_tag_QUADWORD() { UQWORD[0] = 0; }
	_tag_QUADWORD(const ::_tag_QUADWORD& obj) { UQWORD[0] = obj.UQWORD[0]; }
#endif
} QUADWORD;

//
// 16バイト共用体
//

typedef union _tag_OCTWORD 
{
	double  DOUBLE[2];
	float   FLOAT[4];
	_SQWORD SQWORD[2];
	_UQWORD UQWORD[2];
	_SDWORD SDWORD[4];
	_UDWORD UDWORD[4];
	_SWORD  SWORD[8];
	_UWORD  UWORD[8];
	_SBYTE  SBYTE[16];
	_UBYTE  UBYTE[16];
#ifdef __cplusplus
	_tag_OCTWORD() { UQWORD[0] = 0; UQWORD[1] = 0; }
	_tag_OCTWORD(const ::_tag_OCTWORD& obj) { UQWORD[0] = obj.UQWORD[0]; UQWORD[1] = obj.UQWORD[1]; }
#endif
} OCTWORD;

/******************************************************************************
*******************************************************************************
	制御計算用インライン関数
*******************************************************************************
******************************************************************************/
inline double Saturation(double value, double lb, double ub) // lb 以下 or ub 以上で飽和
{
	if( value < lb ) return lb;
	else if( value > ub ) return ub;
	else return value;
}

inline double ULimit(double value, double ulim)
{
	return (value > ulim) ? ulim : value;
}

inline double LLimit(double value, double llim)
{
	return (value < llim) ? llim : value;
}

inline double DeadBand(double value, double deadband) // 不感帯．|value| < deadband → 0
{
	if( value > deadband )       return value - deadband;
	else if( value < -deadband ) return value + deadband;
	else                         return 0.0;
}

inline double normalize_angle(double angle) // 角度を-π～πの範囲に補正する関数，方位の差もこれで補正できる
{
	if (angle < 0) {
		return -(fmod(fabs(angle) + M_PI, 2.0*M_PI) - M_PI);
	}
	else {
		return fmod(angle + M_PI, 2.0*M_PI) - M_PI;
	}
}


extern QUADWORD gDebug;

#endif