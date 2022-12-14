#if !defined(AFX_EDITEX_H__491E6B45_E6B4_42AC_BFC0_6579B8EBF1F3__INCLUDED_)
#define AFX_EDITEX_H__491E6B45_E6B4_42AC_BFC0_6579B8EBF1F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditEx.h : header file
//

enum {
	EX_FMT_NORMAL,
	EX_FMT_DEC,
	EX_FMT_HEX,
	EX_FMT_MAX
};

/////////////////////////////////////////////////////////////////////////////
// CEditEx window
class CEditEx : public CEdit
{
// Construction
public:
	CEditEx();

    void SetFmtMode(UINT nMode);
    long GetDigit();

// Attributes
protected:
    // When you want to change the
    // edit content,m_strOldText contains
    // the current content of the edit
    CString m_strOldText;

    // If your input is invalid,the content will
    // be restore.m_bReturn is used to avoid enter
    // the update function again
    BOOL m_bReturn;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditEx)
	//afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdate();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	UINT   m_nFmtMode;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITEX_H__491E6B45_E6B4_42AC_BFC0_6579B8EBF1F3__INCLUDED_)
