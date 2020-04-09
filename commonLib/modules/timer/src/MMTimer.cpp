///////////////////////////////////////////////////////////////////////////////
// MMTimer.cpp
// 
// class CMMTimer : インプリメント
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mmtimer.h"


///////////////////////////////////////////////////////////////////////////////
// CMMTimer 構築、消滅

CMMTimer::CMMTimer()
{	// <public> 構築
	// [初期化]
	// タイマ識別子
	this->m_uTimerID	= (UINT)NULL;
	// タイムアウトメッセージ通知先ウィンドウ
	this->m_hWnd		= (HWND)NULL;
	// タイムアウト通知イベント
	this->m_hEvent		= ::CreateEvent(NULL, TRUE, FALSE, NULL);
	// タイムアウトカウンタ
	this->m_nCount		= 0;

	// タイマ分解能の取得
	::timeGetDevCaps(&this->m_TimeCaps, sizeof(TIMECAPS) );
}

CMMTimer::~CMMTimer()
{	// <public> 消滅
	// タイマのクローズ
	this->CloseTimer();
	// イベントハンドルの破棄
	::CloseHandle(this->m_hEvent);
}

///////////////////////////////////////////////////////////////////////////////
// CMMTimer 操作

bool CMMTimer::OpenTimer(UINT uInterval, HWND hWnd)
{	// <public> タイマのオープン
	// 検査
	if(this->m_uTimerID != (UINT)NULL)	// タイマがオープンされている場合
		return false;	// 異常終了

	// タイムアウトメッセージ通知先ウィンドウの設定
	this->m_hWnd	= hWnd;
	// タイムアウトイベントのリセット
	::ResetEvent(this->m_hEvent);
	// 最小タイマ分解能の設定
	if(::timeBeginPeriod(this->m_TimeCaps.wPeriodMin) != TIMERR_NOERROR)	// エラーが発生した場合
		return false;	// 異常終了
	// オープン
	this->m_uTimerID = (UINT)::timeSetEvent(uInterval, 0, (LPTIMECALLBACK)::MMTimerCallBackProc, (DWORD_PTR)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
	if(this->m_uTimerID == (UINT)NULL)
	{	// タイマのオープンに失敗した場合
		// 最小タイマ分解能の設定解除
		::timeEndPeriod(this->m_TimeCaps.wPeriodMin);
		// 異常終了
		return false;
	}

	// 正常終了
	return true;
}

bool CMMTimer::CloseTimer(void)
{	// <public> タイマのクローズ
	// 処理
	if(this->m_uTimerID != (UINT)NULL)
	{	// タイマがオープンされている場合
		// クローズ
		if(TIMERR_NOERROR != ::timeKillEvent(this->m_uTimerID) )	// クローズに失敗した場合
			return false;	// 異常終了
		// 最小タイマ分解能の設定解除
		::timeEndPeriod(this->m_TimeCaps.wPeriodMin);
		// タイマ識別子の初期化
		this->m_uTimerID	= (UINT)NULL;
		// タイムアウトイベントの発生
		::SetEvent(this->m_hEvent);
	}
	// 正常終了
	return true;
}

bool CMMTimer::WaitTimeoutEvent(DWORD dwMilliseconds /* = INFINITE */)
{	// <public> タイムアウトイベントの待機
	// 待機
	if(::WaitForSingleObject(this->m_hEvent, dwMilliseconds) == WAIT_OBJECT_0)	// シグナル状態になった場合
		return true;	// 正常終了
	else	// シグナル状態にならなかった場合
		return false;	// 異常終了
}

bool CMMTimer::ResetTimeoutEvent(void)
{	// <public> タイムアウトイベントのリセット
	// リセット
	if(::ResetEvent(this->m_hEvent) != FALSE)	// 成功した場合
		return true;	// 正常終了
	else	// 失敗した場合
		return false;	// 異常終了
}

bool CMMTimer::ResetTimeoutCount(void)
{	// <public> タイムアウトカウンタのリセット
	// リセット
	this->m_nCount	= 0;
	// 正常終了
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// CMMTimer フレンド関数

void CALLBACK MMTimerCallBackProc(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD dw1, DWORD dw2)
{	// コールバック関数
	CMMTimer*	pMMTimer	= (CMMTimer*)dwUser;

	// WM_TIMER の書き込み
	if(pMMTimer->m_hWnd != (HWND)NULL)
		::PostMessage(pMMTimer->m_hWnd, WM_TIMER, 0, 0);
	// イベントの発生
	::SetEvent(pMMTimer->m_hEvent);
	// タイムアウトカウンタの更新
	++pMMTimer->m_nCount;

	//OutputDebugString(L"hoge\n");
}


///////////////////////////////////////////////////////////////////////////////
DWORD CMMTimer::timeGetTime()
{
	return ::timeGetTime();
}