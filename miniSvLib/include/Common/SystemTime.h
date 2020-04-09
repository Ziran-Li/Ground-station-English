#ifndef INCLUDED_SYSTEMTIME_H
#define INCLUDED_SYSTEMTIME_H

#include "StandardDef.h"

//
// 用途：1分以内の時間変化の取得
//
// 絶対時間の取得に使用しないで下さい
// 約1分でオーバーフロー
//
inline _UWORD GetSystemTimeShort()
{
	extern _UQWORD __gSystemTime;
	return (_UWORD)(__gSystemTime);
}

//
// 用途：1分以上1ヶ月以内の時間変化の取得
//
// 絶対時間の取得に使用しないで下さい
// 約1ヶ月弱でオーバーフロー
//
inline _UDWORD GetSystemTime()
{
	extern _UQWORD __gSystemTime;
	return (_UDWORD)(__gSystemTime);
}

//
// 用途：1ヶ月以上の時間変化の計測
// 　　　絶対時間の取得
//
// 約50万年でオーバーフロー
//
inline _UQWORD GetSystemTimeLong()
{
	extern _UQWORD __gSystemTime;
	return __gSystemTime;
}

/*
// t0 
inline bool IsTimePassedShort(_UWORD t0, _UWORD t1)
{

}
*/

#endif