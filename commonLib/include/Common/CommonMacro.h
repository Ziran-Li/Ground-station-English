#pragma once

#include <windows.h>
#include <tchar.h>

#define _USE_MATH_DEFINES
#include <math.h>

// 用户定义的常量
#define CREF_RED       (COLORREF)0x0000FF
#define CREF_GREEN     (COLORREF)0x00FF00
#define CREF_BLUE      (COLORREF)0xFF0000
#define CREF_BLACK     (COLORREF)0x000000
#define CREF_WHITE     (COLORREF)0xFFFFFF
#define CREF_GRAY      (COLORREF)0x808080
#define CREF_DARKGRAY  (COLORREF)0x404040
#define CREF_LIGHTGRAY (COLORREF)0xC0C0C0
#define CREF_DARKBLUE  (COLORREF)0x400000

// 宏函数
#define SAFE_DELETE(p)       {if( p ) {delete p; p = NULL;}}
#define SAFE_DELETE_ARRAY(p) {if( p ) {delete[] p; p = NULL;}}

#define ROUND(x) (static_cast<int>(floor((x) + 0.5))) //舍入函数。 然而，-0.5 将为 0。 (-0.4-> 0-0.5-> 0-0.6->-1)
#define BOOL2HRESULT(x) ((x) ? S_OK : E_FAIL)         // 将 HRESULT 值转换为的类型 BOOL 值与成功的真谛

#define TODEGREE(x) ((x) * 180.0 / M_PI)
#define TORADIAN(x) ((x) * M_PI / 180.0)

// 为子线程结束消息的处理
inline BOOL CheckQuitMessage()
{
	MSG msg;
	if( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) ) {
		while( GetMessage(&msg, NULL, 0, 0) >= 0 ) {
			if( msg.message == WM_QUIT ) {
				// 终止语句
				return TRUE;
			}

			if( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == 0 ) { // 退出循环，如果你有邮件
				return FALSE;
			}
		}
	}

	return FALSE;
}

// 能写的库文件包含在单独的一行是命中注定
#ifdef _DEBUG
#define DEBUG_LIB_CHAR "D"
#else
#define DEBUG_LIB_CHAR
#endif

// (仅在调试版本中有效) 将引发异常，当你运行在主线程以外的
inline void AllowOnlyMainThread(BOOL InitialRun = FALSE)
{
#ifdef _DEBUG
	static DWORD MainThreadId = 0;
	if( InitialRun ) {
		if( MainThreadId ) {
			throw(_T("已初始化"));
		}
		MainThreadId = ::GetCurrentThreadId();
	}

	if( MainThreadId == 0 ) {
		throw(_T("未初始化"));
	}

	if( MainThreadId != ::GetCurrentThreadId() ) {
		throw(_T("在主线程中运行"));
	}
#endif
}