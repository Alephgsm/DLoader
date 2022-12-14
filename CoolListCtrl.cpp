// CoolListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CoolListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCoolListCtrl

CCoolListCtrl::CCoolListCtrl()
{
	m_nRowHeight = 20;
}

CCoolListCtrl::~CCoolListCtrl()
{
}


BEGIN_MESSAGE_MAP(CCoolListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CCoolListCtrl)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY(HDN_BEGINTRACK, 0, OnBegintrack)
	ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, OnCustomDrawList )
	ON_WM_DESTROY()
	ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoolListCtrl message handlers

void CCoolListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CCoolListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CCoolListCtrl::OnBegintrack(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UNUSED_ALWAYS(pNMHDR);
	// TODO: Add your control notification handler code here
	SetFocus();
	*pResult = 0;
}

void CCoolListCtrl::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
    *pResult = CDRF_DODEFAULT;
    
    NMLVCUSTOMDRAW *lplvdr=(NMLVCUSTOMDRAW*)pNMHDR;
    switch(lplvdr->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        {
            *pResult = CDRF_NOTIFYITEMDRAW;
            break;
        }
    case CDDS_ITEMPREPAINT:
        {
            *pResult = CDRF_DODEFAULT;
            break;
        }
    default:
        break;        
    }
	
}

void CCoolListCtrl::PreSubclassWindow()
{
	// the list control must have the report style.
	ModifyStyle(0,LVS_OWNERDRAWFIXED);
	ASSERT( GetStyle() & LVS_REPORT );
	CListCtrl::PreSubclassWindow();
}

void CCoolListCtrl::OnDestroy() 
{
	CListCtrl::OnDestroy();
}

void CCoolListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (m_nRowHeight>0)
	{
		lpMeasureItemStruct->itemHeight = m_nRowHeight;
	}
}

void CCoolListCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your message handler code here and/or call default	
	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CCoolListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    TCHAR lpBuffer[MAX_PATH]={0};
	
	LV_ITEM lvi;
	
	lvi.mask = LVIF_TEXT | LVIF_PARAM ;
	lvi.iItem = lpDrawItemStruct->itemID ; 
	lvi.iSubItem = 0;
	lvi.pszText = lpBuffer ;
	lvi.cchTextMax = sizeof(lpBuffer);
	VERIFY(GetItem(&lvi));
	
	LV_COLUMN lvc, lvcprev ;
	::ZeroMemory(&lvc, sizeof(lvc));
	::ZeroMemory(&lvcprev, sizeof(lvcprev));
	lvc.mask = LVCF_WIDTH | LVCF_FMT;
	lvcprev.mask = LVCF_WIDTH | LVCF_FMT;
	
	for ( int nCol=0; GetColumn(nCol, &lvc); nCol++)
	{
		if ( nCol > 0 ) 
		{
			// Get Previous Column Width in order to move the next display item
			GetColumn(nCol-1, &lvcprev) ;
			lpDrawItemStruct->rcItem.left += lvcprev.cx ;
			lpDrawItemStruct->rcItem.right += lpDrawItemStruct->rcItem.left ; 
		}
		
		CRect rcItem;   
		if (!GetSubItemRect(lpDrawItemStruct->itemID,nCol,LVIR_LABEL,rcItem))   
			continue;  
		// Get the text 
		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = lpDrawItemStruct->itemID;
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iSubItem = nCol;
		lvi.pszText = lpBuffer;
		lvi.cchTextMax = sizeof(lpBuffer);
		VERIFY(GetItem(&lvi));
		
		CDC* pDC;
		pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		if ( lpDrawItemStruct->itemState & ODS_SELECTED )
		{
			pDC->FillSolidRect(rcItem, GetSysColor(COLOR_HIGHLIGHT)) ; 
            pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT)) ; 
			
		}
		else
		{
			pDC->FillSolidRect(rcItem,RGB(232, 232, 232));	
		    pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT)) ; 		
			
		}
		
		pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));
		
		UINT   uFormat    = DT_LEFT|DT_VCENTER|DT_SINGLELINE;//DT_CENTER ;
		CFont cFont ,*pOldFont; 
        //cFont.CreateFont(0,0,0,0,0,FALSE,FALSE,0,0,0,0,0,0,_T("Arial"));
        //pOldFont = pDC->SelectObject(&cFont);
		CFont* pFont = GetParent()->GetFont();
        pOldFont = pDC->SelectObject(pFont);
        CRect rcTmpItem=rcItem;   
        rcTmpItem.left += 4;
		::DrawText(lpDrawItemStruct->hDC, lpBuffer, _tcslen(lpBuffer), rcTmpItem, uFormat) ;
	   // pDC->SelectStockObject(SYSTEM_FONT) ;

	}
}

void CCoolListCtrl::SetRowHeigt(int nHeight) 
{
	m_nRowHeight = nHeight;
	
	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}
