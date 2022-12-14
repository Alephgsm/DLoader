#if !defined(AFX_COOLLISTCTRL_H__76A7A50A_AA16_4B70_BF2E_CD59F88E8665__INCLUDED_)
#define AFX_COOLLISTCTRL_H__76A7A50A_AA16_4B70_BF2E_CD59F88E8665__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CoolListCtrl.h : header file
//
// #ifndef SORTHEADERCTRL_H
// #include "SortHeaderCtrl.h"
// #endif	// SORTHEADERCTRL_H

#ifdef _DEBUG
#define ASSERT_VALID_STRING( str ) ASSERT( !IsBadStringPtr( str, 0xfffff ) )
#else	//	_DEBUG
#define ASSERT_VALID_STRING( str ) ( (void)0 )
#endif	//	_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCoolListCtrl window

class CCoolListCtrl : public CListCtrl
{
// Construction
public:
	CCoolListCtrl();

// Attributes
public:
	void SetRowHeigt(int nHeight); 
	

protected:
	virtual void PreSubclassWindow();

	
	// Implementation

public:
	virtual ~CCoolListCtrl();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	// Generated message map functions
protected:
	//{{AFX_MSG(CCoolListCtrl)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBegintrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDrawList ( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnDestroy();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public://member
	int m_nRowHeight;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COOLLISTCTRL_H__76A7A50A_AA16_4B70_BF2E_CD59F88E8665__INCLUDED_)
