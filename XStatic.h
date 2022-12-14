#if !defined(AFX_XSTATIC_H__A1FB5697_A7E8_4D89_AF80_F5AB1BB38982__INCLUDED_)
#define AFX_XSTATIC_H__A1FB5697_A7E8_4D89_AF80_F5AB1BB38982__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXStatic window

class CXStatic : public CStatic
{
// Construction
public:
	CXStatic();

// Attributes
public:
	void SetTextColor(UINT uColor);
	void SetBKGColor(UINT uColor);
	void SetFontSize(int nSize);
	void SetText(LPCTSTR lpszText);
	void SetBKGBmp(UINT uBmpID);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXStatic)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void DrawBkgBmp(CPaintDC *pDC,const CRect &rect);
	void DrawBkgColor(CPaintDC *pDC,const CRect &rect);
	void DrawText(CPaintDC *pDC,const CRect &rect);

private:
	UINT m_uTextColor;
	UINT m_uBKGColor;
	int  m_nFontSize;
    CString m_strText;
	UINT m_uBKGBmpID;
	CBitmap m_bmpBkg;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XSTATIC_H__A1FB5697_A7E8_4D89_AF80_F5AB1BB38982__INCLUDED_)
