
// Main.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��

#include "XMonitorSyncClientExtension.h"

// CMainApp:
// ���̃N���X�̎����ɂ��ẮAMain.cpp ���Q�Ƃ��Ă��������B
//

class CMainApp : public CWinAppEx
{
public:
	CMainApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

protected:
	XMonitorSyncClientExt m_Network;

public:
	BOOL IsXMonitorConnected() { return m_Network.IsConnected(); }
	BOOL GetReceiveData(std::vector<std::string> &recvData, DWORD msecTimeout);
	BOOL SendData(const std::string &sendData);

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual int ExitInstance();
};

extern CMainApp theApp;

