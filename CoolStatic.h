#if !defined(AFX_COOLSTATIC_H__D5CFC94C_CC11_41CB_B399_4F6E0F2507E7__INCLUDED_)
#define AFX_COOLSTATIC_H__D5CFC94C_CC11_41CB_B399_4F6E0F2507E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CoolStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCoolStatic window

class CCoolStatic : public CStatic
{
// Construction
public:
	CCoolStatic();
	void SetColor(UINT rgb);
// 	void SetBitMap(UINT uBmpID);
	void SetText(LPCTSTR lpszText);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoolStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCoolStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCoolStatic)
	afx_msg void OnPaint();
	//}}AFX_MSG
	// afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
private:
	UINT m_nColor;
	CBitmap m_bmpBckGrnd;
	CString m_strText;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COOLSTATIC_H__D5CFC94C_CC11_41CB_B399_4F6E0F2507E7__INCLUDED_)
