
// MainDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include <string>
#include <fstream>
#include "Main.h"
#include "MainDlg.h"
#include "Base64Data.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMainDlg �_�C�A���O




CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
	, m_editSendXML(_T(""))
	, m_editRecvXML(_T(""))
	, m_recvCount(0)
	//, m_editFirstData(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	// ���\��������̓o�^�T���v��
	DDX_Text(pDX, IDC_EDIT_SEND_XML, m_editSendXML);
	DDX_Text(pDX, IDC_EDIT_RECV_XML, m_editRecvXML);
	DDX_Text(pDX, IDC_EDIT_RECV_COUNT, m_recvCount);
	//DDX_Text(pDX, IDC_EDIT_FIRST_DATA, m_editFirstData);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_DATA_RECEIVE, CMainDlg::_OnDataReceive)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_REF, &CMainDlg::OnBnClickedButtonRef)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CMainDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()


void CMainDlg::OnDataReceive(const std::string &recvData)
{
	// ��M����XML���̕\��
	m_editRecvXML = recvData.c_str();

    // ��M�J�E���g�X�V
	++m_recvCount;

	//// XML����XML�f�[�^�Ǘ��I�u�W�F�N�g�ɓo�^
	//HRESULT hr;
	//XmlMessage *xml;
	//UINT dataID = 0;    // ���C��
	//BSTR bstrRecvData = A2BSTR(recvData.c_str());	// XML��(std::string�^)��BSTR�^�ɕϊ�(MSXML���g�p���邽��)
	//xml = new XmlMessage(bstrRecvData, &hr);
	//if (hr == S_OK)
	//{
	//	m_xmlDataDb.saveData(xml, dataID); // �o�^
	//	delete xml;
	//}
	//// XML������ŏ��̃f�[�^�i<UAV>��<PresAlt>�j�𔲂��o���ĕ\��
	//vector<_bstr_t> path;
	//path.clear();
	//path.push_back(L"UAV");
	//path.push_back(L"PresAlt");
	//double aaa;
	//if (m_xmlDataDb.getOneElement(path, NULL, &aaa) == 0)
	//{
	//	m_editFirstData.Format(L"%.2f", aaa);
	//}
	//else
	//{
	//	m_editFirstData = L"";
	//}
	//::SysFreeString(bstrRecvData);	// �����Y���ƃ��������[�N����

	this->UpdateData(FALSE);
}

// CMainDlg ���b�Z�[�W �n���h��

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B

	// �f�[�^�^�C�vMap������(XML�f�[�^�Ǘ��N���X)
	Base64Data::datatypeMap();

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CMainDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMainDlg::_OnDataReceive(WPARAM wParam, LPARAM lParam)
{ 
	std::vector<std::string> recvData;

	if(theApp.GetReceiveData(recvData, 0) == TRUE) 
    {
		for (UINT i = 0; i < recvData.size(); ++i)
		{
			OnDataReceive(recvData[i]);
		}
	}

	return 0; 
}

void CMainDlg::OnBnClickedButtonRef()
{
	// �t�@�C���I���_�C�A���O�̃t�B���^��ݒ肷��
	CString filter("XML Files (*.txt;*.xml)|*.txt; *.xml||");
	CFileDialog selDlg(TRUE, NULL, NULL, NULL, filter);

	// �t�@�C���I���_�C�A���O��\������
	if (selDlg.DoModal() == IDOK)
	{
		CStdioFile fin(selDlg.GetPathName(), CFile::modeRead);
		if (!fin) return;
		m_editSendXML = _T("");	// ��U�N���A
		CString oneLine = _T("");
		while (fin.ReadString(oneLine))
		{
			m_editSendXML += oneLine;
			m_editSendXML += "\r\n";
		}
		        
		UpdateData(FALSE);
	}
}


void CMainDlg::OnBnClickedButtonSend()
{
	UpdateData(TRUE);
	const size_t XML_SIZE = 4096;
	char  cSendXML[XML_SIZE];
	size_t outputSize;

    // ���C�h�������}���`�o�C�g����
	wcstombs_s(&outputSize, cSendXML, XML_SIZE, (const wchar_t *)m_editSendXML, _TRUNCATE);

	std::string sendXML = cSendXML;

    // �f�[�^���M
	theApp.SendData(sendXML);
}
