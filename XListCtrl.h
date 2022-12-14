#if !defined(AFX_XLISTCTRL_H__C73922E0_DF3D_4414_A204_921C61EC0F45__INCLUDED_)
#define AFX_XLISTCTRL_H__C73922E0_DF3D_4414_A204_921C61EC0F45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXListCtrl window
#define DI_IS_GRAY(d)		((d) & (0x00010000))
#define DI_SET_GRAY(d)		((d) | (0x00010000))
#define DI_UNSET_GRAY(d)	((d) & (0xFFFEFFFF))
#define DI_IS_OMIT(d)		((d) & (0x00020000))
#define DI_SET_OMIT(d)		((d) | (0x00020000))
//#define DI_UNSET_OMIT(d)	((d) | (0xFFFDFFFF))  
#define DI_SET_KEY_ITEM(d)  ((d) | (0x00000100))
#define DI_IS_KEY_ITEM(d)   ((d) & (0x00000100))
#define DI_IS_NEED_FILE(d)  ((d) & (0x00000001))

class CXListCtrl : public CListCtrl
{
// Construction
public:
	CXListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXListCtrl)
	protected:
	virtual void PreSubclassWindow();
	
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXListCtrl();
	virtual int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
	// Generated message map functions
protected:
	//{{AFX_MSG(CXListCtrl)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBegintrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
    virtual afx_msg BOOL OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	DECLARE_MESSAGE_MAP()
private:
	void GetColors();
	void DrawCheckbox( int nItem, 
						  int nSubItem, 
						  CDC *pDC, 
						  COLORREF crText,
						  COLORREF crBkgnd,
						  const CRect& rect,
						  BOOL bEnable);
	void GetDrawColors(int nItem,
							   int nSubItem,
							   COLORREF& colorText,
							   COLORREF& colorBkgnd);

private:
	COLORREF		m_cr3DFace;
	COLORREF		m_cr3DHighLight;
	COLORREF		m_cr3DShadow;
	COLORREF		m_crActiveCaption;
	COLORREF		m_crBtnFace;
	COLORREF		m_crBtnShadow;
	COLORREF		m_crBtnText;
	COLORREF		m_crGrayText;
	COLORREF		m_crHighLight;
	COLORREF		m_crHighLightText;
	COLORREF		m_crInactiveCaption;
	COLORREF		m_crInactiveCaptionText;
	COLORREF		m_crWindow;
	COLORREF		m_crWindowText;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XLISTCTRL_H__C73922E0_DF3D_4414_A204_921C61EC0F45__INCLUDED_)
