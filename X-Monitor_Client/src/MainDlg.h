
// MainDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "DataManage.h"


// CMainDlg �_�C�A���O
class CMainDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CMainDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_MAIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	HICON m_hIcon;

	void OnDataReceive(const std::string &recvData);

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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
	dataManage m_xmlDataDb;	// XML�f�[�^�Ǘ��N���X
};
