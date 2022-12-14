#include "stdafx.h"
#include "ProgressCtrlST.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CProgressCtrlST::CProgressCtrlST()
{
	// Default range of the control
	m_nLower = 0;
	m_nUpper = 100;
	CalcRange();

	// Default position
	m_nPos = 0;

	// Default step
	m_nStep = 10;
    m_dwRate = 1;


	FreeResources(FALSE);
}

CProgressCtrlST::~CProgressCtrlST()
{
/*lint -save -e1551 */
	FreeResources();
/*lint -restore */
}

BEGIN_MESSAGE_MAP(CProgressCtrlST, CProgressCtrl)
	//{{AFX_MSG_MAP(CProgressCtrlST)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CProgressCtrlST::FreeResources(BOOL bCheckForNULL)
{
	if (bCheckForNULL == TRUE)
	{
		// Destroy bitmap
		if (m_hBitmap)	::DeleteObject(m_hBitmap);
	} // if

	m_hBitmap = NULL;
	m_dwWidth = 0;
	m_dwHeight = 0;
} // End of FreeResources

void CProgressCtrlST::OnPaint() 
{
	int	   nPercentage		= 0;
//	static temp_nPercentage =0;
//	// Calculate control's percentage to draw
	nPercentage = (int)((100.0/m_nRange)*(abs(m_nLower)+m_nPos));
//	if (nPercentage == temp_nPercentage && nPercentage != 0) return;
//	temp_nPercentage =nPercentage;

	

// If there is no bitmap loaded
//	if (m_hBitmap == NULL) 
//	{
//		CProgressCtrl::OnPaint();
//		return;
//	} // if

	CRect		rcCtrl;
	DWORD		dwStyle			= 0;
	BOOL		bVertical		= FALSE;
	float		f				= 0;

	HDC			hdcMem			= NULL;
	HDC			hdcTemp			= NULL;
	HBITMAP		hOldBitmap		= NULL;
	HBITMAP		hbmpTemp		= NULL;
	HBITMAP		hOldTempBitmap	= NULL;
	CPaintDC	dc(this);

	GetClientRect(rcCtrl);

	dwStyle = GetStyle();
	bVertical = (dwStyle & PBS_VERTICAL) == PBS_VERTICAL;

	// Has border ?
	if ((dwStyle & WS_BORDER) == WS_BORDER)
	{
		CBrush brBtnShadow(RGB(0, 0, 0));
		dc.FrameRect(rcCtrl, &brBtnShadow);
		rcCtrl.DeflateRect(1, 1);
	} // if
//  @hongliang.xin 2007-7-9  cr87132
//	rcCtrl.DeflateRect(1, 1);

	hdcMem = ::CreateCompatibleDC(dc.m_hDC);
	if(hdcMem == NULL)
	{
		CString strError;
		strError.Format(_T("Error:%d"),GetLastError());
		AfxMessageBox(strError);
		return ;
	}

	// Select bitmap
	hOldBitmap = (HBITMAP)::SelectObject(hdcMem, m_hBitmap);

	// Create temporary DC & bitmap
	hdcTemp = ::CreateCompatibleDC(dc.m_hDC);
	if(hdcTemp == NULL)
	{
		CString strError;
		strError.Format(_T("Error:%d"),GetLastError());
		AfxMessageBox(strError);
		return ;
	}

	hbmpTemp = ::CreateCompatibleBitmap(hdcMem, rcCtrl.Width(), rcCtrl.Height());
	if(hbmpTemp == NULL)
	{
		CString strError;
		strError.Format(_T("Error:%d"),GetLastError());
		AfxMessageBox(strError);
		return ;
	}
	
	hOldTempBitmap = (HBITMAP)::SelectObject(hdcTemp, hbmpTemp);

	// Calculate control's percentage to draw
 	//nPercentage = (int)((100.0/m_nRange)*(abs(m_nLower)+m_nPos));

	// Adjust rectangle to draw on screen
	if (bVertical)
	{
		f = ((float)(rcCtrl.Height())/100)*nPercentage;
		if ((rcCtrl.bottom - (int)f) < rcCtrl.top)
			rcCtrl.top += 1;
		else
			rcCtrl.top = rcCtrl.bottom - (int)f;
	} // if
	else
	{
		f = ((float)(rcCtrl.Width())/100)*nPercentage;
		if ((rcCtrl.left + (int)f) > rcCtrl.right)
			rcCtrl.right -= 1;
		else
			rcCtrl.right = rcCtrl.left + (int)f;
	} // else

	// Tile the bitmap into the temporary rectangle
	TileBitmap(hdcTemp, hdcMem, rcCtrl);

	// Copy from temporary DC to screen (only the percentage rectangle)
	if (rcCtrl.IsRectEmpty() == FALSE)
		::BitBlt(dc.m_hDC, rcCtrl.left, rcCtrl.top, rcCtrl.Width(), rcCtrl.Height(), hdcTemp, 0, 0, SRCCOPY);

	// Restore old selected bitmaps
	::SelectObject(hdcTemp, hOldTempBitmap);
	::SelectObject(hdcMem, hOldBitmap);
//  ::ReleaseDC(this->m_hWnd,hdcTemp);
//	::ReleaseDC(this->m_hWnd,hdcMem);
	::DeleteObject(hbmpTemp); 
	::DeleteDC(hdcTemp);
	::DeleteDC(hdcMem);
	
//	CRect	rcFullCtrl;
//	GetClientRect(rcFullCtrl);

//	OnDrawText(&dc, nPercentage, rcFullCtrl, rcCtrl, bVertical);

	
	
} // End of OnPaint

