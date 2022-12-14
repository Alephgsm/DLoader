// DLoaderView.cpp : implementation of the CDLoaderView class
//

#include "stdafx.h"
#include "DLoader.h"

#ifndef _SPUPGRADE
#include "atlconv.h"
#include "./phasecheck/PhaseCheckBuild.h"
#endif

#define START_ICON_SIZE 25

#include "MainFrm.h"
#include "DLoaderView.h"

#define COLOR_PASS    RGB(0,202,0)
#define COLOR_FAILED  RGB(255,0,0)
#define COLOR_WAIT    RGB(0,0,255)

#define BUTTON_POS    2

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAXHEIGHT 30

static CString StatusString[] = 
{
    _T("Waiting"),
    _T("Checking baudrate"),
    _T("Connecting"),
    _T("Erasing flash"),
    _T("Downloading..."),
    _T("Reading Flash"),
    _T("Reseting"),
    _T("Read Chip Type"),
    _T("Read NV Item"),
    _T("Change Baud"),
    _T("Finish"),
    _T("Unplugged"),
	_T("Paused"),
    _T("Enumerating port")

};


#define   DL_TIMER   0x10000
/////////////////////////////////////////////////////////////////////////////
// CDLoaderView


IMPLEMENT_DYNCREATE(CDLoaderView, CListView)

BEGIN_MESSAGE_MAP(CDLoaderView, CListView)
	//{{AFX_MSG_MAP(CDLoaderView)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
    ON_NOTIFY(HDN_ENDTRACK,0,OnEndTrack)
    ON_WM_MEASUREITEM_REFLECT()
    ON_MESSAGE( BM_CHECK_BAUDRATE, OnBMCheckBaud )
    ON_MESSAGE( BM_CONNECT, OnBMConnect )
    ON_MESSAGE( BM_ERASE_FLASH, OnBMEraseFlash )
    ON_MESSAGE( BM_DOWNLOAD, OnBMDownLoad )
    ON_MESSAGE( BM_DOWNLOAD_PROCESS, OnBMDLoadProcess )
    ON_MESSAGE( BM_READ_FLASH, OnBMReadFlash )
    ON_MESSAGE( BM_READ_FLASH_PROCESS, OnBMReadFlashProcess )
    ON_MESSAGE( BM_RESET, OnBMReset )
    ON_MESSAGE( BM_SET_FIRST_MODE, OnBMSetFirstMode )
    ON_MESSAGE( BM_READ_CHIPTYPE, OnBMReadChipType )
    ON_MESSAGE( BM_READ_NVITEM, OnBMReadNVItem )
    ON_MESSAGE( BM_CHANGE_BAUD, OnBMChangeBuad )    
    ON_MESSAGE (BM_BEGIN, OnBMBegin )
    ON_MESSAGE (BM_FILE_BEGIN, OnBMFileBegin )
    ON_MESSAGE (BM_END, OnBMEnd )
	ON_MESSAGE (BM_UN_TRANSCODE, OnBMUnTransCode )
#ifndef _SPUPGRADE
	ON_MESSAGE(WM_REQUIRE_SN, OnAcquireBarcode)
#endif
	ON_NOTIFY (HDN_BEGINTRACK, 0, OnBeginTrack)
	ON_COMMAND_RANGE( IDC_BTN_START1, IDC_BTN_START24, OnBtnStart )
    ON_NOTIFY(HDN_ITEMCHANGEDA, 0, OnHdnItemchanged)
    ON_NOTIFY(HDN_ITEMCHANGEDW, 0, OnHdnItemchanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDLoaderView construction/destruction

CDLoaderView::CDLoaderView()
{
	m_nHeaderHeight = 0;
#ifndef _SPUGRADE
	m_bSNDlgShow = FALSE;
	m_pCurBarcodeDlg = NULL;
#endif

	m_hBtnBitmap = NULL;
}

CDLoaderView::~CDLoaderView()
{
/*lint -save -e1551 */
    m_pCurBarcodeDlg = NULL;
	if(m_hBtnBitmap)
	{
		DeleteObject(m_hBtnBitmap);
		m_hBtnBitmap = NULL;
	}

/*lint -restore */
}

BOOL CDLoaderView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDLoaderView drawing

void CDLoaderView::OnDraw(CDC* /*pDC*/)
{
	CDLoaderDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CDLoaderView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	TCHAR szPath[_MAX_PATH] = {0};
	GetModuleFilePath(AfxGetApp()->m_hInstance,szPath);
	CString strBKMarkPath = _T("");

    CListCtrl* pListCtrl = &GetListCtrl();
    
    //modify the style
    DWORD dwStyle = pListCtrl->GetStyle();
    dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED;
    pListCtrl->ModifyStyle(0,dwStyle);

    DWORD dwExStyle = pListCtrl->GetExtendedStyle();
    dwExStyle |= LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
    pListCtrl->SetExtendedStyle(dwExStyle);
#ifdef _SPUPGRADE
	pListCtrl->SetBkColor(RGB(227,238,230));		
	strBKMarkPath.Format(_T("%s\\udl_bkmark.bmp"),szPath);
#elif defined _FACTORY	
	pListCtrl->SetBkColor(RGB(236,233,219));	
	strBKMarkPath.Format(_T("%s\\fdl_bkmark.bmp"),szPath);
#else	
	pListCtrl->SetBkColor(RGB(232, 232, 232));	
	strBKMarkPath.Format(_T("%s\\rdl_bkmark.bmp"),szPath);	
#endif
	pListCtrl->SetBkImage(const_cast<LPTSTR>(strBKMarkPath.operator LPCTSTR()),FALSE,100,95);
    
    //Add columns

	//Load Column
	
	pListCtrl->InsertColumn( 0, _T("Port"), LVCFMT_CENTER, 50 );
	pListCtrl->InsertColumn( STEP_INDEX, _T("Step"), LVCFMT_CENTER, 120 );
	pListCtrl->InsertColumn( STATUS_INDEX, _T("Status"),LVCFMT_CENTER,180);	
	pListCtrl->InsertColumn( PROG_INDEX, _T("Progress"),LVCFMT_CENTER,315);
	pListCtrl->InsertColumn( TIME_INDEX, _T("Time(s)"),LVCFMT_CENTER,100);
	pListCtrl->InsertColumn( MCPTYPE_INDEX, _T("MCP Type"),LVCFMT_CENTER,180);
#ifdef _RESEARCH
	pListCtrl->InsertColumn( IMEI_INDEX, _T("IMEI"),LVCFMT_CENTER,180);
#endif

    CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
    if(pMF->m_bShowRate)
    {
        pListCtrl->InsertColumn( RATE_INDEX, _T("Rate(MB/s)"),LVCFMT_CENTER,180);
    }

	m_imgList.Create(1,24,TRUE|ILC_COLOR24,1,0);
	pListCtrl->SetImageList(&m_imgList,LVSIL_SMALL);

    //Get headerCtrl's height
    CHeaderCtrl* pHeader = pListCtrl->GetHeaderCtrl();
    CRect rcHeader;
    pHeader->GetItemRect(0,rcHeader);
    m_nHeaderHeight = rcHeader.Height();


	m_hBtnBitmap = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_START));


	CFont font;
	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));
	lf.lfHeight = 20;
	_tcscpy(lf.lfFaceName,_T("Arial"));
	font.CreateFontIndirect(&lf);
	SetFont(&font);
