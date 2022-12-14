// CoolStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "CoolStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int CCoolStatusBar::PRGRSCTRL_WIDTH  = 160;
const int CCoolStatusBar::PRGRSCTRL_HEIGHT = 16;

const COLORREF CCoolStatusBar::CLR_RECT_EDGE = RGB(49, 105, 198);
/////////////////////////////////////////////////////////////////////////////
// CCoolStatusBar

CCoolStatusBar::CCoolStatusBar()
{
	m_nIDCount = 0;
	m_pClrPaneText = NULL;
}

CCoolStatusBar::~CCoolStatusBar()
{
	delete []m_pClrPaneText;
}

BEGIN_MESSAGE_MAP(CCoolStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CCoolStatusBar)
	ON_WM_NCCALCSIZE()
	ON_WM_CREATE()
	//ON_WM_PAINT()
	//ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoolStatusBar message handlers

CSize CCoolStatusBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	CSize size = CStatusBar::CalcFixedLayout(bStretch, bHorz);

	m_rcPrgrsCtrl.top    = (size.cy - PRGRSCTRL_HEIGHT) / 2 + 1;
	m_rcPrgrsCtrl.bottom = m_rcPrgrsCtrl.top + PRGRSCTRL_HEIGHT;
	
	return size;
}

void CCoolStatusBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// TODO: Add your message handler code here and/or call default

	CRect rcNew = lpncsp->rgrc[0];

	m_rcPrgrsCtrl.left   = (rcNew.Width() - PRGRSCTRL_WIDTH) / 2;
	m_rcPrgrsCtrl.right  = m_rcPrgrsCtrl.left + PRGRSCTRL_WIDTH+5;

	if( m_prgrsCtrl.m_hWnd != NULL )
		m_prgrsCtrl.MoveWindow(m_rcPrgrsCtrl, FALSE);

	CStatusBar::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CCoolStatusBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CStatusBar::OnUpdateCmdUI( pTarget, bDisableIfNoHndler );
}

int CCoolStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CRect rc(0, 0, 0, 0);
	if( !m_prgrsCtrl.Create(WS_DISABLED | WS_CHILD , rc, this, AFX_IDW_PANE_FIRST) )
		return -1;

	lpCreateStruct->style |= WS_CLIPCHILDREN;

	return 0;
}

BOOL CCoolStatusBar::SetIndicators( const UINT* lpIDArray, int nIDCount )
{
	BOOL bRet = CStatusBar::SetIndicators(lpIDArray, nIDCount);
	
	m_nIDCount = nIDCount;
	
	m_pClrPaneText = new COLORREF[nIDCount];
	if( m_pClrPaneText != NULL )
	{
		COLORREF clrDefault = RGB(0, 0, 0);
		for( int i = 0; i < nIDCount; ++i )
			m_pClrPaneText[i] = clrDefault;
	}

	return bRet;
}

void CCoolStatusBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect(rcClient);

	CDC memDC;
	VERIFY( memDC.CreateCompatibleDC(&dc) );

	CBitmap bmp;
	VERIFY( bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height()) );
	CBitmap * pOldBmp = memDC.SelectObject(&bmp);

	memDC.SetBkMode(TRANSPARENT);

	DrawBackGrnd(&memDC);
	// TODO: Add your message handler code here
	for( int i = 0; i < m_nIDCount; ++i )
	{
		DrawRect(&memDC, i);
		DrawText(&memDC, i);
	}

	DrawSizingFlag(&memDC);

	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();
	bmp.DeleteObject();
	// Do not call CStatusBar::OnPaint() for painting messages
}

void CCoolStatusBar::DrawBackGrnd(CDC *pDC)
{
	CRect rect;
	GetClientRect(rect);

	pDC->FillSolidRect(rect, GetSysColor(COLOR_3DFACE));
}

void CCoolStatusBar::DrawRect(CDC *pDC, int nIdx)
{
	if( !(GetPaneStyle(nIdx) & SBPS_NOBORDERS) )
	{
		CRect rect;
		GetItemRect(nIdx, rect);
		pDC->Draw3dRect(rect, CLR_RECT_EDGE, CLR_RECT_EDGE);
	}
}

void CCoolStatusBar::DrawText(CDC *pDC, int nIdx)
{
	if( !(GetPaneStyle(nIdx) & SBPS_DISABLED) )
	{
		CRect rect;
		GetItemRect(nIdx, rect);

		UINT uFormat;
		if( nIdx == 0 )
		{
			rect.left += 2;
			uFormat = DT_LEFT | DT_VCENTER;
		}
		else
		{
			uFormat = DT_CENTER | DT_VCENTER;
		}
		CFont * pFont = GetFont();
		_ASSERTE( pFont != NULL );
		
		rect.top += 1;
		COLORREF clrOld = pDC->GetTextColor();
		if( m_pClrPaneText != NULL )
			clrOld = pDC->SetTextColor( m_pClrPaneText[nIdx] );
		CFont * pOldFont = pDC->SelectObject(pFont);
		pDC->DrawText(GetPaneText(nIdx), rect, uFormat);
		pDC->SelectObject(pOldFont);
		pDC->SetTextColor(clrOld);
	}
}

void CCoolStatusBar::DrawSizingFlag(CDC *pDC)
{
	if( GetParent()->IsZoomed() )
		return;

	CRect rect;
	GetClientRect(rect);

	CPen pen(PS_SOLID, 1, CLR_RECT_EDGE);

	CPen * pOldPen = pDC->SelectObject(&pen);

	int nHeight = rect.Height();
	for( int i = 0; i < nHeight; i += 3 )
	{
		pDC->MoveTo(rect.right - i, rect.bottom);
		pDC->LineTo(rect.right, rect.bottom - i);
	}

	pDC->SelectObject(pOldPen);
}

void CCoolStatusBar::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CStatusBar::OnTimer(nIDEvent);
}

void CCoolStatusBar::SetPaneTextClr(int nIdx, COLORREF clr)
{
	_ASSERTE( nIdx >= 0 && nIdx < m_nIDCount );

	if( m_pClrPaneText != NULL )
		m_pClrPaneText[nIdx] = clr;
}
