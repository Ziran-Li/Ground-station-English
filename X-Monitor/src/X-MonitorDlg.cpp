
// X-MonitorDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "X-Monitor.h"
#include "X-MonitorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CXMonitorDlg ダイアログ



CXMonitorDlg::CXMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CXMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// ログ表示用
	DDX_Control(pDX, IDC_EDIT_LOG, m_Edit_Log);
}

BEGIN_MESSAGE_MAP(CXMonitorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_APPEND_LOG, CXMonitorDlg::OnAppendLog)
END_MESSAGE_MAP()


// CXMonitorDlg メッセージ ハンドラー

BOOL CXMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	// コア部ではバージョンダイアログを表示しない
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

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定


	// TODO: 初期化をここに追加します。

	// 最小化ボタンをつける、×ボタンを無効にする、ダイアログを最小化しておく
	GetSystemMenu(FALSE)->EnableMenuItem( SC_CLOSE, MF_GRAYED );
	ShowWindow(SW_MINIMIZE);

	// Monitor Logs に初期メッセージを表示
	m_Edit_Log.AppendDate();
	m_Edit_Log.AppendText(_T(" "));
	m_Edit_Log.AppendTime();
	m_Edit_Log.AppendText(_T(" - X-Monitor 起動"));

	// ウィンドウの初期化が完了したので App の関数をコール
	theApp.OnMainWndInit(this);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
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

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CXMonitorDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CXMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//*******************************************************************************
//	WM_APPEND_LOG により m_Edit_Log にログを表示する
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
	//if (IDOK == MessageBox(_T("終了しますか？"), _T("終了確認"), MB_OKCANCEL)){
	//	CDialogEx::OnCancel();
	//}

	CDialogEx::OnCancel();
}
