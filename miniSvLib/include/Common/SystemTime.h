#ifndef INCLUDED_SYSTEMTIME_H
#define INCLUDED_SYSTEMTIME_H

#include "StandardDef.h"

//
// �p�r�F1���ȓ��̎��ԕω��̎擾
//
// ��Ύ��Ԃ̎擾�Ɏg�p���Ȃ��ŉ�����
// ��1���ŃI�[�o�[�t���[
//
inline _UWORD GetSystemTimeShort()
{
	extern _UQWORD __gSystemTime;
	return (_UWORD)(__gSystemTime);
}

//
// �p�r�F1���ȏ�1�����ȓ��̎��ԕω��̎擾
//
// ��Ύ��Ԃ̎擾�Ɏg�p���Ȃ��ŉ�����
// ��1������ŃI�[�o�[�t���[
//
inline _UDWORD GetSystemTime()
{
	extern _UQWORD __gSystemTime;
	return (_UDWORD)(__gSystemTime);
}

//
// �p�r�F1�����ȏ�̎��ԕω��̌v��
// �@�@�@��Ύ��Ԃ̎擾
//
// ��50���N�ŃI�[�o�[�t���[
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