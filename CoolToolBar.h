#if !defined(AFX_COOLTOOLBAR_H__96B09993_1F47_41C9_A0CF_E5A1F4D59CDB__INCLUDED_)
#define AFX_COOLTOOLBAR_H__96B09993_1F47_41C9_A0CF_E5A1F4D59CDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CoolToolBar.h : header file
//
#include "CoolStatic.h"
#include "XStatic.h"
/////////////////////////////////////////////////////////////////////////////
// CCoolToolBar window

class CCoolToolBar : public CToolBar
{
// Construction
public:
	CCoolToolBar();

// Attributes
public:
	CXStatic m_stcWnd;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoolToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:

	BOOL SetBitmaps(UINT uBmpID, UINT  uDisabledBmpID, UINT uHotBmpID);

	virtual ~CCoolToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCoolToolBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CImageList * m_pImgLst;
	CImageList * m_pDisabledImgLst;
	CImageList * m_pHotImgLst;

	CBitmap m_bmpBckGrnd;
	CBrush  m_brshBckGrnd;
	
	static const COLORREF CLR_TB_BCKGRND;

	static const int IMG_CX;
	static const int IMG_CY;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COOLTOOLBAR_H__96B09993_1F47_41C9_A0CF_E5A1F4D59CDB__INCLUDED_)
