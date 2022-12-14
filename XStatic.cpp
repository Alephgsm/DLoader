// XStatic.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "XStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXStatic

CXStatic::CXStatic()
{
	m_uTextColor = RGB(0,0,0); /*RGB(68,85,123);*/
	
#ifdef _SPUPGRADE
	m_uBKGColor  = RGB(5,207,132);	
#elif defined _FACTORY
	m_uBKGColor  = RGB(225,178,43);	
#else	
	m_uBKGColor  = RGB(236,233,216);
#endif

	m_nFontSize  = 9;
    m_strText    = _T("");
	m_uBKGBmpID  = 0xFFFFFFFF;
}

CXStatic::~CXStatic()
{
}


BEGIN_MESSAGE_MAP(CXStatic, CStatic)
	//{{AFX_MSG_MAP(CXStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXStatic message handlers

void CXStatic::OnPaint() 
{
	
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	CRect rect;
	GetClientRect(&rect);
	//draw background
	DrawBkgColor(&dc,rect);

    //draw text
	DrawText(&dc,rect);

	// Do not call CStatic::OnPaint() for painting messages
}
void CXStatic::SetTextColor(UINT uColor)
{
	m_uTextColor = uColor;
}

void CXStatic::SetBKGColor(UINT uColor)
{
	m_uBKGColor = uColor;
}

void CXStatic::SetFontSize(int nSize)
{
	m_nFontSize = nSize;
}

void CXStatic::SetText(LPCTSTR lpszText)
{
	m_strText = lpszText;
}

void CXStatic::SetBKGBmp(UINT uBmpID)
{
	m_uBKGBmpID = uBmpID;
	m_bmpBkg.LoadBitmap(uBmpID);
}

void CXStatic::DrawBkgBmp(CPaintDC *pDC,const CRect &rect)
{
	if(m_uBKGBmpID == 0xFFFFFFFF)
		return;

	CDC memDC;
	if(!memDC.CreateCompatibleDC(pDC) )
		return;
	
	CBitmap * pOldBmp = memDC.SelectObject(&m_bmpBkg);
	BITMAP bmpInfo;
	if( !m_bmpBkg.GetBitmap( &bmpInfo ))
		return;

	pDC->BitBlt( 0, 0, rect.Width(), bmpInfo.bmHeight-1, 
		&memDC, 0, 0, SRCCOPY );	

//    pDC->StretchBlt( 0, 0, rect.Width(), rect.Height(), 
//		&memDC, 
//		0, 0, bmpInfo.bmWidth,  bmpInfo.bmHeight, 
// 		SRCCOPY );
	
	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();

}

void CXStatic::DrawBkgColor(CPaintDC *pDC,const CRect &rect)
{
	CBrush brush(m_uBKGColor);
	pDC->FillRect(&rect,&brush);
}
void CXStatic::DrawText(CPaintDC *pDC,const CRect &rect)
{
	CFont	Font;
	CFont*	pOldFont = NULL;
	LOGFONT	csLogFont;
    CRect rc(rect);
	// Get log-font used by parent window
	GetParent()->GetFont()->GetLogFont(&csLogFont);
	// Make it bold
 	csLogFont.lfWeight = FW_BOLD;
	csLogFont.lfHeight = 20;
	// Create font
	Font.CreateFontIndirect(&csLogFont);
	pOldFont = pDC->SelectObject(&Font);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_uTextColor);
	rc.top += (rc.Height()-20)/2;
	rc.left += 20;
	pDC->DrawText(m_strText, -1, rc, DT_LEFT | DT_VCENTER   );

	if (pOldFont)	
		pDC->SelectObject(pOldFont);
}