#ifndef INCLUDED_SYSTEMTIME_W_H
#define INCLUDED_SYSTEMTIME_W_H

#include "StandardDef.h"

inline void SetSystemTime(_UDWORD newtime)
{
	extern _UQWORD __gSystemTime;
	extern _UQWORD __gSystemTicks;
	static _UDWORD SystemTimeOffeset = 0;

	// �I�t�Z�b�g�̏�����
	if (SystemTimeOffeset == 0) SystemTimeOffeset = newtime;

	// �o�ߎ��ԁi�I�[�o�[�t���[���ĂĂ�OK�j
	_UDWORD delta = (newtime - SystemTimeOffeset) - (static_cast<_UDWORD>(__gSystemTime));

	// �o�ߎ��Ԃ�1�b�ȏ�̂Ƃ��i�X���[�v���畜�A�����Ƃ��Ȃǁj
	if (delta >= 1000)
	{
		SystemTimeOffeset = newtime - 1000; 
	}

	// 64�r�b�g�����̍X�V
	__gSystemTime += delta;
	__gSystemTicks = __gSystemTime;
}

#endif