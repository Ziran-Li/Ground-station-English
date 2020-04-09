
#include "stdafx.h"

#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

#ifndef CDEBUG_DONT_CHANGE_LOCALE
#ifdef _UNICODE
#include <locale>
#endif
#endif

#ifdef CDEBUG_DEBUGMODE
#ifndef _DEBUG
#define _DEBUG
#endif
#endif

// スタティックメンバ
int  CDebug::GlobalInt[8]  = {0, 0, 0, 0, 0, 0, 0, 0};

void CDebug::puts(LPCSTR string)
{
#ifdef _DEBUG

#ifdef CDEBUG_CONSOLEMODE
#ifdef _CONSOLE
	::puts(string);
#endif
#else
	OutputDebugStringA(string);
	OutputDebugStringA("\n");
#endif

#endif
}

void CDebug::puts(LPCWSTR string)
{
#ifdef _DEBUG

#ifdef CDEBUG_CONSOLEMODE
#ifdef _CONSOLE
	::_putws(string);
#endif
#else
	OutputDebugStringW(string);
	OutputDebugStringW(L"\n");
#endif

#endif
}

void CDebug::printf(LPCSTR pszFormat, ...)
{
#ifdef _DEBUG
	va_list p;
	va_start(p, pszFormat);

	CHAR t[512];
	vsprintf_s(t, sizeof(t) / sizeof(t[0]) - 1, pszFormat, p);

	va_end(p);

#ifdef CDEBUG_CONSOLEMODE
#ifdef _CONSOLE
	::printf(t);
#endif
#else
	OutputDebugStringA(t);
#endif

#endif
}

void CDebug::printf(LPCWSTR pszFormat, ...)
{
#ifdef _DEBUG
	va_list p;
	va_start(p, pszFormat);

	WCHAR t[512];
	vswprintf_s(t, sizeof(t) / sizeof(t[0]) - 1, pszFormat, p);

	va_end(p);

#ifdef CDEBUG_CONSOLEMODE
#ifdef _CONSOLE
	::wprintf(t);
#endif
#else
	OutputDebugStringW(t);
#endif

#endif
}

void CDebug::PrintGlobalInt(LPCTSTR pAdditionalMessage)
{
#ifdef _DEBUG
	if( pAdditionalMessage ) {
		CDebug::printf(_T("| %4d %4d %4d %4d | %4d %4d %4d %4d | %s\n"), 
					   CDebug::GlobalInt[0], CDebug::GlobalInt[1], CDebug::GlobalInt[2], CDebug::GlobalInt[3],
					   CDebug::GlobalInt[4], CDebug::GlobalInt[5], CDebug::GlobalInt[6], CDebug::GlobalInt[7], 
					   pAdditionalMessage);
	} else {
		CDebug::printf(_T("| %4d %4d %4d %4d | %4d %4d %4d %4d |\n"), 
					   CDebug::GlobalInt[0], CDebug::GlobalInt[1], CDebug::GlobalInt[2], CDebug::GlobalInt[3],
					   CDebug::GlobalInt[4], CDebug::GlobalInt[5], CDebug::GlobalInt[6], CDebug::GlobalInt[7]);
	}
#endif
}
