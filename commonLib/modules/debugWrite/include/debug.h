//
//	�f�o�b�M���O�N���X
//
//	X-Monitor �̃\�����[�V�����ŊJ�����D
//	�J���������惉�C�u�����Ɉړ�
//
//	stdafx.h �� CDEBUG_CONSOLEMODE ���`����Əo�͂��R���\�[���ɕ\���ł��܂��D
//
//	stdafx.h �� CDEBUG_DEBUGMODE �� CDEBUG_CONSOLEMODE ���`����ƁC
//	Release �r���h�ł��o�͂��R���\�[���ɕ\���ł��܂��D
//
//	stdafx.h �� CDEBUG_DONT_CHANGE_LOCALE ��ݒ肷��ƁC���P�[���̕ύX���s���܂���D
//


#pragma once

#include <windows.h>
#include <tchar.h>

class CDebug
{
public:
	static int  GlobalInt[8];

public:
	// ��ʕ\���n�֐�
	static void puts(LPCSTR string);
	static void puts(LPCWSTR string);
	static void printf(LPCSTR pszFormat, ...);
	static void printf(LPCWSTR pszFormat, ...);

	static void PrintGlobalInt(LPCTSTR pAdditionalMessage = NULL);

	// ���O�n�֐�
	//static void D
};