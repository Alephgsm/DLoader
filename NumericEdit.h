/*********************************************************************
 * NumericEdit.h : header file
 * CNumericEdit is a numeric edit control that only accept
 * decimal or heximal number.Heximal number is begin with 0x

*============================================================================*
 *                          Edit History
 *-----------------------------------------------------------------------------
 * 2002-12-07   Kai Liu     Created
 *-----------------------------------------------------------------------------
 * 2003-02-11   Kai Liu     Add SetDispMode() and GetDigit() functions
 *                          Use in the DLoader project
 *============================================================================*

 *********************************************************************/

#if !defined(AFX_NUMERICEDIT_H__9D9D8A72_8A7E_4517_AD42_7572370393CE__INCLUDED_)
#define AFX_NUMERICEDIT_H__9D9D8A72_8A7E_4517_AD42_7572370393CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

/////////////////////////////////////////////////////////////////////////////
// CNumericEdit window

#define DEC 0
#define HEX 1

class CNumericEdit : public CEdit
{
// Construction
public:
	CNumericEdit();

    void SetDispMode(int nMode);

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

	int  m_nMode;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumericEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNumericEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNumericEdit)
	afx_msg void OnUpdate();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUMERICEDIT_H__9D9D8A72_8A7E_4517_AD42_7572370393CE__INCLUDED_)
