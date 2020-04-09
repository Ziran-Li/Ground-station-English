#ifndef INCLUDED_SYSTEMTIME_W_H
#define INCLUDED_SYSTEMTIME_W_H

#include "StandardDef.h"

inline void SetSystemTime(_UDWORD newtime)
{
	extern _UQWORD __gSystemTime;
	extern _UQWORD __gSystemTicks;
	static _UDWORD SystemTimeOffeset = 0;

	// オフセットの初期化
	if (SystemTimeOffeset == 0) SystemTimeOffeset = newtime;

	// 経過時間（オーバーフローしててもOK）
	_UDWORD delta = (newtime - SystemTimeOffeset) - (static_cast<_UDWORD>(__gSystemTime));

	// 経過時間が1秒以上のとき（スリープから復帰したときなど）
	if (delta >= 1000)
	{
		SystemTimeOffeset = newtime - 1000; 
	}

	// 64ビット時刻の更新
	__gSystemTime += delta;
	__gSystemTicks = __gSystemTime;
}

#endif