///////////////////////////////////////////////////////////////////////////////
// MMTimer.h
// 
// class CMMTimer : 宣言
///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////
// [使用上の注意]
// 以下のファイルが必要です。
// windows.h	: Include
// mmsystem.h	: Include
// winmm.lib	: Link
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

///////////////////////////////////////////////////////////////////////////////
// CMMTimer 宣言

class CMMTimer
{
protected:
	// タイマ分解能
	// 構造体TIMECAPSはタイマーの分解能についての情報を含んでいる
	TIMECAPS	m_TimeCaps;
	// タイマ識別子
	UINT		m_uTimerID;
	// タイムアウトメッセージ通知先ウィンドウ
	HWND		m_hWnd;
	// タイムアウト通知イベント
	HANDLE		m_hEvent;
	// タイムアウトカウンタ
	int			m_nCount;

protected:
	// コールバック関数（フレンド関数）
	friend	void	CALLBACK	MMTimerCallBackProc(UINT, UINT, DWORD_PTR, DWORD, DWORD);
public:
	// 構築・消滅
	CMMTimer();
	~CMMTimer();
	// 分解能
	int		GetMinimumTimerResolution(void) const;
	// 操作
	bool	OpenTimer(UINT, HWND);
	bool	CloseTimer(void);
	// タイムアウト処理
	bool	WaitTimeoutEvent(DWORD = INFINITE);
	bool	ResetTimeoutEvent(void);
	int		GetTimeoutCount(void) const;
	bool	ResetTimeoutCount(void);
	// timeGetTime
	static DWORD timeGetTime();
};


///////////////////////////////////////////////////////////////////////////////
// CMMTimer 定数


///////////////////////////////////////////////////////////////////////////////
// CMMTimer インラインメンバ関数

inline int CMMTimer::GetMinimumTimerResolution(void) const
{	// <public> 最小タイマ分解能の取得
	return (int)this->m_TimeCaps.wPeriodMin;
}

inline int CMMTimer::GetTimeoutCount(void) const
{	// <public> タイムアウトカウンタの取得
	return this->m_nCount;
}


///////////////////////////////////////////////////////////////////////////////