void CProgressCtrlST::TileBitmap(HDC hdcDest, HDC hdcSrc, const CRect &rect)
{
	int nHLoop = 0;
	int nVLoop = 0;
	int nHTiles = 0;
	int nVTiles = 0;
 
	if(m_dwWidth == 0)
		m_dwWidth = 1;
	if(m_dwHeight == 0)
		m_dwHeight = 1;

	// Calculate number of horizontal tiles
	nHTiles = (rect.Width() / (int)m_dwWidth);
	if (rect.Width() % (int)m_dwWidth != 0) nHTiles++;
	// Calculate number of vertical tiles
	nVTiles = (rect.Height() / (int)m_dwHeight);
	if (rect.Height() % (int)m_dwHeight != 0) nVTiles++;

	// Tile bitmap horizontally
	for (nHLoop = 0; nHLoop < nHTiles; nHLoop++)
	{
		// Tile bitmap vertically
		for (nVLoop = 0; nVLoop < nVTiles; nVLoop++)
		{
			::BitBlt(hdcDest, (nHLoop * m_dwWidth), (nVLoop * m_dwHeight), m_dwWidth, m_dwHeight, hdcSrc, 0, 0, SRCCOPY);
		} // for
	} // for
} // End of TileBitmap

void CProgressCtrlST::CalcRange()
{
	m_nRange = abs(m_nUpper - m_nLower);
	// Avoid divide by zero
	if (m_nRange == 0) m_nRange = 1;
} // End of CalcRange

// This function sets the bitmap to use to draw the progress bar.
//
// Parameters:
//		[IN]	nBitmap
//				Resource ID of the bitmap to use as background.
//				Pass NULL to remove any previous bitmap.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		PROGRESSST_OK
//			Function executed successfully.
//		PROGRESSST_INVALIDRESOURCE
//			The resource specified cannot be found or loaded.
//
DWORD CProgressCtrlST::SetBitmap(int nBitmap, BOOL bRepaint)
{
	HBITMAP		hBitmap			= NULL;
	HINSTANCE	hInstResource	= NULL;

	// Find correct resource handle
	hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nBitmap), RT_BITMAP);

	// Load bitmap
	hBitmap = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(nBitmap), IMAGE_BITMAP, 0, 0, 0);
	
	return SetBitmap(hBitmap, bRepaint);
} // End of SetBitmap

// This function sets the bitmap to use to draw the progress bar.
//
// Parameters:
//		[IN]	hBitmap
//				Handle to the bitmap to use as background.
//				Pass NULL to remove any previous bitmap.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		PROGRESSCTRLST_OK
//			Function executed successfully.
//		PROGRESSCTRLST_INVALIDRESOURCE
//			The resource specified cannot be found or loaded.
//
DWORD CProgressCtrlST::SetBitmap(HBITMAP hBitmap, BOOL bRepaint)
{
	int		nRetValue;
	BITMAP	csBitmapSize;

	// Free any loaded resource
	FreeResources();

	if (hBitmap)
	{
		m_hBitmap = hBitmap;

		// Get bitmap size
		nRetValue = ::GetObject(hBitmap, sizeof(csBitmapSize), &csBitmapSize);
		if (nRetValue == 0)
		{
			FreeResources();
			return PROGRESSCTRLST_INVALIDRESOURCE;
		} // if
		m_dwWidth = (DWORD)csBitmapSize.bmWidth;
		m_dwHeight = (DWORD)csBitmapSize.bmHeight;
	} // if

	if (bRepaint)	Invalidate();

	return PROGRESSCTRLST_OK;
} // End of SetBitmap

