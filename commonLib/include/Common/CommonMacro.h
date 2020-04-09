#pragma once

#include <windows.h>
#include <tchar.h>

#define _USE_MATH_DEFINES
#include <math.h>

// �û�����ĳ���
#define CREF_RED       (COLORREF)0x0000FF
#define CREF_GREEN     (COLORREF)0x00FF00
#define CREF_BLUE      (COLORREF)0xFF0000
#define CREF_BLACK     (COLORREF)0x000000
#define CREF_WHITE     (COLORREF)0xFFFFFF
#define CREF_GRAY      (COLORREF)0x808080
#define CREF_DARKGRAY  (COLORREF)0x404040
#define CREF_LIGHTGRAY (COLORREF)0xC0C0C0
#define CREF_DARKBLUE  (COLORREF)0x400000

// �꺯��
#define SAFE_DELETE(p)       {if( p ) {delete p; p = NULL;}}
#define SAFE_DELETE_ARRAY(p) {if( p ) {delete[] p; p = NULL;}}

#define ROUND(x) (static_cast<int>(floor((x) + 0.5))) //���뺯���� Ȼ����-0.5 ��Ϊ 0�� (-0.4-> 0-0.5-> 0-0.6->-1)
#define BOOL2HRESULT(x) ((x) ? S_OK : E_FAIL)         // �� HRESULT ֵת��Ϊ������ BOOL ֵ��ɹ�������

#define TODEGREE(x) ((x) * 180.0 / M_PI)
#define TORADIAN(x) ((x) * M_PI / 180.0)

// Ϊ���߳̽�����Ϣ�Ĵ���
inline BOOL CheckQuitMessage()
{
	MSG msg;
	if( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) ) {
		while( GetMessage(&msg, NULL, 0, 0) >= 0 ) {
			if( msg.message == WM_QUIT ) {
				// ��ֹ���
				return TRUE;
			}

			if( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == 0 ) { // �˳�ѭ������������ʼ�
				return FALSE;
			}
		}
	}

	return FALSE;
}

// ��д�Ŀ��ļ������ڵ�����һ��������ע��
#ifdef _DEBUG
#define DEBUG_LIB_CHAR "D"
#else
#define DEBUG_LIB_CHAR
#endif

// (���ڵ��԰汾����Ч) �������쳣���������������߳������
inline void AllowOnlyMainThread(BOOL InitialRun = FALSE)
{
#ifdef _DEBUG
	static DWORD MainThreadId = 0;
	if( InitialRun ) {
		if( MainThreadId ) {
			throw(_T("�ѳ�ʼ��"));
		}
		MainThreadId = ::GetCurrentThreadId();
	}

	if( MainThreadId == 0 ) {
		throw(_T("δ��ʼ��"));
	}

	if( MainThreadId != ::GetCurrentThreadId() ) {
		throw(_T("�����߳�������"));
	}
#endif
}