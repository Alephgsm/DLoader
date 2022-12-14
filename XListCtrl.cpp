// XListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "XListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXListCtrl

CXListCtrl::CXListCtrl()
{
	GetColors();
}

CXListCtrl::~CXListCtrl()
{
}


BEGIN_MESSAGE_MAP(CXListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CXListCtrl)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY(HDN_BEGINTRACK, 0, OnBegintrack)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW,OnCustomDraw)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXListCtrl message handlers
int CXListCtrl::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
	LVHITTESTINFO lvhitTestInfo;
	
	lvhitTestInfo.pt = point;
	
	int nItem = ListView_SubItemHitTest(this->m_hWnd, &lvhitTestInfo);
	int nSubItem = lvhitTestInfo.iSubItem;
	//XLISTCTRL_TRACE(_T("in CToolTipListCtrl::OnToolHitTest: %d,%d\n"), nItem, nSubItem);

	UINT nFlags = lvhitTestInfo.flags;

	// nFlags is 0 if the SubItemHitTest fails
	// Therefore, 0 & <anything> will equal false
	if (nFlags & LVHT_ONITEMLABEL)
	{
		// If it did fall on a list item,
		// and it was also hit one of the
		// item specific subitems we wish to show tool tips for
		
		// get the client (area occupied by this control
		RECT rcClient;
		GetClientRect(&rcClient);
		
		// fill in the TOOLINFO structure
		pTI->hwnd = m_hWnd;
		pTI->uId = (UINT) (nItem * 1000 + nSubItem + 1);
		pTI->lpszText = LPSTR_TEXTCALLBACK;
		pTI->rect = rcClient;
		
		return pTI->uId;	// By returning a unique value per listItem,
							// we ensure that when the mouse moves over another
							// list item, the tooltip will change
	}
	else
	{
		//Otherwise, we aren't interested, so let the message propagate
		return -1;
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnToolTipText
BOOL CXListCtrl::OnToolTipText(UINT /*id*/, NMHDR * pNMHDR, LRESULT * pResult)
{
	UINT nID = pNMHDR->idFrom;
	//XLISTCTRL_TRACE(_T("in CXListCtrl::OnToolTipText: id=%d\n"), nID);
	
	// check if this is the automatic tooltip of the control
	if (nID == 0) 
		return TRUE;	// do not allow display of automatic tooltip,
						// or our tooltip will disappear
	
	// handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

	
	*pResult = 0;
	
	// get the mouse position
	const MSG* pMessage;
	pMessage = GetCurrentMessage();
	ASSERT(pMessage);
	if(pMessage == NULL)
	{
		return FALSE;
	}
	CPoint pt;
	pt = pMessage->pt;		// get the point from the message
	ScreenToClient(&pt);	// convert the point's coords to be relative to this control

	// see if the point falls onto a list item
	
	LVHITTESTINFO lvhitTestInfo;
	
	lvhitTestInfo.pt = pt;
	
	int nItem = SubItemHitTest(&lvhitTestInfo);
	int nSubItem = lvhitTestInfo.iSubItem;

	if(nSubItem != 1)
		return TRUE;
	
	UINT nFlags = lvhitTestInfo.flags;
	
	// nFlags is 0 if the SubItemHitTest fails
	// Therefore, 0 & <anything> will equal false
	if (nFlags & LVHT_ONITEMLABEL)
	{
		// If it did fall on a list item,
		// and it was also hit one of the
		// item specific subitems we wish to show tooltips for
		
		CString strToolTip;
		strToolTip = _T("");


		strToolTip = GetItemText(nItem,nSubItem);
		int nLength = strToolTip.GetLength();
		CString strTemp = strToolTip;
		BOOL bOK = FALSE;
		if(nLength>79)
		{
			int nLeft = nLength;
			int nFind = strTemp.Find(_T('\\'));
			while(nFind != -1)
			{
				nLeft -= nFind;
				if(nLeft + 3 <=79)
				{
					strTemp = strTemp.Right(nLeft);
					strTemp.Insert(0,_T("..."));
					strToolTip = strTemp;
					bOK = TRUE;
					break;
				}
				else
				{
					nLeft--;
					strTemp = strTemp.Right(nLeft);					
					nFind = strTemp.Find(_T('\\'));
				}
			}
			if(!bOK)
			{
				strTemp = strTemp.Right(76);
				strTemp.Insert(0,_T("..."));
				strToolTip = strTemp;
			}
		}


		if (!strToolTip.IsEmpty())
		{
			// If there was a CString associated with the list item,
			// copy it's text (up to 80 characters worth, limitation 
			// of the TOOLTIPTEXT structure) into the TOOLTIPTEXT 
			// structure's szText member
			
#ifndef _UNICODE
			if (pNMHDR->code == TTN_NEEDTEXTA) //lint !e648
				lstrcpyn(pTTTA->szText, strToolTip, 80);
			else
				_mbstowcsz(pTTTW->szText, strToolTip, 80);
#else
			if (pNMHDR->code == TTN_NEEDTEXTA) //lint !e648
				_wcstombsz(pTTTA->szText, strToolTip, 80);
			else
				lstrcpyn(pTTTW->szText, strToolTip, 80);
#endif
			return FALSE;	 // we found a tool tip,
		}
	}
	
	return FALSE;	// we didn't handle the message, let the 
					// framework continue propagating the message
}

void CXListCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class	
	CListCtrl::PreSubclassWindow();
	EnableToolTips(TRUE);
}

void CXListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CXListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CXListCtrl::OnBegintrack(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UNUSED_ALWAYS(pNMHDR);
//	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	SetFocus();
	*pResult = 0;
}
void CXListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	LPNMLVCUSTOMDRAW pNMLvCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	if(pNMLvCustomDraw)
	{
		nItem = pNMLvCustomDraw->nmcd.dwItemSpec;
		nSubItem = pNMLvCustomDraw->iSubItem;
	}
	else
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;
	}

	int nCount = CListCtrl::GetItemCount();
	if(nItem >= 0 && nItem < nCount && nSubItem >= 0)
	{
		DWORD dwData = CListCtrl::GetItemData(nItem);
		BOOL bEnable = TRUE;
		
		if(DI_IS_GRAY(dwData))
		{
			pNMLvCustomDraw->clrText = m_crGrayText;
			bEnable = FALSE;
		}
		else
		{
			pNMLvCustomDraw->clrText = m_crWindowText;
			bEnable = TRUE;
		}
//		pNMLvCustomDraw->clrTextBk = m_crGrayText;	

		if(!bEnable && nSubItem == 0 && (GetExtendedStyle() & LVS_EX_CHECKBOXES) != 0)
		{
			COLORREF crText  = m_crWindowText;
			COLORREF crBkgnd = m_crWindow;

			CDC* pDC = CDC::FromHandle(pNMLvCustomDraw->nmcd.hdc);
			CRect rect;
			GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);
			DrawCheckbox(nItem,nSubItem,pDC,crText,crBkgnd,rect,bEnable);
			*pResult = CDRF_SKIPDEFAULT;
			return;
		}

	}

	*pResult = CDRF_NOTIFYITEMDRAW;
}

BOOL CXListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
/*lint -save -e1746*/
}/*lint -restore*/

///////////////////////////////////////////////////////////////////////////////
// GetColors
void CXListCtrl::GetColors()
{
	m_cr3DFace              = ::GetSysColor(COLOR_3DFACE);
	m_cr3DHighLight         = ::GetSysColor(COLOR_3DHIGHLIGHT);
	m_cr3DShadow            = ::GetSysColor(COLOR_3DSHADOW);
	m_crActiveCaption       = ::GetSysColor(COLOR_ACTIVECAPTION);
	m_crBtnFace             = ::GetSysColor(COLOR_BTNFACE);
	m_crBtnShadow           = ::GetSysColor(COLOR_BTNSHADOW);
	m_crBtnText             = ::GetSysColor(COLOR_BTNTEXT);
	m_crGrayText            = ::GetSysColor(COLOR_GRAYTEXT);
	m_crHighLight           = ::GetSysColor(COLOR_HIGHLIGHT);
	m_crHighLightText       = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_crInactiveCaption     = ::GetSysColor(COLOR_INACTIVECAPTION);
	m_crInactiveCaptionText = ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);
	m_crWindow              = ::GetSysColor(COLOR_WINDOW);
	m_crWindowText          = ::GetSysColor(COLOR_WINDOWTEXT);
}

