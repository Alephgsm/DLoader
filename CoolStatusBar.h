#if !defined(AFX_COOLSTATUSBAR_H__DD12C5C9_93C8_4A71_A4F1_458A0187EF80__INCLUDED_)
#define AFX_COOLSTATUSBAR_H__DD12C5C9_93C8_4A71_A4F1_458A0187EF80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CoolStatusBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCoolStatusBar window

class CCoolStatusBar : public CStatusBar
{
// Construction
public:
	CCoolStatusBar();

// Attributes
public:

	CProgressCtrl m_prgrsCtrl;

// Operations
public:
	BOOL SetIndicators( const UINT* lpIDArray, int nIDCount );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoolStatusBar)
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetPaneTextClr(int nIdx, COLORREF clr);
	virtual ~CCoolStatusBar();

	// Generated message map functions
protected:
	void DrawSizingFlag(CDC * pDC);
	void DrawText(CDC * pDC, int nIdx);
	void DrawRect(CDC * pDC, int nIdx);
	void DrawBackGrnd(CDC * pDC);
	//{{AFX_MSG(CCoolStatusBar)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_nIDCount;
    CRect         m_rcPrgrsCtrl;
	COLORREF    * m_pClrPaneText;
	static const int PRGRSCTRL_WIDTH;
	static const int PRGRSCTRL_HEIGHT;

	static const COLORREF CLR_RECT_EDGE;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COOLSTATUSBAR_H__DD12C5C9_93C8_4A71_A4F1_458A0187EF80__INCLUDED_)
