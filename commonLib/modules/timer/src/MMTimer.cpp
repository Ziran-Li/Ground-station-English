///////////////////////////////////////////////////////////////////////////////
// MMTimer.cpp
// 
// class CMMTimer : �C���v�������g
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mmtimer.h"


///////////////////////////////////////////////////////////////////////////////
// CMMTimer �\�z�A����

CMMTimer::CMMTimer()
{	// <public> �\�z
	// [������]
	// �^�C�}���ʎq
	this->m_uTimerID	= (UINT)NULL;
	// �^�C���A�E�g���b�Z�[�W�ʒm��E�B���h�E
	this->m_hWnd		= (HWND)NULL;
	// �^�C���A�E�g�ʒm�C�x���g
	this->m_hEvent		= ::CreateEvent(NULL, TRUE, FALSE, NULL);
	// �^�C���A�E�g�J�E���^
	this->m_nCount		= 0;

	// �^�C�}����\�̎擾
	::timeGetDevCaps(&this->m_TimeCaps, sizeof(TIMECAPS) );
}

CMMTimer::~CMMTimer()
{	// <public> ����
	// �^�C�}�̃N���[�Y
	this->CloseTimer();
	// �C�x���g�n���h���̔j��
	::CloseHandle(this->m_hEvent);
}

///////////////////////////////////////////////////////////////////////////////
// CMMTimer ����

bool CMMTimer::OpenTimer(UINT uInterval, HWND hWnd)
{	// <public> �^�C�}�̃I�[�v��
	// ����
	if(this->m_uTimerID != (UINT)NULL)	// �^�C�}���I�[�v������Ă���ꍇ
		return false;	// �ُ�I��

	// �^�C���A�E�g���b�Z�[�W�ʒm��E�B���h�E�̐ݒ�
	this->m_hWnd	= hWnd;
	// �^�C���A�E�g�C�x���g�̃��Z�b�g
	::ResetEvent(this->m_hEvent);
	// �ŏ��^�C�}����\�̐ݒ�
	if(::timeBeginPeriod(this->m_TimeCaps.wPeriodMin) != TIMERR_NOERROR)	// �G���[�����������ꍇ
		return false;	// �ُ�I��
	// �I�[�v��
	this->m_uTimerID = (UINT)::timeSetEvent(uInterval, 0, (LPTIMECALLBACK)::MMTimerCallBackProc, (DWORD_PTR)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
	if(this->m_uTimerID == (UINT)NULL)
	{	// �^�C�}�̃I�[�v���Ɏ��s�����ꍇ
		// �ŏ��^�C�}����\�̐ݒ����
		::timeEndPeriod(this->m_TimeCaps.wPeriodMin);
		// �ُ�I��
		return false;
	}

	// ����I��
	return true;
}

bool CMMTimer::CloseTimer(void)
{	// <public> �^�C�}�̃N���[�Y
	// ����
	if(this->m_uTimerID != (UINT)NULL)
	{	// �^�C�}���I�[�v������Ă���ꍇ
		// �N���[�Y
		if(TIMERR_NOERROR != ::timeKillEvent(this->m_uTimerID) )	// �N���[�Y�Ɏ��s�����ꍇ
			return false;	// �ُ�I��
		// �ŏ��^�C�}����\�̐ݒ����
		::timeEndPeriod(this->m_TimeCaps.wPeriodMin);
		// �^�C�}���ʎq�̏�����
		this->m_uTimerID	= (UINT)NULL;
		// �^�C���A�E�g�C�x���g�̔���
		::SetEvent(this->m_hEvent);
	}
	// ����I��
	return true;
}

bool CMMTimer::WaitTimeoutEvent(DWORD dwMilliseconds /* = INFINITE */)
{	// <public> �^�C���A�E�g�C�x���g�̑ҋ@
	// �ҋ@
	if(::WaitForSingleObject(this->m_hEvent, dwMilliseconds) == WAIT_OBJECT_0)	// �V�O�i����ԂɂȂ����ꍇ
		return true;	// ����I��
	else	// �V�O�i����ԂɂȂ�Ȃ������ꍇ
		return false;	// �ُ�I��
}

bool CMMTimer::ResetTimeoutEvent(void)
{	// <public> �^�C���A�E�g�C�x���g�̃��Z�b�g
	// ���Z�b�g
	if(::ResetEvent(this->m_hEvent) != FALSE)	// ���������ꍇ
		return true;	// ����I��
	else	// ���s�����ꍇ
		return false;	// �ُ�I��
}

bool CMMTimer::ResetTimeoutCount(void)
{	// <public> �^�C���A�E�g�J�E���^�̃��Z�b�g
	// ���Z�b�g
	this->m_nCount	= 0;
	// ����I��
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// CMMTimer �t�����h�֐�

void CALLBACK MMTimerCallBackProc(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD dw1, DWORD dw2)
{	// �R�[���o�b�N�֐�
	CMMTimer*	pMMTimer	= (CMMTimer*)dwUser;

	// WM_TIMER �̏�������
	if(pMMTimer->m_hWnd != (HWND)NULL)
		::PostMessage(pMMTimer->m_hWnd, WM_TIMER, 0, 0);
	// �C�x���g�̔���
	::SetEvent(pMMTimer->m_hEvent);
	// �^�C���A�E�g�J�E���^�̍X�V
	++pMMTimer->m_nCount;

	//OutputDebugString(L"hoge\n");
}


///////////////////////////////////////////////////////////////////////////////
DWORD CMMTimer::timeGetTime()
{
	return ::timeGetTime();
}