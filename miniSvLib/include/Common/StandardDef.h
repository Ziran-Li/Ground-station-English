///
/// @file       StandardDef.h
///
/// @brief      �v���W�F�N�g���ʒ�`
///

#ifndef INCLUDED_STANDARDDEF_H
#define INCLUDED_STANDARDDEF_H

// C�������p�C���N���[�h�t�@�C��
#ifndef __cplusplus
#include <stdbool.h>
#endif

// �����Ɏ����̈قȂ�C���N���[�h�t�@�C��
#include "CompatibleDefine/_math.h"
#include "CompatibleDefine/typedefine.h"
#include "CompatibleDefine/heap.h"

// �f�o�b�O�p���W���[��
#include "Debug.h"

// VisualStudio + c �ł̃C�����C���W�J
#if !defined(__cplusplus) && defined(WIN32)
#define inline _inline
#endif

// NULL�|�C���^�̒�`(WinDef.h �Ɠ���)
#ifndef NULL

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#endif

// BOOL�^�̒�`(WinDef.h �Ɠ���)
#ifndef BOOL

typedef int BOOL;
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif

// 1�o�C�g�u�[���^ (TRUE, FALSE �w��\)
typedef _UBYTE CBOOL;

// 1�o�C�g�G���[�^
typedef char CRESULT;
#define C_OK             (CRESULT)0   //!< ����
#define C_FAIL           (CRESULT)1   //!< ���s�i���b�Z�[�W�F�G���[�ł��B�����͓���ł��Ȃ����A��`����Ă��܂���j
#define C_UNSUPPORTED    (CRESULT)2   //!< ���s�i���b�Z�[�W�F���̋@�\�̓T�|�[�g����Ă��܂���B�j
#define C_NOTIMPLIMENTED (CRESULT)3   //!< ���s�i���b�Z�[�W�F���̋@�\�͎�������Ă��܂���B�j
#define C_INVALIDARG     (CRESULT)4   //!< ���s�i���b�Z�[�W�F�������w�肪�Ԉ���Ă��܂��B�j
#define C_INVALIDRANGE   (CRESULT)5   //!< ���s�i���b�Z�[�W�F�����͈̔͂��Ԉ���Ă��܂��B�j
#define C_INVALIDPOINTER (CRESULT)6   //!< ���s�i���b�Z�[�W�F�|�C���^���s���ł��B�j
#define C_OUTOFMEMORY    (CRESULT)7   //!< ���s�i���b�Z�[�W�F���̑������������̂ɏ\���ȋL���悪����܂���B�j
#define C_OUTOFBUFFER    (CRESULT)8   //!< ���s�i���b�Z�[�W�F�o�b�t�@�T�C�Y�̐����ɒB�������߁A���̑���������ł��܂���ł����B�j
#define C_NODATA         (CRESULT)9   //!< ���s�i���b�Z�[�W�F�f�[�^�����݂��܂���B�j
#define C_BUSY           (CRESULT)10  //!< ���s�i���b�Z�[�W�F�r�W�[��Ԃ̂��߁A����������ł��܂���ł����B�j
#define C_NOTACCEPTED    (CRESULT)11  //!< ���s�i���b�Z�[�W�F���̑���͎󂯕t�����܂���ł����B�j
#define C_NOTINITIALIZED (CRESULT)12  //!< ���s�i���b�Z�[�W�F����������Ă��Ȃ����߁A���̑���������ł��܂���ł����B�j
#define C_TIMEOUT        (CRESULT)13  //!< ���s�i���b�Z�[�W�F����̎��ԓ��ɏ������I�����Ȃ��������߁A���̑���������ł��܂���ł����B�j
#define C_INVALIDDATA    (CRESULT)14  //!< ���s�i���b�Z�[�W�F�f�[�^�������ł��B�j
#define C_UNEXPECTED     (CRESULT)127 //!< ���s�i���b�Z�[�W�F�z��O�ł��B�^�v���I�ȃG���[���������܂����B�j
#define C_USER           (CRESULT)128 //!< 128�`255�̓��W���[�����Ɏ��R�ɒ�`�ł���G���[�l

#define ISSUCCESS(X) ((CRESULT)(X) == 0)
#define ISOK(X)      ((CRESULT)(X) == 0) // �Z�k��
#define ISFAIL(X)    ((CRESULT)(X) != 0)

// ������





#ifdef __cplusplus
template<typename FUNC_PTR> struct Callback
{
	FUNC_PTR pFunc;
	void*    pUser;
	
	Callback() : pFunc(NULL), pUser(NULL) {}
};
#endif


//
// 8�o�C�g���p��
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
// 16�o�C�g���p��
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
	����v�Z�p�C�����C���֐�
*******************************************************************************
******************************************************************************/
inline double Saturation(double value, double lb, double ub) // lb �ȉ� or ub �ȏ�ŖO�a
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

inline double DeadBand(double value, double deadband) // �s���сD|value| < deadband �� 0
{
	if( value > deadband )       return value - deadband;
	else if( value < -deadband ) return value + deadband;
	else                         return 0.0;
}

inline double normalize_angle(double angle) // �p�x��-�΁`�΂͈̔͂ɕ␳����֐��C���ʂ̍�������ŕ␳�ł���
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