// just for pc-lint
	UNUSED_ALWAYS(IDC_BTN_START1);
	UNUSED_ALWAYS(IDC_BTN_START2);
	UNUSED_ALWAYS(IDC_BTN_START3);
	UNUSED_ALWAYS(IDC_BTN_START4);
	UNUSED_ALWAYS(IDC_BTN_START5);
	UNUSED_ALWAYS(IDC_BTN_START6);
	UNUSED_ALWAYS(IDC_BTN_START7);
	UNUSED_ALWAYS(IDC_BTN_START8);
	UNUSED_ALWAYS(IDC_BTN_START9);
	UNUSED_ALWAYS(IDC_BTN_START10);
	UNUSED_ALWAYS(IDC_BTN_START11);
	UNUSED_ALWAYS(IDC_BTN_START12);
	UNUSED_ALWAYS(IDC_BTN_START13);
	UNUSED_ALWAYS(IDC_BTN_START14);
	UNUSED_ALWAYS(IDC_BTN_START15);
	UNUSED_ALWAYS(IDC_BTN_START16);
	UNUSED_ALWAYS(IDC_BTN_START17);
	UNUSED_ALWAYS(IDC_BTN_START18);
	UNUSED_ALWAYS(IDC_BTN_START19);
	UNUSED_ALWAYS(IDC_BTN_START20);
	UNUSED_ALWAYS(IDC_BTN_START21);
	UNUSED_ALWAYS(IDC_BTN_START22);
	UNUSED_ALWAYS(IDC_BTN_START23);
	UNUSED_ALWAYS(IDC_BTN_START24);

    // TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CDLoaderView diagnostics

#ifdef _DEBUG
void CDLoaderView::AssertValid() const
{
	CListView::AssertValid();
}

void CDLoaderView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CDLoaderDoc* CDLoaderView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDLoaderDoc)));
	return (CDLoaderDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDLoaderView message handlers
int CDLoaderView::IsValidPort( int nPort)
{
    return GetPortIndex( nPort ); 
}


BOOL CDLoaderView::StartProg(int nPort,int nMin,int nMax)
{
    int nItem = IsValidPort( nPort );
    if( nItem == -1 )
        return FALSE;
    
    CListCtrl* pList = &GetListCtrl();

    CRect rc;
    pList->GetSubItemRect(nItem,PROG_INDEX,LVIR_BOUNDS,rc);

	PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData(nItem);
	if(g_theApp.m_bColorFlag)
	{
		if(psi->bLastSuccess)
		{
			psi->pProg->SetBitmap(IDB_BMP_PROG_BLOCK);
		}
		else
		{
			psi->pProg->SetBitmap(IDB_BMP_PROG_BROWN);
		}
	}

	if(psi && psi->pProg)
	{
		psi->pProg->SetRange(nMin,nMax);
		psi->pProg->MoveWindow(rc);
		psi->pProg->ShowWindow(SW_SHOW);
	}

    return TRUE;
}

BOOL CDLoaderView::StepProg(int nPort,int nStep)
{
    int nItem = IsValidPort( nPort );
    if( nItem == -1 )
        return FALSE;

	CListCtrl* pList = &GetListCtrl();
	PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData(nItem);    
    psi->pProg->OffsetPos( nStep );

    return TRUE;
}

BOOL CDLoaderView::EndProg(int nPort)
{
    int nItem = IsValidPort( nPort );
    if( nItem == -1 )
        return FALSE;

	CListCtrl* pList = &GetListCtrl();
	PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData(nItem);    
    psi->pProg->SetPos(0);
    psi->pProg->ShowWindow(SW_HIDE);

    return TRUE;
}

BOOL CDLoaderView::SetProgStep(int nPort,int nStep)
{
    int nItem = IsValidPort( nPort );
    if( nItem == -1 )
        return FALSE;
    
    if(nStep <= 0)
        nStep = 1;

	CListCtrl* pList = &GetListCtrl();
	PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData(nItem);    
    psi->pProg->SetStep( nStep );

    return TRUE;
}

BOOL CDLoaderView::SetProgPos(int nPort,int nPos)
{
    int nItem = IsValidPort( nPort );
    if( nItem == -1 )
        return FALSE;    

	CListCtrl* pList = &GetListCtrl();
	PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData(nItem);    
    psi->pProg->SetPos( nPos );
	psi->pProg->Invalidate(FALSE);

    ReportProcessToPipe(psi->pProg);

    return TRUE;
}


void CDLoaderView::ResizeProg()
{
    CListCtrl* pList = &GetListCtrl();
	int nCount = pList->GetItemCount();

	for(int i=0;i<nCount;i++)
    {
		PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData(i);   
		if(psi)
		{			
			CRect rc;
			pList->GetSubItemRect(i,PROG_INDEX,LVIR_BOUNDS,rc);
			rc.DeflateRect(0,1);			
			psi->pProg->MoveWindow(rc);
			psi->pProg->Invalidate();
		}
    }
}

BOOL CDLoaderView::AddProg( int nPort,BOOL bOpen )
{
    CListCtrl* pListCtrl = &GetListCtrl();

	PORT_STATE_INFO_PTR psi;
	if(m_PortMap.Lookup(nPort,psi))
	{		
		CString strPort;
		if( bOpen )
		{
			strPort.Format( _T("%d"), nPort );
#if defined( _FACTORY ) || defined(_SPUPGRADE)
			if(g_theApp.m_bManual && psi->pButton)
			{
				if(!psi->pButton->IsWindowEnabled())
				{
					psi->pButton->ShowWindow(SW_SHOW);
					psi->pButton->EnableWindow(TRUE);
				}
			}
#endif
		}
		else
		{
			if( nPort < 0 )
			{
				strPort = _T("   ");
			}
			else
			{
				strPort.Format( _T("%d"), nPort );
				strPort += STRING_BADPORT;
			}
		}
        psi->nEnumPortStartTime = 0;
		pListCtrl->SetItemText( psi->nIndex,0,strPort );	
		return TRUE;
	}
    
    int nCount = m_PortMap.GetCount();
	psi = new PORT_STATE_INFO;
	psi->nIndex = nCount;
	psi->nPort  = nPort;
	psi->nCurFile = DL_NO_FILE;
	psi->nLastFile = psi->nCurFile;
	psi->nUsedTime = 0;
	psi->bLastSuccess = TRUE;
    psi->nEnumPortStartTime = 0;
    m_PortMap.SetAt(nPort,psi);

     
    //Create progress controls
    //CProgressCtrl* pProg = new CProgressCtrl;
	psi->pProg = new CProgressCtrlST;
    BOOL bRet;
    bRet = psi->pProg->Create( PBS_SMOOTH | WS_CHILD,CRect(0,0,0,0),this,1);
    if(!bRet)
        return bRet;
	
    psi->pProg->SetBitmap(IDB_BMP_PROG_BLOCK,FALSE);
    psi->pProg->SetRange(0,100);
    psi->pProg->SetStep(1);	

#ifdef _SPUPGRADE
	if(g_theApp.m_bManual)
	{
#endif
		UINT nId = IDC_BTN_START1 + m_PortMap.GetCount() - 1;
		psi->pButton = new CButton;
		psi->pButton->Create(_T("Start"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP, CRect(0,0,10,10), this, nId);
		CBitmap bitmap ;
		HBITMAP hBmp = psi->pButton->SetBitmap(m_hBtnBitmap);
		if (hBmp)	::DeleteObject(hBmp);
		if(g_theApp.m_bManual)
		{
			psi->pButton->ShowWindow(SW_SHOW);
			psi->pButton->EnableWindow(TRUE);
		}
		else
		{
			psi->pButton->ShowWindow(SW_HIDE);
			psi->pButton->EnableWindow(FALSE);
		}
		CListCtrl* pList = &GetListCtrl();
		CRect rc;
		pList->GetSubItemRect(m_PortMap.GetCount()-1, BUTTON_POS, LVIR_BOUNDS, rc);
		rc.right = rc.left + START_ICON_SIZE; 
		rc.top     = rc.top + (rc.Height() - START_ICON_SIZE)/2;
		rc.bottom  = rc.top + START_ICON_SIZE;
		psi->pButton->MoveWindow(rc);	
#ifdef _SPUPGRADE
	}
#endif

    CString strText;
    if( bOpen )
    {
        strText.Format( _T("%d"), nPort );
    }
    else
    {
        if( nPort < 0 )
        {
            strText = _T("   ");
        }
        else
        {
            strText.Format( _T("%d"), nPort );
			strText += STRING_BADPORT;
        }
    }
    pListCtrl->InsertItem( nCount,strText );
	pListCtrl->SetItemData(nCount,(DWORD)psi); 

    return TRUE;
}

BOOL CDLoaderView::RemoveAll()
{
    m_StepDescription.RemoveAll();
       
	POSITION pos = m_PortMap.GetStartPosition();
    while( pos )
    {
		PORT_STATE_INFO_PTR psi = NULL;
		int nPort = 0;
        m_PortMap.GetNextAssoc( pos, nPort, psi );	
		if(psi)
		{
			psi->Clear();
			delete psi;
			psi = NULL;
		}		
    }
	m_PortMap.RemoveAll();

    GetListCtrl().DeleteAllItems();

    return TRUE;
}

void CDLoaderView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    ResizeProg();
	ResizeButton();
	CListView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDLoaderView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    ResizeProg();
	ResizeButton(); 
	CListView::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CDLoaderView::OnEndTrack(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
    ResizeProg();
	TRACE(_T("On End Track!\n"));
	ResizeButton();
	
	CListCtrl* pList = &GetListCtrl();
	int nCount = pList->GetItemCount();
	for(int i=0; i<nCount;i++)
    {
		PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData(i);
		if(psi && psi->pButton && psi->pButton->IsWindowEnabled())
		{
		   psi->pButton->ShowWindow(SW_SHOW);  
		}
    }

	*pResult = 0;
}

void CDLoaderView::MeasureItem( LPMEASUREITEMSTRUCT pMeasureItem )
{
    //pMeasureItem->itemHeight = 30;
    int size = m_PortMap.GetCount();

    if(size == 0)
    {
        //have no progress ctrl,use default
        return;
    }

    //Adjust the list view to just contain progress ctrls
    CRect rc;
    GetClientRect(rc);

    int Height = ( rc.Height() - m_nHeaderHeight ) / size;
    if(Height > MAXHEIGHT)
        Height = MAXHEIGHT;

    pMeasureItem->itemHeight = Height;
}


void CDLoaderView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);	
    ResizeProg();	
	ResizeButton();
}

