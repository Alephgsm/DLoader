// CoolToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "DLoader.h"
#include "CoolToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const COLORREF CCoolToolBar::CLR_TB_BCKGRND = RGB(232, 232, 232);
const int CCoolToolBar::IMG_CX = 48;
const int CCoolToolBar::IMG_CY = 48;
/////////////////////////////////////////////////////////////////////////////
// CCoolToolBar

CCoolToolBar::CCoolToolBar()
{
	m_pImgLst         = NULL;
	m_pDisabledImgLst = NULL;
	m_pHotImgLst      = NULL;

	VERIFY( m_bmpBckGrnd.LoadBitmap(IDB_TB_BACKGRND) );
    VERIFY( m_brshBckGrnd.CreateSolidBrush(CLR_TB_BCKGRND) );
}

CCoolToolBar::~CCoolToolBar()
{
/*lint -save -e1551 */
	if( m_pImgLst != NULL )
	{
		m_pImgLst->DeleteImageList();  
		delete m_pImgLst;  
		m_pImgLst = NULL;
	}

	if( m_pDisabledImgLst != NULL )
	{
		m_pDisabledImgLst->DeleteImageList();
		delete m_pDisabledImgLst;
		m_pDisabledImgLst = NULL;
	}

	if( m_pHotImgLst != NULL )
	{
		m_pHotImgLst->DeleteImageList();
		delete m_pHotImgLst;
		m_pHotImgLst = NULL;
	}

	m_brshBckGrnd.DeleteObject();
	m_bmpBckGrnd.DeleteObject();
/*lint -restore */
}


BEGIN_MESSAGE_MAP(CCoolToolBar, CToolBar)
	//{{AFX_MSG_MAP(CCoolToolBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoolToolBar message handlers

BOOL CCoolToolBar::SetBitmaps(UINT uBmpID, UINT uDisabledBmpID,UINT uHotBmpID)
{
	_ASSERTE( m_pImgLst == NULL && m_pDisabledImgLst == NULL );

	try{
		m_pImgLst = new CImageList;
		VERIFY( m_pImgLst->Create( IMG_CX, IMG_CY, ILC_COLOR24 | ILC_MASK, 0, 1 ) );
		
		CBitmap bmp;
		VERIFY( bmp.LoadBitmap(uBmpID) );
		
		VERIFY( m_pImgLst->Add(&bmp, RGB(0, 0, 0) ) != -1 );
		
		m_pDisabledImgLst = new CImageList;
		VERIFY( m_pDisabledImgLst->Create( IMG_CX, IMG_CY, ILC_COLOR24 | ILC_MASK, 0, 1 ) );
		
		bmp.DeleteObject();
		VERIFY( bmp.LoadBitmap(uDisabledBmpID) );
		
		VERIFY( m_pDisabledImgLst->Add(&bmp, RGB(0, 0, 0) ) != -1 );

		m_pHotImgLst = new CImageList;
		VERIFY( m_pHotImgLst->Create( IMG_CX, IMG_CY, ILC_COLOR24 | ILC_MASK, 0, 1 ) );
		bmp.DeleteObject();
		VERIFY( bmp.LoadBitmap(uHotBmpID) );		
		VERIFY( m_pHotImgLst->Add(&bmp, RGB(0, 0, 0) ) != -1 );
		
		CToolBarCtrl & tbCtrl = GetToolBarCtrl();
		
		tbCtrl.SetImageList( m_pImgLst );
		tbCtrl.SetDisabledImageList(m_pDisabledImgLst);
		tbCtrl.SetHotImageList(m_pHotImgLst);
		//tbCtrl.SetStyle(TBSTYLE_FLAT);
	}
	catch(...)
	{
		TRACE0("!! CCoolToolBar::SetBitmaps occur fatal error !!\r\n");

		return  FALSE;
	}

	return TRUE;
}


BOOL CCoolToolBar::OnEraseBkgnd(CDC* pDC)
{	
	CRect rect;
	GetClientRect(rect);
	
//	CDC memDC;
//	if(!memDC.CreateCompatibleDC(pDC) )
//		return FALSE;
//	
//	CBitmap * pOldBmp = memDC.SelectObject(&m_bmpBckGrnd);
//	BITMAP bmpInfo;
//	if( !m_bmpBckGrnd.GetBitmap( &bmpInfo ))
//		return FALSE;
//
//	const int nTBHeight = bmpInfo.bmHeight;
//	
//	// 本图片我们提供了一个较长的，使用时只取所需的长度部分
//	pDC->BitBlt( 0, 0, rect.Width(), bmpInfo.bmHeight, 
//		&memDC, 0, 0, SRCCOPY );	
//
//    pDC->StretchBlt( 0, nTBHeight, rect.Width(), rect.Height(), 
//		&memDC, 
//		0, 0, bmpInfo.bmWidth,  bmpInfo.bmHeight, 
//		SRCCOPY );
//
//	
//	memDC.SelectObject(pOldBmp);
//	memDC.DeleteDC();

	rect.left -= 2;
	rect.bottom +=2;

	
#ifdef _SPUPGRADE
	CBrush brush(RGB(5,207,132));
#elif defined _FACTORY	
	CBrush brush(RGB(225,178,43));
#else	
	CBrush brush(RGB(236,233,216));
#endif
	pDC->FillRect(&rect,&brush);

	
	return TRUE;
//	return CToolBar::OnEraseBkgnd(pDC);
}
