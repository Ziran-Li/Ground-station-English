#include "stdafx.h"

#include <windows.h>
#include <process.h>

#include "Win32Thread.h"

//*******************************************************************************
//	構築・破棄
//*******************************************************************************
Win32Thread::Win32Thread()
{
	m_hThread = NULL;
	m_nThreadId = NULL;
	m_pThreadProc = NULL;
	m_pParam = NULL;
}

Win32Thread::~Win32Thread()
{
	CloseThread();
}

//*******************************************************************************
//	スレッドの生成
//*******************************************************************************
BOOL Win32Thread::CreateSuspendThread(ThreadProc pThreadProc, LPVOID pParam) 
{
	UINT   NewThreadId = NULL;
	HANDLE hNewThread = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, 0, _PreThreadProc, (LPVOID)this, CREATE_SUSPENDED, &NewThreadId));

	if( hNewThread ) {
		// スレッドの生成に成功

		CloseThread();
		m_hThread     = hNewThread;
		m_nThreadId   = static_cast<DWORD>(NewThreadId);
		m_pThreadProc = pThreadProc;
		m_pParam      = pParam;
	}

	return hNewThread != NULL;
}

//*******************************************************************************
//	スレッドの破棄
//*******************************************************************************
void Win32Thread::CloseThread()
{
	::CloseHandle(m_hThread);
	m_hThread   = NULL;
	m_nThreadId = NULL;
	m_pThreadProc = NULL;
	m_pParam = NULL;
}


//*******************************************************************************
//	スレッドにメッセージをポストする
//*******************************************************************************
BOOL Win32Thread::PostThreadMessage(UINT Msg, WPARAM wParam, LPARAM lParam) 
{
	return ::PostThreadMessage(m_nThreadId, Msg, wParam, lParam);
}

//*******************************************************************************
//	スレッドに終了メッセージをポストする
//*******************************************************************************
BOOL Win32Thread::PostQuitMessage() 
{
	return this->PostThreadMessage(WM_QUIT);
}

//*******************************************************************************
//	スレッドが終了するまで待つ
//*******************************************************************************
DWORD Win32Thread::WaitThreadExit(DWORD dwMilliseconds)
{
	return ::WaitForSingleObject(m_hThread, dwMilliseconds);
}

//*******************************************************************************
//	スレッドの動作を再開させる
//	（スレッドのサスペンドカウントを１減らす）
//*******************************************************************************
DWORD Win32Thread::ResumeThread() 
{
	return ::ResumeThread(m_hThread);
}

//*******************************************************************************
//	スレッドの動作を停止する
//	（スレッドのサスペンドカウントを１増やす）
//*******************************************************************************
DWORD Win32Thread::SuspendThread()
{
	return ::SuspendThread(m_hThread);
}

UINT Win32Thread::_PreThreadProc(LPVOID pParam)
{
	Win32Thread* pThis = static_cast<Win32Thread*>(pParam);
	
	MSG dummy;
	::PeekMessage(&dummy, NULL, 0, 0, PM_NOREMOVE);  
	
	return pThis->m_pThreadProc(pThis->m_pParam);
}