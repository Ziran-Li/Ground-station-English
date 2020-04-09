///////////////////////////////////////////////////////////////////////////////
// MMTimer.h
// 
// class CMMTimer : �錾
///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////
// [�g�p��̒���]
// �ȉ��̃t�@�C�����K�v�ł��B
// windows.h	: Include
// mmsystem.h	: Include
// winmm.lib	: Link
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

///////////////////////////////////////////////////////////////////////////////
// CMMTimer �錾

class CMMTimer
{
protected:
	// �^�C�}����\
	// �\����TIMECAPS�̓^�C�}�[�̕���\�ɂ��Ă̏����܂�ł���
	TIMECAPS	m_TimeCaps;
	// �^�C�}���ʎq
	UINT		m_uTimerID;
	// �^�C���A�E�g���b�Z�[�W�ʒm��E�B���h�E
	HWND		m_hWnd;
	// �^�C���A�E�g�ʒm�C�x���g
	HANDLE		m_hEvent;
	// �^�C���A�E�g�J�E���^
	int			m_nCount;

protected:
	// �R�[���o�b�N�֐��i�t�����h�֐��j
	friend	void	CALLBACK	MMTimerCallBackProc(UINT, UINT, DWORD_PTR, DWORD, DWORD);
public:
	// �\�z�E����
	CMMTimer();
	~CMMTimer();
	// ����\
	int		GetMinimumTimerResolution(void) const;
	// ����
	bool	OpenTimer(UINT, HWND);
	bool	CloseTimer(void);
	// �^�C���A�E�g����
	bool	WaitTimeoutEvent(DWORD = INFINITE);
	bool	ResetTimeoutEvent(void);
	int		GetTimeoutCount(void) const;
	bool	ResetTimeoutCount(void);
	// timeGetTime
	static DWORD timeGetTime();
};


///////////////////////////////////////////////////////////////////////////////
// CMMTimer �萔


///////////////////////////////////////////////////////////////////////////////
// CMMTimer �C�����C�������o�֐�

inline int CMMTimer::GetMinimumTimerResolution(void) const
{	// <public> �ŏ��^�C�}����\�̎擾
	return (int)this->m_TimeCaps.wPeriodMin;
}

inline int CMMTimer::GetTimeoutCount(void) const
{	// <public> �^�C���A�E�g�J�E���^�̎擾
	return this->m_nCount;
}


///////////////////////////////////////////////////////////////////////////////
