
// UAVSimDlg.h : ヘッダー ファイル
//

#pragma once

#include "CLogEdit.h"

// CUAVSimDlg ダイアログ
class CUAVSimDlg : public CDialogEx
{
// コンストラクション
public:
	CUAVSimDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_UAVSIM_DIALOG };

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
public:
	afx_msg void OnBnClickedButtonWlSend();
	afx_msg void OnBnClickedButtonWlSendStart();
	afx_msg void OnBnClickedButtonWlSendStop();
};
