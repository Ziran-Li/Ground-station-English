#include "SystemTime.h"
#include "SystemTimeSetter.h"

_UQWORD __gSystemTime  = 0;     //!< �V�X�e������ [ms]
_UQWORD __gSystemTicks = 0;     //!< �����J�E���g��
_UBYTE  __gSystemTickWidth = 1; //!< ���Ԃ̍��ݕ�

void SetSystemTickWidth(_UBYTE newvalue)
{
	if (__gSystemTicks == 0)
	{
		// �V�X�e�������̍X�V�O�̂ݐݒ�\
		__gSystemTickWidth = newvalue;
	}
}

void CountUpSystemTime()
{
	__gSystemTime += __gSystemTickWidth;
	__gSystemTicks++;
}