///////////////////////////////////////////////////////////////////////////////
// DrawCheckbox
void CXListCtrl::DrawCheckbox(int nItem,
							  int nSubItem,
							  CDC *pDC,
							  COLORREF crText,
							  COLORREF crBkgnd,
							  const CRect& rect,
							  BOOL bEnable)
{
	ASSERT(pDC);
	if(NULL == pDC)
	{
		return;
	}

	if (rect.IsRectEmpty())
	{
		return;
	}

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);
// 	if(bEnable)
// 	{
// 		crText = m_crWindowText;
// 		crBkgnd = m_crWindow;
// 	}
// 	else
// 	{
// 		crText = m_crGrayText;
// 		crBkgnd = m_crWindow;
// 	}

	pDC->FillSolidRect(&rect, crBkgnd);

	CRect chkboxrect;
	chkboxrect = rect;
	chkboxrect.bottom -= 2;
	chkboxrect.top += 2;
	chkboxrect.left += 4;		// line up checkbox with header checkbox
	chkboxrect.right = chkboxrect.left + chkboxrect.Height();	// width = height

	CRect chkboxrectBkgnd;
	chkboxrectBkgnd = rect;
	chkboxrectBkgnd.right = chkboxrect.right+1;

	pDC->FillSolidRect(&chkboxrectBkgnd, RGB(232, 232, 232));

	CString str;
	str = GetItemText(nItem, nSubItem);

	if (str.IsEmpty())
	{
		// center the checkbox

		chkboxrect.left = rect.left + rect.Width()/2 - chkboxrect.Height()/2 - 1;
		chkboxrect.right = chkboxrect.left + chkboxrect.Height();
	}

	// fill rect around checkbox with white
	pDC->FillSolidRect(&chkboxrect, m_crWindow);

	// draw border
	CBrush brush(m_crGrayText);
	pDC->FrameRect(&chkboxrect, &brush);
	CRect chkboxrect2;
	chkboxrect2 = chkboxrect;
	chkboxrect2.bottom -= 1;
	chkboxrect2.top += 1;
	chkboxrect2.left += 1;	
	chkboxrect2.right -=1;
	pDC->FrameRect(&chkboxrect2, &brush);


 	if (GetCheck(nItem) && bEnable)
 	{
		CPen *pOldPen = NULL;

//		CPen graypen(PS_SOLID, 1, m_crGrayText);
		CPen blackpen(PS_SOLID, 1, RGB(51,153,51));

//		if (bEnable)
			pOldPen = pDC->SelectObject(&blackpen);
//		else
//			pOldPen = pDC->SelectObject(&graypen);

		// draw the checkmark
		int x = chkboxrect.left + 9;
		ASSERT(x < chkboxrect.right);
		int y = chkboxrect.top + 3;
		int i;
		for (i = 0; i < 4; i++)
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x, y+3);
			x--;
			y++;
		}
		for (i = 0; i < 3; i++)
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x, y+3);
			x--;
			y--;
		}

		if (pOldPen)
			pDC->SelectObject(pOldPen);
 	}

	if (!str.IsEmpty())
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(crText);
		if(crText == m_crWindowText)
		{
			pDC->SetTextColor(m_crGrayText);
		}
		pDC->SetBkColor(crBkgnd);
		CRect textrect;
		textrect = rect;
		textrect.left = chkboxrect.right + 3;

		UINT nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;	//+++
// 		if (m_bUseEllipsis)
// 			nFormat |= DT_END_ELLIPSIS;

		pDC->DrawText(str, &textrect, nFormat);
	}
}

void CXListCtrl::GetDrawColors(int nItem,
							   int nSubItem,
							   COLORREF& colorText,
							   COLORREF& colorBkgnd)
{
	DWORD dwStyle    = GetStyle();
	DWORD dwExStyle  = GetExtendedStyle();

	COLORREF crText  = colorText;
	COLORREF crBkgnd = colorBkgnd;

	if (GetItemState(nItem, LVIS_SELECTED))
	{
		if (dwExStyle & LVS_EX_FULLROWSELECT)
		{
			// selected?  if so, draw highlight background
			crText  = m_crHighLightText;
			crBkgnd = m_crHighLight;

			// has focus?  if not, draw gray background
			if (m_hWnd != ::GetFocus())
			{
				if (dwStyle & LVS_SHOWSELALWAYS)
				{
					crText  = m_crWindowText;
					crBkgnd = m_crBtnFace;
				}
				else
				{
					crText  = colorText;
					crBkgnd = colorBkgnd;
				}
			}
		}
		else	// not full row select
		{
			if (nSubItem == 0)
			{
				// selected?  if so, draw highlight background
				crText  = m_crHighLightText;
				crBkgnd = m_crHighLight;

				// has focus?  if not, draw gray background
				if (m_hWnd != ::GetFocus())
				{
					if (dwStyle & LVS_SHOWSELALWAYS)
					{
						crText  = m_crWindowText;
						crBkgnd = m_crBtnFace;
					}
					else
					{
						crText  = colorText;
						crBkgnd = colorBkgnd;
					}
				}
			}
		}
	}

	colorText = crText;
	colorBkgnd = crBkgnd;
}