void CDLoaderView::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CListCtrl* pList = &GetListCtrl();

    CRect rc;
    CString str;

    ::SetBkMode(lpDrawItemStruct->hDC,OPAQUE);

    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    COLORREF color = ::GetSysColor(COLOR_WINDOW);
    COLORREF colorText = ::GetSysColor(COLOR_WINDOWTEXT);

    //draw the fisrt subitem based on second sub item
    pList->GetSubItemRect(lpDrawItemStruct->itemID,STEP_INDEX,LVIR_BOUNDS,rc);
    CRect rcFirst = lpDrawItemStruct->rcItem;
    rcFirst.right = rc.left;
    str = pList->GetItemText(lpDrawItemStruct->itemID,0);
    if(pDC != NULL)
    {
        pDC->FillSolidRect(rcFirst,color);
    }
    ::DrawText(lpDrawItemStruct->hDC,str,str.GetLength(),rcFirst,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    //Draw the second subitem
    pList->GetSubItemRect(lpDrawItemStruct->itemID,STEP_INDEX,LVIR_BOUNDS,rc);
    str = pList->GetItemText(lpDrawItemStruct->itemID,STEP_INDEX);
    if(pDC != NULL)
    {
        pDC->FillSolidRect(rc,color);
    }
    ::DrawText(lpDrawItemStruct->hDC,str,str.GetLength(),rc,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    //draw the third subitem
    pList->GetSubItemRect(lpDrawItemStruct->itemID,STATUS_INDEX,LVIR_BOUNDS,rc);
    str = pList->GetItemText(lpDrawItemStruct->itemID,STATUS_INDEX);
    if(pDC != NULL)
    {
        pDC->FillSolidRect(rc,color);
    }
    ::DrawText(lpDrawItemStruct->hDC,str,str.GetLength(),rc,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    //set font
	CFont* pFont = (CFont*)::SelectObject(lpDrawItemStruct->hDC,HFONT(m_Font));

    //draw the fourth subitem
    pList->GetSubItemRect(lpDrawItemStruct->itemID,PROG_INDEX,LVIR_BOUNDS,rc);
    str = pList->GetItemText(lpDrawItemStruct->itemID,PROG_INDEX);

    
    if(str.Find(STRING_PASS) != -1 )
    {		
		::SetTextColor(lpDrawItemStruct->hDC,COLOR_PASS);		
    }
    else if(str.Find(STRING_FAILED) != -1)
    {
        ::SetTextColor(lpDrawItemStruct->hDC,COLOR_FAILED);  
    }
    else
    {
		PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData(lpDrawItemStruct->itemID);	
		if(g_theApp.m_bColorFlag && psi!= NULL && !psi->bLastSuccess)
		{
			::SetTextColor(lpDrawItemStruct->hDC,COLOR_FAILED);
		}
		else
		{
			::SetTextColor(lpDrawItemStruct->hDC,COLOR_WAIT);
		}
    }

    if(pDC != NULL)
    {
        pDC->FillSolidRect(rc,color);
    }
    ::DrawText(lpDrawItemStruct->hDC,str,str.GetLength(),rc,DT_CENTER | DT_VCENTER  | DT_SINGLELINE);

	//draw the 5th subitem
    pList->GetSubItemRect(lpDrawItemStruct->itemID,TIME_INDEX,LVIR_BOUNDS,rc);
    str = pList->GetItemText(lpDrawItemStruct->itemID,TIME_INDEX);
	::SetTextColor(lpDrawItemStruct->hDC,COLOR_WAIT);
    if(pDC != NULL)
    {
        pDC->FillSolidRect(rc,color);
    }
    ::DrawText(lpDrawItemStruct->hDC,str,str.GetLength(),rc,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	//draw the 6th subitem
    pList->GetSubItemRect(lpDrawItemStruct->itemID,MCPTYPE_INDEX,LVIR_BOUNDS,rc);
    str = pList->GetItemText(lpDrawItemStruct->itemID,MCPTYPE_INDEX);
	::SetTextColor(lpDrawItemStruct->hDC,COLOR_WAIT);
    if(pDC != NULL)
    {
        pDC->FillSolidRect(rc,color);
    }
    ::DrawText(lpDrawItemStruct->hDC,str,str.GetLength(),rc,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

#ifdef _RESEARCH
	//draw the 7th subitem
    pList->GetSubItemRect(lpDrawItemStruct->itemID,IMEI_INDEX,LVIR_BOUNDS,rc);
    str = pList->GetItemText(lpDrawItemStruct->itemID,IMEI_INDEX);
	::SetTextColor(lpDrawItemStruct->hDC,COLOR_WAIT);
    if(pDC != NULL)
    {
        pDC->FillSolidRect(rc,color);
    }
    ::DrawText(lpDrawItemStruct->hDC,str,str.GetLength(),rc,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

#endif

    CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
    if(pMF->m_bShowRate)
    {
        pList->GetSubItemRect(lpDrawItemStruct->itemID,RATE_INDEX,LVIR_BOUNDS,rc);
        str = pList->GetItemText(lpDrawItemStruct->itemID,RATE_INDEX);
        ::SetTextColor(lpDrawItemStruct->hDC,COLOR_WAIT);
        if(pDC != NULL)
        {
            pDC->FillSolidRect(rc,color);
        }
        ::DrawText(lpDrawItemStruct->hDC,str,str.GetLength(),rc,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

	//设置回系统颜色
    ::SetTextColor(lpDrawItemStruct->hDC,colorText);
    
    ::SelectObject(lpDrawItemStruct->hDC,pFont);

}

int CDLoaderView::GetPortIndex(int nPort)
{
	PORT_STATE_INFO_PTR psi;
    BOOL bRet = m_PortMap.Lookup(nPort,psi);
    if(bRet)
        return psi->nIndex;
    else
        return -1;
}

BOOL CDLoaderView::SetStatus(int nPort,DL_STAGE stage,BOOL bNeedProg,
                             int nMin /* = 0 */,int nMax /* = 0 */)
{
    int nItem = IsValidPort( nPort );
    if( nItem == -1 )
        return FALSE;
    
    CListCtrl* pList = &(GetListCtrl());
	PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData(nItem);
	if(psi == NULL)
		return FALSE;
    
    DL_STAGE oriStage = (DL_STAGE)psi->nStage;
    if( oriStage == DL_FINISH_STAGE && stage == DL_UNPLUGGED_STAGE )
    {
        // Ignore this message
        return TRUE;
    }

#ifdef _FACTORY
	if( oriStage == DL_FINISH_STAGE && stage == DL_NONE_STAGE )
    {
        // Ignore this message
        return TRUE;
    }
#endif

    //save status
  	psi->nStage = stage;
    pList->SetItemText( nItem,STATUS_INDEX,StatusString[stage]);
	if(DL_CONNECT == stage)
	{
		CString strTime = pList->GetItemText(nItem,TIME_INDEX);
		if(strTime.IsEmpty() || strTime.Find(_T('s')) != -1)
		{
			pList->SetItemText( nItem,TIME_INDEX,_T("0"));
			pList->SetItemText( nItem,MCPTYPE_INDEX,_T("----"));
#ifdef _RESEARCH
			pList->SetItemText( nItem,IMEI_INDEX,_T("----"));
#endif
            CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
            if(pMF->m_bShowRate)
            {
                pList->SetItemText( nItem,RATE_INDEX,_T("----"));
            }
		}
	}

    EndProg( nPort );

    if(bNeedProg)
    {        
        StartProg( nPort,nMin,nMax );
    }
    else
    {
        if(stage < DL_CONNECT)
        {
#if defined( _FACTORY ) || defined(_SPUPGRADE)
			// keep the previous result
			if(g_theApp.m_bManual || !g_theApp.m_bResultHolding)
			{
				pList->SetItemText( nItem,PROG_INDEX,STRING_WAIT);
			}
			else
			{
				CString strText = pList->GetItemText(nItem,PROG_INDEX);
				if( !strText.IsEmpty() && ( strText.Find(STRING_PASS)==0  || 
						 strText.Find(STRING_PASS) == 10 || // [Removed]+space
						 strText.Find(STRING_PASS) == 9 || // Previous+space
					     strText.Find(STRING_FAILED) != -1) )
				{	
					//if not add "Previous" at head of strText, add it
					//if(strText.Find(_T("Previous")) != 0)
					//{
					//	strText.Insert(0,_T("Previous "));
					//}
					pList->SetItemText( nItem,PROG_INDEX,strText);
				}
				else
				{
					pList->SetItemText( nItem,PROG_INDEX,STRING_WAIT);
				}
			}
#else
			CString strText = pList->GetItemText(nItem,PROG_INDEX);
			if( !strText.IsEmpty() && ( strText.Find(STRING_PASS)==0  || 
					 strText.Find(STRING_PASS) == 10 || // [Removed]+space
					 strText.Find(STRING_PASS) == 9 || // Previous+space
					 strText.Find(STRING_FAILED) != -1) )
			{		
				pList->SetItemText( nItem,PROG_INDEX,strText);
			}
			else
			{
				pList->SetItemText( nItem,PROG_INDEX,STRING_WAIT);
				pList->SetItemText( nItem,MCPTYPE_INDEX,_T("----"));
				pList->SetItemText( nItem,IMEI_INDEX,_T("----"));
			}
#endif            
        }
        else if( stage < DL_UNPLUGGED_STAGE )
        {            
            pList->SetItemText( nItem,PROG_INDEX,STRING_DOING);
        }
    }

    if (g_theApp.m_bCMDFlash)
    {
        CString strInfo;
        strInfo.Format(_T("%-25s %s "),pList->GetItemText(nItem,STEP_INDEX),pList->GetItemText(nItem,STATUS_INDEX));
        g_theApp.ReportToPipe(strInfo,FALSE);
    }
   
    return TRUE;    
}

BOOL CDLoaderView::GetStatus( int nPort,DL_STAGE& stage )
{
    int nItem = IsValidPort( nPort );
    if( nItem == -1 )
        return FALSE;

	CListCtrl* pList = &GetListCtrl();
    PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData( nItem );
	if(psi)
	{
		stage = (DL_STAGE)psi->nStage;
	}       
    return TRUE;
}

void CDLoaderView::ResetStatus( int nPort )
{
    SetStatus( nPort, DL_NONE_STAGE, FALSE );

	int nItem = IsValidPort( nPort );
    if( nItem == -1 )
        return;
	CListCtrl* pList = &GetListCtrl();
    PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData( nItem );
	if(psi)
	{
		psi->bLastSuccess = TRUE;
	}
}

BOOL CDLoaderView::SetResult(int nPort,BOOL bSuccess,
							  LPVOID lpObj /* = NULL  */,int nFlag /*= 1*/ )
{    
	UNUSED_ALWAYS(bSuccess);
	
    int nItem = IsValidPort( nPort );
    if( nItem == -1 )
        return FALSE;

	CListCtrl* pListCtrl = &GetListCtrl();
    PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pListCtrl->GetItemData( nItem );

	BOOL bRemoved = FALSE;

	CString strItemText = pListCtrl->GetItemText( nItem, STATUS_INDEX);
	if(strItemText.CompareNoCase(StatusString[DL_UNPLUGGED_STAGE]) == 0 || psi->nStage == DL_UNPLUGGED_STAGE)
	{
		bRemoved = TRUE;
	}

	_BMOBJ * pbj = (_BMOBJ *)lpObj;
	strItemText = pListCtrl->GetItemText( nItem, PROG_INDEX);

	if(psi->nStage == DL_UNPLUGGED_STAGE )
	{
		if( nFlag && pbj != NULL && _tcslen(pbj->szErrorMsg) != 0)
		{
			CString strErrText = STRING_FAILED;	
			strErrText += _T(": ");
			strErrText += pbj->szErrorMsg;                    
			pListCtrl->SetItemText( nItem, PROG_INDEX, strErrText );
		}
		else if( bSuccess && _tcslen(pbj->szErrorMsg) == 0)
		{
			pListCtrl->SetItemText( nItem, PROG_INDEX, STRING_PASS );
		}

		ReportResultToPipe(nPort);
		return TRUE;
	}

	CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

	if(nFlag == 2) //port is removed
	{
		EndProg( nPort );		
		//if(!bRemoved) //not mark removed yet, do it	
		//{	
		//	if(pMF->m_bPortSecondEnum && pbj != NULL && pbj->nStage == 1)
		//	{
		//	}
		//	else
		//	{
		//		strItemText.Insert(0,_T("[Removed] "));	
		//	}
		//}
		psi->nStage = DL_UNPLUGGED_STAGE;
		pListCtrl->SetItemText( nItem, PROG_INDEX, (-1 != strItemText.Find(STRING_FAILED) ) ? strItemText : STRING_READY );	
		pListCtrl->SetItemText( nItem, STATUS_INDEX, StatusString[DL_UNPLUGGED_STAGE]);
		if (psi->nCurFile != DL_NO_FILE)
		{
			psi->nLastFile = psi->nCurFile;
		}	
		psi->nCurFile = DL_NO_FILE;
		StopOnePortTimer(nItem);
		ReportResultToPipe(nPort);
		return TRUE;
	}
	
	
    BOOL bSuc = FALSE;
 
	if((nFlag == 1 && pbj != NULL && _tcslen(pbj->szErrorMsg) == 0) || ( nFlag == 0 && bSuccess))
		bSuc = TRUE;          
    
	psi->nStage = DL_FINISH_STAGE;
    pListCtrl->SetItemText( nItem, STATUS_INDEX, StatusString[DL_FINISH_STAGE]);
	
	StopOnePortTimer(nItem);	

	EndProg( nPort );	
	
    if( bSuc )
    {        
		if(pMF->m_bPortSecondEnum && pbj != NULL && pbj->nStage == 1)//second enum port
		{
			pListCtrl->SetItemText( nItem, PROG_INDEX,STRING_WAIT);
            pListCtrl->SetItemText( nItem, STATUS_INDEX, StatusString[DL_SECOND_ENUMPORT]);
            psi->nEnumPortStartTime = GetTickCount();
		}
		else
		{
            psi->nEnumPortStartTime = 0;
			pListCtrl->SetItemText( nItem, PROG_INDEX,STRING_PASS);
		}
    }
    else
    { 
        CString strWarning = pbj->szErrorMsg;

        if(!(g_theApp.m_bStopDownloadIfOldMemory) && strWarning.Left(6).CompareNoCase(_T("Second")) == 0)
        {
            CString strErrText = pbj->szErrorMsg;

            pListCtrl->SetItemText( nItem, PROG_INDEX, strErrText );

            if(bRemoved)
            {
                psi->nStage = DL_UNPLUGGED_STAGE;	
                pListCtrl->SetItemText( nItem, STATUS_INDEX, StatusString[DL_UNPLUGGED_STAGE]);
            }
        }
        else
        {
            CString strErrText = STRING_FAILED;
            if( pbj != NULL && 0 != _tcslen(pbj->szErrorMsg))
            {
                strErrText += _T(": ");
                strErrText += pbj->szErrorMsg;   
            }
            if(bRemoved)
            {	
                if(strErrText.CompareNoCase(STRING_FAILED) == 0)
                {	
                    bSuc = TRUE;
                    strErrText = strItemText;
                }
            }
            pListCtrl->SetItemText( nItem, PROG_INDEX, strErrText );

            if(bRemoved)
            {
                psi->nStage = DL_UNPLUGGED_STAGE;	
                pListCtrl->SetItemText( nItem, STATUS_INDEX, StatusString[DL_UNPLUGGED_STAGE]);
            }
        }
    }	

	psi->nLastFile = psi->nCurFile;
	psi->nCurFile = DL_NO_FILE;
    psi->bLastSuccess = bSuc;

	ReportResultToPipe(nPort);

    return TRUE;
}

BOOL CDLoaderView::SetStep( int nPort,  __int64 * pllCurCodeSize)
{
	int nItem = IsValidPort( nPort );
    if( nItem == -1 )
        return FALSE;
    
    CListCtrl* pListCtrl = &GetListCtrl();
	PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pListCtrl->GetItemData( nItem );

	psi->nLastFile = psi->nCurFile;
	psi->nCurFile ++;
    psi->llDownloadedSizeBeforeCurImg += psi->llImgSize;
    psi->llImgSize = *pllCurCodeSize;
    psi->nImgStartTime = psi->nUsedTime;
        
    CString strText;
    if( psi->nCurFile >= m_StepDescription.GetSize() )
    {
        strText = m_StepDescription[m_StepDescription.GetSize()-1];
    }
    else
    {
        strText = m_StepDescription[ psi->nCurFile - 1 ];
    }
    pListCtrl->SetItemText( nItem,STEP_INDEX,strText);
	if (g_theApp.m_bCMDFlash && g_theApp.m_bEZMode)
	{
		CString strInfo;
		strInfo.Format(_T("%s %d%%"),EZ_MODE_STATUS_FLAG,(psi->nCurFile * 100)/m_StepDescription.GetSize(),strText);
		g_theApp.ReportToPipe(strInfo,FALSE);
	}
    return TRUE;    
}

LRESULT CDLoaderView::OnBMCheckBaud( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    UNUSED_ALWAYS( lParam );
    return SetStatus( wParam, DL_CHK_BAUD, FALSE, 0 , 0 );
}

LRESULT CDLoaderView::OnBMConnect( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    UNUSED_ALWAYS( lParam );
    return SetStatus( wParam, DL_CONNECT, FALSE, 0 , 0 );
}

LRESULT CDLoaderView::OnBMEraseFlash( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    UNUSED_ALWAYS( lParam );
    return SetStatus( wParam, DL_ERASE_FLASH, FALSE, 0 , 0 );
}

LRESULT CDLoaderView::OnBMDownLoad( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    return SetStatus( wParam, DL_DL_STAGE,TRUE, 0, lParam  );  
}

LRESULT CDLoaderView::OnBMDLoadProcess( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
//[[ @hongliang.xin 2010-7-21 for PowerManagement	
	CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
    if(pMF->m_bPowerManage && !pMF->m_bPMInDLProcess )
	{   
		int nPort = wParam;
		int nPos = lParam;
		int nItem = IsValidPort( nPort );
		if( -1 != nItem)
		{
			int nLower = 0;
			int nUpper = 0;	
			PORT_STATE_INFO_PTR psi= NULL;
			m_PortMap.Lookup(wParam,psi);
			if(psi && psi->pProg)
			{
				psi->pProg->GetRange(nLower,nUpper);
			}	
			if(nPos == nUpper)
			{
				CListCtrl* pList = &GetListCtrl();
				CString strFileID = pList->GetItemText(nItem,STEP_INDEX);
				if(strFileID.Find(_T("NV")) == 0)
				{
					pMF->PostMessage(WM_POWER_MANAGE,wParam,0);
				}
			}
		}	
	}
//]]

    int nItem = IsValidPort( wParam );
    if( nItem == -1 )
        return FALSE;

    CListCtrl* pListCtrl = &GetListCtrl();
    PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pListCtrl->GetItemData( nItem );
    psi->llCurImgDownloadSize = psi->llImgSize * lParam / 100;
    psi->llDownloadedSize = psi->llCurImgDownloadSize + psi->llDownloadedSizeBeforeCurImg ;
   
    return SetProgPos( wParam, lParam );
}

LRESULT CDLoaderView::OnBMReadFlash( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    return SetStatus( wParam, DL_READ_STAGE,TRUE, 0, lParam  );
}

LRESULT CDLoaderView::OnBMReset( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    UNUSED_ALWAYS( lParam );
    return SetStatus( wParam, DL_RESET_STAGE,FALSE, 0, 0  );
}

LRESULT CDLoaderView::OnBMSetFirstMode( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    UNUSED_ALWAYS( lParam );
    return SetStatus( wParam, DL_SET_FIRST_MODE,FALSE, 0, 0  );
}

LRESULT CDLoaderView::OnBMReadChipType( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    UNUSED_ALWAYS( lParam );
    return SetStatus( wParam, DL_READCHIPTYPE_STAGE ,FALSE, 0, 0  );
}

LRESULT CDLoaderView::OnBMReadNVItem( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    UNUSED_ALWAYS( lParam );
    return SetStatus( wParam, DL_READNVITEM_STAGE,FALSE, 0, 0  );
}

LRESULT CDLoaderView::OnBMChangeBuad( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    UNUSED_ALWAYS( lParam );
    return SetStatus( wParam, DL_CHANGEBUAD_STAGE,FALSE, 0, 0  );
}

LRESULT CDLoaderView::OnBMReadFlashProcess( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    return SetProgPos( wParam, lParam );
}

LRESULT CDLoaderView::OnBMBegin( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    UNUSED_ALWAYS( lParam ); 
	PORT_STATE_INFO_PTR psi= NULL;
	m_PortMap.Lookup(wParam,psi);
	if(psi)
	{
		psi->nLastFile = psi->nCurFile;
		psi->nCurFile = DL_NO_FILE;
        psi->llImgSize = 0;
        psi->llCurImgDownloadSize = 0;
        psi->llDownloadedSize = 0;
        psi->llDownloadedSizeBeforeCurImg = 0;
        psi->nImgStartTime = 0;
        psi->fPeakRate = 0;
	}
    return SetStatus( wParam, DL_NONE_STAGE, FALSE, 0 ,0 );
}


LRESULT CDLoaderView::OnBMFileBegin( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
    return SetStep( wParam, (__int64 *)lParam);
}

LRESULT CDLoaderView::OnBMEnd( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
   return SetResult( wParam, NULL, (LPVOID)lParam );
}

LRESULT CDLoaderView::OnBMUnTransCode( WPARAM wParam /* = 0 */, LPARAM lParam /* = 0  */ )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	if ( g_theApp.m_bCMDFlash )
	{
		CString strInfo;
		strInfo.Format(_T("%s tanscode=false"),EZ_MODE_REPORT_FLAG);
		g_theApp.ReportToPipe(strInfo, FALSE);
	}
	return TRUE;
}

#ifndef _SPUPGRADE
LRESULT CDLoaderView::OnAcquireBarcode(WPARAM wParam,LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	m_arrReadyPorts.Add( wParam );

	if(m_bSNDlgShow) 
    {
        // There is already a port waiting for barcode,
        return 0;
    }

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
    
    for(;;)
    {
        int nCount = m_arrReadyPorts.GetSize();
        if(0 == nCount)
        {
            // No port is ready
            break;
        }
        
        m_bSNDlgShow = TRUE;
        
        DWORD dwPort = m_arrReadyPorts[0];
        m_arrReadyPorts.RemoveAt( 0 );

		PORT_STATE_INFO_PTR psi = NULL;
		m_PortMap.Lookup(wParam,psi);
		int nIndex = -1;
		if(psi)
			nIndex = psi->nIndex;
        
        CBarcodeDlg dlg(dwPort);
		m_pCurBarcodeDlg = &dlg;
		dlg.SetSNInputMethod(pFrame->m_sheetSettings.IsAutoGenSN());
		UINT nRet;
		LPCSTR pSN =NULL;	   
		nRet = dlg.DoModal();
		USES_CONVERSION;
		pSN= T2CA(dlg.m_strBarcode);
     
        if( IDOK == nRet )
        {	
            int nSize = PRODUCTION_INFO_SIZE;
			ASSERT(NULL != pSN);
			//PORT_DATA * pPortData = reinterpret_cast<PORT_DATA*>(lParam);
			// not use lParam
			// if while loop run twice the lParam is not changed
			// so must re-get the port information 
			
			PORT_DATA* pPortData = pFrame->GetPortDataByPort(dwPort); 
            if (NULL == pPortData->lpPhaseCheck)
            {
                pPortData->lpPhaseCheck = new BYTE[nSize];
            }	
            if (NULL == pPortData->lpPhaseCheck)
            {
                SetEvent(pPortData->hSNEvent);
                return S_FALSE;
            }
			memset(pPortData->lpPhaseCheck,0xFF,nSize);
			CPhaseCheckBuild ccb;
			BOOL bOK = ccb.Cnst8KBuffer(pSN, pPortData->lpPhaseCheck, nSize);
			if(bOK)
			{	
				strcpy(pPortData->szSN,pSN);			
				SetEvent(pPortData->hSNEvent); 			
			}
			else
			{
				CString strError = _T("Construct phasecheck information fail.");
				OnBMEnd((WPARAM)dwPort,(LPARAM)(LPCTSTR)strError);
				//set this flag,let the port wait
				nRet =  IDIGNORE;
			}
        }
        if ( IDIGNORE == nRet)
        {
			if(nIndex != -1)
			{
				if(psi)
				{
					if(psi->pButton)
					{
						psi->pButton->ShowWindow(SW_SHOW);
						psi->pButton->EnableWindow(TRUE);
					}			
				}
			}
        }

		m_pCurBarcodeDlg = NULL;        
        m_bSNDlgShow = FALSE;

		//this is for manual download for DOWNLOAD and SPUPGRADE
		//to judge if the user select the IGNORE button.
		if( g_theApp.m_bManual && IDIGNORE == nRet )
		{
			return S_FALSE;
		}
    }
	return S_OK;
}

#endif 

void CDLoaderView::ClearPortInfo(int nPort)
{
#ifndef _SPUPGRADE
	//Close the SN Dialog
	if(NULL != m_pCurBarcodeDlg)
	{
		if( m_pCurBarcodeDlg->GetPortNum() == nPort )
		{
			m_pCurBarcodeDlg->Close();
		}
	}
	
	//Remove the port from the SN dlg array;
	for(int i=0; i< m_arrReadyPorts.GetSize(); i++)
	{
		if( m_arrReadyPorts[i] == (DWORD)nPort)
		{
			m_arrReadyPorts.RemoveAt(i);
			break;
		}
	}
#endif

	PORT_STATE_INFO_PTR psi = NULL;
	m_PortMap.Lookup(nPort, psi);
	if(psi && psi->pButton)
	{
		psi->pButton->ShowWindow(SW_HIDE);
		psi->pButton->EnableWindow(FALSE);
	}
}

void CDLoaderView::ResizeButton()
{
	CListCtrl* pList = &GetListCtrl();
	int nCount = pList->GetItemCount();
    for(int i=0; i<nCount;i++)
    {
		PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData(i);
		if(psi && psi->pButton)
		{
			CRect rc;
			pList->GetSubItemRect(i, BUTTON_POS, LVIR_BOUNDS, rc);
			rc.right = rc.left + START_ICON_SIZE; 
			rc.top     = rc.top + (rc.Height() - START_ICON_SIZE)/2;
			rc.bottom  = rc.top + START_ICON_SIZE;
			
			psi->pButton->MoveWindow(rc);
			psi->pButton->Invalidate();
		}
    }
}

void CDLoaderView::OnBtnStart(UINT nID)
{
	CListCtrl* pList = &GetListCtrl();
	int nCount = pList->GetItemCount();
	int nIndex = nID - IDC_BTN_START1;
	int nPort = -1;
    
    if(nCount <= nIndex)
    {
        return;
    }

	PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData(nIndex);
    if(psi )
	{
		nPort = psi->nPort;
		if(psi->pButton)
		{	
			psi->pButton->ShowWindow(SW_HIDE);
			psi->pButton->EnableWindow(FALSE);
		}	
	}
	else
	{
		return;
	}
	
	
    
   CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();   
#ifndef _SPUPGRADE
   if(pFrame->m_bNeedPhaseCheck)
   {
	   PORT_DATA* pPortData = pFrame->GetPortDataByPort(nPort); 
	   HRESULT hr = OnAcquireBarcode(nPort, (LPARAM)pPortData);
	   if(hr==S_FALSE)
	   {
		   return;
	   }
   }
#endif 

   if(g_theApp.m_bManual)
   {
	   if(!pFrame->StartOnePortWork(nPort,FALSE))
	   {
		   if(psi->pButton)
		   {	
			   psi->pButton->ShowWindow(SW_SHOW);
			   psi->pButton->EnableWindow(TRUE);
		   }		 
	   }
   }


}

void CDLoaderView::OnBeginTrack(NMHDR* pNMHDR, LRESULT* pResult)
{	
	UNREFERENCED_PARAMETER(pNMHDR);
	CListCtrl* pList = &GetListCtrl();
	int nCount = pList->GetItemCount();
    for(int i=0; i<nCount;i++)
    {
		PORT_STATE_INFO_PTR psi = (PORT_STATE_INFO_PTR)pList->GetItemData(i);
		if(psi && psi->pButton)
		{
			psi->pButton->ShowWindow(SW_HIDE); 
		}
	}
	
	TRACE(_T("On Begin Track!\n"));
	*pResult = 0;	
}

void CDLoaderView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(DL_TIMER == nIDEvent)
	{
		CListCtrl &lc = GetListCtrl();	
		for(int i = 0; i< lc.GetItemCount(); i++)
		{
			PORT_STATE_INFO_PTR psi  = (PORT_STATE_INFO_PTR)lc.GetItemData(i);
			CString strText = lc.GetItemText(i,TIME_INDEX);
			if(!strText.IsEmpty() && strText.Find(_T("s"))==-1)
			{
				strText.Format(_T("%d"),_ttoi(strText)+1);
				lc.SetItemText(i,TIME_INDEX,strText);
				if(psi)
				{
					psi->nUsedTime = _ttoi(strText);
				}

                
                double dDownloadedSize = (double)(psi->llDownloadedSize/(1024*1024));
                float fAvgSpeed = (float)(dDownloadedSize/psi->nUsedTime);
                double dCurImgDownloadedSize = (double)(psi->llCurImgDownloadSize/(1024*1024));
                if((psi->nUsedTime - psi->nImgStartTime) == 0)
                {
                    return;
                }
                float fPeakSpeed = (float)(dCurImgDownloadedSize/(psi->nUsedTime - psi->nImgStartTime));
                if(fPeakSpeed > (psi->fPeakRate))
                {
                    psi->fPeakRate = fPeakSpeed;
                }
                CString strRateFormat;
                strRateFormat.Format(_T("Avg:%0.2f, Peak:%0.2f"), fAvgSpeed, psi->fPeakRate);
                SetRate(psi->nPort, strRateFormat.operator LPCTSTR());
             
			}
            else if (!strText.IsEmpty() && strText.Find(_T("s")) !=-1)
            {
                if (psi->nEnumPortStartTime)// check second enum port timeout
                {
                    int nEndTime = GetTickCount();
                    if (nEndTime- psi->nEnumPortStartTime > g_theApp.m_nEnumPortTimeOut)
                    {
                        CString strErrText = STRING_FAILED;
                        strErrText += _T(": ");  
                        strErrText += g_theApp.m_strEnumPortErrorMsg; 
                        lc.SetItemText( i, PROG_INDEX,  strErrText);
                        lc.SetItemText( i, STATUS_INDEX,StatusString[DL_FINISH_STAGE]);
                        psi->nEnumPortStartTime = 0;

                        DWORD dwPort = _ttoi(lc.GetItemText(i,0).operator LPCTSTR());
                        ReportResultToPipe(dwPort);
                        CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
                        pMF->PostMessage(WM_STOP_AUTODLOADER,dwPort,NULL);
                    }
                }
            }
		}		
		return;
	}
	CListView::OnTimer(nIDEvent);
}

void CDLoaderView::StartDLTimer()
{
	::SetTimer(this->m_hWnd,DL_TIMER,1000,NULL);
}
void CDLoaderView::StopDLTimer()
{
	::KillTimer(this->m_hWnd,DL_TIMER);
}

void CDLoaderView::SetMcpType(int nPort, LPCTSTR lpszMcpType)
{
	int nItem = IsValidPort(nPort);
	if(nItem == -1)
		return;

	CListCtrl &lc = GetListCtrl();
	lc.SetItemText(nItem,MCPTYPE_INDEX,lpszMcpType);
}

void CDLoaderView::SetIMEI(int nPort, LPCTSTR lpszIMEI)
{
	int nItem = IsValidPort(nPort);
	if(nItem == -1)
		return;

	CListCtrl &lc = GetListCtrl();
	lc.SetItemText(nItem,IMEI_INDEX,lpszIMEI);
}

void CDLoaderView::SetRate(int nPort, LPCTSTR lpszRate)
{
    int nItem = IsValidPort(nPort);
    if(nItem == -1)
        return;

    CListCtrl &lc = GetListCtrl();
    CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
    if(pMF->m_bShowRate)
    {
        lc.SetItemText(nItem,RATE_INDEX,lpszRate);
    }
    
}

void CDLoaderView::StopOnePortTimer(int nItem)
{
	CListCtrl* pListCtrl = &GetListCtrl();
	CString strTime = pListCtrl->GetItemText(nItem,TIME_INDEX);
	if(!strTime.IsEmpty())
	{
		// "s" is the stop flag, you can see OnTimer function of this view
		if(strTime.Find(_T("s")) == -1)
		{
			strTime += _T("s");
		}
		pListCtrl->SetItemText( nItem, TIME_INDEX, strTime);
	}
}

BOOL CDLoaderView::GetInfo(int nPort, STATE_INFO_T &info)
{
	PORT_STATE_INFO_PTR psi  = NULL;
	m_PortMap.Lookup(nPort, psi);
	if(psi == NULL)
		return FALSE;

	_stprintf(info.szUsedTime,_T("%ds"),psi->nUsedTime);

 	int nCurFile = psi->nLastFile;
	if(DL_NO_FILE != nCurFile && psi->nCurFile == DL_NO_FILE)
	{
		nCurFile--;
	}
 
 	if( nCurFile < m_StepDescription.GetSize() && nCurFile>=0 )
 	{
 		_tcsncpy(info.szCurFile,m_StepDescription[nCurFile],MAX_INFO_LEN);
 	}
	return TRUE;
}

void CDLoaderView::OnHdnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    // TODO: Add your control notification handler code here
    UNUSED_ALWAYS( phdr );
    ResizeProg();	
    ResizeButton();
    Invalidate();
    *pResult = 0;
}
void CDLoaderView::ReportProcessToPipe(CProgressCtrlST* pProg)
{
    if (g_theApp.m_bCMDFlash && pProg)
    {
        int nLower = 0;
        int nUpper = 0;
        int nPos   = pProg->GetPos();
        int nProcess = nPos;
        pProg->GetRange(nLower,nUpper);
        if ( 0 != nUpper )
        {
            nProcess = (int)( ( (LONGLONG)nPos)*100/(LONGLONG)nUpper);
            CString strInfo;
            strInfo.Format(_T("%3d%%"),nProcess);	
            g_theApp.ReportToPipe(strInfo,FALSE);
        }

    }

}
void CDLoaderView::ReportResultToPipe(int nPort)
{  
    static BOOL bReported = FALSE;
    if ( g_theApp.m_bCMDFlash && !bReported )
    {
        CListCtrl* pListCtrl = &(GetListCtrl());
        int nItem = IsValidPort( nPort );
        if( nItem == -1 || NULL == pListCtrl )
            return ;

        CString strResult(pListCtrl->GetItemText(nItem,PROG_INDEX));

        if ( -1 != strResult.Find(STRING_FAILED) || -1 != strResult.Find(STRING_PASS))
        {
            BOOL bFailed = ( 0 == strResult.Find(STRING_FAILED) ) ? TRUE : FALSE;
            CString strInfo;
			CString strElapsedTime = pListCtrl->GetItemText(nItem,TIME_INDEX);
			int nSeconds = _ttoi(strElapsedTime.operator LPCTSTR());
			CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
			double dPacSize = (double)GetFileSizeEx(pMF->m_strPacketPath.operator LPCTSTR())/(1024*1024);

			if (g_theApp.m_bEZMode)
			{
				strInfo.Format(_T("%s product=%s, swversion=%s, port=COM%d"),EZ_MODE_REPORT_FLAG,pMF->m_sheetSettings.GetCurProduct(),pMF->m_sheetSettings.GetCurPacVersion(),nPort);
				g_theApp.ReportToPipe(strInfo);
				if (bFailed)
				{
					if (!strElapsedTime.IsEmpty())
					{
						strInfo.Format(_T("%s time=%s"),EZ_MODE_REPORT_FLAG,strElapsedTime);
						g_theApp.ReportToPipe(strInfo);
					}
					
                    strInfo.Format(_T("%s fail:%s<%s>"),EZ_MODE_STATUS_FLAG,pListCtrl->GetItemText(nItem,PROG_INDEX),pListCtrl->GetItemText(nItem,STEP_INDEX));
					g_theApp.ReportToPipe(strInfo,bFailed);

				}
				else
				{
					
					strInfo.Format(_T("%s time=%s"),EZ_MODE_REPORT_FLAG,strElapsedTime);
					g_theApp.ReportToPipe(strInfo,bFailed);
					if (nSeconds)
					{
						_BMOBJ * pbj				= NULL;
						if(pMF->m_mapBMObj.Lookup(nPort,pbj) && pbj != NULL )
						{
							dPacSize = (double)(pbj->llCodeSize/(1024*1024));
						}
						float fSpeed = (float)(dPacSize/nSeconds);
						strInfo.Format(_T("%s speed=%.2fM/s"),EZ_MODE_REPORT_FLAG,fSpeed);
						g_theApp.ReportToPipe(strInfo,bFailed);
					}
					
					strInfo.Format(_T("%s pass"),EZ_MODE_STATUS_FLAG);
					g_theApp.ReportToPipe(strInfo,bFailed);
				}
			}
			else
			{
				strInfo.Format(_T("DownLoad %s, Elapsed Times = %s"),pListCtrl->GetItemText(nItem,PROG_INDEX),strElapsedTime);
				g_theApp.ReportToPipe(strInfo,bFailed);
			}
            if (g_theApp.m_dwMultiCount == 1 )
            {
				bReported = TRUE;
            }
            
        }
    }
}
