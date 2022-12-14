// CoolStatic.cpp : implementation file
//

#include "stdafx.h"
#include "DLoader.h"
#include "CoolStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCoolStatic

CCoolStatic::CCoolStatic()
{
	VERIFY( m_bmpBckGrnd.LoadBitmap(IDB_BMP_TM) );
	m_nColor = RGB(255,255,255);
}

CCoolStatic::~CCoolStatic()
{
	m_bmpBckGrnd.DeleteObject(); //lint !e1551
}


BEGIN_MESSAGE_MAP(CCoolStatic, CStatic)
	//{{AFX_MSG_MAP(CCoolStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
// 	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoolStatic message handlers

void CCoolStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	CRect rect;
	GetClientRect(&rect);
	//draw background
	CDC memDC;
	if(!memDC.CreateCompatibleDC(&dc) )
		return;
	
	CBitmap * pOldBmp = memDC.SelectObject(&m_bmpBckGrnd);
	BITMAP bmpInfo;
	if( !m_bmpBckGrnd.GetBitmap( &bmpInfo ))
		return;

	dc.BitBlt( 0, 0, rect.Width(), bmpInfo.bmHeight-1, 
		&memDC, 0, 0, SRCCOPY );	

//    dc.StretchBlt( 0, 0, rect.Width(), rect.Height(), 
//		&memDC, 
//		0, 0, bmpInfo.bmWidth,  bmpInfo.bmHeight, 
//		SRCCOPY );
	
	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();

    //draw text
	CFont	Font;
	CFont*	pOldFont = NULL;
	LOGFONT	csLogFont;

	// Get log-font used by parent window
	GetParent()->GetFont()->GetLogFont(&csLogFont);
	// Make it bold
	//csLogFont.lfWeight = FW_BOLD;
	// Create font
	Font.CreateFontIndirect(&csLogFont);
	pOldFont = dc.SelectObject(&Font);
	dc.SetBkMode(TRANSPARENT);
	rect.right = rect.left + (LONG)((rect.right - rect.left)*2/3);
	dc.DrawText(m_strText, -1, rect, DT_LEFT /*| DT_WORDBREAK*/ );

	if (pOldFont)	
		dc.SelectObject(pOldFont);

	Font.DeleteObject();
	// Do not call CStatic::OnPaint() for painting messages
}

void CCoolStatic::SetColor(UINT rgb)
{
	m_nColor = rgb;
}
void CCoolStatic::SetText(LPCTSTR lpszText)
{
	m_strText = lpszText;
    this->Invalidate(FALSE);
}