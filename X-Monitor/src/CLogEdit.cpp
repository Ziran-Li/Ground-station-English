// CLogEdit.cpp : �����t�@�C��
//

#include "stdafx.h"
//#include "X-Monitor.h"
#include "CLogEdit.h"


#define LineLimit  (100)   //���O�ŕ\������ő�s���i����ȏ�ɂȂ�Ɛ擪�s���폜����Ă����j

// CLogEdit

IMPLEMENT_DYNAMIC(CLogEdit, CEdit)

CLogEdit::CLogEdit()
{	
	m_brBackground.DeleteObject();
	m_brBackground.CreateSolidBrush(GetBackColor());
}

CLogEdit::~CLogEdit()
{
}

//*******************************************************************************
//	�Ō�ɕ�����ǉ�����
//*******************************************************************************
void CLogEdit::AppendText(const LPCTSTR text, BOOL newline)
{
	UINT nEditMax = this->GetLimitText();
	this->SetSel(nEditMax, nEditMax, FALSE);

	if( newline ) {

		//�\������Ă���s�����擾
		int iLineCnt = this->SendMessage(EM_GETLINECOUNT, 0, 0);
		if (iLineCnt >= LineLimit){ DeleteLine(); }  //�擪�s���폜

		// �V�����s�ɕ�����ǉ�����
		ReplaceSel(_T("\r\n"));
		ReplaceSel(text);
	} else {
		// �Ō�̕����̌��ɒǉ�
		ReplaceSel(text);
	}

	// ��ԉ��܂ŃX�N���[��
	this->SendMessage(WM_VSCROLL, SB_BOTTOM);
}

//*******************************************************************************
//	�Ō�ɓ��t�̕������ǉ�����
//*******************************************************************************
void CLogEdit::AppendDate(BOOL newline)
{
	__time64_t t1;
	_time64(&t1);

	tm t2;
	_localtime64_s(&t2, &t1);

	CString d;
	d.Format(_T("%04d/%02d/%02d"), t2.tm_year + 1900, t2.tm_mon + 1, t2.tm_mday);

	this->AppendText(d, newline);
}

//*******************************************************************************
//	�Ō�Ɏ����̕������ǉ�����
//*******************************************************************************
void CLogEdit::AppendTime(BOOL newline)
{
	__time64_t t1;
	_time64(&t1);

	tm t2;
	_localtime64_s(&t2, &t1);

	CString t;
	t.Format(_T("%02d:%02d:%02d"), t2.tm_hour, t2.tm_min, t2.tm_sec);

	this->AppendText(t, newline);
}

//*******************************************************************************
//	�擪��1�s���폜����
//*******************************************************************************
void CLogEdit::DeleteLine()
{
	int Start, End;
	Start = this->SendMessage(EM_LINEINDEX, 0, 0);  //�s�̐擪�|�C���^
	if (Start >= 0){
		End = this->SendMessage(EM_LINEINDEX, 1, 0);  //���̍s�̐擪�|�C���^
		if (End<0) End = Start + this->SendMessage(EM_LINELENGTH, Start, 0);
		this->SetSel(Start, End, FALSE);
		ReplaceSel(_T(""));  //�폜
	}
	int len = GetWindowTextLength();
	SendMessage(EM_SETSEL, len, len);
	// ��ԉ��܂ŃX�N���[��
	this->SendMessage(WM_VSCROLL, SB_BOTTOM);
}

BEGIN_MESSAGE_MAP(CLogEdit, CEdit)
END_MESSAGE_MAP()


// CLogEdit ���b�Z�[�W �n���h��

