// SettingsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "SettingsSheet.h"
#include "MainFrm.h"
#include <afxpriv.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_RESIZEPAGE (WM_APP+1)

enum { CDF_CENTER, CDF_TOPLEFT, CDF_NONE };

BOOL g_bInitSheet = FALSE;

static void ChangeDialogFont(CWnd* pWnd, CFont* pFont, int nFlag)
{
    CRect windowRect;
    
    // grab old and new text metrics
    TEXTMETRIC tmOld, tmNew;
    CDC * pDC = pWnd->GetDC();
    CFont * pSavedFont = pDC->SelectObject(pWnd->GetFont());
    pDC->GetTextMetrics(&tmOld);
    pDC->SelectObject(pFont);
    pDC->GetTextMetrics(&tmNew);
    pDC->SelectObject(pSavedFont);
    pWnd->ReleaseDC(pDC);
    
    long oldHeight = tmOld.tmHeight+tmOld.tmExternalLeading;
    long newHeight = tmNew.tmHeight+tmNew.tmExternalLeading;
    
    if (nFlag != CDF_NONE)
    {
        // calculate new dialog window rectangle
        CRect clientRect, newClientRect, newWindowRect;
        
        pWnd->GetWindowRect(windowRect);
        pWnd->GetClientRect(clientRect);
        long xDiff = windowRect.Width() - clientRect.Width();
        long yDiff = windowRect.Height() - clientRect.Height();
        
        newClientRect.left = newClientRect.top = 0;
        newClientRect.right = clientRect.right * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
        newClientRect.bottom = clientRect.bottom * newHeight / oldHeight;
        
        if (nFlag == CDF_TOPLEFT) // resize with origin at top/left of window
        {
            newWindowRect.left = windowRect.left;
            newWindowRect.top = windowRect.top;
            newWindowRect.right = windowRect.left + newClientRect.right + xDiff;
            newWindowRect.bottom = windowRect.top + newClientRect.bottom + yDiff;
        }
        else if (nFlag == CDF_CENTER) // resize with origin at center of window
        {
            newWindowRect.left = windowRect.left - 
                (newClientRect.right - clientRect.right)/2;
            newWindowRect.top = windowRect.top -
                (newClientRect.bottom - clientRect.bottom)/2;
            newWindowRect.right = newWindowRect.left + newClientRect.right + xDiff;
            newWindowRect.bottom = newWindowRect.top + newClientRect.bottom + yDiff;
        }
        pWnd->MoveWindow(newWindowRect);
    }
    
    pWnd->SetFont(pFont);
    
    // iterate through and move all child windows and change their font.
    CWnd* pChildWnd = pWnd->GetWindow(GW_CHILD);
    
    while (pChildWnd)
    {
        pChildWnd->SetFont(pFont);
        pChildWnd->GetWindowRect(windowRect);
        
        CString strClass;
        ::GetClassName(pChildWnd->m_hWnd, strClass.GetBufferSetLength(32), 31);
        strClass.MakeUpper();
        if(strClass==_T("COMBOBOX"))
        {
            CRect rect;
            pChildWnd->SendMessage(CB_GETDROPPEDCONTROLRECT,0,(LPARAM) &rect);
            windowRect.right = rect.right;
            windowRect.bottom = rect.bottom;
        }
        
        pWnd->ScreenToClient(windowRect);
        windowRect.left = windowRect.left * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
        windowRect.right = windowRect.right * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
        windowRect.top = windowRect.top * newHeight / oldHeight;
        windowRect.bottom = windowRect.bottom * newHeight / oldHeight;
        pChildWnd->MoveWindow(windowRect);
        
        pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
    }
}
/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet

IMPLEMENT_DYNAMIC(CSettingsSheet, CPropertySheet)

CSettingsSheet::CSettingsSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	memset(&m_rctPage,0,sizeof(m_rctPage));
	m_pageMain.SetCaliPage(&m_pageCalibration);
	m_pageMain.SetOptionsPage(&m_pageOptions);
	m_pageMain.SetMultiLangPage(&m_pageMultiLang);
	m_pageMain.SetLCDCfigPage(&m_pageLCDCfig);
	m_pageMain.SetFlashOptPage(&m_pageFlashOpt);
    m_pageMain.SetCustomizationPage(&m_pageCustomization);
	
	AddPage(&m_pageMain);
	AddPage(&m_pageOptions );
#if defined(_SPUPGRADE) || defined(_FACTORY)
	if(g_theApp.m_bShowOtherPage)
	{		
		AddPage( &m_pageCalibration );
	}
#else
	/* @hongliang.xin 2009-6-29 must show other page for DloaderR, 
	   otherwise, the Calibration Page will assert for debug */
	AddPage( &m_pageCalibration );
	AddPage( &m_pageFlashOpt );
#endif

	AddPage(&m_pageMultiLang);
	AddPage(&m_pageLCDCfig);
#if defined(_SPUPGRADE) || defined(_FACTORY)
	if(g_theApp.m_bShowMcpTypePage)
	{
		AddPage(&m_pageMcp);
	}
#else
	AddPage(&m_pageMcp);
	AddPage(&m_pageVolFreq);
    AddPage(&m_pageUartPortSwitch);

	m_pageMain.SetVolFreqPage(&m_pageVolFreq);
    m_pageMain.SetUartPortSwitchPage(&m_pageUartPortSwitch);

#endif

#if defined(_SPUPGRADE)	|| defined(_FACTORY)
    AddPage(&m_pageCustomization);
#endif


//  AddPage(&m_pageMode);	

}

CSettingsSheet::CSettingsSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	memset(&m_rctPage,0,sizeof(m_rctPage));
	m_pageMain.SetCaliPage(&m_pageCalibration);
	m_pageMain.SetOptionsPage(&m_pageOptions);
	m_pageMain.SetMultiLangPage(&m_pageMultiLang);
	m_pageMain.SetLCDCfigPage(&m_pageLCDCfig);
	AddPage(&m_pageMain);

#if defined(_SPUPGRADE) || defined(_FACTORY)
	if(g_theApp.m_bShowOtherPage)
	{
		AddPage( &m_pageOptions );
		AddPage( &m_pageCalibration );
	}
#else
	/* @hongliang.xin 2009-6-29 must show other page for DloaderR, 
	   otherwise, the Calibration Page will assert for debug */
	AddPage( &m_pageOptions );
	AddPage( &m_pageCalibration );
#endif

	AddPage(&m_pageMultiLang);
	AddPage(&m_pageLCDCfig);

#if defined(_SPUPGRADE) || defined(_FACTORY)
	if(g_theApp.m_bShowMcpTypePage)
	{
		AddPage(&m_pageMcp);
	}
#else
	AddPage(&m_pageMcp);
	AddPage(&m_pageVolFreq);
    AddPage(&m_pageUartPortSwitch);
	m_pageMain.SetVolFreqPage(&m_pageVolFreq);
    m_pageMain.SetUartPortSwitchPage(&m_pageUartPortSwitch);
#endif
}

CSettingsSheet::~CSettingsSheet()
{
/*lint -save -e1551*/
    if (m_fntPage.m_hObject)
        VERIFY (m_fntPage.DeleteObject());
/*lint -restore */
}


BEGIN_MESSAGE_MAP(CSettingsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSettingsSheet)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_RESIZEPAGE, OnResizePage)
	ON_BN_CLICKED(ID_APPLY_NOW, OnApply)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet message handlers
BOOL CSettingsSheet::LoadSettings(LPCTSTR pFileName,CString &strErrorMsg)
{
#if  defined(_SPUPGRADE) || defined(_FACTORY)
    return 
        (
		 m_pageMain.LoadConfig(pFileName,strErrorMsg) &&         // Load Config
		 m_pageOptions.LoadSettings( pFileName,strErrorMsg )&&
		 m_pageCalibration.LoadSettings(pFileName,strErrorMsg) &&
		 m_pageMultiLang.LoadSettings(pFileName,strErrorMsg) &&
		 m_pageLCDCfig.LoadSettings(pFileName,strErrorMsg) &&
		 m_pageFlashOpt.LoadSettings(pFileName,strErrorMsg) &&
		 m_pageMcp.LoadSettings(pFileName,strErrorMsg) &&
         m_pageCustomization.LoadSettings(pFileName, strErrorMsg)
		 );
#else
    return 
        (
		 m_pageMain.LoadSettings(pFileName,strErrorMsg) &&       // Load Settings
		 m_pageOptions.LoadSettings( pFileName,strErrorMsg )&&
		 m_pageCalibration.LoadSettings(pFileName,strErrorMsg) &&
		 m_pageMultiLang.LoadSettings(pFileName,strErrorMsg) &&
		 m_pageLCDCfig.LoadSettings(pFileName,strErrorMsg) &&
		 m_pageFlashOpt.LoadSettings(pFileName,strErrorMsg) &&
		 m_pageMcp.LoadSettings(pFileName,strErrorMsg)&&
		 m_pageVolFreq.LoadSettings(pFileName,strErrorMsg)&&
         m_pageUartPortSwitch.LoadSettings(pFileName, strErrorMsg)&&
         m_pageCustomization.LoadSettings(pFileName, strErrorMsg)
        );
#endif
}

int CSettingsSheet::GetBaudRate()
{
    return m_pageMain.m_nBaudRate;
}

int CSettingsSheet::GetComPort()
{
    return m_pageMain.m_nComPort;
}

int CSettingsSheet::GetDownloadFile( CStringArray& aryDownloadFile  )
{
	return m_pageMain.GetDLFiles(aryDownloadFile);
}

BOOL CSettingsSheet::CheckCrcDLFiles(CString& strCrcFailFileId)
{
    return m_pageMain.CheckCrcDLFiles(strCrcFailFileId);
}

void CSettingsSheet::BuildPropPageArray()
{
    CPropertySheet::BuildPropPageArray();
    
    // get first page
    CPropertyPage* pPage = GetPage(0);
    ASSERT (pPage);
	if(pPage==NULL)
	{
		return;
	}
    
    // dialog template class in afxpriv.h
    CDialogTemplate dlgtemp;
    // load the dialog template
    VERIFY (dlgtemp.Load(pPage->m_psp.pszTemplate));
    // get the font information
    CString strFace;
    WORD	wSize;
    VERIFY (dlgtemp.GetFont (strFace, wSize));
    if (m_fntPage.m_hObject)
        VERIFY (m_fntPage.DeleteObject());
    // create a font using the info from first page
    VERIFY (m_fntPage.CreatePointFont(wSize*10, strFace));
}


BOOL CSettingsSheet::OnInitDialog() 
{
    CPropertySheet::OnInitDialog();


	CWnd *pWnd = GetDlgItem(IDOK);
	if(pWnd != NULL)
	{
		pWnd->SetWindowText(_T("OK"));
	}

	pWnd = GetDlgItem(IDCANCEL);
	if(pWnd != NULL)
	{
		pWnd->SetWindowText(_T("Cancel"));
	}
	
	pWnd = GetDlgItem(ID_APPLY_NOW);
	if(pWnd != NULL)
	{
#if defined(_SPUPGRADE) || defined(_FACTORY)
    	pWnd->SetWindowText(_T("Apply"));				
#else
		pWnd->SetWindowText(_T("Packet"));
		pWnd->EnableWindow(TRUE);
#endif
	}	
    
    g_bInitSheet = TRUE;
    // get the font for the first active page
    CPropertyPage* pCurActPage = GetActivePage ();
    ASSERT (pCurActPage);
    
    // change the font for the sheet
    ChangeDialogFont (this, &m_fntPage, CDF_CENTER);

    // change the font for each page
    for (int iCntr = 0; iCntr < GetPageCount (); iCntr++)
    {
        VERIFY (SetActivePage (iCntr));
        CPropertyPage *pPage = GetActivePage ();
        ASSERT (pPage);
        ChangeDialogFont (pPage, &m_fntPage, CDF_CENTER);
    }
   
    VERIFY (SetActivePage (pCurActPage));
    
    Resize( 0, 0 );
    g_bInitSheet = FALSE;


 	if(g_theApp.m_bCMDPackage || g_theApp.m_bCMDFlash)
 	{
 		SetWindowPos(&wndTop,0,0,0,0,NULL);
		CenterWindow();
 	}

    return TRUE;
}


BOOL CSettingsSheet::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    NMHDR* pnmh = (LPNMHDR) lParam;
    
    // the sheet resizes the page whenever it is activated so we need to size it correctly
    if (TCN_SELCHANGE == pnmh->code) //lint !e648
        PostMessage (WM_RESIZEPAGE);
    
    return CPropertySheet::OnNotify(wParam, lParam, pResult);
}

LONG CSettingsSheet::OnResizePage (UINT, LONG)
{
    // resize the page
    CPropertyPage* pPage = GetActivePage();
    if(pPage != NULL)
	{
		pPage->MoveWindow (&m_rctPage);
	}
    
    return 0;
}

BOOL CSettingsSheet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    // the sheet resizes the page whenever the Apply button is clicked so we need to size it correctly
    if (ID_APPLY_NOW == wParam ||
        ID_WIZNEXT == wParam ||
        ID_WIZBACK == wParam)
        PostMessage (WM_RESIZEPAGE);
    
    return CPropertySheet::OnCommand(wParam, lParam);
}

void CSettingsSheet::Resize( int nChangeWidth, int nChangeHeight  )
{
    RECT rc;
    
    // resize the sheet
    //@ Liu Kai 2004-11-19
    // Avoid flickering
    GetWindowRect (&rc);
    rc.left -= nChangeWidth / 2;
    rc.top -= nChangeHeight / 2;
    rc.right += nChangeWidth / 2;
    rc.bottom += nChangeHeight / 2;

    MoveWindow (&rc);
    
    // resize the CTabCtrl
    CTabCtrl* pTab = GetTabControl ();
    ASSERT (pTab);
	if(pTab != NULL)
	{
		pTab->GetWindowRect (&rc);
		ScreenToClient (&rc);
		rc.right += nChangeWidth;
		rc.bottom += nChangeHeight;
		pTab->MoveWindow (&rc);
	}
    
    // resize the page
    CPropertyPage* pPage = GetActivePage ();
    ASSERT (pPage);
	if(pPage != NULL)
	{
		// store page size in m_PageRect
		pPage->GetWindowRect (&m_rctPage);
		ScreenToClient (&m_rctPage);
		m_rctPage.right += nChangeWidth;
		m_rctPage.bottom += nChangeHeight;
		pPage->MoveWindow (&m_rctPage);
	}
    
    // move the OK, Cancel, and Apply buttons
    CWnd* pWnd = GetDlgItem(IDOK);
	if(pWnd != NULL)
	{
		pWnd->GetWindowRect(&rc);
		rc.bottom += nChangeHeight;
		rc.top += nChangeHeight;
		ScreenToClient(&rc);
		pWnd->MoveWindow(&rc);
	}
    
    pWnd = GetDlgItem(IDCANCEL);
	if(pWnd != NULL)
	{
		pWnd->GetWindowRect(&rc);
		rc.bottom += nChangeHeight;
		rc.top += nChangeHeight;
		ScreenToClient(&rc);
		pWnd->MoveWindow(&rc);
	}
    
    pWnd = GetDlgItem(ID_APPLY_NOW);
	if(pWnd != NULL)
	{
		pWnd->GetWindowRect(&rc);
		rc.bottom += nChangeHeight;
		rc.top += nChangeHeight;
		ScreenToClient(&rc);
		pWnd->MoveWindow(&rc);
	}
}

int CSettingsSheet::GetRepartitionFlag()
{
	return m_pageOptions.GetNandRepartStrategy();
}

int CSettingsSheet::GetFlashPageType()
{ 
	return m_pageOptions.GetFlashPageType();
}

BOOL CSettingsSheet::IsReset()
{
	return m_pageOptions.m_bReset;
}

BOOL CSettingsSheet::IsPowerOff()
{
	return m_pageOptions.m_bPowerOff;
}

BOOL CSettingsSheet::IsNandFlash()
{
	if(m_pageMain.m_pCurProductInfo != NULL &&
	   m_pageMain.m_pCurProductInfo->dwFlashType)
	{
		return TRUE;
	}
    return FALSE;
}

BOOL  CSettingsSheet::IsNvBaseChange()
{
	if( m_pageMain.m_pCurProductInfo != NULL &&
		m_pageMain.m_pCurProductInfo->dwNvBaseChangeFlag == 0)
		return FALSE;
	else
		return TRUE;
}

int  CSettingsSheet::GetNvNewBasePosition()
{
	return m_pageMain.m_pCurProductInfo->dwNvNewBasePosition;
}


BOOL CSettingsSheet::IsMainPageInit()
{
	return m_pageMain.m_bInit;
}

int CSettingsSheet::GetFileInfo(LPCTSTR lpszFileID, LPDWORD ppFileInfo)
{
	return m_pageMain.GetFileInfo(lpszFileID,ppFileInfo);
}

int CSettingsSheet::GetAllFileInfo(LPDWORD ppFileInfo)
{
	return m_pageMain.GetAllFileInfo(ppFileInfo);
}

CString CSettingsSheet::GetDownloadFilePathByPartitionName(LPCTSTR lpszPartitionName)
{
	if(
		m_pageMain.m_pCurProductInfo == NULL				||
		m_pageMain.m_pCurProductInfo->dwFileCount == 0		||
		m_pageMain.m_pCurProductInfo->pFileInfoArr == NULL
		)
	{
		return _T("");
	}

	CString strFile;
	for(unsigned int i = 0; i<m_pageMain.m_pCurProductInfo->dwFileCount; ++i )
	{
		FILE_INFO_T* pItem = m_pageMain.m_pCurProductInfo->pFileInfoArr + i;
		if ( _tcsicmp(pItem->arrBlock[0].szRepID,lpszPartitionName) == 0)
		{
			strFile = GetDownloadFilePath(pItem->szID,FALSE);
			break;
		}
	}

	return strFile;
}

CString CSettingsSheet::GetDownloadFilePath(LPCTSTR lpszFileID,BOOL bCheckSel/* = TRUE*/)
{
	CString strPath;
	BOOL    bCheck = FALSE;
	CString strID = lpszFileID;
	strID.MakeUpper();

    m_pageMain.m_mapDLFiles.Lookup(strID,strPath);
    if (bCheckSel)
    {
        m_pageMain.m_mapDLState.Lookup(strID,bCheck);
        if(bCheck)
        {
            return strPath;
        }
        else
        {
            return _T("");
        }
    }
    else
    {
        return strPath;
    }

}

BOOL CSettingsSheet::IsBackupNV(LPCTSTR lpszNVID/* = NULL*/)
{
	CStringArray agID;
	int nCount = m_pageMain.GetDLNVID(agID);
	if(nCount == 0)
		return FALSE;	

    if(NULL == lpszNVID)
    {
        return ( m_pageCalibration.m_lstBackup.m_bBackNV || IsBackupLang() );
    }
    else
    {
        return ( m_pageCalibration.m_lstBackup.IsBackupNV(lpszNVID) || IsBackupLang() );
    }	
}

int CSettingsSheet::GetNvBkpItemCount(LPCTSTR lpNvID)
{
    return m_pageCalibration.m_lstBackup.GetNvBkpItemCount(lpNvID);
}

PNV_BACKUP_ITEM_T CSettingsSheet::GetNvBkpItemInfo(int nIndex,LPCTSTR lpNvID)
{
	int nUper = GetNvBkpItemCount(lpNvID);
	ASSERT(nIndex>=0 && nIndex<nUper );
	
    return m_pageCalibration.m_lstBackup.GetNvBkpItemInfo(nIndex, lpNvID);
}

BOOL CSettingsSheet::LoadPacket(LPCTSTR pFileName)
{
	return m_pageMain.LoadPacket(pFileName);
}


BOOL CSettingsSheet::IsReadFlashInFDL2()
{
	if(!IsNandFlash())
		return FALSE;

	CStringArray agID;

	if(!IsBackupNV() && GetBackupFiles(agID)== 0)
		return FALSE;

	if(!m_pageOptions.m_bRepart)
	{
		return FALSE;
	}

	return TRUE;
}

void CSettingsSheet::OnApply()
{
	if(!g_theApp.m_bCMDPackage)
	{
		if(AfxMessageBox(_T("Do packet operation right now?"),MB_YESNO) == IDNO)
			return;
	}
	MSG msg;
	while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	CWaitCursor wait;
	m_pageMain.DoPacket();
}

BOOL    CSettingsSheet::IsNVSaveToLocal()
{
	return m_pageCalibration.m_bSaveToLocal;
}

CString CSettingsSheet::GetNVSavePath()
{
	return m_pageCalibration.m_strLocalPath;
}

BOOL CSettingsSheet::IsBackupLang()
{
	return m_pageMultiLang.m_bBackupLang;
}
WORD CSettingsSheet::GetLangNVItemID()
{
	return m_pageMain.m_wLangNVItemID;
}

BOOL CSettingsSheet::IsHasLang()
{
	return m_pageMultiLang.m_bRightNV;
}

BOOL CSettingsSheet::IsNVOrgDownload()
{
	if(m_pageMain.m_pCurProductInfo->dwNVOrgFlag != 0 && IsOmaDM())
		return TRUE;
	else
		return FALSE;
}

int CSettingsSheet::GetNVOrgBasePosition()
{
	return m_pageMain.m_pCurProductInfo->dwNVOrgBasePosition;
}

BOOL CSettingsSheet::IsOmaDM()
{
	return m_pageMain.m_bOmaDM;
}


int CSettingsSheet::GetFlashOprFileInfo(CUIntArray &agFlashOpr)
{
	agFlashOpr.RemoveAll();
	if(m_pageFlashOpt.m_agFlashOpr.GetSize()>0)
	{
	   agFlashOpr.Copy(m_pageFlashOpt.m_agFlashOpr);
	   return agFlashOpr.GetSize();
	}
	else
	{
		return 0;
	}
	   
}

BOOL CSettingsSheet::IsEnableChipDspMap()
{
	return m_pageMain.m_bEnableChipDspMap;
}

int CSettingsSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;	
	// TODO: Add your specialized creation code here

	if(g_theApp.m_bCMDPackage)
	{
		PostMessage(WM_COMMAND,ID_APPLY_NOW);
	}
	
	return 0;
}

BOOL CSettingsSheet::IsAutoGenSN()
{
	return !m_pageMain.m_bWriteSN;
}


BOOL CSettingsSheet::IsHasLCD()
{
	return m_pageLCDCfig.m_bRightPS;
}

BOOL CSettingsSheet::FindLCDItem(LPCTSTR lpszFileName, VEC_LCD_CFIG &vLcdCfig)
{
	return m_pageMain.FindLCDItem(lpszFileName,vLcdCfig,NULL);
}

BOOL CSettingsSheet::IsEraseAll()
{
	return m_pageFlashOpt.m_bEraseAll;
}

BOOL CSettingsSheet::IsEraseIMEI()
{
    return m_pageCalibration.IsEraseIMEI();
}

BOOL CSettingsSheet::IsReadMcpType()
{
	return m_pageOptions.m_bReadMcpType;
}
BOOL CSettingsSheet::IsReadChipUID()
{
    return m_pageOptions.m_bReadChipUID;
}
BOOL CSettingsSheet::IsCheckMatch()
{
    return m_pageOptions.m_bCheckMatch;
}

BOOL CSettingsSheet::IsEnableSecureBoot()
{
    return m_pageOptions.m_bEnableSecureBoot;
}

BOOL CSettingsSheet::IsEnableDebugMode()
{
	return m_pageOptions.m_bEnableDebugMode;
}

BOOL CSettingsSheet::IsEnableMultiFileBuf()
{
	if( m_pageMain.m_pCurProductInfo != NULL &&
		m_pageMain.m_pCurProductInfo->tChips.bEnable &&
		m_pageMain.m_pCurProductInfo->tChips.dwCount != 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL     CSettingsSheet::IsEnableRFChipType()
{
	BOOL bOK = FALSE;
	if( m_pageMain.m_pCurProductInfo != NULL &&
		m_pageMain.m_pCurProductInfo->tRfChips.bEnable &&
		0 != m_pageMain.m_pCurProductInfo->tRfChips.dwCount)
	{
		bOK = TRUE;
	}
	return bOK;
}

int CSettingsSheet::GetAllChipName(CStringArray &agChipNames,CUIntArray &agChipIDs)
{
	agChipNames.RemoveAll();
	agChipIDs.RemoveAll();
	if(IsEnableMultiFileBuf())
	{
		for(UINT i = 0 ; i< m_pageMain.m_pCurProductInfo->tChips.dwCount; i++)
		{
			agChipNames.Add(m_pageMain.m_pCurProductInfo->tChips.pChips[i].szName);
			agChipIDs.Add(m_pageMain.m_pCurProductInfo->tChips.pChips[i].dwID);
		}
	}

	return agChipNames.GetSize();
}

BOOL CSettingsSheet::GetChipName(DWORD dwChipID, CString &strName)
{
	strName.Empty();

	if(IsEnableMultiFileBuf())
	{
		for(UINT i = 0 ; i< m_pageMain.m_pCurProductInfo->tChips.dwCount; i++)
		{
			if(m_pageMain.m_pCurProductInfo->tChips.pChips[i].dwID == dwChipID)
			{
				strName = m_pageMain.m_pCurProductInfo->tChips.pChips[i].szName;
				return TRUE;
			}			
		}
	}

	return FALSE;
}

int CSettingsSheet::GetAllRFChipName(CStringArray &agChipNames,CUIntArray &agChipIDs)
{
	agChipNames.RemoveAll();
	agChipIDs.RemoveAll();
	if(IsEnableRFChipType())
	{
		for(UINT i = 0 ; i< m_pageMain.m_pCurProductInfo->tRfChips.dwCount; i++)
		{
			agChipNames.Add(m_pageMain.m_pCurProductInfo->tRfChips.pChips[i].szName);
			agChipIDs.Add(m_pageMain.m_pCurProductInfo->tRfChips.pChips[i].dwID);
		}
	}

	return agChipNames.GetSize();
}

BOOL CSettingsSheet::GetRFChipName(DWORD dwChipID, CString &strName)
{
	strName.Empty();

	if(IsEnableRFChipType())
	{
		for(UINT i = 0 ; i< m_pageMain.m_pCurProductInfo->tRfChips.dwCount; i++)
		{
			if(m_pageMain.m_pCurProductInfo->tRfChips.pChips[i].dwID == dwChipID)
			{
				strName = m_pageMain.m_pCurProductInfo->tRfChips.pChips[i].szName;
				return TRUE;
			}			
		}
	}

	return FALSE;
}

BOOL	CSettingsSheet::GetRFChipID(CString strName,DWORD& dwChipID)
{
	BOOL bOK = FALSE;
	if(IsEnableRFChipType())
	{
		for(UINT i = 0 ; i< m_pageMain.m_pCurProductInfo->tRfChips.dwCount; i++)
		{
			if(0 == _tcsicmp(strName.operator LPCTSTR(),m_pageMain.m_pCurProductInfo->tRfChips.pChips[i].szName))
			{
				dwChipID = m_pageMain.m_pCurProductInfo->tRfChips.pChips[i].dwID;
				return TRUE;
			}			
		}
	}
	return bOK;
}

int CSettingsSheet::GetAllFileID(CStringArray &agFileID)
{
	agFileID.RemoveAll();
	if(m_pageMain.m_pCurProductInfo != NULL)
	{
		for(UINT i = 0; i< m_pageMain.m_pCurProductInfo->dwFileCount; i++)
		{
			agFileID.Add(m_pageMain.m_pCurProductInfo->pFileInfoArr[i].szID);
		}
	}

	return agFileID.GetSize();
	
}

BOOL CSettingsSheet::IsCheckMCPType()
{
	if(m_pageMcp.m_bCheckMcpType && m_pageMcp.m_mapCheckMcpType.GetCount() > 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
CString CSettingsSheet::GetMCPTypeDesc(LPCTSTR lpszMcpType,BOOL &bMatch)
{
	CString strDesc;
	strDesc.Empty();
	bMatch = m_pageMcp.m_mapCheckMcpType.Lookup(lpszMcpType,strDesc);
	if(!bMatch)
	{
		m_pageMcp.m_mapAllMcpType.Lookup(lpszMcpType,strDesc);		
	}

	return strDesc;
}

BOOL CSettingsSheet::IsNeedCheckNV()
{
   _TCHAR szFilePath[_MAX_PATH]={0};
   GetModuleFilePath(g_theApp.m_hInstance,szFilePath);    
   CString strBMFileType;
   strBMFileType.Format(_T("%s\\BMFileType.ini"),szFilePath);

   int nTimes = GetPrivateProfileInt(_T("DownloadNV"),_T("CheckNVTimes"),0,strBMFileType);

   if(nTimes > 0)
	   return TRUE;

   return FALSE;
}

int CSettingsSheet::GetBackupFiles(CStringArray &agID)
{
	return m_pageMain.GetBackupFiles(agID,IsSharkNand());
}

int CSettingsSheet::IsBackupFile(LPCTSTR lpszFileID)
{
	CStringArray agID;
	int nCount = GetBackupFiles(agID);

	for(int i = 0; i<nCount; i++)
	{
		if(agID[i].CompareNoCase(lpszFileID) == 0)
			return i;
	}

	return -1;
}

BOOL  CSettingsSheet::IsModifiedDLFile(LPCTSTR lpszDLFile)
{
    return m_pageMain.IsModifiedDLFile(lpszDLFile);
}

BOOL CSettingsSheet::IsNeedRebootByAT()
{
	if(m_pageMain.m_pCurProductInfo!= NULL)
	{
		return m_pageMain.m_pCurProductInfo->bRebootByAT;
	}

	return FALSE;
}

BOOL CSettingsSheet::IsEnableAPR()
{
	if(m_pageMain.m_pCurProductInfo!= NULL)
	{
		return m_pageMain.m_pCurProductInfo->bEnableAPR;
	}

	return FALSE;
}


BOOL CSettingsSheet::IsLoadFromPac(LPCTSTR lpDLFile)
{
	return m_pageMain.IsLoadFromPac(lpDLFile);
}

int CSettingsSheet::GetDLNVID(CStringArray &agID)
{
	return m_pageMain.GetDLNVID(agID);
}

int CSettingsSheet::GetDLNVIDIndex(LPCTSTR lpszFileID)
{
	CStringArray agID;
	int nCount = m_pageMain.GetDLNVID(agID);
	
	for(int i = 0; i<nCount; i++)
	{
		if(agID[i].CompareNoCase(lpszFileID) == 0)
			return i;
	}
	
	return -1;
}

BOOL CSettingsSheet::IsBackupNVFile(LPCTSTR lpszFileID)
{
	if( GetDLNVIDIndex(lpszFileID) == -1 )
		return FALSE;
		
	return TRUE;
}

BOOL CSettingsSheet::IsKeepCharge()
{
	return m_pageOptions.m_bKeepCharge;
}

DWORD CSettingsSheet::GetComparePolicy()
{
	DWORD dwComparePolicy = m_pageOptions.m_bAutoCompare ? E_BUILT_IN_COMPARE : E_NOT_COMPARE;
	if (m_pageOptions.m_bAutoCompare)
	{
		if (!m_pageOptions.m_strCompareApp.IsEmpty() && PathFileExists(m_pageOptions.m_strCompareApp.operator LPCTSTR() ))
		{
			dwComparePolicy = (m_pageOptions.m_dwComparePolicy == 0) ? E_3RD_PROGRAM_COMPARE : E_BUILT_IN_PLUS_3RD_PROGRAM_COMPARE;
		}
	}
	return dwComparePolicy;
}

DWORD CSettingsSheet::GetUnPacPolicy()
{
	return m_pageOptions.GetUnPacPolicy();
}

CString CSettingsSheet::GetCompareApp()
{
	return m_pageOptions.m_strCompareApp;
}

CString CSettingsSheet::GetCompareParameter()
{
	return m_pageOptions.m_strCompareParameter;
}

BOOL  CSettingsSheet::IsUartDownlod()
{
	return m_pageOptions.m_bUartDownload;
}

BOOL CSettingsSheet::IsDdrCheck()
{
    return m_pageOptions.m_bDdrCheck;
}

BOOL CSettingsSheet::IsCheckOldMemory()
{
    return m_pageOptions.m_bSelfRefresh;
}

BOOL CSettingsSheet::IsSelfRefresh()
{
    return m_pageOptions.m_bSelfRefresh;
}

BOOL CSettingsSheet::IsEmmcCheck()
{
    return m_pageOptions.m_bEmmcCheck;
}

BOOL CSettingsSheet::IsEnableLog()
{
    return m_pageOptions.m_bEnableLog;
}

BOOL CSettingsSheet::HasPartitionInfo()
{
	return (m_pageMain.m_pCurProductInfo && m_pageMain.m_pCurProductInfo->dwPartitionCount > 0);
}

BOOL CSettingsSheet::IsHostProType()
{

	BOOL bIsHostProType = FALSE;
	if (m_pageMain.m_pCurProductInfo && m_pageMain.m_pCurProductInfo->pFileInfoArr && m_pageMain.m_pCurProductInfo->dwFileCount)
	{
		FILE_INFO_T* pFdlInfo = &(m_pageMain.m_pCurProductInfo->pFileInfoArr[0]);
		if ( 0 == _tcsicmp(pFdlInfo->szType,_T("HOST_FDL")) )
		{
			bIsHostProType = TRUE;
		}
	}

	return bIsHostProType;
}

BOOL CSettingsSheet::IsValidSoftSIM(LPCTSTR lpSoftSimDir)
{
	_TCHAR szAppPath[_MAX_PATH]={0};
	CString strID(_T("FDL"));
	CString strSoftSimDir(lpSoftSimDir);
	CString strFDLFile;
	BOOL bValid = FALSE;
	GetModuleFilePath(g_theApp.m_hInstance,szAppPath);
	m_pageMain.m_mapDLFiles.Lookup(strID,strFDLFile);
	CString strFDLDir = GetStrFilePath(strFDLFile);

	if (strSoftSimDir.CompareNoCase(szAppPath) && strFDLDir.CompareNoCase(lpSoftSimDir))
	{
		bValid = TRUE;
	}
	return bValid;

}
CString CSettingsSheet::GetSoftSimFile()
{
	CString strDir;
	CString strID(_T("SOFTSIM"));
	CString strFile;
	BOOL bCheck = FALSE;

	m_pageMain.m_mapDLState.Lookup(strID,bCheck);
	m_pageMain.m_mapDLFiles.Lookup(strID,strFile);

	if (bCheck && !strFile.IsEmpty())
	{
		strDir = strFile;
	}

	return strDir;
}
BOOL CSettingsSheet::IsSoftSimFile(LPCTSTR lpFile)
{
	CString strID(_T("SOFTSIM"));
	CString strFile;
	BOOL	bSoftSimFile = FALSE;
	m_pageMain.m_mapDLFiles.Lookup(strID,strFile);
	if (!strFile.IsEmpty() && (0 == strFile.CompareNoCase(lpFile)) )
	{
		bSoftSimFile = TRUE;
	}
	return bSoftSimFile;
}
BOOL CSettingsSheet::IsSoftSimPrj()
{
	BOOL bSoftSimPrj = FALSE;
	CString strID(_T("SOFTSIM"));
	m_pageMain.m_mapDLState.Lookup(strID,bSoftSimPrj);
	return bSoftSimPrj;
}
BOOL CSettingsSheet::HasExtTblInfo()
{
	return (m_pageMain.m_pCurProductInfo && m_pageMain.m_pCurProductInfo->dwExtTblCount > 0);
}
LPBYTE  CSettingsSheet::GetExtTblData(DWORD &dwSize)
{

	dwSize = 0;
	if(	m_pageMain.m_pCurProductInfo == NULL ||
		m_pageMain.m_pCurProductInfo->dwExtTblCount == 0)
	{
		return NULL;
	}
	DWORD dwExtTblCount =m_pageMain.m_pCurProductInfo->dwExtTblCount;

	dwSize = EXTTABLE_COUNT_LEN + sizeof(EXTTBL_HEADER_T)*dwExtTblCount;
	dwSize += m_pageMain.m_pCurProductInfo->dwExtTblDataSize;

	LPBYTE pBuf = new BYTE[dwSize];
	if (NULL == pBuf)
	{
		return NULL;
	}
	int nOffset = EXTTABLE_COUNT_LEN;
	memcpy(pBuf,&m_pageMain.m_pCurProductInfo->dwExtTblCount,EXTTABLE_COUNT_LEN);
	memcpy(pBuf+nOffset,m_pageMain.m_pCurProductInfo->pExtTblHeader,sizeof(EXTTBL_HEADER_T)*dwExtTblCount);

	nOffset += sizeof(EXTTBL_HEADER_T)*dwExtTblCount;
	for (int i=0; i < (int)dwExtTblCount; ++i)
	{
		EXTTBL_DATA_PTR pSrcItem = m_pageMain.m_pCurProductInfo->pExtTblData + i;
		memcpy(pBuf+nOffset,pSrcItem->pData,pSrcItem->dwSize);
		nOffset += pSrcItem->dwSize;
	}
	return pBuf;
}

LPBYTE CSettingsSheet::GetPartitionData(DWORD &dwSize)
{
	const int nSpareLen = 12;
	dwSize = 0;
	if(m_pageMain.m_pCurProductInfo == NULL ||
		m_pageMain.m_pCurProductInfo->dwPartitionCount == 0)
	{
		return NULL;
	}
	LPBYTE pBuf = NULL ; 
	if(m_pageMain.m_pCurProductInfo->byteSPPTUnit == 0xFF )
	{
		dwSize = sizeof(PARTITION_T)*m_pageMain.m_pCurProductInfo->dwPartitionCount;
		
		dwSize -= nSpareLen*m_pageMain.m_pCurProductInfo->dwPartitionCount;
		pBuf = new BYTE[dwSize];
		if(pBuf)
		{
			DWORD dwTblSize = 0 ;
			DWORD dwOffset = 0 ;
			for(int i = 0; i<m_pageMain.m_pCurProductInfo->dwPartitionCount;++i  )
			{
				PARTITION_T* pPart = m_pageMain.m_pCurProductInfo->pPartitions + i;

				dwOffset = i*(sizeof(PARTITION_T)-nSpareLen);
				memcpy(pBuf+dwOffset,pPart->szID,sizeof(pPart->szID));

				dwTblSize = (DWORD) pPart->llSize;
				dwOffset += sizeof(pPart->szID);
				memcpy(pBuf+dwOffset,&dwTblSize,4);
			}
		}
	}
	else
	{
		SPPT_HEADER_T	headerSPPT;
		SPPT_TABLE_T	tblInfo;

		if (m_pageMain.m_pCurProductInfo->byteSPPTUnit == 0x04) //Unit is sector
		{
			headerSPPT.byteUnit = m_pageMain.m_pCurProductInfo->byteSPPTUnit;
		}	
		headerSPPT.byteTblCount = 1;

		tblInfo.wOffset = sizeof(SPPT_HEADER_T) + sizeof(SPPT_TABLE_T);
		tblInfo.wSize =(WORD) (m_pageMain.m_pCurProductInfo->dwPartitionCount*sizeof(PARTITION_T));

		dwSize = sizeof(SPPT_HEADER_T) + sizeof(SPPT_TABLE_T) + sizeof(PARTITION_T)*m_pageMain.m_pCurProductInfo->dwPartitionCount;
		pBuf = new BYTE[dwSize];
		if(pBuf)
		{
			memcpy(pBuf,&headerSPPT,sizeof(SPPT_HEADER_T));
			memcpy(pBuf+sizeof(SPPT_HEADER_T),&tblInfo,sizeof(SPPT_TABLE_T));
			memcpy(pBuf+sizeof(SPPT_HEADER_T)+sizeof(SPPT_TABLE_T),m_pageMain.m_pCurProductInfo->pPartitions,dwSize-sizeof(SPPT_HEADER_T) - sizeof(SPPT_TABLE_T));
		}
	}

	return pBuf;
}

BOOL CSettingsSheet::IsMapPBFileBuf()
{
	return m_pageMain.IsMapPBFileBuf();
}

BOOL CSettingsSheet::IsSharkNand()
{
	if(!m_pageOptions.m_bSharkNandOption)
		return FALSE;

	if(!HasPartitionInfo()) // not shark
		return FALSE;
	return IsMapPBFileBuf(); // NAND	

}

BOOL CSettingsSheet::IsEnableEndProcess()
{
    //if(!m_pageOptions.m_bEnableEndProcess || m_pageOptions.m_bReset || m_pageOptions.m_bPowerOff)
    //{
    //    return FALSE;
    //}
    return m_pageOptions.m_bEnableEndProcess;
}

int CSettingsSheet::GetBackupProdnvMiscdataType()
{
    return m_pageCalibration.m_nBackupProdnvMiscdata;
}

DATA_INFO_T CSettingsSheet::GetDataInfo(LPCTSTR lpDLFile)
{
	return m_pageMain.GetDataInfo(lpDLFile);
}

BOOL CSettingsSheet::CheckCU(LPCTSTR lpCU)
{
    return m_pageMain.CheckCU(lpCU);
}