// This function sets the upper and lower limits of the progress bar control's range
// and redraws the bar to reflect the new ranges.
//
// Parameters:
//		[IN]	nLower
//				Specifies the lower limit of the range (default is zero).
//		[IN]	nUpper
//				Specifies the upper limit of the range (default is 100).
//
void CProgressCtrlST::SetRange(int nLower, int nUpper)
{
    m_dwRate = ((DWORD)nUpper> 0x7FFFFFFF) ? MAX_PROGRESS_RATE : 1;
    m_nLower = (DWORD)nLower/m_dwRate;
    m_nUpper = (DWORD)nUpper/m_dwRate;
	CalcRange();

	CProgressCtrl::SetRange32(m_nLower, m_nUpper);
} // End of SetRange

// This function specifies the step increment for a progress bar control.
// The step increment is the amount by which a call to StepIt increases
// the progress bar's current position.
//
// Parameters:
//		[IN]	nStep
//				New step increment.
//
// Return value:
//		The previous step increment.
//
int CProgressCtrlST::SetStep(int nStep)
{
	m_nStep = nStep;
	return CProgressCtrl::SetStep(nStep);
} // End Of SetStep

// This function sets the progress bar control's current position as specified by nPos
// and redraws the bar to reflect the new position.
// The position of the progress bar control is not the physical location on the screen,
// but rather is between the upper and lower range indicated in SetRange.
//
// Parameters:
//		[IN]	nPos
//				New position of the progress bar control.
//
// Return value:
//		The previous position of the progress bar control.
//
int CProgressCtrlST::SetPos(int nPos)
{
	// Bound checking
    m_nPos = (DWORD)nPos/m_dwRate;
	if (m_nPos < m_nLower) m_nPos = m_nLower;
	if (m_nPos > m_nUpper) m_nPos = m_nUpper;
	return CProgressCtrl::SetPos(m_nPos);
} // End of SetPos

// This function advances the current position for a progress bar control by the step increment
// and redraws the bar to reflect the new position.
// The step increment is set by the SetStep method.
//
// Return value:
//		The previous position of the progress bar control.
//
int CProgressCtrlST::StepIt()
{
	m_nPos += m_nStep;

	// Bound checking
	if (m_nPos > m_nUpper) m_nPos = m_nLower + (m_nPos - m_nUpper);

	return CProgressCtrl::StepIt();
} // End of StepIt

// This function is called each time the progress bar is redrawn.
// It is a virtual function to let derived classes do custom drawing.
// The default implementation does nothing.
//
// Parameters:
//		[IN]	pDC
//				Pointer to the device context.
//		[IN]	nPercentage
//				Current percentage of the progress bar.
//		[IN]	rcCtrl
//				A CRect object that indicates the dimensions of the entire control.
//		[IN]	rcProgress
//				A CRect object that indicates the dimensions of the currently displayed bar.
//		[IN]	bVertical
//				TRUE if the progress is vertical, otherwise FALSE.
//
/*
void CProgressCtrlST::OnDrawText(CDC* pDC, int nPercentage, CRect rcCtrl, CRect rcProgress, BOOL bVertical) 
{
	UNUSED_ALWAYS(pDC);
	UNUSED_ALWAYS(nPercentage);
	UNUSED_ALWAYS(rcCtrl);
	UNUSED_ALWAYS(rcProgress);
	UNUSED_ALWAYS(bVertical);


	CString	sText;
	CFont	Font;
	CFont*	pOldFont = NULL;
	LOGFONT	csLogFont;

	// Get log-font used by parent window
	GetParent()->GetFont()->GetLogFont(&csLogFont);
	// Make it bold
	csLogFont.lfWeight = FW_BOLD;
	// Vertical progress bar ?
	if (bVertical)
	{
		csLogFont.lfEscapement =  900;
		csLogFont.lfOrientation = 900;
	} // if
	// Create font
	Font.CreateFontIndirect(&csLogFont);

	sText.Format(_T("%d%%"), nPercentage);
	pOldFont = pDC->SelectObject(&Font);

	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(sText, -1, rcProgress, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

	if (pOldFont)	pDC->SelectObject(pOldFont);
	Font.DeleteObject();

} // End of OnDrawText
*/