//
//	デバッギングクラス
//
//	X-Monitor のソリューションで開発中．
//	開発完了次第ライブラリに移動
//
//	stdafx.h に CDEBUG_CONSOLEMODE を定義すると出力をコンソールに表示できます．
//
//	stdafx.h に CDEBUG_DEBUGMODE と CDEBUG_CONSOLEMODE を定義すると，
//	Release ビルドでも出力をコンソールに表示できます．
//
//	stdafx.h に CDEBUG_DONT_CHANGE_LOCALE を設定すると，ロケールの変更を行いません．
//


#pragma once

#include <windows.h>
#include <tchar.h>

class CDebug
{
public:
	static int  GlobalInt[8];

public:
	// 画面表示系関数
	static void puts(LPCSTR string);
	static void puts(LPCWSTR string);
	static void printf(LPCSTR pszFormat, ...);
	static void printf(LPCWSTR pszFormat, ...);

	static void PrintGlobalInt(LPCTSTR pAdditionalMessage = NULL);

	// ログ系関数
	//static void D
};