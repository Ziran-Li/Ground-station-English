
// MainDlg.cpp : 実装ファイル
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


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
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


// CMainDlg ダイアログ




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

	// ★表示文字列の登録サンプル
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
	// 受信したXML文の表示
	m_editRecvXML = recvData.c_str();

    // 受信カウント更新
	++m_recvCount;

	//// XML文をXMLデータ管理オブジェクトに登録
	//HRESULT hr;
	//XmlMessage *xml;
	//UINT dataID = 0;    // メイン
	//BSTR bstrRecvData = A2BSTR(recvData.c_str());	// XML文(std::string型)をBSTR型に変換(MSXMLを使用するため)
	//xml = new XmlMessage(bstrRecvData, &hr);
	//if (hr == S_OK)
	//{
	//	m_xmlDataDb.saveData(xml, dataID); // 登録
	//	delete xml;
	//}
	//// XML文から最初のデータ（<UAV>の<PresAlt>）を抜き出して表示
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
	//::SysFreeString(bstrRecvData);	// これを忘れるとメモリリークする

	this->UpdateData(FALSE);
}

// CMainDlg メッセージ ハンドラ

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
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

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	// データタイプMap初期化(XMLデータ管理クラス)
	Base64Data::datatypeMap();

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
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

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
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
	// ファイル選択ダイアログのフィルタを設定する
	CString filter("XML Files (*.txt;*.xml)|*.txt; *.xml||");
	CFileDialog selDlg(TRUE, NULL, NULL, NULL, filter);

	// ファイル選択ダイアログを表示する
	if (selDlg.DoModal() == IDOK)
	{
		CStdioFile fin(selDlg.GetPathName(), CFile::modeRead);
		if (!fin) return;
		m_editSendXML = _T("");	// 一旦クリア
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

    // ワイド文字→マルチバイト文字
	wcstombs_s(&outputSize, cSendXML, XML_SIZE, (const wchar_t *)m_editSendXML, _TRUNCATE);

	std::string sendXML = cSendXML;

    // データ送信
	theApp.SendData(sendXML);
}
