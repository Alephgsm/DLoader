#if !defined(AFX_FILTEREDIT_H__51FADEF4_59EC_455F_A37B_E6D2B4B2793B__INCLUDED_)
#define AFX_FILTEREDIT_H__51FADEF4_59EC_455F_A37B_E6D2B4B2793B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilterEdit window

class CFilterEdit : public CEdit
{
// Construction
public:
	CFilterEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFilterEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFilterEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTEREDIT_H__51FADEF4_59EC_455F_A37B_E6D2B4B2793B__INCLUDED_)
