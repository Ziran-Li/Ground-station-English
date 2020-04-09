//	非MFCアプリケーション用スレッド管理クラス
#pragma once

#include <windef.h>

class Win32Thread
{
public:
	typedef UINT (__stdcall *ThreadProc)(LPVOID pParam);

protected:
	HANDLE m_hThread;
	DWORD  m_nThreadId;

	ThreadProc m_pThreadProc;
	LPVOID     m_pParam;

public:
	Win32Thread();
	virtual ~Win32Thread();

	BOOL CreateSuspendThread(ThreadProc pThreadProc, LPVOID pParam = NULL);
	void CloseThread();

	BOOL  PostThreadMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);
	BOOL  PostQuitMessage();
	DWORD WaitThreadExit(DWORD dwMilliseconds = INFINITE);

	DWORD ResumeThread();
	DWORD SuspendThread();

	inline HANDLE GetHandle() { return m_hThread; }
	inline DWORD  GetId()     { return m_nThreadId; }

private:
	static UINT __stdcall _PreThreadProc(LPVOID pParam);
};