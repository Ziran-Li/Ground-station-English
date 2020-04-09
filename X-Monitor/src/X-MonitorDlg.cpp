
// X-MonitorDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "X-Monitor.h"
#include "X-MonitorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CXMonitorDlg �_�C�A���O



CXMonitorDlg::CXMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CXMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// ���O�\���p
	DDX_Control(pDX, IDC_EDIT_LOG, m_Edit_Log);
}

BEGIN_MESSAGE_MAP(CXMonitorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_APPEND_LOG, CXMonitorDlg::OnAppendLog)
END_MESSAGE_MAP()


// CXMonitorDlg ���b�Z�[�W �n���h���[

BOOL CXMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	// �R�A���ł̓o�[�W�����_�C�A���O��\�����Ȃ�
	//CMenu* pSysMenu = GetSystemMenu(FALSE);
	//if (pSysMenu != NULL)
	//{
	//	BOOL bNameValid;
	//	CString strAboutMenu;
	//	bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
	//	ASSERT(bNameValid);
	//	if (!strAboutMenu.IsEmpty())
	//	{
	//		pSysMenu->AppendMenu(MF_SEPARATOR);
	//		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	//	}
	//}

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�


	// TODO: �������������ɒǉ����܂��B

	// �ŏ����{�^��������A�~�{�^���𖳌��ɂ���A�_�C�A���O���ŏ������Ă���
	GetSystemMenu(FALSE)->EnableMenuItem( SC_CLOSE, MF_GRAYED );
	ShowWindow(SW_MINIMIZE);

	// Monitor Logs �ɏ������b�Z�[�W��\��
	m_Edit_Log.AppendDate();
	m_Edit_Log.AppendText(_T(" "));
	m_Edit_Log.AppendTime();
	m_Edit_Log.AppendText(_T(" - X-Monitor �N��"));

	// �E�B���h�E�̏����������������̂� App �̊֐����R�[��
	theApp.OnMainWndInit(this);

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void CXMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CXMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CXMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//*******************************************************************************
//	WM_APPEND_LOG �ɂ�� m_Edit_Log �Ƀ��O��\������
//*******************************************************************************
LRESULT CXMonitorDlg::OnAppendLog(WPARAM wParam, LPARAM lParam)
{
	CString str;
	theApp.PopLog(str);

	m_Edit_Log.AppendTime(TRUE);
	m_Edit_Log.AppendText(_T(" - "));
	m_Edit_Log.AppendText(str);

	return 0;
}


void CXMonitorDlg::OnCancel()
{
	//if (IDOK == MessageBox(_T("�I�����܂����H"), _T("�I���m�F"), MB_OKCANCEL)){
	//	CDialogEx::OnCancel();
	//}

	CDialogEx::OnCancel();
}
