#include "stdafx.h"

#include <windows.h>
#include <process.h>

#include "Win32Thread.h"

//*******************************************************************************
//	�\�z�E�j��
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
//	�X���b�h�̐���
//*******************************************************************************
BOOL Win32Thread::CreateSuspendThread(ThreadProc pThreadProc, LPVOID pParam) 
{
	UINT   NewThreadId = NULL;
	HANDLE hNewThread = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, 0, _PreThreadProc, (LPVOID)this, CREATE_SUSPENDED, &NewThreadId));

	if( hNewThread ) {
		// �X���b�h�̐����ɐ���

		CloseThread();
		m_hThread     = hNewThread;
		m_nThreadId   = static_cast<DWORD>(NewThreadId);
		m_pThreadProc = pThreadProc;
		m_pParam      = pParam;
	}

	return hNewThread != NULL;
}

//*******************************************************************************
//	�X���b�h�̔j��
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
//	�X���b�h�Ƀ��b�Z�[�W���|�X�g����
//*******************************************************************************
BOOL Win32Thread::PostThreadMessage(UINT Msg, WPARAM wParam, LPARAM lParam) 
{
	return ::PostThreadMessage(m_nThreadId, Msg, wParam, lParam);
}

//*******************************************************************************
//	�X���b�h�ɏI�����b�Z�[�W���|�X�g����
//*******************************************************************************
BOOL Win32Thread::PostQuitMessage() 
{
	return this->PostThreadMessage(WM_QUIT);
}

//*******************************************************************************
//	�X���b�h���I������܂ő҂�
//*******************************************************************************
DWORD Win32Thread::WaitThreadExit(DWORD dwMilliseconds)
{
	return ::WaitForSingleObject(m_hThread, dwMilliseconds);
}

//*******************************************************************************
//	�X���b�h�̓�����ĊJ������
//	�i�X���b�h�̃T�X�y���h�J�E���g���P���炷�j
//*******************************************************************************
DWORD Win32Thread::ResumeThread() 
{
	return ::ResumeThread(m_hThread);
}

//*******************************************************************************
//	�X���b�h�̓�����~����
//	�i�X���b�h�̃T�X�y���h�J�E���g���P���₷�j
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