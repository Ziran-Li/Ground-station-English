#pragma once


// CLogEdit

class CLogEdit : public CEdit
{
	DECLARE_DYNAMIC(CLogEdit)

public:
	CLogEdit();
	virtual ~CLogEdit();

	CBrush m_brBackground;

	void     AppendText(const LPCTSTR text, BOOL newline = FALSE);
	void     AppendDate(BOOL newline = FALSE);
	void     AppendTime(BOOL newline = FALSE);
	HBRUSH   GetBackBrush() {return (HBRUSH)m_brBackground;}
	COLORREF GetBackColor() {return RGB(255, 255, 255);}

	void     DeleteLine();

protected:
	DECLARE_MESSAGE_MAP()
public:

};
