
// MainDlg.h : ヘッダー ファイル
//

#pragma once
#include "DataManage.h"


// CMainDlg ダイアログ
class CMainDlg : public CDialog
{
// コンストラクション
public:
	CMainDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_MAIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	void OnDataReceive(const std::string &recvData);

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT _OnDataReceive(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonRef();

private:
	CString m_editSendXML;
	CString m_editRecvXML;
	int m_recvCount;
	//CString m_editFirstData;
	dataManage m_xmlDataDb;	// XMLデータ管理クラス
};
