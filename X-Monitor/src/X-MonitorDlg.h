
// X-MonitorDlg.h : ヘッダー ファイル
//

#pragma once

#include "CLogEdit.h"

// CXMonitorDlg ダイアログ
class CXMonitorDlg : public CDialogEx
{
// コンストラクション
public:
	CXMonitorDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_XMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnAppendLog(WPARAM wParam, LPARAM lParam);

private:
	// ログ表示用
	CLogEdit m_Edit_Log;
	virtual void OnCancel();
};
