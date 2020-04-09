#include "SystemTime.h"
#include "SystemTimeSetter.h"

_UQWORD __gSystemTime  = 0;     //!< システム時刻 [ms]
_UQWORD __gSystemTicks = 0;     //!< 時刻カウント数
_UBYTE  __gSystemTickWidth = 1; //!< 時間の刻み幅

void SetSystemTickWidth(_UBYTE newvalue)
{
	if (__gSystemTicks == 0)
	{
		// システム時刻の更新前のみ設定可能
		__gSystemTickWidth = newvalue;
	}
}

void CountUpSystemTime()
{
	__gSystemTime += __gSystemTickWidth;
	__gSystemTicks++;
}