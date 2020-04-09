// CLogEdit.cpp : 実装ファイル
//

#include "stdafx.h"
//#include "X-Monitor.h"
#include "CLogEdit.h"


#define LineLimit  (100)   //ログで表示する最大行数（これ以上になると先頭行が削除されていく）

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
//	最後に文字を追加する
//*******************************************************************************
void CLogEdit::AppendText(const LPCTSTR text, BOOL newline)
{
	UINT nEditMax = this->GetLimitText();
	this->SetSel(nEditMax, nEditMax, FALSE);

	if( newline ) {

		//表示されている行数を取得
		int iLineCnt = this->SendMessage(EM_GETLINECOUNT, 0, 0);
		if (iLineCnt >= LineLimit){ DeleteLine(); }  //先頭行を削除

		// 新しい行に文字を追加する
		ReplaceSel(_T("\r\n"));
		ReplaceSel(text);
	} else {
		// 最後の文字の後ろに追加
		ReplaceSel(text);
	}

	// 一番下までスクロール
	this->SendMessage(WM_VSCROLL, SB_BOTTOM);
}

//*******************************************************************************
//	最後に日付の文字列を追加する
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
//	最後に時刻の文字列を追加する
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
//	先頭の1行を削除する
//*******************************************************************************
void CLogEdit::DeleteLine()
{
	int Start, End;
	Start = this->SendMessage(EM_LINEINDEX, 0, 0);  //行の先頭ポインタ
	if (Start >= 0){
		End = this->SendMessage(EM_LINEINDEX, 1, 0);  //次の行の先頭ポインタ
		if (End<0) End = Start + this->SendMessage(EM_LINELENGTH, Start, 0);
		this->SetSel(Start, End, FALSE);
		ReplaceSel(_T(""));  //削除
	}
	int len = GetWindowTextLength();
	SendMessage(EM_SETSEL, len, len);
	// 一番下までスクロール
	this->SendMessage(WM_VSCROLL, SB_BOTTOM);
}

BEGIN_MESSAGE_MAP(CLogEdit, CEdit)
END_MESSAGE_MAP()


// CLogEdit メッセージ ハンドラ

