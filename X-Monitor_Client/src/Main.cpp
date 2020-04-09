
// Main.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "Main.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// global�ϐ�(XML�f�[�^�Ǘ��N���X�Ŏg�p)
map<int, string> dataTypeMap;


// CMainApp

BEGIN_MESSAGE_MAP(CMainApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMainApp �R���X�g���N�V����

CMainApp::CMainApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� CMainApp �I�u�W�F�N�g�ł��B

CMainApp theApp;


// CMainApp ������

BOOL CMainApp::InitInstance()
{
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// TODO: ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
	//SetRegistryKey(_T("�A�v���P�[�V���� �E�B�U�[�h�Ő������ꂽ���[�J�� �A�v���P�[�V����"));


	// COM�̏�����
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (!SUCCEEDED(hr))
	{
		AfxMessageBox(_T("COM�̏��������s"));
	}

	// --------------------------------------------------
	//	X-Monitor �Ƃ̐ڑ�
	//
	//	IP�A�h���X��ύX���邱�ƂŁC�قȂ�R���s���[�^�Ԃ̒ʐM���\�ɂȂ�܂��D
	//	�������C�l�b�g���[�N���u����邽�߁C�f�[�^�̒x�����傫���Ȃ�܂��D
	//	�i����R���s���[�^���ł����\ms�̒x���������܂��D�j
	//
	//	�|�[�g�ԍ��́CX-Monitor �ƈ�v�����Ă��������D�i�f�t�H���g 53799�j
	//	�|�[�g�ԍ�����v���� X-Monitor �ƒʐM���s���܂��D
	//	
	//	X-Monitor �𕡐��N�����C���ꂼ��قȂ�|�[�g�ԍ���ݒ肵�Ă������ƂŁC
	//	�N���C�A���g�v���O������ʁX�� X-Monitor �Ɛڑ������邱�Ƃ��ł��܂��D
	//	��F
	//		X-Monitor1(�ݒ�F53799) �� �N���C�A���g�v���O����A (�ڑ���F53799)
	//		X-Monitor2(�ݒ�F53800) �� �N���C�A���g�v���O����B (�ڑ���F53800)
	// --------------------------------------------------
	m_Network.Initialize("127.0.0.1", 53799);

	CMainDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �_�C�A���O�� <OK> �ŏ����ꂽ���̃R�[�h��
		//  �L�q���Ă��������B
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �_�C�A���O�� <�L�����Z��> �ŏ����ꂽ���̃R�[�h��
		//  �L�q���Ă��������B
	}

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	//  �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}

BOOL CMainApp::GetReceiveData(std::vector<std::string> &recvData, DWORD msecTimeout)
{
	return m_Network.GetData(recvData, msecTimeout);
}

BOOL CMainApp::SendData(const std::string &sendData)
{
	return m_Network.SendData(sendData);
}

int CMainApp::ExitInstance()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B

	// COM�̏I������
	CoUninitialize();

	return CWinAppEx::ExitInstance();
}
