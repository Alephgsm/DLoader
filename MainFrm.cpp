// MainFrm.cpp : implementation of the CMainFrame class
//
#include "stdafx.h"
#include "DLoader.h"
#include "XAboutDlg.h"
#include "MainFrm.h"
#include "Calibration.h"
#include "Calibration_Struct.h"
#include "DLoaderView.h"
#include "BMFile.h"
#include "MasterImgGen.h"
//#include <dbt.h>
#include "BarcodeDlg.h"
#include <atlconv.h>
#pragma warning(push,3)
#include <vector>
#include <algorithm>
#pragma warning(pop)

#include "./phasecheck/PhaseCheckBuild.h"
#include "DlgPassword.h"
#include "ProcessMgr.h"
#include "UsageStatistics.h"

//wei.song 20140901
#include "ProcessFlowLib.h"

extern "C"
{
#include "crc16.h"
#include "md5.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define NV_LENGTH       0x10000
#define UBOOT_LENGTH    0x100000

#define WM_WARN_MESSAGEBOX (WM_USER+1000)
#define WM_INIT_PACKET     (WM_USER+1001)


#define OPR_SUCCESS			(0)
#define OPR_FAIL			(1)
#define IMG_BACKUP_FOLDER   _T("\\imgstore")

//////////////////////////////////////////////////////////////////////////
static HWND g_hLayer1TesterHwnd = NULL;
static BOOL CALLBACK EnumWindowsProc(HWND hParent, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
	if (NULL == hParent)
    {
		return FALSE;
    }
	else
	{
		_TCHAR szTitle[_MAX_PATH] = {0};
		::GetWindowText(hParent, szTitle, _MAX_PATH);
     	if ( NULL != _tcsstr(szTitle, _T("Layer1Tester")) )
		{
			g_hLayer1TesterHwnd = hParent;
			return FALSE;
		}		
	}    
	return TRUE;
}

static BOOL GetAbsolutePath(CString &strAbsoluteFilePath,LPCTSTR lpszFilePath )
{
    TCHAR szFileName[_MAX_FNAME];    
    TCHAR szDir[_MAX_DIR];
    TCHAR szDirve[_MAX_DRIVE];	
    
    _tsplitpath( lpszFilePath,szDirve,NULL,NULL,NULL);	
	if(szDirve[0] != _T('\0') || (_tcslen(lpszFilePath)>2 && lpszFilePath[0] == _T('\\') && lpszFilePath[1] == _T('\\')))
	{		
		strAbsoluteFilePath = lpszFilePath;
	}
	else
	{
		//do it if strHelpTopic is ralatively 
		GetModuleFileName( AfxGetApp()->m_hInstance, szFileName,_MAX_FNAME);
		_tsplitpath( szFileName , szDirve , szDir , NULL , NULL );	
		strAbsoluteFilePath = szDirve;
		strAbsoluteFilePath += szDir;
		if( lpszFilePath[0] == _T('\\') || lpszFilePath[0] == _T('/') )
			lpszFilePath++;

		strAbsoluteFilePath += lpszFilePath;

	}
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
//{{AFX_MSG_MAP(CMainFrame)
ON_WM_CREATE()
ON_COMMAND(ID_SETTINGS, OnSettings)
ON_COMMAND(ID_START, OnStart)
ON_COMMAND(ID_STOP, OnStop)
ON_UPDATE_COMMAND_UI(ID_START, OnUpdateStart)
ON_UPDATE_COMMAND_UI(ID_STOP, OnUpdateStop)
ON_WM_CLOSE()
ON_UPDATE_COMMAND_UI(ID_SETTINGS, OnUpdateSettings)
ON_WM_HELPINFO()
ON_COMMAND(ID_LOAD_PACKET, OnLoadPacket)
ON_UPDATE_COMMAND_UI(ID_LOAD_PACKET, OnUpdateLoadPacket)
//}}AFX_MSG_MAP
ON_MESSAGE(WM_WARN_MESSAGEBOX,OnWarnMessageBox)
ON_MESSAGE(WM_INIT_PACKET,OnInitalPacket)
ON_MESSAGE(WM_STOP_AUTODLOADER,OnStopAutoDloader)
ON_MESSAGE(WM_STOP_ONE_PORT,OnStopOnePort)
ON_MESSAGE(WM_POWER_MANAGE,OnPowerManage)
ON_MESSAGE(WM_DWONLOAD_START,OnStartDownload)
ON_MESSAGE(WM_DEV_HOUND,OnDevHound)
ON_MESSAGE(WM_USER_HELP,OnHelp)
ON_MESSAGE(WM_BINPAC_PROG_MSG,OnProgressMsg)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame():m_sheetSettings(IDS_DL_SETTINGS)
{
    m_bStarted				= FALSE;
	m_lStartNVRef			= 0;	
    m_bAutoStart			= FALSE;
    m_nFileCount			= 0;
	m_pBMAF					= g_theApp.m_pBMAFramework;	
    m_pMasterImg			= NULL;	
	m_bPacketOpen			= FALSE;
	m_strSpecConfig			= _T("");
	m_strPacketPath			= _T("");
	m_pReportFile			= NULL;	
	m_dwMaxNVLength			= NV_LENGTH;
    m_dwMaxUbootLen         = UBOOT_LENGTH;
    m_strKeyPartID          = _T("uboot");
	m_strOprErrorConfigFile = _T("BMError.ini");
	m_bCheckCali			= FALSE;
	m_strCodeChipID			= _T("");
	m_bShowFailedMsgbox		= FALSE;
	m_bFailedMsgboxShowed	= FALSE;
	m_bPowerManage			= FALSE;
	m_bPMInDLProcess		= TRUE;
	m_dwPowerMgrInter		= 10;	
	m_bNeedPhaseCheck		= TRUE;
	m_bDoReport				= FALSE;
    m_nDUTID                = E_IMEI;
	m_strLoadSettingMsg		= _T("");
	m_pPartitionData		= NULL;
	m_dwPartitionSize		= 0;
	m_pExtTblData			= NULL;
	m_dwExtTblSize			= 0;
	m_bNameWithSN			= FALSE;
    m_bShowRate             = FALSE;
    m_bSetFirstMode         = FALSE;
	m_bSptFPFirstMode		= FALSE;
    m_nFirstMode            = 0;
	m_bShowSafetyTips		= FALSE;
    m_bSupportCU            = FALSE;
    m_bSupportCheckRoot     = FALSE;
    m_bSupportCheckInproduction = FALSE;
	m_bNeedCompare			= FALSE;
    m_bPacLoading           = FALSE;
    m_dwCheckMatchPolicy    = 0;
    m_bEnableWriteFlash     = FALSE;
	m_strSafetyTipsMsg.Empty();
    m_strEraseIMEIMsg.Empty();
    m_strEraseAllFlashMsg.Empty();
    memset(m_szPacKey,0,sizeof(m_szPacKey));
	m_aPacReleaseDir.RemoveAll();	
	m_mapBMObj.RemoveAll();
	m_mapNVFileInfo.RemoveAll();
	m_mapMultiNVInfo.clear();
	m_dwSinglePort = 0;

	m_nRmImgStoreInterval       = 7;
	m_strPacDirBase             = _T("");

	m_bPortSecondEnum           = FALSE;
    m_bPacHasKey                = FALSE;
    m_pUsbMoniter               = NULL;
	ZeroMemory(&m_ftPacInfo,sizeof(FILETIME));
	
	InitializeCriticalSection( &m_csReportFile );
	InitializeCriticalSection( &m_csPowerManage );
	InitializeCriticalSection( &m_csPortData );
	InitializeCriticalSection( &m_csProcessFlow ); //Add for ProcessFlow, wei.song 20140901
	InitializeCriticalSection( &m_csSoftSim );
	InitializeCriticalSection( &m_csMultCmdFlash );
    CreateDevHound(&m_pUsbMoniter);

}//lint !e1401

CMainFrame::~CMainFrame()
{
/*lint -save -e1551 */
	m_pBMAF = NULL;
	if(m_pMasterImg)
    {
		delete m_pMasterImg;
		m_pMasterImg = NULL;
    }
	
	DeleteCriticalSection( &m_csReportFile );
	DeleteCriticalSection( &m_csPowerManage );
	DeleteCriticalSection( &m_csProcessFlow ); //wei.song 20140901
	DeleteCriticalSection( &m_csSoftSim );
	DeleteCriticalSection( &m_csMultCmdFlash );
	
	if(NULL != m_pReportFile)
	{
		fclose(m_pReportFile);
		m_pReportFile = NULL;
	}
	
	DWORD dwPort = 0;	
    _BMOBJ  *pStruct = NULL;
	POSITION pos = m_mapBMObj.GetStartPosition();
    while( pos )
    {
        m_mapBMObj.GetNextAssoc( pos, dwPort, pStruct );	
		pStruct->Clear();
		delete pStruct;
		pStruct = NULL;
    }
	m_mapBMObj.RemoveAll();
	DeleteCriticalSection( &m_csPortData );	
    pos = m_mapPortData.GetStartPosition();
    while( NULL != pos )
    {
        DWORD dwKey;
		PORT_DATA* pPortData;
        m_mapPortData.GetNextAssoc( pos, dwKey, pPortData );
        SAFE_CLOSE_HANDLE( pPortData->hSNEvent );
		SAFE_DELETE(pPortData->lpPhaseCheck);
		SAFE_DELETE(pPortData);
    }
	m_mapPortData.RemoveAll();
	SAFE_DELETE_ARRAY(m_pPartitionData);
	SAFE_DELETE_ARRAY(m_pExtTblData);
	RemoveLstSoftSim();
	ClearExtImgMap(m_mapUDiskIMg);
	ClearExtImgMap(m_mapChipDsp);
	ClearMultiFileBuf();
	ClearNVMap();
	ClearMultiNVMap();
    if(m_pUsbMoniter)
    {
        ReleaseDevHound(m_pUsbMoniter);
    }
	m_tDebugData.Clear();
/*lint -restore */
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	
    SetMenu(NULL);
	m_xBMOprObserver.pThis = this;
	m_xBMOprObserver.bExistFlashOpr = FALSE;
	

	if(!_LoadSettings())
	{
		if(!m_strLoadSettingMsg.IsEmpty())
		{
			CString strError;
			strError.LoadString(IDS_LOAD_SETTINGS_FAIL);
			strError += m_strLoadSettingMsg;
			g_theApp.MessageBox(strError,MB_ICONSTOP);
		}
		return -1;
	}
    
    //@ Liu Kai 2004-3-25 CR8123
    // Read .ini file
#ifndef _SPUPGRADE
#ifndef _FACTORY	
    if(!LoadSettings())
    {  
		if(!m_strLoadSettingMsg.IsEmpty())
		{
			CString strError;
			strError.LoadString(IDS_LOAD_SETTINGS_FAIL);
			strError += m_strLoadSettingMsg;
			g_theApp.MessageBox(strError,MB_ICONSTOP);
		}
        return -1;
    }
#endif
#endif
	
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//	EnableDocking(CBRS_ALIGN_ANY);
	//	DockControlBar(&m_wndToolBar);
	
	UINT uStyle = m_wndToolBar.GetButtonStyle(1);
	uStyle |=  BS_FLAT;
	m_wndToolBar.SetButtonStyle(1,uStyle);
//  just for pc-lint
	UNUSED_ALWAYS(IDB_BMP_TB_FACTORY);
	UNUSED_ALWAYS(IDB_BMP_TB_FACTORY_DIS);
	UNUSED_ALWAYS(IDB_BMP_TB_FACTORY_HOT);
	UNUSED_ALWAYS(IDB_BMP_TB_UPGRADE);
	UNUSED_ALWAYS(IDB_BMP_TB_UPGRADE_DIS);
	UNUSED_ALWAYS(IDB_BMP_TB_UPGRADE_HOT);
	UNUSED_ALWAYS(IDB_BMP_TOOLBAR);
	UNUSED_ALWAYS(IDB_BMP_TOOLBAR_DIS);
	UNUSED_ALWAYS(IDB_BMP_TOOLBAR_HOT);
#ifdef _SPUPGRADE
	m_wndToolBar.SetBitmaps(IDB_BMP_TB_FACTORY,IDB_BMP_TB_FACTORY_DIS,IDB_BMP_TB_FACTORY_HOT);	
#elif defined _FACTORY
	m_wndToolBar.SetBitmaps(IDB_BMP_TB_UPGRADE,IDB_BMP_TB_UPGRADE_DIS,IDB_BMP_TB_UPGRADE_HOT);
#else	
	m_wndToolBar.SetBitmaps(IDB_BMP_TOOLBAR,IDB_BMP_TOOLBAR_DIS,IDB_BMP_TOOLBAR_HOT);	
#endif

	int nIndex =0;
	CRect rcStatic;
	CRect rcWnd;
	GetClientRect(rcWnd);
	int nRight;
	nRight= rcWnd.right;
	nIndex= m_wndToolBar.CommandToIndex(ID_SPD_VERSION);
    m_wndToolBar.SetButtonInfo(nIndex,ID_SPD_VERSION,TBBS_SEPARATOR,800);
	m_wndToolBar.GetItemRect(nIndex,&rcStatic);
    rcStatic.top+=2;
	rcStatic.bottom-=2;
	//	rcStatic.right -= 400;
	m_wndToolBar.m_stcWnd.Create(_T(""),WS_CHILD|WS_VISIBLE |SS_LEFTNOWORDWRAP,rcStatic,&m_wndToolBar,ID_SPD_VERSION);
	m_wndToolBar.m_stcWnd.SetText(_T(""));
    m_wndToolBar.m_stcWnd.ShowWindow(SW_SHOWNA);
    m_wndToolBar.GetToolBarCtrl().EnableButton(ID_SPD_VERSION,TRUE);

	SetDLTitle();

	//m_usbMoniter.SetReceiver((DWORD)this->GetSafeHwnd(),WM_DEV_HOUND,FALSE);
    m_pUsbMoniter->SetReceiver((DWORD)this->GetSafeHwnd(),WM_DEV_HOUND,FALSE);
	
	PostMessage(WM_INIT_PACKET);

	if(g_theApp.m_bCMDPackage || g_theApp.m_bCMDFlash)
	{
		AfxGetApp()->m_nCmdShow = SW_HIDE;
	}	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
    cs.style &= ~FWS_ADDTOTITLE;	
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnSettings() 
{
	if(g_theApp.m_bNeedPassword)
	{
		CDlgPassword dlgPW;
		if(dlgPW.DoModal() == IDCANCEL)
		{
			return;
		}
	}
    m_sheetSettings.DoModal();

	SetDLTitle();
}

void CMainFrame::OnStart()
{
	if (m_bShowSafetyTips && !m_strSafetyTipsMsg.IsEmpty() &&!g_theApp.m_bScriptCtrl && !g_theApp.m_bCMDPackage)
	{
		if(MessageBox(m_strSafetyTipsMsg.operator LPCTSTR(),_T("Warning"),MB_YESNO | MB_ICONWARNING) == IDNO)
		{
			return;
		}
	}

	m_bNeedCompare = NeedCompare();

	if(m_bNeedCompare && m_sheetSettings.GetUnPacPolicy())
	{
		MessageBox(_T("Auto compare function does not enable fast load pac function.\r\nPlease modify LoadPolicy=0 in BinPack.ini and reload pac."),_T("Error"));
		return;
	}
	
    _TCHAR szConfigPath[_MAX_PATH]={0};
    int nHoundType = E_SETUPAPI;
    if(g_theApp.GetIniFilePath(szConfigPath))
    {
        nHoundType = GetPrivateProfileInt(_T("PortSetting"),_T("HoundType"),0,szConfigPath);
    }
   // m_usbMoniter.Start();
    m_pUsbMoniter->Start(nHoundType);
	m_bStarted = TRUE;	
	m_xBMOprObserver.bExistFlashOpr = FALSE;

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_START,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_SETTINGS,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_LOAD_PACKET,FALSE);
	
	PUMP_MESSAGES();
	PostMessage(WM_DWONLOAD_START);
}

void CMainFrame::StartWork()
{
	CWaitCursor wait;

	DWORD dwWaitTime = g_theApp.m_dwWaitTimeForNextChip;
	_TCHAR szConfigPath[_MAX_PATH]={0};
	if(g_theApp.GetIniFilePath(szConfigPath))
	{
		CString strProduct = m_sheetSettings.GetCurProduct();
		if(strProduct.Find(_T("PAC_")) == 0)
		{
			strProduct = strProduct.Right(strProduct.GetLength()-4);
		}
		dwWaitTime = GetPrivateProfileInt(_T("Settings"),strProduct,dwWaitTime,szConfigPath);
	}	
	BOOL bUartDownload = m_sheetSettings.IsUartDownlod();
	if (bUartDownload && 0 == dwWaitTime )
	{
		_TCHAR szTimeoutIniFile[MAX_PATH];
		GetModuleFilePath( g_theApp.m_hInstance, szTimeoutIniFile );
		_tcscat( szTimeoutIniFile,  _T("\\BMTimeout.ini") );
		dwWaitTime = GetPrivateProfileInt(_T("CheckDUT"),_T("WaitTimeForNextChip"),2000,szTimeoutIniFile);;
	}

	if(g_theApp.m_bManual)
	{// manual to control download start. so it will close the port, when download finished.
		dwWaitTime = 0;		
	}	

	DWORD dwRepFlag         = m_sheetSettings.GetRepartitionFlag();
    BOOL  bEnablePowerOff    = m_sheetSettings.IsPowerOff();
	m_pBMAF->BMAF_SetProperty(BMAF_TIME_WAIT_FOR_NEXT_CHIP,(LPCVOID)&dwWaitTime);		
	m_pBMAF->BMAF_SetProperty(BMAF_NAND_REPARTION_FLAG, (LPCVOID)&dwRepFlag);				
	m_pBMAF->BMAF_SetProperty(BMAF_SPECIAL_CONFIG_FILE, (LPCVOID)(LPCTSTR)m_strSpecConfig );	
	m_pBMAF->BMAF_SetProperty(BMAF_ENABLE_PORT_SECOND_ENUM, (LPCVOID)&m_bPortSecondEnum);	
    m_pBMAF->BMAF_SetProperty(BMAF_POWER_OFF_DEVICE,(LPCVOID)&bEnablePowerOff);
	m_pBMAF->BMAF_SetProperty(BMAF_UART_MODE_DOWNLOAD,(LPCVOID)&bUartDownload);
	m_bAutoStart = TRUE;
	
	std::vector<DEV_INFO> lstPort;
	
    //m_usbMoniter.ScanPort(lstPort); //sorted by ScanPort
    DEV_INFO* pDevInfo = NULL;
    int nCount = m_pUsbMoniter->ScanPort(&pDevInfo);
    lstPort.clear();
    for(int j = 0; j<nCount; ++j)
    {
        lstPort.push_back(pDevInfo[j]);
    }
    
	for(int i = 0; i< (int)lstPort.size(); i++)
	{
		DWORD dwPort = lstPort[i].nPortNum;

        DWORD dwSelPort = (DWORD)m_sheetSettings.GetComPort();

        if(dwSelPort != 0 && dwPort != dwSelPort)
        {
            continue;
        }

		if(m_dwSinglePort != 0 && dwPort != m_dwSinglePort)
		{
			continue;
		}

		CString strPortName = lstPort[i].szFriendlyName;
		if(g_theApp.m_bFilterPort)
		{
			DWORD dwValue = 0;
			if(g_theApp.m_mapFilterPort.Lookup((DWORD)dwPort,dwValue))
			{
				continue;
			}
		}

		if(m_sheetSettings.IsNeedRebootByAT())
		{						
			if(IsATPort(strPortName)) // Is AT port?
			{
				SendAT2Reboot(dwPort);
				continue;
			}			
		}

		if(!IsDLPort(strPortName,dwPort)) // If not DL port, continue
		{
			continue;
		}

		CString strError;
        if(strPortName.CompareNoCase(_T("SPRD U2S Diag")) == 0)
        {
            if(!IsValidVComDriverVer(strError))
            {
                m_pUsbMoniter->Stop();
                g_theApp.MessageBox(strError);
                PostMessage(WM_COMMAND, ID_STOP, 0);
                return;
            }
        }

        CreatePortData( dwPort );
		if(g_theApp.m_bManual)
		{				
			((CDLoaderView*)GetActiveView())->AddProg( dwPort, TRUE ); 	
		}
		else
		{			
			if( !StartOnePortWork(dwPort,TRUE) ) // The port enumed is regarded as UART
			{
				OnStop();
				return;
			}
			wait.Restore();
			if (m_bNeedCompare)
			{
				m_dwSinglePort = dwPort;
			}
		}
	}
    
    m_pUsbMoniter->FreeMemory(pDevInfo);
}

BOOL CMainFrame::StartOnePortWork( DWORD dwPort, BOOL bUart )
{
	CWaitCursor wait;
	if (m_bNeedCompare)
	{
		CString strReadPath = GetReadFlashPath();
		if (!strReadPath.IsEmpty())
		{
			DeleteDirectory(strReadPath.operator LPCTSTR(),FALSE );
		}
		
	}

	BMOBJ_PTR pStruct = InitBMObj(dwPort,bUart);
	if(pStruct == NULL)
	{
	    g_theApp.ReportToPipe(_T( "Failure to InitBMObj."));
		return FALSE;
	}
	HRESULT hr = 0;
    CString strErrMsg = _T("");
    BYTE btOpenArgument[ 8 ] = {0};
    *(DWORD *)&btOpenArgument[ 0 ] = dwPort; 
    *(DWORD *)&btOpenArgument[ 4 ] = m_sheetSettings.GetBaudRate(); 	

	LPCTSTR *ppFileList = new LPCTSTR[m_nFileCount];
	if(ppFileList == NULL)
	{
	    g_theApp.ReportToPipe(_T("StartOnePortWork ppFileList == NULL,Insufficient memory."));
		return FALSE;
	}
	for(int i = 0; i<m_nFileCount; i++)
	{
		ppFileList[i] = (LPCTSTR)m_arrFile.GetAt(i);
	}	

	m_pBMAF->BMAF_SubscribeObserver( dwPort,&m_xBMOprObserver, NULL );  
	
	HWND hwnd = GetActiveView()->GetSafeHwnd();
    hr = m_pBMAF->BMAF_StartOneWork( m_sheetSettings.GetCurProduct(),
		ppFileList, 
		m_nFileCount,
		(LPVOID)btOpenArgument, 
		TRUE, 
		dwPort, 
		FALSE, 
		(LPVOID)&hwnd,
		L"Channel.Uart" );

	SAFE_DELETE_ARRAY(ppFileList);
	
    BOOL bReturn = TRUE;
	BOOL bFirst = TRUE;
	PBOOTMODEOBJ_T pBMO = NULL;
	m_pBMAF->BMAF_GetBootModeObjInfo(dwPort,(LPVOID*)&pBMO);
	bFirst = pBMO->bFirstStart;
	
    switch( (DWORD)hr )
    {
    case BM_E_CREATETHREAD_FAILED:
        strErrMsg.LoadString( IDS_CREATETHREAD_FAIL );
        bReturn =  FALSE;   
        break;
    case BM_E_FILEINFO_ERROR:
        strErrMsg.LoadString( IDS_FILEINFO_ERROR );
        bReturn =  FALSE;   
        break;
    case BM_E_OPR_NOTREG:
        strErrMsg.LoadString( IDS_BMOPR_NOTREG );
        bReturn =  FALSE;   
        break;
    case BM_E_FAILED:
        strErrMsg.LoadString( IDS_START_FAIL );
        bReturn =  FALSE;   
        break;
	case BMAF_E_INIT_BMFILES_FAIL:		
		strErrMsg = pStruct->szErrorMsg; // prepare file fail, mainly for making master image
		if(strErrMsg.IsEmpty())
		{
			strErrMsg.Format(_T("Init BMFiles failed!"));
		}
        bReturn =  FALSE;   
        break;
    default:
		if(!g_theApp.m_bManual)
		{
			((CDLoaderView*)GetActiveView())->AddProg( dwPort, SUCCEEDED(hr) ); //lint !e730
		}
        break;        
    }
	
    if( !bReturn )
    {
		if(!strErrMsg.IsEmpty() && !g_theApp.m_bCMDFlash)
		{
			AfxMessageBox(strErrMsg);
		}
		m_pBMAF->BMAF_StopOneWork( dwPort);
		m_pBMAF->BMAF_UnsubscribeObserver( dwPort );

		if ( g_theApp.m_bCMDFlash )
		{
			CString strInfo;
			strInfo.Format(_T("DownLoad Failed : %s Elapsed Times = 0"),strErrMsg);
			g_theApp.ReportToPipe(strInfo,TRUE);
		}

		if(g_theApp.m_bScriptCtrl )
		{
			::PostMessage( GetSafeHwnd(), WM_COMMAND, ID_STOP, 0 );
			::PostMessage( GetSafeHwnd(), WM_CLOSE, 0, 0 );
		}
        return FALSE;        
    }     
    RecalcLayout(); 
    return TRUE;
}

BOOL CMainFrame::StopOnePortWork(DWORD dwPort,BOOL bRemoved /*= FALSE*/)
{
	UNREFERENCED_PARAMETER(bRemoved);
	HRESULT hr;
    PBOOTMODEOBJ_T pStruct = NULL;
	
	BMOBJ_PTR pMonitor = NULL;
	if( m_mapBMObj.Lookup(dwPort, pMonitor) && pMonitor )
	{
		pMonitor->bStoping = TRUE;
		ResetEvent(pMonitor->hStopEvent);
	}
    PORT_DATA *pPortData = NULL;
    if( m_mapPortData.Lookup(dwPort, pPortData) && pPortData)
    {
        SetEvent(pPortData->hSNEvent);
    }
	
	hr = m_pBMAF->BMAF_GetBootModeObjInfo(dwPort,(LPVOID*)&pStruct);
	BOOL bSuc = TRUE;
	if( SUCCEEDED(hr) && pStruct != NULL )
	{
		if(!pStruct->bStop)
		{
            if(bRemoved && g_theApp.m_dwWaitTimeForNextChip == 0 && pMonitor && 0 == _tcslen(pMonitor->szErrorMsg))
            {
                _tcscpy(pMonitor->szErrorMsg,_T("User cancel"));
            }
			if (pMonitor->bRMDev)
			{
				pMonitor->szErrorMsg[0] = '\0';
			}
#ifdef _FACTORY
			DL_STAGE stage;
			((CDLoaderView*)GetActiveView())->GetStatus((int)dwPort, stage);
			if ( stage != DL_FINISH_STAGE )
#endif
			{
				((CDLoaderView*)GetActiveView())->SetResult( dwPort,pMonitor->bRMDev ? TRUE :FALSE,pMonitor,bRemoved? 2: 0 );
			}	

			bSuc = FALSE;
		}
		else
		{
			// for release port
			pStruct->bStop = FALSE;
		}	

		((CDLoaderView *)GetActiveView())->ClearPortInfo(dwPort);
				
		m_pBMAF->BMAF_StopOneWork(dwPort);
					
	}
	if(!bRemoved && g_theApp.m_bManual)
	{
		((CDLoaderView *)GetActiveView())->AddProg(dwPort,TRUE);
	}

	if (pMonitor)
	{
		SetEvent(pMonitor->hStopEvent);
	}
	return TRUE;
}

void CMainFrame::OnStop()
{
    CWaitCursor wait;	

	POSITION pos = m_mapPortData.GetStartPosition();
    DWORD dwCookie = 0;
    
    PORT_DATA* pPortData = NULL;
    while( pos )
    {
        m_mapPortData.GetNextAssoc( pos, dwCookie, pPortData );
		SetEvent(pPortData->hSNEvent);        
	}    

	WaitForStopedObj();

    m_pBMAF->BMAF_StopAllWork();
	m_arrFile.RemoveAll();
	m_nFileCount = 0;	
	
    ((CDLoaderView*)GetActiveView())->RemoveAll();
    m_bStarted = FALSE;
    m_bAutoStart = FALSE;
	m_lStartNVRef = 0;
    if( m_pMasterImg )
    {
        delete m_pMasterImg;
        m_pMasterImg = NULL;
    }
	
	if(NULL != m_pReportFile)
	{
		fclose(m_pReportFile);
		m_pReportFile = NULL;
	}
	m_dwSinglePort = 0;
	m_bNeedCompare = FALSE;
	ClearMapPBFileBuf();

	((CDLoaderView *)(this->GetActiveView()))->StopDLTimer(); 
    m_pUsbMoniter->Stop();
}

void CMainFrame::OnUpdateStart(CCmdUI* pCmdUI) 
{
#if defined( _FACTORY ) || defined(_SPUPGRADE)
	BOOL bOK = m_bPacketOpen &&!m_bStarted && m_sheetSettings.IsMainPageInit();
#else // _DLOADERR
    BOOL bOK = !m_bStarted && m_sheetSettings.IsMainPageInit();
#endif
	
    pCmdUI->Enable(bOK);
}

void CMainFrame::OnUpdateStop(CCmdUI* pCmdUI) 
{
#if defined(_SPUPGRADE)
	//not allowed to click stop button when dealing with NV
	pCmdUI->Enable( m_bStarted && m_lStartNVRef == 0 );
#elif defined _FACTORY
	pCmdUI->Enable( m_bStarted && m_lStartNVRef == 0 ); //lint !e730
#else
	pCmdUI->Enable(m_bStarted);	
#endif
}

void CMainFrame::DeletePacTmpDir()
{
	CWaitCursor wait;

	if(g_theApp.m_bDeleteTmpDir)
	{
		CString strPath = _T("");
		int n = m_aPacReleaseDir.GetSize();
		for(int i = 0; i< n; i++)
		{
			strPath = m_aPacReleaseDir.GetAt(i);
			if(!strPath.IsEmpty())
			{				
				DeleteDirectory(strPath.operator LPCTSTR());
			}
		}
		// Find the old download tmp dir and delete them
		RemoveOldPacTmpDir();
	}
	m_aPacReleaseDir.RemoveAll();

	RemoveOldBKFiles();
}

void CMainFrame::RemoveOldPacTmpDir()
{
	CWaitCursor wait;
	_TCHAR szFilePath[_MAX_PATH] = {0};
	if(::GetTempPath(_MAX_PATH,szFilePath)==0)
	{		
		return;
	}
	CString strFind;
	strFind.Format(_T("%s_DownloadFiles*"),szFilePath);
	CTime curTime = CTime::GetCurrentTime();
	CFileFind finder;
	BOOL bFind = finder.FindFile(strFind);
	while(bFind)
	{		
		bFind = finder.FindNextFile();
		if(finder.IsDirectory())
		{
			CTime dirTime;
			finder.GetCreationTime(dirTime);
			CTimeSpan ts = curTime - dirTime;
			if(ts.GetDays() >= 1)
			{
                DeleteDirectory(finder.GetFilePath().operator LPCTSTR());
			}
		}	
	}
}

void CMainFrame::RemoveOldBKFiles()
{
	CWaitCursor wait;
	_TCHAR szFilePath[_MAX_PATH] = {0};
	//BOOL bOK = FALSE;

	g_theApp.GetIniFilePath(szFilePath);
	
	CString strFolder = szFilePath;
	int nFind = strFolder.ReverseFind('\\');
	if(nFind == -1)
		return;	
	strFolder = strFolder.Left(nFind);
	strFolder += IMG_BACKUP_FOLDER;	
	strFolder += _T("\\*.bin");

	CTime curTime = CTime::GetCurrentTime();
	CFileFind finder;
	BOOL bFind = finder.FindFile(strFolder);
	while(bFind)
	{		
		bFind = finder.FindNextFile();
		if(!finder.IsDirectory() && !finder.IsDots())
		{
			CTime dirTime;
			finder.GetCreationTime(dirTime);
			CTimeSpan ts = curTime - dirTime;
			if(ts.GetDays() >= m_nRmImgStoreInterval)
			{
				DeleteFile(finder.GetFilePath());
			}
		}	
	}
}

void CMainFrame::OnClose() 
{
    if (m_bPacLoading)
    {
        return;
    }
	if(m_bStarted)
    {
        CString str;
        str.LoadString(IDS_QUIT_TEST);
        AfxMessageBox(str);
    }
    else
    {
        ShowWindow(SW_HIDE);     		
		DeletePacTmpDir();
		ClearNVMap(); 
		ClearMultiNVMap();
		m_tDebugData.Clear();
		CFrameWnd::OnClose();
    }
}


void CMainFrame::OnUpdateSettings(CCmdUI* pCmdUI) 
{
	// Disable Settings button while downloading
#if defined(_SPUPGRADE) || defined(_FACTORY)
	pCmdUI->Enable(m_bPacketOpen && !m_bStarted); //lint !e730
#else
    pCmdUI->Enable(!m_bStarted);
#endif	
}

BOOL CMainFrame::_LoadSettings()
{
	m_strLoadSettingMsg = _T("");

	_TCHAR szFilePath[_MAX_PATH]={0};
	if(!g_theApp.GetIniFilePath(szFilePath))
	{
		return FALSE;
	}
	_TCHAR szAppPath[_MAX_PATH]={0};
	GetModuleFilePath(g_theApp.m_hInstance,szAppPath);
	CFileFind finder;

    _TCHAR szBuf[_MAX_PATH]={0};
	CString strBMFileType;
	strBMFileType.Format(_T("%s\\BMFileType.ini"),szAppPath);	
	if(finder.FindFile(strBMFileType))
	{
		m_dwMaxNVLength = GetPrivateProfileInt(_T("DownloadNV"),_T("MaxReadLength"),NV_LENGTH,strBMFileType);
        m_dwMaxUbootLen = GetPrivateProfileInt(_T("ReadDUTInfo"),_T("MaxReadLength"),UBOOT_LENGTH,strBMFileType);
        m_dwCheckMatchPolicy = GetPrivateProfileInt(_T("ReadDUTInfo"),_T("CheckMatchPolicy"),0,strBMFileType);
        m_bEnableWriteFlash  = GetPrivateProfileInt(_T("ReadDUTInfo"),_T("EnableWriteFlash"),0,strBMFileType);
        m_bStopDownloadIfOldMemory = GetPrivateProfileInt( _T("Misc") , _T("StopDownloadIfOldMemory"), 0, strBMFileType);
        GetPrivateProfileString(_T("ReadDUTInfo"),_T("PartitionName"),_T("uboot"),szBuf,_MAX_PATH,strBMFileType);
        m_strKeyPartID = szBuf;

        GetPrivateProfileString(_T("AT_REBOOT_SETTING"),_T("ATPort"),_T(""),szBuf,_MAX_PATH,strBMFileType);
        SplitStr(szBuf,m_agATPort,',');	

        memset(szBuf,0,sizeof(szBuf));
        GetPrivateProfileString(_T("AT_REBOOT_SETTING"),_T("DLPort"),_T(""),szBuf,_MAX_PATH,strBMFileType);
        SplitStr(szBuf,m_agDLPort,',');
	}
	m_strOprErrorConfigFile.Format(_T("%s\\BMError.ini"),szAppPath); 

#ifdef _SPUPGRADE
	m_bCheckCali =  GetPrivateProfileInt(_T("Settings"),_T("CheckCali"),TRUE,szFilePath);
    m_bShowSafetyTips = GetPrivateProfileInt(_T("GUI"),_T("ShowSecurityTips"),TRUE,szFilePath);
    m_bSupportCU = GetPrivateProfileInt(_T("Options"),_T("SupportCU"),FALSE,szFilePath);
    m_bSupportCheckRoot = GetPrivateProfileInt(_T("Options"),_T("SupportCheckRoot"),FALSE,szFilePath);
    m_bSupportCheckInproduction = GetPrivateProfileInt(_T("Options"),_T("SupportInproduction"),FALSE,szFilePath);
#else
	m_bCheckCali =  GetPrivateProfileInt(_T("Settings"),_T("CheckCali"),FALSE,szFilePath);
    m_bShowSafetyTips = GetPrivateProfileInt(_T("GUI"),_T("ShowSecurityTips"),FALSE,szFilePath);
#endif
	GetPrivateProfileString(_T("GUI"),_T("SafetyTipsMsg"),_T(""),szBuf,_MAX_PATH,szFilePath);
	m_strSafetyTipsMsg = szBuf;

    GetPrivateProfileString(_T("GUI"),_T("EraseAllFlashMsg"),_T("You have selected [Erase All Flash], this will clear all data stored in the flash including calibration.IMEI will be erased too and need to be rewritten,If continue?"),szBuf,_MAX_PATH,szFilePath);
    m_strEraseAllFlashMsg = szBuf;

    GetPrivateProfileString(_T("GUI"),_T("EraseIMEIMsg"),_T("You have not selected to backup IMEI item,IMEI will be erased and need to be rewritten,If continue?"),szBuf,_MAX_PATH,szFilePath);
    m_strEraseIMEIMsg = szBuf;

	m_nRmImgStoreInterval =  GetPrivateProfileInt(_T("Settings"),_T("RemoveImgStoreInterval"),7,szFilePath);
	if(m_nRmImgStoreInterval < 0)
		m_nRmImgStoreInterval = 7;

	//m_bPortSecondEnum = GetPrivateProfileInt(_T("Settings"),_T("EnablePortSecondEnum"),FALSE,szFilePath);
	//@ hongliang.xin 2010-6-17
	m_bShowFailedMsgbox = GetPrivateProfileInt(_T("GUI"),_T("ShowFailedMsgBox"),FALSE,szFilePath);
	m_bPowerManage		= GetPrivateProfileInt(_T("GUI"),_T("PowerManageFlag"),FALSE,szFilePath);
	m_dwPowerMgrInter	= GetPrivateProfileInt(_T("GUI"),_T("PowerManageInterval"),10,szFilePath);
	m_bPMInDLProcess	= GetPrivateProfileInt(_T("GUI"),_T("InDLProcess"),1,szFilePath);
	m_bNeedPhaseCheck	= GetPrivateProfileInt(_T("SN"),_T("NeedPhaseCheck"),1,szFilePath);
	m_bDoReport			= GetPrivateProfileInt(_T("Report"),_T("enable"),0,szFilePath);	
	m_nReportType		= GetPrivateProfileInt(_T("Report"),_T("Type"),0,szFilePath);	
    m_bShowRate         = GetPrivateProfileInt(_T("GUI"),_T("ShowRate"),FALSE,szFilePath);
	
    m_bSetFirstMode     = GetPrivateProfileInt(_T("FirstMode"),_T("Enable"),FALSE,szFilePath);
	m_bSptFPFirstMode   = GetPrivateProfileInt(_T("FirstMode"),_T("SupportFeaturePhone"),FALSE,szFilePath);
    m_nFirstMode        = GetPrivateProfileInt(_T("FirstMode"),_T("FirstMode"),0,szFilePath);
	
#ifndef _FACTORY   
    m_nDUTID            = GetPrivateProfileInt(_T("Report"),_T("DUTID"),E_IMEI,szFilePath);
	m_bNameWithSN		= GetPrivateProfileInt(_T("NV"),_T("FileNameWithSN"),TRUE,szFilePath);
#else
    m_nDUTID            = GetPrivateProfileInt(_T("Report"),_T("DUTID"),E_SN,szFilePath);
#endif

	memset(szBuf,0,sizeof(szBuf));
	GetPrivateProfileString(_T("Download_Packet"),_T("UnpacketDir"),_T(""),szBuf,_MAX_PATH,szFilePath);
	m_strPacDirBase = szBuf;

	if(m_strPacDirBase.CompareNoCase(_T(".\\"))==0)
	{
		m_strPacDirBase.Format(_T("%s\\ImageFiles"),szAppPath);
        RmLatestTempFiles();
		if(!CreateDirectory(m_strPacDirBase,NULL))
		{
			if( GetLastError() != ERROR_ALREADY_EXISTS )
			{ 		
				m_strPacDirBase = _T("");
				m_strLoadSettingMsg.Format(_T("Can not create unpacket folder \"%s\"!"),m_strPacDirBase);
				return FALSE;
			} 
		}
	}
	else if(!m_strPacDirBase.IsEmpty() && m_strPacDirBase.GetLength()!=2)
	{
        RmLatestTempFiles();
		if(!CreateDeepDirectory(m_strPacDirBase))
		{					
			m_strPacDirBase = _T("");
			m_strLoadSettingMsg.Format(_T("Can not create unpacket folder \"%s\"!"),m_strPacDirBase);
			return FALSE;			
		}
	}
    
	return TRUE;    
}
void  CMainFrame::RmLatestTempFiles()
{
    if (m_strPacDirBase.IsEmpty() || !g_theApp.m_bDeleteTmpDir)
    {
        return;
    }
    TCHAR szModuleProc[MAX_PATH] = {0};
    ProcessMgr cProcMgr;
    GetModuleFileName(NULL,szModuleProc,MAX_PATH);
    if (cProcMgr.IsSingleIntance(szModuleProc))
    {
        DeleteDirectory(m_strPacDirBase.operator LPCTSTR(),FALSE);
        CreateDeepDirectory(m_strPacDirBase.operator LPCTSTR());       
    }
    
}
//@ Liu Kai 2004-3-25 CR8123
BOOL CMainFrame::LoadSettings()
{
	m_strLoadSettingMsg = _T("");
	_TCHAR szFilePath[_MAX_PATH]={0};
	if(!g_theApp.GetIniFilePath(szFilePath))
	{
		return FALSE;
	}
    return m_sheetSettings.LoadSettings(szFilePath,m_strLoadSettingMsg);    
}



HRESULT CBMOprObserver::OnOperationStart( DWORD dwOprCookie, 
														  LPCWSTR cbstrFileID,
                                                          LPCWSTR cbstrFileType, 
                                                          LPCWSTR cbstrOperationType,
                                                          LPVOID pBMFileInterface )
{
    
    UNUSED_ALWAYS( pBMFileInterface );
	USES_CONVERSION;
	
	CString strErrMsg = _T("");
	_BMOBJ * pbj = NULL;
	if( !pThis->m_mapBMObj.Lookup(dwOprCookie,pbj) ||  NULL == pbj )	
	{
		return E_FAIL;
	}
	CString strFileID = W2T((LPWSTR) cbstrFileID );	

	do 
	{
		/*
		if( _tcsnicmp( strFileID, _T("_BKF_"), 5) == 0 &&
			((_tcsicmp( W2T( (LPWSTR)cbstrFileType ), _T("ReadFlash")) == 0 &&
			_tcsicmp( W2T( (LPWSTR)cbstrOperationType ),_T("ReadFlash")) == 0 ) ||
			((_tcsicmp( W2T( (LPWSTR)cbstrFileType ), _T("ReadFlash2")) == 0 &&
			_tcsicmp( W2T( (LPWSTR)cbstrOperationType ),_T("ReadFlashByID")) == 0 ))))
		{
			CString strID = W2T( (LPWSTR)cbstrFileID );
			strID = strID.Right(strID.GetLength() - 5); // "_BKF_"
			PFILE_INFO_T pFileInfo = NULL;
			if(pThis->m_sheetSettings.GetFileInfo(strID,(LPDWORD)&pFileInfo) == -1)
			{
				strErrMsg.Format( _T("Can not find %s!"), strID);
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}
			
			BOOTMODEOBJ_T *pBMO = NULL;
			pThis->m_pBMAF->BMAF_GetBootModeObjInfo(dwOprCookie, (LPVOID*)&pBMO);
			IBMSettings *pSetting = (IBMSettings *)pBMO->pSnapin;
			DWORD dwPacketLen = pSetting->GetPacketLength(pFileInfo->szType);
			if(dwPacketLen == 0)
			{
				dwPacketLen = 0x1000;
			}

			IBMFile* pBMFile = (IBMFile*)pBMFileInterface;
			pBMFile->SetCurMaxLength(dwPacketLen);
			break;
		}
		*/

		if(	_tcsicmp( strFileID, _T("PhaseCheck")) == 0 && 
			((_tcsicmp( W2T( (LPWSTR)cbstrFileType ), _T("CODE")) == 0 &&
			_tcsicmp( W2T( (LPWSTR)cbstrOperationType ),_T("Download")) == 0)||
			(_tcsicmp( W2T( (LPWSTR)cbstrFileType ), _T("CODE2")) == 0 &&
			_tcsicmp( W2T( (LPWSTR)cbstrOperationType ),_T("DownloadByID")) == 0)))
			
		{
			int nID = pThis->m_sheetSettings.IsBackupFile(_T("PhaseCheck"));
			if(nID != -1)
			{
				IBMFile* pBMFile = (IBMFile*)pBMFileInterface;
				pBMFile->SetCurCode(pbj->tFileBackup[nID].pBuf, pbj->tFileBackup[nID].dwSize);
			}
			else
			{
				#ifndef _SPUPGRADE
					PORT_DATA* pPortData = NULL;
					BOOL bFound = pThis->m_mapPortData.Lookup(dwOprCookie, pPortData);
					if( bFound && pPortData && pPortData->lpPhaseCheck)
					{
						IBMFile* pBMFile = (IBMFile*)pBMFileInterface;
						DWORD dwPhaseCheckSize  = pBMFile->GetCurCodeSize();
						if(dwPhaseCheckSize == 0  || dwPhaseCheckSize > PRODUCTION_INFO_SIZE )
						{
							dwPhaseCheckSize = PRODUCTION_INFO_SIZE;
						}
						pBMFile->SetCurCode(pPortData->lpPhaseCheck, dwPhaseCheckSize);
					}	
					else
					{
						strErrMsg.Format( _T("Cannot find phasecheck info!"));
						_tcscpy(pbj->szErrorMsg,strErrMsg);
						return E_FAIL;
					}	
				#endif //_SPUPGRADE 
			}
			break;
		}

		if( _tcsicmp( strFileID, _T("PhaseCheck")) != 0 && 
			( _tcsicmp( W2T( (LPWSTR)cbstrOperationType ),_T("Download")) == 0  ||
			  _tcsicmp( W2T( (LPWSTR)cbstrOperationType ),_T("DownloadByID")) == 0 ) )
		{
			int nID = pThis->m_sheetSettings.IsBackupFile(strFileID);
			if(nID != -1)
			{
				IBMFile* pBMFile = (IBMFile*)pBMFileInterface;
				pBMFile->SetCurCode(pbj->tFileBackup[nID].pBuf, pbj->tFileBackup[nID].dwSize);
				break;
			}			
		}

		if( _tcsicmp( strFileID, _T("WriteDluid")) == 0 && 
			_tcsicmp( W2T( (LPWSTR)cbstrOperationType ),_T("DownloadByID")) == 0 ) 
		{		
			IBMFile* pBMFile = (IBMFile*)pBMFileInterface;				
			pBMFile->SetCurCode(pbj->dluid.pBuf, pbj->dluid.dwSize);					
		}

		strFileID.MakeUpper();
		if( pThis->m_bPowerManage && pThis->m_bPMInDLProcess && 
			pThis->m_sheetSettings.IsBackupNV() && 
			(_tcsnicmp( W2T((LPWSTR)cbstrFileID ), _T("NV"),2) == 0 || 
			 _tcsnicmp( W2T( (LPWSTR)cbstrFileID ), _T("_CHECK_NV"),9) == 0) && 
			(_tcsicmp( W2T( (LPWSTR)cbstrOperationType ),_T("Download")) == 0  ||
			 _tcsicmp( W2T( (LPWSTR)cbstrOperationType ),_T("DownloadByID")) == 0 ))
		{
			::PostMessage(pThis->GetSafeHwnd(),WM_POWER_MANAGE,dwOprCookie,0);
			Sleep( pThis->m_dwPowerMgrInter);
		}	

        if( pThis->m_bPowerManage && pThis->m_bPMInDLProcess &&  
            (_tcsnicmp( W2T((LPWSTR)cbstrFileID ), _T("EraseSPL"),8) == 0))
        {
            ::PostMessage(pThis->GetSafeHwnd(),WM_POWER_MANAGE,dwOprCookie, (WPARAM)_T("EraseSPL"));
            Sleep( pThis->m_dwPowerMgrInter);
        }	

        if( pThis->m_bPowerManage && pThis->m_bPMInDLProcess &&  
            (_tcsnicmp( W2T((LPWSTR)cbstrFileID ), _T("SPLLoader"),9) == 0))
        {
            ::PostMessage(pThis->GetSafeHwnd(),WM_POWER_MANAGE,dwOprCookie, (WPARAM)_T("SPLLoader"));
            Sleep( pThis->m_dwPowerMgrInter);
        }

		//	for softSIM @polo.jiang on 20141013
		if( 
			_tcsnicmp(W2T( (LPWSTR)cbstrFileID),_T("SoftSIM"),7) == 0  &&
			_tcsicmp( W2T( (LPWSTR)cbstrOperationType ),_T("Download")) == 0
		  )
		{
			BOOL bRet = FALSE;
			CAutoCS cs( pThis->m_csSoftSim );
			TCHAR *pFile = NULL;
			if ( pThis->m_lstSoftSim.size() )
			{
				pFile = pThis->m_lstSoftSim.front();
				if (pFile)
				{
					_tcscpy(pbj->tSoftSim.szFile,pFile);
					try
					{
						CFile file(pFile,CFile::modeRead | CFile::shareDenyWrite);
						pbj->tSoftSim.dwSize = (DWORD)file.GetLength();
						if(pbj->tSoftSim.dwSize != 0 )
						{	
							pbj->tSoftSim.pBuf = new BYTE[pbj->tSoftSim.dwSize];
							file.Read(pbj->tSoftSim.pBuf,pbj->tSoftSim.dwSize);
						}			
						file.Close();
					}
					catch (CException* e)
					{	
						UNUSED_ALWAYS( e );
						_tcscpy(pbj->szErrorMsg,_T("CFile Fail."));
					}
										
					if (pbj->tSoftSim.pBuf && pbj->tSoftSim.dwSize != 0 )
					{
						IBMFile* pBMFile = (IBMFile*)pBMFileInterface;				
						pBMFile->SetCurCode(pbj->tSoftSim.pBuf, pbj->tSoftSim.dwSize);
						pThis->m_lstSoftSim.pop_front();
						bRet = TRUE;
					}
				}
				
			}
			SAFE_DELETE_ARRAY(pFile);
			if (!bRet)
			{
				return E_FAIL;
			}
			
		}
		// end

	} while(0);

    return S_OK;
}

HRESULT CBMOprObserver::OnOperationEnd( DWORD dwOprCookie, 
														 LPCWSTR cbstrFileID,
														 LPCWSTR cbstrFileType, 
														 LPCWSTR cbstrOperationType,
														 DWORD dwResult,
														 LPVOID pBMFileInterface)
{
    UNUSED_ALWAYS( cbstrFileID );
    
	USES_CONVERSION;
	
	HRESULT hr = NULL;
	PBOOTMODEOBJ_T pStruct = NULL;
	CString strErrMsg = _T("");
	_BMOBJ * pbj = NULL;
	if( !pThis->m_mapBMObj.Lookup(dwOprCookie,pbj) ||  NULL == pbj )	
	{
		return E_FAIL;
	}

	hr = pThis->m_pBMAF->BMAF_GetBootModeObjInfo(dwOprCookie,(LPVOID*)&pStruct);
	
	if( FAILED(hr) || pStruct == NULL)
    {
		return E_FAIL;
	}

	CString strFileID = W2T( (LPWSTR) cbstrFileID);

	if(OPR_SUCCESS != dwResult)
	{
		if( OPR_FAIL != dwResult)
		{
			pThis->GetOprErrorCodeDescription(dwResult,pbj->szErrorMsg,_MAX_PATH);
			if(  dwResult == BSL_CHIPID_NOT_MATCH && !pThis->m_strCodeChipID.IsEmpty() )
			{
				CString strChipID;
				strChipID.Format(_T(" [CodeVer:%s]"),pThis->m_strCodeChipID.operator LPCTSTR());
				_tcscat(pbj->szErrorMsg,strChipID);	
				
				hr = pThis->m_pBMAF->BMAF_GetBootModeObjInfo(dwOprCookie,(LPVOID*)&pStruct);
		
				if( SUCCEEDED(hr) )
				{
					LPBYTE lpReadBuffer = NULL; 
					DWORD dwReadSize = 0;			
				
					lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
					dwReadSize = pStruct->pSnapin->GetReadBufferSize();
				
					if( lpReadBuffer != NULL && dwReadSize >= sizeof( DWORD) )
					{
						DWORD dwSoruceValue, dwDestValue;            
						dwSoruceValue =  *(DWORD *)&lpReadBuffer[ 0 ];    
						dwDestValue   = 0;
						CONVERT_INT( dwSoruceValue, dwDestValue); 
						pbj->dwChipID = dwDestValue;
						CString strMduChipID;
						strMduChipID.Format(_T(" [ModuleVer:%08X]"),dwDestValue);
						_tcscat(pbj->szErrorMsg,strMduChipID);
					}
				}
			}
			else if( g_theApp.m_bNeedUpRefInfo && (_tcsicmp( W2T( (LPWSTR) cbstrFileType), _T("ReadSN")) == 0 || _tcsicmp( W2T( (LPWSTR) cbstrFileType), _T("ReadSN2")) == 0) )
			{
				pbj->szErrorMsg[0] = '\0';
				return S_OK;
			}
            else if(!(g_theApp.m_bStopDownloadIfOldMemory) && dwResult >= BSL_REP_OLD_MEMORY_REGISTER_CHANGE_ERROR && dwResult <= BSL_REP_OLD_MEMORY_REGISTER_EMMC_RPMB_ERROR)
            {
                pbj->dwOldMemoryType = dwResult;
                return S_OK;
            }
			
		}
		return E_FAIL;
	}  

	if(  ( _tcsicmp(W2T( (LPWSTR)cbstrFileID),_T("CheckBaud")) == 0 || _tcsicmp(W2T( (LPWSTR)cbstrFileID),_T("2ndCheckBaud")) == 0 )
		&& _tcsicmp( W2T( (LPWSTR)cbstrOperationType),_T("GetCheckBaudCrcType")) == 0 )
	{ 
		hr = pThis->m_pBMAF->BMAF_GetBootModeObjInfo(dwOprCookie,(LPVOID*)&pStruct);

		if( SUCCEEDED(hr) )
		{
			LPBYTE lpReadBuffer = NULL; 
			DWORD dwReadSize = 0;

			lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
			dwReadSize = pStruct->pSnapin->GetReadBufferSize();   

			if(lpReadBuffer != NULL && dwReadSize >= 4)
			{
				int nCrcType = (int)(*(DWORD*)lpReadBuffer);
				if(nCrcType == -1)
				{
					_tcscpy(pbj->szErrorMsg,_T("Can not confirm crc type"));
					return E_FAIL;
				}
				else
				{
					pbj->nStage = nCrcType == 1? 1: 2;
				}
			}
			else
			{
				_tcscpy(pbj->szErrorMsg,_T("Can not confirm crc type"));
				return E_FAIL;
			}			
		}

	}
    
    if(  (_tcsnicmp( W2T( (LPWSTR) cbstrFileType), _T("CODE"),4) == 0 || 
		  _tcsicmp( W2T( (LPWSTR) cbstrFileType), _T("READ_CHIPID")) == 0 ) &&
          _tcsicmp( W2T( (LPWSTR) cbstrOperationType),_T("ReadChipType")) == 0 ) 
    {
        
		LPBYTE lpReadBuffer = NULL; 
		DWORD dwReadSize = 0;          
		lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
		dwReadSize = pStruct->pSnapin->GetReadBufferSize();         
		if( lpReadBuffer == NULL || dwReadSize < sizeof( DWORD) )
		{
			return E_FAIL;
		}
		
		DWORD dwSoruceValue, dwDestValue;            
		dwSoruceValue =  *(DWORD *)&lpReadBuffer[ 0 ];    
		dwDestValue   = 0;
		CONVERT_INT( dwSoruceValue, dwDestValue); 
		pbj->dwChipID = dwDestValue;
      
        return S_OK;
    }

	if( _tcsicmp( W2T( (LPWSTR)cbstrFileType), _T("READ_RF_CHIP_TYPE")) == 0 &&
		_tcsicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadTransceiverType")) == 0 )
	{
		LPBYTE lpReadBuffer = NULL; 
		DWORD dwReadSize = 0;
		lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
		dwReadSize = pStruct->pSnapin->GetReadBufferSize();

		if( lpReadBuffer == NULL || dwReadSize < sizeof( DWORD) )
		{
			_tcscpy(pbj->szErrorMsg,_T("Read RF chip type failed."));
			return E_FAIL;
		}
		pbj->dwRFChipType = *(DWORD*)lpReadBuffer;
		return S_OK;
	}

	if( _tcsicmp(W2T( (LPWSTR)cbstrFileID),_T("ReadDluid")) == 0 
		&& _tcsnicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadFlash"),9) == 0 )
	{        
		LPBYTE lpReadBuffer = NULL; 
		DWORD dwReadSize = 0;          
		lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
		dwReadSize = pStruct->pSnapin->GetReadBufferSize();         
		if( lpReadBuffer == NULL || dwReadSize < DLUID_SIZE )
		{
			return E_FAIL;
		}
		
		DLUID_INFO dluid = {0};
		dluid.pBuf = lpReadBuffer;
		dluid.dwSize = dwReadSize;
		if(pThis->GetDluidInfo(dluid))
		{
			_tcscpy(pbj->dluid.szUID,dluid.szUID);
		}
		else
		{
			strErrMsg.Format(_T("Read Dluid failed."));
			_tcscpy(pbj->szErrorMsg,strErrMsg);
            return E_FAIL;
		}		
        return S_OK;
    }

    if( _tcsicmp( W2T( (LPWSTR)cbstrFileType), _T("ReadNandBlockInfo")) == 0 &&
        _tcsicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadNandBlockInfo")) == 0 &&
        pThis->m_sheetSettings.m_pageCustomization.GetBadFlashRate() >= 0)
    {

        LPBYTE lpReadBuffer = NULL; 
        DWORD dwReadSize = 0;

        lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
        dwReadSize = pStruct->pSnapin->GetReadBufferSize();

        if( lpReadBuffer == NULL || dwReadSize < sizeof( DWORD)*3 )
        {
            strErrMsg.Format(_T("Read Nand Block Info failed."));
            _tcscpy(pbj->szErrorMsg,strErrMsg);
            return E_FAIL;
        }
        LPDWORD pDw = (LPDWORD)lpReadBuffer;

        DWORD dwFlag = 0;
        DWORD dwTotalBlock = 0;
        DWORD dwBadBlock = 0;

        dwFlag = *(pDw);
        dwTotalBlock = *(pDw+1);
        dwBadBlock = *(pDw+2);

        if((((float)dwBadBlock/(float)dwTotalBlock)*100) > pThis->m_sheetSettings.m_pageCustomization.GetBadFlashRate())
        {
            strErrMsg.Format(_T("Bad block rate is bigger than %f. Total Block:%d, Bad Block:%d"),(float)(pThis->m_sheetSettings.m_pageCustomization.GetBadFlashRate())/100.0f, dwTotalBlock, dwBadBlock);
            _tcscpy(pbj->szErrorMsg,strErrMsg);
            return E_FAIL;
        }

        return S_OK;
    }

    if( 
        _tcsicmp( W2T( (LPWSTR) cbstrFileID), _T("ReadDUTInfo")) == 0 &&
        _tcsnicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadFlash"),9) == 0
        ) 
    {
        LPBYTE lpReadBuffer = NULL; 
        DWORD dwReadSize = 0;          
        lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
        dwReadSize = pStruct->pSnapin->GetReadBufferSize();         
        if( lpReadBuffer == NULL || dwReadSize < pThis->m_dwMaxUbootLen )
        {
            _tcscpy(pbj->szErrorMsg,_T("Fail to ReadDUTInfo."));
            return E_FAIL;
        }
        DUT_KEY_T stDutKey;
        if (pThis->GetDUTKeyInfo(lpReadBuffer,dwReadSize,stDutKey))
        {
            
            if(! pThis->CheckKeyMatch(stDutKey,pbj->szErrorMsg))
            {
                return E_FAIL;
            }
        }
        return S_OK;
    }

#ifdef _RESEARCH
	if (
			_tcsicmp( W2T( (LPWSTR) cbstrFileType), _T("REF_INFO")) == 0    &&
			g_theApp.m_bNeedUpRefInfo && _tcsicmp( W2T( (LPWSTR)cbstrOperationType ), _T("ReadRefInfo")) == 0
		)
	{
		LPBYTE lpReadBuffer = NULL; 
		DWORD dwReadSize = 0;          
		lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
		dwReadSize = pStruct->pSnapin->GetReadBufferSize();         
		if( pbj->lpRefData && lpReadBuffer && MAX_REF_INFO_LEN == dwReadSize )
		{
			memcpy(pbj->lpRefData,lpReadBuffer,dwReadSize);
		}
	}
#endif

    if( 
        _tcsicmp( W2T( (LPWSTR) cbstrFileType), _T("READ_CHIPUID")) == 0    &&
        _tcsicmp( W2T( (LPWSTR) cbstrOperationType),_T("ReadChipUID")) == 0 
       ) 
    {

        LPBYTE lpReadBuffer = NULL; 
        DWORD dwReadSize = 0;          
        lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
        dwReadSize = pStruct->pSnapin->GetReadBufferSize();         
        if( lpReadBuffer == NULL || dwReadSize < sizeof( DWORD)*2 )
        {
            _tcscpy(pbj->szErrorMsg,_T("Length of ChipUID is invalid."));
            return E_FAIL;
        }
		if(dwReadSize == sizeof( DWORD)*2)
		{
			LPDWORD pDw = (LPDWORD)lpReadBuffer; // blk0+blk1,little Endian
			pThis->ParseChipUID((*pDw),(*(pDw+1)),pbj->szChipUID,CHIP_UID_LEN);
		}
		else
		{
			strncpy(pbj->szChipUID,(const char *)lpReadBuffer,CHIP_UID_LEN);
		}
        return S_OK;
    }
	
    if( (_tcsicmp( W2T( (LPWSTR) cbstrFileType), _T("CHECK_MCPTYPE")) == 0  ||
		 _tcsicmp( W2T( (LPWSTR) cbstrFileType), _T("READFLASHTYPE")) == 0 )&&
        _tcsicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadFlashType")) == 0 )
    {
	
        LPBYTE lpReadBuffer = NULL; 
        DWORD dwReadSize = 0;
      
        lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
        dwReadSize = pStruct->pSnapin->GetReadBufferSize();
       
        if( lpReadBuffer == NULL || dwReadSize < sizeof( DWORD)*4 )
        {
			strErrMsg.Format(_T("Read flash type failed."));
			_tcscpy(pbj->szErrorMsg,strErrMsg);
            return E_FAIL;
        }
        LPDWORD pDw = (LPDWORD)lpReadBuffer;
		CONVERT_INT((*pDw),    pbj->aFlashType[0]);  // MID
		CONVERT_INT((*(pDw+1)),pbj->aFlashType[1]);  // DID
		CONVERT_INT((*(pDw+2)),pbj->aFlashType[2]);  // EID  
		CONVERT_INT((*(pDw+3)),pbj->aFlashType[3]);  // SUPPORT, not used now.

		//Check MCP Type;
		CString strMCPType;
		if(pbj->aFlashType[2] != MAXDWORD)
		{
			strMCPType.Format(_T("%X-%X-%X"),pbj->aFlashType[0],pbj->aFlashType[1],pbj->aFlashType[2]);
		}
		else
		{
			strMCPType.Format(_T("%X-%X"),pbj->aFlashType[0],pbj->aFlashType[1]);
		}
		BOOL bMatch = FALSE;
		CString strDesc = pThis->m_sheetSettings.GetMCPTypeDesc(strMCPType.operator LPCTSTR(),bMatch);
		
		CString strIdDesc;
		strIdDesc.Format(_T("ID: %s, DESC: %s"),
			strMCPType.operator LPCTSTR(),strDesc.operator LPCTSTR());			
		((CDLoaderView*)pThis->GetActiveView())->SetMcpType((int)dwOprCookie,strIdDesc);
		
		strDesc.MakeLower();
		_tcscpy(pbj->szMcpInfo,strDesc);
		if(_tcsicmp( W2T( (LPWSTR) cbstrFileType), _T("CHECK_MCPTYPE")) == 0 )
		{					
			CString strTmp = strDesc;
			int nFind = strTmp.ReverseFind('-');
			if(nFind != -1) strTmp = strTmp.Left(nFind);
			if( !bMatch )
			{
				if(strDesc.IsEmpty())
				{
					strErrMsg.Format(_T("Invalid MCP Type, [ID: %s]."),
						strMCPType.operator LPCTSTR());
				}
				else
				{
					strErrMsg.Format(_T("Invalid MCP Type, [ID: %s, DESC: %s]."),
						strMCPType.operator LPCTSTR(),strDesc.operator LPCTSTR());
				}
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}
		}				
        return S_OK;
    }

	if( _tcsicmp( W2T( (LPWSTR)cbstrFileType), _T("READ_FLASHINFO")) == 0 &&
        _tcsicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadFlashInfo")) == 0 )
    {
		
        LPBYTE lpReadBuffer = NULL; 
        DWORD dwReadSize = 0;

		if ( g_theApp.m_bCMDFlash )
		{
			CString strInfo;
			strInfo.Format(_T("%s flash=NAND"),EZ_MODE_REPORT_FLAG);
			g_theApp.ReportToPipe(strInfo, FALSE);
		}
		
        lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
        dwReadSize = pStruct->pSnapin->GetReadBufferSize();
		
        if( lpReadBuffer == NULL || dwReadSize < sizeof( DWORD)*3 )
        {
			strErrMsg.Format(_T("Read flash info failed."));
			_tcscpy(pbj->szErrorMsg,strErrMsg);
            return E_FAIL;
        }
        LPDWORD pDw = (LPDWORD)lpReadBuffer;

		DWORD dwFlag = 0;
		DWORD dwBlock = 0;
		DWORD dwPage = 0;
		CONVERT_INT((*pDw),    dwFlag);   // FLAG
		CONVERT_INT((*(pDw+1)),dwBlock);  // BLOCK
		CONVERT_INT((*(pDw+2)),dwPage);   // PAGE

		if(dwFlag!=0)
		{
			strErrMsg.Format(_T("Unsupported flash info flag [0x%X]."),dwFlag);
			_tcscpy(pbj->szErrorMsg,strErrMsg);
            return E_FAIL;
		}
		BOOL bBlockK = FALSE;
		if(dwBlock >= 1024)
		{
			bBlockK = TRUE;
			dwBlock = dwBlock / 1024;
		}
		BOOL bPageK = FALSE;
		if(dwPage >= 1024)
		{
			bPageK = TRUE;
			dwPage = dwPage / 1024;
		}
		
		//Check MCP Type;
		CString szBlockPageSize;	
		szBlockPageSize.Format(_T("_b%d%s_p%d%s"),dwBlock,bBlockK?_T("k"):_T(""),dwPage,bPageK?_T("k"):_T(""));

		DWORD count=0;
		if( !pThis->m_mapPBInfo.Lookup(szBlockPageSize,count) || count == 0)
		{
			strErrMsg.Format(_T("Unsupported block-page, [%s]."),szBlockPageSize);
			_tcscpy(pbj->szErrorMsg,strErrMsg);
			return E_FAIL;
		}
		
		_tcscpy(pbj->szBlockPageSize,szBlockPageSize);
  
        return S_OK;
    }

	if( _tcsicmp( W2T( (LPWSTR)cbstrFileType), _T("Soft_SIM")) == 0 &&
        _tcsicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadFlashUID")) == 0 )
    {

		LPBYTE lpReadBuffer = NULL; 
        DWORD dwReadSize = 0;
		
        lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
        dwReadSize = pStruct->pSnapin->GetReadBufferSize();
		
        if( lpReadBuffer == NULL || dwReadSize < 8 )
        {
			strErrMsg.Format(_T("Read flash UID failed."));
			_tcscpy(pbj->szErrorMsg,strErrMsg);
            return E_FAIL;
        }
					
		TCHAR	szWUID[100]	= {0};
		char	szUID[100]	= {0};
		pThis->Buf2TString(lpReadBuffer,dwReadSize,szWUID,100);
#if defined(_UNICODE) || defined(UNICODE)
		WideCharToMultiByte(CP_ACP,0,szWUID,100,szUID,100,NULL,NULL);
#else
		strcpy(szUID,szWUID);
#endif
		if(strlen(szUID) <= FLASH_UID_LEN)
		{
			strcpy(pbj->szFlashUID,szUID);
		}
		else
		{
			memcpy(pbj->szFlashUID,szUID,FLASH_UID_LEN);
		}
		return S_OK;
	}

	//read EID
    if(_tcsicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadSoftSimEID")) == 0 )
    {
		
		LPBYTE lpReadBuffer = NULL; 
        DWORD dwReadSize = 0;
		
        lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
        dwReadSize = pStruct->pSnapin->GetReadBufferSize();
		
        if( lpReadBuffer == NULL || dwReadSize < 10 )
        {
			strErrMsg.Format(_T("Read softSIM EID failed."));
			_tcscpy(pbj->szErrorMsg,strErrMsg);
            return E_FAIL;
        }
						
		TCHAR	szTEID[100]	= {0};
		pThis->Buf2TString(lpReadBuffer,dwReadSize,szTEID,100);

		CString strSimFile = GetStrFileTitle(pbj->tSoftSim.szFile);
		if (-1 == strSimFile.Find(szTEID))
		{
			strErrMsg.Format(_T("SoftSIM EID don't match with file name."));
			_tcscpy(pbj->szErrorMsg,strErrMsg);
            return E_FAIL;
		}
		return S_OK;
	}
	
	
	//read sn from flash
	if( (_tcsicmp( W2T( (LPWSTR) cbstrFileType), _T("ReadSN")) == 0 &&
		_tcsicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadFlash")) == 0) ||
        ( _tcsicmp( W2T( (LPWSTR) cbstrFileType), _T("ReadSN2")) == 0 &&
		_tcsicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadFlashByID")) == 0))
    {
		hr = pThis->m_pBMAF->BMAF_GetBootModeObjInfo(dwOprCookie,(LPVOID*)&pStruct);
		
		if( SUCCEEDED(hr) )
        {
            LPBYTE lpReadBuffer = NULL; 
            DWORD dwReadSize = 0;
        
            lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
            dwReadSize = pStruct->pSnapin->GetReadBufferSize();            

			if(lpReadBuffer != NULL && dwReadSize >0)
			{	
				CPhaseCheckBuild pcb;
				if(dwReadSize < PRODUCTION_INFO_SIZE )
				{
					LPBYTE pBuf = new BYTE[PRODUCTION_INFO_SIZE];
					if(pBuf != NULL)
					{
						memset(pBuf,0xFF,PRODUCTION_INFO_SIZE);
						memcpy(pBuf,lpReadBuffer,dwReadSize);
						pcb.FindSnFrom8K(pBuf,PRODUCTION_INFO_SIZE,(BYTE*)pbj->szSN,X_SN_LEN);
						delete []  pBuf;
						pBuf = NULL;
					}			
				}
				else
				{
					pcb.FindSnFrom8K(lpReadBuffer,dwReadSize,(BYTE*)pbj->szSN,X_SN_LEN);	
				}						
			}

        }
        return S_OK;
    } 
	
	/************************************************************************/
	/*  backup NV                                                           */
	/************************************************************************/
    if( (_tcsnicmp( W2T( (LPWSTR) cbstrFileID), _T("NV"), 2 ) == 0 &&
		 pThis->m_sheetSettings.IsBackupNV(W2T( (LPWSTR) cbstrFileID)) &&
		_tcsnicmp( W2T( (LPWSTR) cbstrFileType), _T("NV"), 2 ) == 0 &&
		_tcsnicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadFlash"),9) == 0 ) ||
		(_tcsnicmp( W2T( (LPWSTR) cbstrFileID), _T("_BKF_NV"),7) == 0 &&
		_tcsnicmp( W2T( (LPWSTR) cbstrFileType), _T("ReadFlash"),9) == 0 &&
        _tcsnicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadFlash"),9) == 0))
    {
		IBMFile* pBMFile = (IBMFile*)pBMFileInterface;
		
		hr = pThis->m_pBMAF->BMAF_GetBootModeObjInfo(dwOprCookie,(LPVOID*)&pStruct);
		
		if( SUCCEEDED(hr) )
        {
            LPBYTE lpReadBuffer = NULL; 
            DWORD dwReadSize = 0;
            DWORD dwCodeSize = 0;
			LPVOID pDestCode = NULL;
			CString strNVID = strFileID;
          
            lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
            dwReadSize = pStruct->pSnapin->GetReadBufferSize();
         
            if( lpReadBuffer == NULL || dwReadSize == 0)
            {
				strErrMsg.Format(_T("Read Flash (NV) failed, [bufptr=0x%X,size=0x%x]."),lpReadBuffer,dwReadSize);
				_tcscpy(pbj->szErrorMsg,strErrMsg);
                return E_FAIL;
            }           
            
			if( _tcsnicmp( W2T( (LPWSTR) cbstrFileType), _T("NV"), 2 ) == 0 )
			{				
				const LPVOID pSrcCode = pBMFile->GetCurCode();
				dwCodeSize = pBMFile->GetCurCodeSize();
				pDestCode = pSrcCode;
			}
			else
			{		
				if(_tcsnicmp( strNVID, _T("_BKF_NV"),7) == 0)
				{
					strNVID = strNVID.Mid(5);
				}

				int nIndex = pThis->m_sheetSettings.GetDLNVIDIndex(strNVID);
				if(nIndex == -1)
				{				
					return E_FAIL;
				}

				PFILE_INFO_T pNvFileInfo = NULL;
				pThis->m_sheetSettings.GetFileInfo(strNVID.operator LPCTSTR(),(LPDWORD)&pNvFileInfo);
				if(pThis->m_sheetSettings.IsEnableRFChipType() && pNvFileInfo && pNvFileInfo->isSelByRf == 1)
				{
					BACKUP_INFO_PTR pNVInfo = pThis->m_mapMultiNVInfo[std::make_pair(pbj->dwRFChipType,strNVID)];
					if (NULL == pNVInfo)
					{
						CString strRfChip;
						pThis->m_sheetSettings.GetRFChipName(pbj->dwRFChipType,strRfChip);
						strErrMsg.Format(_T("Not find [id:%d,name:%s] RF nv."),pbj->dwRFChipType,strRfChip);
						_tcscpy(pbj->szErrorMsg,strErrMsg.operator LPCTSTR());	
						return E_FAIL;
					}
					pbj->tNVBackup[nIndex].Clear();
					pbj->tNVBackup[nIndex].dwSize = pNVInfo->dwSize;				
					pbj->tNVBackup[nIndex].pBuf = new BYTE[pNVInfo->dwSize];

					if(pbj->tNVBackup[nIndex].pBuf != NULL)
					{
						memcpy(pbj->tNVBackup[nIndex].pBuf, pNVInfo->pBuf,pNVInfo->dwSize);
					}
					else
					{
						_tcscpy(pbj->szErrorMsg,_T("Multi nv memory full!"));		
						return E_FAIL;
					}
				}

				dwCodeSize = pbj->tNVBackup[nIndex].dwSize;
				pDestCode = (LPVOID)(pbj->tNVBackup[nIndex].pBuf);
			}
//////////////////////////////////////////////////////////////////////////
            // Check NV struct, lpReadBuffer will be changed if it is the driver level endian
			BOOL _bBigEndian = TRUE;
			if(!XCheckNVStructEx(lpReadBuffer,dwReadSize,_bBigEndian,TRUE))
			{
				_tcscpy(pbj->szErrorMsg,_T("NV data in device is crashed."));
				return E_FAIL;
			}
            
            //check NV Struct in nv file
            if(!XCheckNVStructEx((LPBYTE)pDestCode,dwCodeSize,_bBigEndian,FALSE))
            {
                _tcscpy(pbj->szErrorMsg,_T("NV data in nvitem.bin is crashed."));
                return E_FAIL;
            }

//////////////////////////////////////////////////////////////////////////
            // check calibration flag		
			CString strErr=_T("");
			if (!pThis->CheckCalibration(strNVID.operator LPCTSTR(), lpReadBuffer,dwReadSize,strErr))
			{
				_tcscpy(pbj->szErrorMsg,strErr.operator LPCTSTR() );
				return E_FAIL;	
			}
//////////////////////////////////////////////////////////////////////////
			PNV_BACKUP_ITEM_T pNvBkpItem = NULL;
			int nCount = pThis->m_sheetSettings.GetNvBkpItemCount(strNVID.operator LPCTSTR());	
			BOOL bReplace = FALSE;
			BOOL bContinue = FALSE;
			for(int k=0;k<nCount;k++)
			{
				bReplace = FALSE;
				bContinue = FALSE;
				
				pNvBkpItem = pThis->m_sheetSettings.GetNvBkpItemInfo(k,strNVID.operator LPCTSTR());
                if(NULL == pNvBkpItem)
                {
                    strErr.Format(_T("GetNvBkpItemInfo fail,[ID=%s,Index = %d]"),strNVID.operator LPCTSTR(),k);
                    _tcscpy(pbj->szErrorMsg,strErr.operator LPCTSTR() );
                    return E_FAIL;	
                }
                if (!pNvBkpItem->wIsBackup)
                {
                    continue;
                }
				
				int nNvBkpFlagCount = pNvBkpItem->dwFlagCount;
				if(nNvBkpFlagCount > MAX_NV_BACKUP_FALG_NUM)
				{
					nNvBkpFlagCount = MAX_NV_BACKUP_FALG_NUM;
				}
				for(int m=0;m<nNvBkpFlagCount;m++)
				{
					if(_tcscmp(pNvBkpItem->nbftArray[m].szFlagName,_T("Replace"))==0)
					{
						if(pNvBkpItem->nbftArray[m].dwCheck == 1)
							bReplace = TRUE;
					}
					else if(_tcscmp(pNvBkpItem->nbftArray[m].szFlagName,_T("Continue"))==0)
					{
						if(pNvBkpItem->nbftArray[m].dwCheck == 1)
							bContinue = TRUE;
					}
				}
				
                if(_tcsnicmp(pNvBkpItem->szItemName,g_CaliFlagTable[E_GSM_CALI].szCaliKeyWord,_tcslen(g_CaliFlagTable[E_GSM_CALI].szCaliKeyWord))==0)      //_T("Calibration")        
				{
#ifndef _SPUPGRADE
					if(pNvBkpItem->wIsBackup == 1)
#endif
					{
                        WORD wNVItemID = (WORD)(pNvBkpItem->dwID&0xFFFF);
                        if(wNVItemID == 0xFFFF)
                        {
                            wNVItemID = g_CaliFlagTable[E_GSM_CALI].wDefNvID;//GSM_CALI_ITEM_ID;
                        }
						DWORD dwErrorRCID = GSMCaliPreserve( wNVItemID,(LPBYTE)pDestCode, dwCodeSize, lpReadBuffer, dwReadSize, 
															  bReplace, bContinue);
						if(  dwErrorRCID !=0  )
						{
							strErrMsg = GetErrorDesc( dwErrorRCID );
                            strErrMsg.Format( _T("ID=%d,Name:\"%s\",Reason:\"%s\""), wNVItemID,pNvBkpItem->szItemName,strErrMsg.operator LPCTSTR() );
							_tcscpy(pbj->szErrorMsg,strErrMsg);
							return E_FAIL;
						}						
					}
					continue;					
				}
                else if(_tcsnicmp(pNvBkpItem->szItemName,g_CaliFlagTable[E_TD_CALI].szCaliKeyWord,_tcslen(g_CaliFlagTable[E_TD_CALI].szCaliKeyWord))==0)       //_T("TD_Calibration")     
                {					
#ifndef _SPUPGRADE
					if(pNvBkpItem->wIsBackup == 1)
#endif
					{
                        WORD wNVItemID = (WORD)(pNvBkpItem->dwID&0xFFFF);
                        if(wNVItemID == 0xFFFF)
                        {
                            wNVItemID = g_CaliFlagTable[E_TD_CALI].wDefNvID;//XTD_CALI_ITEM_ID;
                        }
						DWORD dwErrorRCID = XTDCaliPreserve(wNVItemID, (LPBYTE)pDestCode, dwCodeSize, lpReadBuffer, dwReadSize, 
							                                 bReplace,bContinue);
						if(  dwErrorRCID !=0  )
						{
							strErrMsg = GetErrorDesc( dwErrorRCID );
							strErrMsg.Format( _T("ID=%d,Name:\"%s\",Reason:\"%s\""),wNVItemID,pNvBkpItem->szItemName,strErrMsg.operator LPCTSTR() );
							_tcscpy(pbj->szErrorMsg,strErrMsg);
							return E_FAIL;
						}
					}
				}
                else if(_tcsnicmp(pNvBkpItem->szItemName,g_CaliFlagTable[E_LTE_CALI].szCaliKeyWord,_tcslen(g_CaliFlagTable[E_LTE_CALI].szCaliKeyWord))==0)       //_T("LTE_Calibration")     
				{					
#ifndef _SPUPGRADE
					if(pNvBkpItem->wIsBackup == 1)
#endif
					{
						//结构还不稳定，暂时采用简单的全备份方式
						WORD wNVItemID = (WORD)(pNvBkpItem->dwID&0xFFFF);
						if(wNVItemID == 0xFFFF)
						{
							wNVItemID = g_CaliFlagTable[E_LTE_CALI].wDefNvID;//LTE_CALI_ITEM_ID;
						}

						DWORD dwErrorRCID = XPreserveNVItem( wNVItemID,(LPBYTE)pDestCode, dwCodeSize, 
							lpReadBuffer, dwReadSize, bReplace, bContinue);
						if(  dwErrorRCID !=0  )
						{
							strErrMsg = GetErrorDesc( dwErrorRCID );
							strErrMsg.Format( _T("ID=%d,Name:\"%s\",Reason:\"%s\""), wNVItemID,pNvBkpItem->szItemName,strErrMsg.operator LPCTSTR() );
							_tcscpy(pbj->szErrorMsg,strErrMsg);
							return E_FAIL;
						}

					}
				}
				else if(_tcscmp(pNvBkpItem->szItemName,_T("IMEI"))==0)
				{
#ifndef _SPUPGRADE
					if(pNvBkpItem->wIsBackup == 1)
#endif
					{
						int nIMEIIdx = 0;
						DWORD dwErrorRCID = XPreserveIMEIs(  &pThis->m_aIMEIID, (LPBYTE)pDestCode,dwCodeSize,
															  lpReadBuffer, dwReadSize, nIMEIIdx, bReplace, bContinue);
						if(  dwErrorRCID != 0 && nIMEIIdx != -1 )
						{
							strErrMsg = GetErrorDesc( dwErrorRCID );
                            strErrMsg.Format( _T("ID=%d,Name:\"IMEI%d\",Reason:\"%s\""), pThis->m_aIMEIID.GetAt(nIMEIIdx),nIMEIIdx+1,strErrMsg.operator LPCTSTR() );
							_tcscpy(pbj->szErrorMsg,strErrMsg);
							return E_FAIL;
						}	
					}
					continue;					
				}
				else
				{
#ifdef _SPUPGRADE
					if(_tcscmp(pNvBkpItem->szItemName,_T("MMITest"))==0 || _tcscmp(pNvBkpItem->szItemName,_T("MMITest Result"))==0)
					{
						bContinue = TRUE;
					}
#endif
					if(pNvBkpItem->wIsBackup == 1 && pNvBkpItem->dwID != 0xFFFFFFFF)
					{						
						DWORD dwErrorRCID = XPreserveNVItem( (WORD)(pNvBkpItem->dwID),(LPBYTE)pDestCode, dwCodeSize, 
															 lpReadBuffer, dwReadSize, bReplace, bContinue);
						if(  dwErrorRCID !=0  )
						{
							strErrMsg = GetErrorDesc( dwErrorRCID );
                            strErrMsg.Format( _T("ID=%d,Name:\"%s\",Reason:\"%s\""),pNvBkpItem->dwID,pNvBkpItem->szItemName,strErrMsg.operator LPCTSTR() );
							_tcscpy(pbj->szErrorMsg,strErrMsg);
							return E_FAIL;
						}	
					}
					continue;	
				}
			}
			/************************************************************************/
			/* backup multi-language item, uint8 type and need not consider endian  */
			/************************************************************************/
			WORD wLangID = pThis->m_sheetSettings.GetLangNVItemID();
			if(pThis->m_sheetSettings.IsBackupLang() && wLangID != 0xFFFF)
			{					
				DWORD dwErrorRCID = XPreserveNVItem( wLangID,(LPBYTE)pDestCode, dwCodeSize, 
													 lpReadBuffer, dwReadSize);
				if(  dwErrorRCID !=0  )
				{
					strErrMsg = GetErrorDesc( dwErrorRCID );
					strErrMsg.Format( _T("Preserve multi-language fail [%s]."), strErrMsg.operator LPCTSTR() );
					_tcscpy(pbj->szErrorMsg,strErrMsg);
					return E_FAIL;
				}
			}
			
			if( _tcsnicmp( W2T( (LPWSTR) cbstrFileType), _T("NV"), 2 ) == 0 )
			{
				if(pThis->m_sheetSettings.IsNvBaseChange())
				{
					PFILE_INFO_T pNvFileInfo = NULL;
					pThis->m_sheetSettings.GetFileInfo(W2T( (LPWSTR)cbstrFileID),(LPDWORD)&pNvFileInfo);
					if(pNvFileInfo!=NULL)
					{
						int nPos = pThis->m_sheetSettings.GetNvNewBasePosition();
						if(nPos<0 || nPos>=MAX_BLOCK_NUM)
						{
							strErrMsg.Format(_T("The position index [%d] of the new Nv address is incorrect."),nPos);
							_tcscpy(pbj->szErrorMsg,strErrMsg);
							return E_FAIL;
						}
						
						pBMFile = (IBMFile*)pBMFileInterface;
						pBMFile->SetCurCodeBaseEx(pNvFileInfo->arrBlock[nPos].llBase);
					}			
				}
				CString strNVID = strFileID;
				if(_tcsnicmp( strNVID, _T("_BKF_NV"),7) == 0)
				{
					strNVID = strNVID.Mid(5);
				}

				int nIndex = pThis->m_sheetSettings.GetDLNVIDIndex(strNVID);
				if(nIndex == -1)
				{				
					return E_FAIL;
				}
				// backup to memory				
				if(pbj->tNVBackup[nIndex].pBuf == NULL || pbj->tNVBackup[nIndex].dwSize == 0)
				{
					pbj->tNVBackup[nIndex].pBuf= new BYTE[dwCodeSize];
					pbj->tNVBackup[nIndex].dwSize = dwCodeSize;
				}
				memcpy(pbj->tNVBackup[nIndex].pBuf,pDestCode,dwCodeSize);				
			}

			/* record IMEI */
			{
				DWORD dwIMEIOffset=0;
				DWORD dwIMEILen =0;						
				_TCHAR szWIMEI[100]={0};
				char szIMEI[100]={0};
				BOOL bBigEndian = TRUE;
				if (XFindNVOffsetEx(GSM_IMEI_ITEM_ID,(LPBYTE)pDestCode,dwCodeSize,dwIMEIOffset,dwIMEILen,bBigEndian,FALSE))
				{		
					BYTE bIMEI[100] ={0};
					memcpy(bIMEI,((LPBYTE)pDestCode) + dwIMEIOffset,dwIMEILen);							
					pThis->BCDToWString(bIMEI,dwIMEILen,szWIMEI,100);
#if defined(_UNICODE) || defined(UNICODE)
					WideCharToMultiByte(CP_ACP,0,szWIMEI,100,szIMEI,100,NULL,NULL);
#else
					strcpy(szIMEI,szWIMEI);
#endif
					if(strlen(szIMEI) <= X_SN_LEN)
					{
						strcpy(pbj->szIMEI,szIMEI);
					}
					else
					{
						memcpy(pbj->szIMEI,szIMEI,X_SN_LEN);
					}
#ifdef _RESEARCH
					((CDLoaderView*)pThis->GetActiveView())->SetIMEI((int)dwOprCookie,A2T(szIMEI));
#endif 
				}
			}
			
			if(pThis->m_sheetSettings.IsNVSaveToLocal())
			{
				if(!pThis->SaveBackupFileToLocal(E_SAVE_NV_TO_LOCAL,pbj,(BYTE*)pDestCode,dwCodeSize))
					return E_FAIL;
			} 

#ifdef _SPUPGRADE
			if(pThis->m_sheetSettings.IsSharkNand() && _tcslen(pbj->dluid.szUID)!=0)
			{
				CString strID = strFileID.Right(strFileID.GetLength()-5);
				CString strFileName;
				strFileName.Format(_T("%s\\%s_%s.bin"),pThis->m_strLocalBKFolder,pbj->dluid.szUID,strID);
				if(!SaveFileToLocal(strFileName,lpReadBuffer,dwReadSize))
				{
					strErrMsg.Format(_T("Save file to local faild, maybe space left is not enough."));
					_tcscpy(pbj->szErrorMsg,strErrMsg);
					return E_FAIL;
				}
			}
#endif
			
        }
		else
		{	
			return E_FAIL;		
		}
    }
	
	/************************************************************************/
	/* read nv 3 times if nv readed from phone is not correct               */
	/* check it by the nv memory in _BMOBJ.pBuf,if they are same            */
	/* it is validate                                                       */
	/************************************************************************/	
	if( _tcsnicmp(W2T( (LPWSTR)cbstrFileID),_T("_CHECK_NV"),9) == 0
		&& _tcsnicmp( W2T( (LPWSTR) cbstrFileType), _T("CHECK_NV"), 8 ) == 0 &&
		_tcsnicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadFlash"),9) == 0 )
	{
		CString strNVID = W2T( (LPWSTR)cbstrFileID);
		strNVID = strNVID.Mid(7);
		int nIndex = pThis->m_sheetSettings.GetDLNVIDIndex(strNVID);
		if(nIndex == -1)
			return E_FAIL;

		BOOL bValidate = TRUE;
		if(pbj->tNVBackup[nIndex].pBuf == NULL || pbj->tNVBackup[nIndex].dwSize == 0)
		{
			return E_FAIL;
		}

		hr = pThis->m_pBMAF->BMAF_GetBootModeObjInfo(dwOprCookie,(LPVOID*)&pStruct);		
		if( SUCCEEDED(hr) )
        {
            LPBYTE lpReadBuffer = NULL; 
            DWORD dwReadSize = 0;
       
            lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
            dwReadSize = pStruct->pSnapin->GetReadBufferSize();       

            if( lpReadBuffer == NULL )
            {
                return E_FAIL;
            }       

			BOOL _bBigEndian = TRUE;
			XCheckNVStructEx(lpReadBuffer,dwReadSize,_bBigEndian,TRUE);
			            
			// omit the first 2 crc bytes and 2 timestamp bytes.	
			if(memcmp(pbj->tNVBackup[nIndex].pBuf+4,lpReadBuffer+4,pbj->tNVBackup[nIndex].dwSize-4) != 0)
			{
				bValidate = FALSE;
			}

// #if 0
// 			FILE *pFile = fopen("D:\\nv1.bin","wb");
// 			fwrite(pbj->pBuf,1,pbj->dwBufSize,pFile);
// 			fclose(pFile);
// 
// 			pFile = fopen("D:\\nv2.bin","wb");
// 			fwrite(lpReadBuffer,1,pbj->dwBufSize,pFile);
// 			fclose(pFile);
// 
// #endif

			IBMFile* pBMFile = (IBMFile*)pBMFileInterface;
			pBMFile->SetCurCode((const LPVOID)(pbj->tNVBackup[nIndex].pBuf),pbj->tNVBackup[nIndex].dwSize);
			
			if(pThis->m_sheetSettings.IsNvBaseChange())
			{
				PFILE_INFO_T pNvFileInfo = NULL;
				pThis->m_sheetSettings.GetFileInfo(W2T( (LPWSTR)cbstrFileID),(LPDWORD)&pNvFileInfo);
				if(pNvFileInfo != NULL)
				{
					int nPos = pThis->m_sheetSettings.GetNvNewBasePosition();
					if(nPos<0 || nPos>=MAX_BLOCK_NUM)
					{
						strErrMsg.Format(_T("The position index [%d] of the new Nv address is incorrect."),nPos);
						_tcscpy(pbj->szErrorMsg,strErrMsg);
						return E_FAIL;
					}
					
					pBMFile->SetCurCodeBaseEx(pNvFileInfo->arrBlock[nPos].llBase);
				}		
			}

// #ifdef _DEBUG
//			static int ii= 0;
//			if(ii==0)
//			{
//				ii++;
// 				return E_FAIL;
//			}
// #endif

			if(bValidate)
			{
				return S_OK;
			}			
		}		
		return E_FAIL;
	}

	if( _tcsnicmp( W2T( (LPWSTR) cbstrFileType), _T("UDISK_IMG"),9) == 0 &&
        _tcsicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadSectorSize")) == 0 )
    {
        hr = pThis->m_pBMAF->BMAF_GetBootModeObjInfo(dwOprCookie,(LPVOID*)&pStruct);
		
        if( SUCCEEDED(hr) )
        {
            LPBYTE lpReadBuffer = NULL; 
            DWORD dwReadSize = 0;
			
            lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
            dwReadSize = pStruct->pSnapin->GetReadBufferSize();
          
            if( lpReadBuffer == NULL || dwReadSize < sizeof( DWORD) )
            {
                return E_FAIL;
            }
			
            DWORD dwSoruceValue, dwSectorSize;            
            dwSoruceValue =  *(DWORD *)&lpReadBuffer[ 0 ];    
            dwSectorSize   = 0;
            CONVERT_INT( dwSoruceValue, dwSectorSize);  
	
			EXT_IMG_INFO_PTR pDiskImg = NULL;
			if(pThis->m_mapUDiskIMg.Lookup(dwSectorSize,pDiskImg))
			{
				IBMFile* pBMFile = (IBMFile*)pBMFileInterface;
				if(!pBMFile->SetCurCode(pDiskImg->pBuf,pDiskImg->dwSize))
				{
					strErrMsg.Format(_T("Set udisk_img buffer to bootmodeplatform failed.[SetctorSize:%d]"),dwSectorSize);
					_tcscpy(pbj->szErrorMsg,strErrMsg);
					return E_FAIL;
				}
			}
			else
			{
				strErrMsg.Format(_T("Can not find udisk_img matching the sector size [%d]."),dwSectorSize);
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}
        }
        return S_OK;
    }


	if( _tcsnicmp( W2T( (LPWSTR) cbstrFileType), _T("CHIP_DSP"),8) == 0 &&
        _tcsicmp( W2T( (LPWSTR)cbstrOperationType),_T("ReadChipType")) == 0 )
    {
        hr = pThis->m_pBMAF->BMAF_GetBootModeObjInfo(dwOprCookie,(LPVOID*)&pStruct);
		
        if( SUCCEEDED(hr) )
        {
            LPBYTE lpReadBuffer = NULL; 
            DWORD dwReadSize = 0;
			
            lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
            dwReadSize = pStruct->pSnapin->GetReadBufferSize();
          
            if( lpReadBuffer == NULL || dwReadSize < sizeof( DWORD) )
            {
                return E_FAIL;
            }
			
            DWORD dwSoruceValue=0;
			DWORD dwChipID =0;            
            dwSoruceValue =  *(DWORD *)&lpReadBuffer[ 0 ];       
            CONVERT_INT( dwSoruceValue, dwChipID);  
	
			EXT_IMG_INFO_PTR pChipDsp = NULL;
			if(pThis->m_mapChipDsp.Lookup(dwChipID,pChipDsp))
			{
				IBMFile* pBMFile = (IBMFile*)pBMFileInterface;
				if(!pBMFile->SetCurCode(pChipDsp->pBuf,pChipDsp->dwSize))
				{
					strErrMsg.Format(_T("Set dsp buffer to bootmodeplatform failed.[ChipID:0x%08X]."),dwChipID);
					_tcscpy(pbj->szErrorMsg,strErrMsg);
					return E_FAIL;
				}
			}
			else
			{
				strErrMsg.Format(_T("Can not find DSP file matching the chip ID[0x%08X]."),dwChipID);
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}
        }
        return S_OK;
    }

	
	if( _tcsnicmp( strFileID, _T("_BKF_"), 5) == 0 &&
		_tcsnicmp( strFileID, _T("_BKF_NV"),7) != 0 &&
		_tcsnicmp( W2T( (LPWSTR) cbstrFileType ), _T("ReadFlash"),9) == 0 &&
		_tcsnicmp( W2T( (LPWSTR) cbstrOperationType ),_T("ReadFlash"),9) == 0 )
    {
		CString strID = strFileID.Right(strFileID.GetLength()-5);
		int nID = pThis->m_sheetSettings.IsBackupFile(strID);
		if(nID != -1)
		{
			hr = pThis->m_pBMAF->BMAF_GetBootModeObjInfo(dwOprCookie,(LPVOID *)&pStruct);
			
			if( SUCCEEDED(hr) )
			{
				LPBYTE lpReadBuffer = NULL; 
				DWORD dwReadSize = 0;
			
				lpReadBuffer = pStruct->pSnapin->GetReadBuffer();
				dwReadSize = pStruct->pSnapin->GetReadBufferSize();
			
				if( lpReadBuffer == NULL )
				{
					return E_FAIL;
				}
				pbj->tFileBackup[nID].dwSize = dwReadSize;
				pbj->tFileBackup[nID].pBuf = new BYTE[dwReadSize];
				memcpy(pbj->tFileBackup[nID].pBuf,lpReadBuffer,dwReadSize);

				if(  strID.CompareNoCase(_T("PhaseCheck")) == 0 && lpReadBuffer != NULL && dwReadSize >0 )
				{	
                    USES_CONVERSION;
					CPhaseCheckBuild pcb;
					if(dwReadSize < PRODUCTION_INFO_SIZE )
					{
						LPBYTE pBuf = new BYTE[PRODUCTION_INFO_SIZE];
						if(pBuf != NULL)
						{
							memset(pBuf,0xFF,PRODUCTION_INFO_SIZE);
							memcpy(pBuf,lpReadBuffer,dwReadSize);
							pcb.FindSnFrom8K(pBuf,PRODUCTION_INFO_SIZE,(BYTE*)pbj->szSN,X_SN_LEN);
							delete []  pBuf;
							pBuf = NULL;
						}			
					}
					else
					{
						pcb.FindSnFrom8K(lpReadBuffer,dwReadSize,(BYTE*)pbj->szSN,X_SN_LEN);
                        MISCDATA_TCL stMiscDataTCL;
                        memcpy(&stMiscDataTCL, pbj->tFileBackup[nID].pBuf+CUST_MISCDATA_OFFSET, sizeof(MISCDATA_TCL));
						stMiscDataTCL.CU[TCT_CU_REF_LEN-1] = '\0';
                        
                        if(pThis->IsSupportCU())
                        {
							CString strCU;
							strCU = pThis->m_sheetSettings.m_pageCustomization.GetCU();
                            if(pThis->m_sheetSettings.m_pageCustomization.IsChangedCU() && !strCU.IsEmpty())
                            {
                                if(!pThis->m_sheetSettings.CheckCU(strCU.operator LPCTSTR()))
                                {
                                    strErrMsg.Format(_T("Input CU and the SW version of CU does't match."));
                                    _tcscpy(pbj->szErrorMsg,strErrMsg);
                                    return E_FAIL;
                                }
                                else
                                {
									memset(stMiscDataTCL.CU,0x0,TCT_CU_REF_LEN);
									strncpy(stMiscDataTCL.CU,T2A(strCU), TCT_CU_REF_LEN);
                                    memcpy(pbj->tFileBackup[nID].pBuf+CUST_MISCDATA_OFFSET, &stMiscDataTCL, sizeof(MISCDATA_TCL));
                                }
                            }
                            else
                            {
                                CString strCuTemp = stMiscDataTCL.CU;
                                if(!pThis->m_sheetSettings.CheckCU(strCuTemp.operator LPCTSTR()))
                                {
                                    strErrMsg.Format(_T("Mobile phone CU and the SW version of CU does't match."));
                                    _tcscpy(pbj->szErrorMsg,strErrMsg);
                                    return E_FAIL;
                                }
                            }
                        }

                        if(pThis->IsSupportCheckRoot())
                        {
                            if(pThis->m_sheetSettings.m_pageCustomization.IsCheckRoot())
                            {
                                if(stMiscDataTCL.root_flag == 0x52)
                                {
                                    strErrMsg.Format(_T("The phone is root."));
                                    _tcscpy(pbj->szErrorMsg,strErrMsg);
                                    return E_FAIL;
                                }
                            }
                            else
                            {
                                stMiscDataTCL.root_flag = 0x4e;
                                // 0x4e 代表没有root
                                memcpy(pbj->tFileBackup[nID].pBuf+CUST_MISCDATA_OFFSET, &stMiscDataTCL, sizeof(MISCDATA_TCL));
                            }

                        }


                        if(pThis->IsSupportCheckInproduction())
                        {
                            stMiscDataTCL.inproduction = pThis->m_sheetSettings.m_pageCustomization.IsCheckInproduction();
                            memcpy(pbj->tFileBackup[nID].pBuf+CUST_MISCDATA_OFFSET, &stMiscDataTCL, sizeof(MISCDATA_TCL));
                        }
					}

                    if(pThis->m_sheetSettings.IsNVSaveToLocal())
                    {
                        if(!pThis->SaveBackupFileToLocal(E_SAVE_PHASECHECK_TO_LOCAL,pbj, pbj->tFileBackup[nID].pBuf,pbj->tFileBackup[nID].dwSize))
                            return E_FAIL;
                    } 
				}
                else if(strID.CompareNoCase(_T("ProdNv")) == 0 && lpReadBuffer != NULL && dwReadSize >0 )
                {
                    if(pThis->m_sheetSettings.IsNVSaveToLocal())
                    {
                        if(!pThis->SaveBackupFileToLocal(E_SAVE_PRODNV_TO_LOCAL,pbj, pbj->tFileBackup[nID].pBuf,pbj->tFileBackup[nID].dwSize))
                            return E_FAIL;
                    } 
                }

#ifdef _SPUPGRADE
				if(pThis->m_sheetSettings.IsSharkNand() && _tcslen(pbj->dluid.szUID)!=0)
				{
					CString strFileName;
					strFileName.Format(_T("%s\\%s_%s.bin"),pThis->m_strLocalBKFolder,pbj->dluid.szUID,strID);
					if(!SaveFileToLocal(strFileName,lpReadBuffer,dwReadSize))
					{
						strErrMsg.Format(_T("Save file to local faild, maybe space left is not enough."));
						_tcscpy(pbj->szErrorMsg,strErrMsg);
						return E_FAIL;
					}
				}
#endif

			}
			return S_OK;
		}
    }
	
	
    return S_OK;
}

HRESULT CBMOprObserver::OnFileOprStart( DWORD dwOprCookie,
										LPCWSTR cbstrFileID,
										LPCWSTR cbstrFileType, 
										LPVOID pBMFileInterface )
{
    UNUSED_ALWAYS( cbstrFileType );    
	USES_CONVERSION;
	
    if( _tcsnicmp( W2T((LPWSTR)cbstrFileID), _T("NV"),2) == 0 )
    {
        InterlockedIncrement( &(pThis->m_lStartNVRef) );
    }
	
    PFILE_INFO_T pFileInfo = NULL;
    IBMFile* pBMFile = (IBMFile*)pBMFileInterface;
    m_llCurCodeSize = pBMFile->GetCurCodeSizeEx();

	if(_tcsicmp(cbstrFileID,_T("RefInfo")))
	{
		::PostMessage( ((CDLoaderView*)pThis->GetActiveView())->GetSafeHwnd(), 
			BM_FILE_BEGIN, dwOprCookie, (LPARAM)&m_llCurCodeSize);	
	}

	CString strErrMsg = _T("");
	_BMOBJ * pbj = NULL;
	if( !pThis->m_mapBMObj.Lookup(dwOprCookie,pbj) ||  NULL == pbj )	
	{
		return E_FAIL;
	}

	if (0 == _tcsicmp(cbstrFileID,_T("_RESET_")) || 0 == _tcsicmp(cbstrFileID,_T("_POWEROFF_")))
	{
		pbj->bRMDev = TRUE;
	}
	
	CString strFileID = W2T( (LPWSTR)cbstrFileID);
	CString strFileType = W2T( (LPWSTR)cbstrFileType);
	if(strFileID.CompareNoCase(_T("FDLA")) == 0  && strFileType.CompareNoCase(_T("FDL1")) == 0 && pbj->nStage == 2)
	{
		pBMFile->SetCurFileType(_T("DONOTHING"));
	}

    if(strFileID.CompareNoCase(_T("2ndCheckBaud")) == 0  && strFileType.CompareNoCase(_T("CheckBaud")) == 0 && pbj->nStage == 2)
    {
        pBMFile->SetCurFileType(_T("DONOTHING"));
    }   

	if (g_theApp.IsSkipFile(strFileID.operator LPCTSTR()))
	{
		pBMFile->SetCurFileType(_T("DONOTHING"));
	}

	if( pThis->m_sheetSettings.IsEnableMultiFileBuf() && pBMFile->GetCurIsChangeCode() )
	{
		if(strFileID.CompareNoCase(_T("FDL")) != 0  && strFileID.CompareNoCase(_T("FDL2")) != 0)
		{
			pFileInfo = NULL;
			pThis->m_sheetSettings.GetFileInfo(strFileID,(LPDWORD)&pFileInfo);
			if(pFileInfo != NULL && pFileInfo->dwFlag != 0)
			{
				EXT_IMG_INFO_PTR pImg = NULL;
				pImg = pThis->m_mapMultiFileBuf[std::make_pair(pbj->dwChipID,strFileID)];
				if(pImg != NULL)
				{
					pBMFile->SetCurCode(pImg->pBuf,pImg->dwSize);
				}

				if(_tcsnicmp(W2T( (LPWSTR)cbstrFileID),_T("NV"),2) == 0 && pThis->m_sheetSettings.IsNvBaseChange())
				{
					PFILE_INFO_T pNvFileInfo = NULL;
					pThis->m_sheetSettings.GetFileInfo(W2T( (LPWSTR)cbstrFileID),(LPDWORD)&pNvFileInfo);
					if(pNvFileInfo != NULL)
					{
						int nPos = pThis->m_sheetSettings.GetNvNewBasePosition();
						if(nPos<0 || nPos>=MAX_BLOCK_NUM)
						{
							strErrMsg.Format(_T("The position index [%d] of the new Nv address is incorrect."),nPos);
							_tcscpy(pbj->szErrorMsg,strErrMsg);
							return E_FAIL;
						}
						
						pBMFile->SetCurCodeBaseEx(pNvFileInfo->arrBlock[nPos].llBase);
					}				
				}
			}			
		}
		
	}

#ifdef _SPUPGRADE
	if( pThis->m_sheetSettings.IsSharkNand()&&
		_tcslen(pbj->dluid.szUID)!= 0 &&
		_tcsnicmp( strFileID, _T("_BKF_"), 5) == 0 &&	
		_tcsicmp( W2T( (LPWSTR)cbstrFileType ), _T("ReadFlash2")) == 0 )
	{
		CString strID = strFileID.Mid(5);		
		CString strFilePath;
		strFilePath.Format(_T("%s\\%s_%s.bin"),pThis->m_strLocalBKFolder,pbj->dluid.szUID, strID);
		CFileFind finder;
		if(finder.FindFile(strFilePath))
		{	
			LPBYTE pBuf = NULL;
			DWORD dwSize = 0;			
			if(LoadFileFromLocal(strFilePath,pBuf,dwSize))
			{
				BOOL bMatch = FALSE;								
				int nID = pThis->m_sheetSettings.IsBackupFile(strID);
				if(nID != -1)
				{					
					pbj->tFileBackup[nID].dwSize = dwSize;
					pbj->tFileBackup[nID].pBuf = pBuf;	
					bMatch = TRUE;
				}
				else
				{
					nID = pThis->m_sheetSettings.GetDLNVIDIndex(strID);
					if(nID != -1)
					{
						pbj->tNVBackup[nID].dwSize = dwSize;
						pbj->tNVBackup[nID].pBuf = pBuf;	
						bMatch = TRUE;
					}
				}
				if(bMatch)
				{
					pBMFile->SetCurFileType(_T("DONOTHING"));
				}
				else
				{
					SAFE_DELETE_ARRAY(pBuf);
				}
			}
			else
			{
				strErrMsg.Format(_T("Load file from local failed."));
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}
		}
	}	

	/*
	// Compatible without DLUID
	// if DLUID is empty, need not read and download prodnv and miscdata
	pFileInfo = NULL;
	pThis->m_sheetSettings.GetFileInfo(strFileID,(LPDWORD)&pFileInfo);

	if( pThis->m_sheetSettings.IsSharkNand()&&
		_tcslen(pbj->dluid.szUID)==0 && 
		pFileInfo != NULL &&
		_tcsicmp(pFileInfo->arrBlock[0].szRepID,_T("prodnv")) == 0 ||
		_tcsicmp(pFileInfo->arrBlock[0].szRepID,_T("miscdata")) == 0))
	{
		pBMFile->SetCurFileType(_T("DONOTHING"));
	}
	*/
#endif

	if (pThis->m_sheetSettings.IsEnableRFChipType() && _tcsnicmp(W2T( (LPWSTR)cbstrFileID),_T("NV"),2) == 0)
	{
		CString strNVID(W2T( (LPWSTR)cbstrFileID));
		PFILE_INFO_T pNvFileInfo = NULL;
		pThis->m_sheetSettings.GetFileInfo(strNVID.operator LPCTSTR(),(LPDWORD)&pNvFileInfo);
		if( 
			pNvFileInfo && pNvFileInfo->isSelByRf == 1 &&
			pThis->m_sheetSettings.GetNvBkpItemCount(strNVID.operator LPCTSTR())	&&
			!pThis->m_sheetSettings.IsBackupNV(strNVID.operator LPCTSTR())			&&
			!pThis->m_sheetSettings.IsBackupNVFile(strNVID.operator LPCTSTR())
		  )
		{

			BACKUP_INFO_PTR pNVInfo = pThis->m_mapMultiNVInfo[std::make_pair(pbj->dwRFChipType,strNVID)];
			if (NULL == pNVInfo)
			{
				CString strRfChip;
				pThis->m_sheetSettings.GetRFChipName(pbj->dwRFChipType,strRfChip);
				strErrMsg.Format(_T("Not find [id:%d,name:%s] RF nv file."),pbj->dwRFChipType,strRfChip);
				_tcscpy(pbj->szErrorMsg,strErrMsg.operator LPCTSTR());	
				return E_FAIL;
			}
			
			if(!pBMFile->SetCurFileName(pNVInfo->szNVFile))
			{
				_tcscpy(pbj->szErrorMsg,_T("Set nv file fail!"));		
				return E_FAIL;
			}

		}
	}

	if( pThis->m_sheetSettings.IsMapPBFileBuf())
	{
		pFileInfo = NULL;
		pThis->m_sheetSettings.GetFileInfo(strFileID,(LPDWORD)&pFileInfo);
		
		if( pFileInfo != NULL && 
			pFileInfo->isSelByFlashInfo == 1 &&
			pFileInfo->dwFlag != 0 &&
			pThis->m_sheetSettings.IsBackupFile(strFileID) == -1) // need not backup, if backup, 
			                                                      // it will SetCurCode in OnOperationStart	
		{
			CString strKey = pbj->szBlockPageSize;
			
			if(strKey.IsEmpty())
			{
				strErrMsg.Format(_T("Not read block-page size yet!"));
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}

			EXT_IMG_INFO_PTR pImg	= NULL;
			BOOL bOK				= FALSE;
			pImg = pThis->m_mapPBFileBuf[std::make_pair(strKey,strFileID)];
			if(pImg != NULL)
			{
				if ( pThis->m_sheetSettings.IsLoadFromPac(pImg->szFilePath) )
				{
					DATA_INFO_T dataInfo = pThis->m_sheetSettings.GetDataInfo(pImg->szFilePath);
					if( 0 == dataInfo.llSize || 0 == dataInfo.llOffset )
					{
						strErrMsg.Format(_T("Not find block-page [%s] data info!"),strKey);
						_tcscpy(pbj->szErrorMsg,strErrMsg);
						return E_FAIL;
					}
					bOK = pBMFile->SetCurFileName(pThis->m_strPacketPath.operator LPCTSTR(),dataInfo.llSize,dataInfo.llOffset);
				}
				else
				{
					bOK = pBMFile->SetCurFileName(pImg->szFilePath);
				}

				if (!bOK)
				{
					_tcscpy(pbj->szErrorMsg,_T("SetCurFileName fail!"));
					return E_FAIL;
				}
			}	
			else
			{
				strErrMsg.Format(_T("Not find block-page [%s] matched file!"),strKey);
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}
		}		
	}
	
	if( _tcsnicmp(W2T( (LPWSTR)cbstrFileID),_T("NV"),2) == 0 
		&& !pThis->m_sheetSettings.IsEnableMultiFileBuf() 
		&& pThis->m_sheetSettings.IsBackupNVFile(W2T( (LPWSTR)cbstrFileID))
		&& (  ( pThis->m_sheetSettings.IsReadFlashInFDL2() 
		        && _tcsnicmp(W2T( (LPWSTR)cbstrFileType),_T("CODE"),4) == 0 ) 
		       || pThis->m_sheetSettings.IsHasLang()))
	{
		strFileID = W2T( (LPWSTR)cbstrFileID);

		int nIndex = pThis->m_sheetSettings.GetDLNVIDIndex(strFileID);
		if(nIndex == -1 || nIndex >= MAX_BACKUP_FILE_NUM)
			return E_FAIL;
		if(pbj->tNVBackup[nIndex].pBuf == NULL || pbj->tNVBackup[nIndex].dwSize == 0)
		{
			return E_FAIL;
		}		

		pBMFile->SetCurCode((const LPVOID)(pbj->tNVBackup[nIndex].pBuf),pbj->tNVBackup[nIndex].dwSize);		
		
		if(pThis->m_sheetSettings.IsNvBaseChange())
		{
			PFILE_INFO_T pNvFileInfo = NULL;
			pThis->m_sheetSettings.GetFileInfo(W2T( (LPWSTR)cbstrFileID),(LPDWORD)&pNvFileInfo);
			if(pNvFileInfo != NULL)
			{
				int nPos = pThis->m_sheetSettings.GetNvNewBasePosition();
				if(nPos<0 || nPos>=MAX_BLOCK_NUM)
				{
					strErrMsg.Format(_T("The position index [%d] of the new Nv address is incorrect."),nPos);
					_tcscpy(pbj->szErrorMsg,strErrMsg);
					return E_FAIL;
				}				
				pBMFile->SetCurCodeBaseEx(pNvFileInfo->arrBlock[nPos].llBase);
			}	
		}
	}
	
	//[[ it will SetCurCode in OnOperationStart	

	//int nID = pThis->m_sheetSettings.IsBackupFile(strFileID);
	//if(nID != -1)
	//{
	//	pBMFile->SetCurCode((const LPVOID)(pbj->tFileBackup[nID].pBuf),pbj->tFileBackup[nID].dwSize);
	//}

	//]]
	
	if( _tcsnicmp(W2T( (LPWSTR)cbstrFileID),_T("_CHECK_NV"),9) == 0 )
	{
		strFileID = W2T( (LPWSTR)cbstrFileID);

		CString strNVID = strFileID;		
		strNVID = strNVID.Mid(7);		
		
		int nIndex = pThis->m_sheetSettings.GetDLNVIDIndex(strNVID);
		if(nIndex == -1 || nIndex >= MAX_BACKUP_FILE_NUM)
			return E_FAIL;
		if(pbj->tNVBackup[nIndex].pBuf == NULL || pbj->tNVBackup[nIndex].dwSize == 0)
		{
			return E_FAIL;
		}
		
		pBMFile->SetCurCode((const LPVOID)(pbj->tNVBackup[nIndex].pBuf),pbj->tNVBackup[nIndex].dwSize);
	}	
#ifdef _RESEARCH
	if (g_theApp.m_bNeedUpRefInfo && pbj->lpRefData && _tcsicmp( W2T( (LPWSTR)cbstrFileType ), _T("REF_INFO")) == 0)
	{
		pBMFile->SetCurCode((const LPVOID)(pbj->lpRefData),MAX_REF_INFO_LEN);
	}
#endif
	if (!g_theApp.IsSkipFile(cbstrFileID))
	{
		pbj->llCodeSize += pBMFile->GetCurCodeSizeEx();
	}
	
    return S_OK;
}
														
HRESULT CBMOprObserver::OnFileOprEnd( DWORD dwOprCookie,
									  LPCWSTR cbstrFileID,
									  LPCWSTR cbstrFileType, 
									  DWORD dwResult )
{
    UNUSED_ALWAYS( dwOprCookie );
    UNUSED_ALWAYS( cbstrFileType );
    UNUSED_ALWAYS( dwResult );    
	USES_CONVERSION;

    if( _tcsnicmp( W2T((LPWSTR)cbstrFileID),_T("NV"),2) == 0 )
    {
        InterlockedDecrement( &( pThis->m_lStartNVRef ) );
    }
	_BMOBJ * pbj = NULL;
	if( !pThis->m_mapBMObj.Lookup(dwOprCookie,pbj) ||  NULL == pbj )	
	{
		return E_FAIL;
	}       

	
#ifdef _RESEARCH
	if (g_theApp.m_bNeedUpRefInfo && 0 == _tcsicmp(cbstrFileID,_T("RefInfo")) && pbj->lpRefData)
	{
		CUsageStatistics cUS;
		memcpy(pbj->lpRefData+MAX_REF_INFO_LEN,pbj->szSN,strlen(pbj->szSN) >SP15_MAX_SN_LEN ? SP15_MAX_SN_LEN : strlen(pbj->szSN));
		cUS.UploadData2Srv(pbj->szSN, pbj->lpRefData,CUST_REF_INFO_LEN);
	}
#endif

#ifndef _SPUPGRADE
    CString strFileType = W2T( (LPWSTR)cbstrFileType);
    if( 
        ( !pThis->m_sheetSettings.IsNandFlash()   && ( strFileType.CompareNoCase(_T("FDL1")) == 0  ||strFileType.CompareNoCase(_T("FDL")) == 0 || strFileType.CompareNoCase(_T("HOST_FDL")) == 0) )      ||
        ( pThis->m_sheetSettings.IsNandFlash()   && ( strFileType.CompareNoCase(_T("FDL2")) == 0  ||strFileType.CompareNoCase(_T("NAND_FDL")) == 0 ) )
        )
    { 
        if(!g_theApp.m_bManual)
        {
            if (OPR_SUCCESS != dwResult)
            {
                return S_OK;
            }

            PORT_DATA *pPortData = NULL;
            HANDLE hEvent = NULL;
            BOOL bFound = pThis->m_mapPortData.Lookup(dwOprCookie, pPortData);
            if( !bFound )
            {
                // There is no event associate with the port
                pPortData = pThis->CreatePortData( dwOprCookie );		
            }	
            if(pThis->m_bNeedPhaseCheck)
            {
                hEvent = pPortData->hSNEvent;		
                ::PostMessage( pThis->GetActiveView()->GetSafeHwnd(), WM_REQUIRE_SN, dwOprCookie, (LPARAM)pPortData);
                WaitForSingleObject(hEvent, INFINITE);
                strcpy(pbj->szSN,pPortData->szSN);

				//wei.song 20151020
				if (g_theApp.m_bSprdMESEnable)
				{
					int nStatus = -1;
					nStatus = MES_CheckPreStation(g_theApp.m_szBatchName, pbj->szSN);
					if (nStatus != MES_SUCCESS)
					{
						char szError[1024]={0};
						MES_GetLastError(szError);
						_tcscat(pbj->szErrorMsg, A2W(szError));
						return E_FAIL;
					}
					
					nStatus = MES_InitTest(pbj->szTestResultGUID);
					if (nStatus != MES_SUCCESS)
					{
						char szError[1024]={0};
						MES_GetLastError(szError);
						_tcscat(pbj->szErrorMsg, A2W(szError));
						return E_FAIL;
					}
				}
            }
        }

    }
#endif
    return S_OK;
}

HRESULT CBMOprObserver::OnEnd( DWORD dwOprCookie, 
										   DWORD dwResult )
{
	_BMOBJ * pbj = NULL;
	if( !pThis->m_mapBMObj.Lookup(dwOprCookie,pbj) ||  NULL == pbj )	
	{
		return E_FAIL;
	}

	BOOL bSucceed = TRUE;

	LPARAM lParam = NULL;
	lParam = (LPARAM)pbj;
    if( dwResult != OPR_SUCCESS )
    {
		bSucceed = FALSE;
        if(0 == _tcslen(pbj->szErrorMsg))
        {
            _tcscpy(pbj->szErrorMsg,_T("Unkown Error"));
        }
        
		if(pThis->m_bShowFailedMsgbox)
		{
			::PostMessage(pThis->GetSafeHwnd(),WM_WARN_MESSAGEBOX,dwOprCookie,0);
		}
    }
	else
	{
		pbj->szErrorMsg[0] = '\0';
	}
#ifdef _SPUPGRADE
	if(bSucceed && pThis->m_sheetSettings.IsSharkNand())
		pThis->RemoveLocalFile(pbj->dluid.szUID);
#endif

    if ( 1 != pbj->nStage )
    {
        pThis->DoReport(dwOprCookie,bSucceed);  
		if (bSucceed)
		{
			pThis->AutoCompare(dwOprCookie,pbj);
		}
		
    }

    if(!(g_theApp.m_bStopDownloadIfOldMemory) && dwResult == OPR_SUCCESS && pbj->dwOldMemoryType != 0)
    {
        TCHAR szErrorMsg[_MAX_PATH*2];
        pThis->GetOprErrorCodeDescription(pbj->dwOldMemoryType,szErrorMsg,_MAX_PATH);
        _tcscpy(pbj->szErrorMsg, szErrorMsg);
    }
	
    ::PostMessage( ((CDLoaderView*)pThis->GetActiveView())->GetSafeHwnd(), 
		BM_END, dwOprCookie, lParam );	  
	
    HRESULT hr;
    PBOOTMODEOBJ_T pStruct = NULL;	
	hr = pThis->m_pBMAF->BMAF_GetBootModeObjInfo(dwOprCookie,(LPVOID*)&pStruct);
	//If the port is "USB" regarded (may be not), or not wait for next chip
	//need stop the port to close the channel, prevent for "Blue screen"
	if(g_theApp.m_dwWaitTimeForNextChip == 0 && pStruct ) 
	{
		pStruct->bStop = TRUE;	
	}

	if( ((g_theApp.m_bClosePortFlag && pbj->dwIsUart != 1) || g_theApp.m_bManual) &&
		pStruct)
	{
		pStruct->bStop = TRUE;	
		::PostMessage( pThis->GetSafeHwnd(), WM_STOP_ONE_PORT, dwOprCookie, 0 );
	}   
	if(g_theApp.m_bScriptCtrl &&  1 != pbj->nStage )
	{
		CAutoCS cs( pThis->m_csMultCmdFlash );
		++g_theApp.m_dwSumCMDFlash;
		if (g_theApp.m_dwSumCMDFlash == g_theApp.m_dwMultiCount)
		{
			::PostMessage( pThis->GetSafeHwnd(), WM_COMMAND, ID_STOP, 0 );
			::PostMessage( pThis->GetSafeHwnd(), WM_CLOSE, 0, 0 );
		}
		
	}

	//recovered softSIM file
	if (pThis->m_sheetSettings.IsSoftSimPrj())
	{
		if (bSucceed)
		{
			::DeleteFile(pbj->tSoftSim.szFile);
		}
		else if ( 0 != _tcslen(pbj->tSoftSim.szFile))
		{
			CAutoCS cs( pThis->m_csSoftSim );
			TCHAR* pFile = NULL;
			int nLen = _tcslen(pbj->tSoftSim.szFile) + 1;
			pFile = new TCHAR[nLen];
			if (pFile)
			{
				_tcscpy(pFile,pbj->tSoftSim.szFile);
				pThis->m_lstSoftSim.push_back(pFile);
			}
			
		}	
	}
    return S_OK;
}

HRESULT CBMOprObserver::OnStart( DWORD dwOprCookie, 
										     DWORD dwResult )
{
    UNUSED_ALWAYS( dwOprCookie );
    UNUSED_ALWAYS( dwResult );
		
	CString strErrMsg = _T("");
	_BMOBJ * pbj = NULL;
	if( !pThis->m_mapBMObj.Lookup(dwOprCookie,pbj) ||  NULL == pbj )	
	{
		return E_FAIL;
	}
	if(dwResult==0) //Success
	{
		::PostMessage( ((CDLoaderView*)pThis->GetActiveView())->GetSafeHwnd(), BM_BEGIN, dwOprCookie, 0 );
	}
	else
	{
		pThis->GetOprErrorCodeDescription(dwResult,pbj->szErrorMsg,_MAX_PATH);
	}
    return S_OK;
}

HRESULT CBMOprObserver::OnFilePrepare(DWORD   dwOprCookie,
									  LPCWSTR bstrProduct,
									  LPCWSTR bstrFileName,
									  LPVOID  lpFileInfo,
									  LPVOID  pBMFileInfoArr,
									  LPDWORD lpBMFileInfoCount,
									  LPDWORD lpdwFlag)
{
	UNUSED_ALWAYS(bstrProduct);
	UNUSED_ALWAYS(dwOprCookie);	
	USES_CONVERSION;

	CString strErrMsg = _T("");
	_BMOBJ * pbj = NULL;
	if( !pThis->m_mapBMObj.Lookup(dwOprCookie,pbj) ||  NULL == pbj )	
	{
		return E_FAIL;
	}
	
	*lpdwFlag = 0; //不做处理

	PBMFileInfo pBMFileInfo = (PBMFileInfo)pBMFileInfoArr;
	if(lpFileInfo == NULL)
	{
		CUIntArray agFlashOprFiles;
		int nFlashOprCount = pThis->m_sheetSettings.GetFlashOprFileInfo(agFlashOprFiles);
		if(nFlashOprCount > MAX_RET_FILE_NUM )
		{
			strErrMsg.Format(_T("Too much flash oprations!"));
			_tcscpy(pbj->szErrorMsg,strErrMsg);
			return E_FAIL;
		}
		pThis->m_mapCmpInfo.RemoveAll();
		for(int i= 0; i< nFlashOprCount; i++)
		{
			PFILE_INFO_T pFileInfo = (PFILE_INFO_T)agFlashOprFiles[i];
			PBMFileInfo pBMFileInfoCur = pBMFileInfo+i;
            unsigned __int64 llFileSize = 0;

			//hognliang.xin 2010-9-30
			//BMPlatform to deal with file name appending with the COM Number.
			_tcscpy(pBMFileInfoCur->szFileID,pFileInfo->szID);
			_tcscpy(pBMFileInfoCur->szFileType, pFileInfo->szType);
			_tcscpy(pBMFileInfoCur->szFileName,pFileInfo->szFilePath);
			_tcscpy(pBMFileInfoCur->szRepID, pFileInfo->arrBlock[0].szRepID);
			pBMFileInfoCur->llBase = pFileInfo->arrBlock[0].llBase;
			pBMFileInfoCur->llOprSize = pFileInfo->arrBlock[0].llSize;
			pBMFileInfoCur->bChangeCode = FALSE;
			
			if (pThis->m_sheetSettings.GetComparePolicy() && 0 == _tcsnicmp(pBMFileInfoCur->szFileType,_T("ReadFlashAndSave"),_tcslen(_T("ReadFlashAndSave"))))
			{
				_tcscpy(pBMFileInfoCur->szFileType, _T("ReadFlashAndDirectSave"));
			}

			if(pThis->m_sheetSettings.HasPartitionInfo())
			{
				_tcscat(pBMFileInfoCur->szFileType,_T("2"));
			}
			if(_tcsnicmp(pFileInfo->szType,_T("CODE"),4) == 0)
			{
				pBMFileInfoCur->bLoadCodeFromFile = TRUE;
                llFileSize = GetFileSizeEx(pBMFileInfoCur->szFileName);
			}
			else
			{
				pBMFileInfoCur->bLoadCodeFromFile = FALSE;
			}
			if (llFileSize>0xFFFFFFFF || pBMFileInfoCur->llOprSize>0xFFFFFFFF)
			{
                _tcscat(pBMFileInfoCur->szFileType,_T("_64"));
			}

			CString strFileID = pFileInfo->szID;
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);
			if (!pThis->AddCompareFileInfo(pBMFileInfoCur))
			{
				return E_FAIL;
			}
			
		}
#ifdef _RESEARCH
		if (g_theApp.m_bNeedUpRefInfo && pThis->m_sheetSettings.HasPartitionInfo())
		{
			if((nFlashOprCount+1) > MAX_RET_FILE_NUM)
			{
				strErrMsg.Format(_T("Too much flash oprations!"));
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}			

			PBMFileInfo pBMFileInfoCur = pBMFileInfo+nFlashOprCount;
			_tcscpy(pBMFileInfoCur->szFileID,_T("RefInfo"));
			_tcscpy(pBMFileInfoCur->szFileType, _T("REF_INFO"));	
			pBMFileInfoCur->llBase = 0x0;
			pBMFileInfoCur->llOprSize = 0x0;
			pBMFileInfoCur->bChangeCode = FALSE;
			pBMFileInfoCur->bLoadCodeFromFile = FALSE;

			//CString strFileID = pBMFileInfoCur->szFileID;
			//((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);

			++nFlashOprCount;
		}

		if (bExistFlashOpr && pThis->m_sheetSettings.IsEnableAPR())
		{
			if((nFlashOprCount+1) > MAX_RET_FILE_NUM)
			{
				strErrMsg.Format(_T("Too much flash oprations!"));
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}			

			PBMFileInfo pBMFileInfoCur = pBMFileInfo+nFlashOprCount;
			_tcscpy(pBMFileInfoCur->szFileID,_T("WriteAPR"));
			_tcscpy(pBMFileInfoCur->szFileType, _T("APR"));	
			pBMFileInfoCur->llBase = 0x0;
			pBMFileInfoCur->llOprSize = 0x0;
			pBMFileInfoCur->lpCode = (LPVOID)&(pThis->m_ftPacInfo);
			pBMFileInfoCur->llCodeSize = sizeof(pThis->m_ftPacInfo);
			pBMFileInfoCur->bChangeCode = FALSE;
			pBMFileInfoCur->bLoadCodeFromFile = FALSE;

			++nFlashOprCount;
		}
#endif

		if((!pThis->m_sheetSettings.IsNandFlash()) &&
			pThis->m_sheetSettings.IsBackupNV() &&
			pThis->m_sheetSettings.IsNeedCheckNV())
		{
			CStringArray agNVID;
			int nNVCount = pThis->m_sheetSettings.GetDLNVID(agNVID);
			
			for(int i =0 ; i< nNVCount; i++)
			{
				CString strNVFileID = agNVID.GetAt(i);
				PFILE_INFO_T pBackupFileInfo = NULL;
				if(pThis->m_sheetSettings.GetFileInfo(strNVFileID,(LPDWORD)&pBackupFileInfo) != -1
					&& pBackupFileInfo != NULL)
				{				
					PBMFileInfo pBMFileInfoCur = pBMFileInfo+nFlashOprCount;
					CString strFileIDNew;
					strFileIDNew.Format(_T("_CHECK_%s"),strNVFileID);
					_tcscpy(pBMFileInfoCur->szFileID,strFileIDNew);
					
					if(pThis->m_sheetSettings.HasPartitionInfo())
					{
						_tcscpy(pBMFileInfoCur->szFileType, _T("CHECK_NV2"));
					}
					else
					{
						_tcscpy(pBMFileInfoCur->szFileType, _T("CHECK_NV"));
					}

					_tcscpy(pBMFileInfoCur->szRepID, pBackupFileInfo->arrBlock[0].szRepID);
					pBMFileInfoCur->llBase = pBackupFileInfo->arrBlock[0].llBase;
					
					pBMFileInfoCur->llOprSize = pBackupFileInfo->arrBlock[0].llSize? pBackupFileInfo->arrBlock[0].llSize : pThis->m_dwMaxNVLength;
					pBMFileInfoCur->bChangeCode = TRUE;
					pBMFileInfoCur->bLoadCodeFromFile = FALSE;
					
					CString strFileID = pBMFileInfoCur->szFileID;
					((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);
					
					nFlashOprCount++;
				}
			}	
		}
		
		if( pThis->m_sheetSettings.IsReadMcpType())
		{
			if((nFlashOprCount+1) > MAX_RET_FILE_NUM)
			{
				strErrMsg.Format(_T("Too much flash oprations!"));
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}			
			
			PBMFileInfo pBMFileInfoCur = pBMFileInfo+nFlashOprCount;
			_tcscpy(pBMFileInfoCur->szFileID,_T("READ_MCPTYPE"));
			_tcscpy(pBMFileInfoCur->szFileType, _T("READFLASHTYPE"));			
			pBMFileInfoCur->llBase = 0x0;
			pBMFileInfoCur->llOprSize = 0x0;
			pBMFileInfoCur->bChangeCode = FALSE;
			pBMFileInfoCur->bLoadCodeFromFile = FALSE;
			
			CString strFileID = pBMFileInfoCur->szFileID;
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);
			
			nFlashOprCount++;
		}

        if( pThis->m_sheetSettings.IsEnableSecureBoot())
        {
            if((nFlashOprCount+1) > MAX_RET_FILE_NUM)
            {
                strErrMsg.Format(_T("Too much flash oprations!"));
                _tcscpy(pbj->szErrorMsg,strErrMsg);
                return E_FAIL;
            }			

            PBMFileInfo pBMFileInfoCur = pBMFileInfo+nFlashOprCount;
            _tcscpy(pBMFileInfoCur->szFileID,_T("_Enable_SecureBoot_"));
            _tcscpy(pBMFileInfoCur->szFileType, _T("EnableSecureBoot"));	
            pBMFileInfoCur->llBase = 0x0;
            pBMFileInfoCur->llOprSize = 0x0;
            pBMFileInfoCur->bChangeCode = FALSE;
            pBMFileInfoCur->bLoadCodeFromFile = FALSE;

            CString strFileID = pBMFileInfoCur->szFileID;
            ((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);

            ++nFlashOprCount;
        }

		if(pThis->m_sheetSettings.IsEnableDebugMode())
		{
			if((nFlashOprCount+1) > MAX_RET_FILE_NUM)
			{
				strErrMsg.Format(_T("Too much oprations! [EnableDebugMode]"));
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}			

			PBMFileInfo pBMFileInfoCur = pBMFileInfo+nFlashOprCount;
			_tcscpy(pBMFileInfoCur->szFileID,_T("_Debug_"));
			_tcscpy(pBMFileInfoCur->szFileType, _T("DEBUG_MODE"));	
			pBMFileInfoCur->llBase = 0x0;
			pBMFileInfoCur->llOprSize = 0x0;
			pBMFileInfoCur->lpCode = (LPVOID)(pThis->m_tDebugData.pBuf);
			pBMFileInfoCur->llCodeSize = pThis->m_tDebugData.dwSize;
			pBMFileInfoCur->bChangeCode = FALSE;
			pBMFileInfoCur->bLoadCodeFromFile = FALSE;

			CString strFileID = pBMFileInfoCur->szFileID;
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);

			++nFlashOprCount;
		}

        if (pThis->m_sheetSettings.IsEnableEndProcess())
        {
            if((nFlashOprCount+1) > MAX_RET_FILE_NUM)
            {
                strErrMsg.Format(_T("Too much flash oprations!"));
                _tcscpy(pbj->szErrorMsg,strErrMsg);
                return E_FAIL;
            }			

            PBMFileInfo pBMFileInfoCur = pBMFileInfo+nFlashOprCount;
            _tcscpy(pBMFileInfoCur->szFileID,_T("_END_"));
            _tcscpy(pBMFileInfoCur->szFileType, _T("EndProcess"));	
            pBMFileInfoCur->llBase = 0x0;
            pBMFileInfoCur->llOprSize = 0x0;
            pBMFileInfoCur->bChangeCode = FALSE;
            pBMFileInfoCur->bLoadCodeFromFile = FALSE;

            CString strFileID = pBMFileInfoCur->szFileID;
            ((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);

            nFlashOprCount++;
        }

        if(pThis->m_bSetFirstMode && (pThis->m_sheetSettings.HasPartitionInfo() || pThis->m_bSptFPFirstMode))
        {
            if((nFlashOprCount+1) > MAX_RET_FILE_NUM)
            {
                strErrMsg.Format(_T("Too much flash oprations!"));
                _tcscpy(pbj->szErrorMsg,strErrMsg);
                return E_FAIL;
            }			

            PBMFileInfo pBMFileInfoCur = pBMFileInfo+nFlashOprCount;
            _tcscpy(pBMFileInfoCur->szFileID,_T("_SetFirstMode_"));
            _tcscpy(pBMFileInfoCur->szFileType, _T("SetFirstMode"));	
            pBMFileInfoCur->llBase = 0x0;
            pBMFileInfoCur->llOprSize = 0x0;
            pBMFileInfoCur->bChangeCode = FALSE;
            pBMFileInfoCur->bLoadCodeFromFile = FALSE;
            pBMFileInfoCur->lpCode = &pThis->m_nFirstMode;
            pBMFileInfoCur->llCodeSize = sizeof(int);

            CString strFileID = pBMFileInfoCur->szFileID;
            ((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);

            nFlashOprCount++;
        }

		if(pThis->m_sheetSettings.IsReset())
		{
			if((nFlashOprCount+1) > MAX_RET_FILE_NUM)
			{
				strErrMsg.Format(_T("Too much flash oprations!"));
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}			
			
			PBMFileInfo pBMFileInfoCur = pBMFileInfo+nFlashOprCount;
			_tcscpy(pBMFileInfoCur->szFileID,_T("_RESET_"));
			_tcscpy(pBMFileInfoCur->szFileType, _T("Reset"));	
			pBMFileInfoCur->llBase = 0x0;
			pBMFileInfoCur->llOprSize = 0x0;
			pBMFileInfoCur->bChangeCode = FALSE;
			pBMFileInfoCur->bLoadCodeFromFile = FALSE;
			
			CString strFileID = pBMFileInfoCur->szFileID;
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);
			
			nFlashOprCount++;
		}
		 
		if(pThis->m_sheetSettings.IsPowerOff())
		{
			if((nFlashOprCount+1) > MAX_RET_FILE_NUM)
			{
				strErrMsg.Format(_T("Too much flash oprations!"));
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}			
			
			PBMFileInfo pBMFileInfoCur = pBMFileInfo+nFlashOprCount;
			_tcscpy(pBMFileInfoCur->szFileID,_T("_POWEROFF_"));
			_tcscpy(pBMFileInfoCur->szFileType, _T("PowerOff"));	
			pBMFileInfoCur->llBase = 0x0;
			pBMFileInfoCur->llOprSize = 0x0;
			pBMFileInfoCur->bChangeCode = FALSE;
			pBMFileInfoCur->bLoadCodeFromFile = FALSE;
			
			CString strFileID = pBMFileInfoCur->szFileID;
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);
			
			nFlashOprCount++;
		}

		*lpdwFlag = 1;
		*lpBMFileInfoCount= nFlashOprCount;
		
		return S_OK;
	}

	PFILE_INFO_T pFileInfo = (PFILE_INFO_T)lpFileInfo;	
	CString strID = pFileInfo->szID;
	CString strFileType = pFileInfo->szType;
	CString strFileName = W2T( (LPWSTR)bstrFileName);

	do 
	{			
		strID.MakeUpper();
		if (g_theApp.m_bReadFixNV && strID.CompareNoCase(_T("FDL")) && strID.CompareNoCase(_T("Fdl2")))
		{
			*lpdwFlag = 1;
			*lpBMFileInfoCount =0;
			break;
		}

		if( strFileType.CompareNoCase(_T("MasterImage"))==0 )
		{
			_tcscpy(pBMFileInfo->szFileID,pFileInfo->szID);
			_tcscpy(pBMFileInfo->szFileType, pThis->m_sheetSettings.HasPartitionInfo()?_T("CODE2"):_T("CODE"));
			_tcscpy(pBMFileInfo->szFileName,W2T( (LPWSTR)bstrFileName));
			_tcscpy(pBMFileInfo->szRepID,pFileInfo->arrBlock[0].szRepID);
			pBMFileInfo->llBase = pFileInfo->arrBlock[0].llBase;
			IMAGE_PARAM imageparam;
			DATA_INFO_T dataInfo;
			pBMFileInfo->bLoadCodeFromFile = FALSE;
			memset( &imageparam,0,sizeof( IMAGE_PARAM ) );

			if (pThis->m_sheetSettings.IsLoadFromPac(strFileName.operator LPCTSTR()))
			{
				_tcscpy( imageparam.szPath,pThis->m_strPacketPath.operator LPCTSTR() );
				dataInfo = pThis->m_sheetSettings.GetDataInfo(strFileName.operator LPCTSTR());
			}
			else
			{
				_tcscpy( imageparam.szPath,W2T( (LPWSTR)bstrFileName) );
			}
			
			CMasterImgGen mig;
            DWORD dweMasterImageSize = 0;
			pBMFileInfo->lpCode = mig.MakeMasterImageSingle( &dweMasterImageSize,1,&imageparam,pThis->m_sheetSettings.GetFlashPageType(),dataInfo.llSize,dataInfo.llOffset );
			pBMFileInfo->llCodeSize = dweMasterImageSize;
			if(pBMFileInfo->lpCode == NULL)
			{
				strErrMsg.Format(_T("Make master image faile! [%s]"),pFileInfo->szID);
				_tcscpy(pbj->szErrorMsg,strErrMsg);
				return E_FAIL;
			}
			pThis->m_pMasterImg = pBMFileInfo->lpCode;

			if(pThis->m_sheetSettings.IsEnableMultiFileBuf())
			{
				pBMFileInfo->bChangeCode = TRUE;
			}
			
			*lpdwFlag = 1;
			*lpBMFileInfoCount =1;	
			CString strFileID = pFileInfo->szID;
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);

			break;
		}	

		strID.MakeUpper();
		if( strID.Find(_T("NV"))==0 )
		{
			
			_tcscpy(pBMFileInfo->szFileID,pFileInfo->szID);
			_tcscpy(pBMFileInfo->szRepID,pFileInfo->arrBlock[0].szRepID);

			// need to calculate the CRC16 and add to the head of NV
			// so the bChangeCode must be TRUE;
			pBMFileInfo->bChangeCode = TRUE;

			if(_tcsnicmp(pFileInfo->szType,_T("NV"),2) == 0)
			{
				if(  pThis->m_sheetSettings.IsReadFlashInFDL2() 
					|| !pThis->m_sheetSettings.IsBackupNV(pFileInfo->szID) 
					|| !pThis->m_sheetSettings.IsBackupNVFile(strID))
				{
					_tcscpy(pBMFileInfo->szFileType, pThis->m_sheetSettings.HasPartitionInfo()?_T("CODE2"):_T("CODE"));
				}
				else
				{				
					_tcscpy(pBMFileInfo->szFileType, pFileInfo->szType); // type is "NV" / "NV_NAND" / "NV_COMM"				
				}
			}
			else
			{
				_tcscpy(pBMFileInfo->szFileType, pFileInfo->szType);
			}
#ifdef _FACTORY
            if(!pThis->ClearCalibrationFlag(strID.operator LPCTSTR(),W2T( (LPWSTR)bstrFileName)))
            {
                return E_FAIL;
            }			
#endif
            pBMFileInfo->bLoadCodeFromFile = TRUE;
			pBMFileInfo->llBase = pFileInfo->arrBlock[0].llBase;
			pBMFileInfo->llOprSize = pFileInfo->arrBlock[0].llSize? pFileInfo->arrBlock[0].llSize : pThis->m_dwMaxNVLength;		
            _tcscpy(pBMFileInfo->szFileName,W2T( (LPWSTR)bstrFileName));
			
			strID = pFileInfo->szID;
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strID);

			int nInfoCount = 1;
			
			if(pThis->m_sheetSettings.IsNVOrgDownload())
			{
				int nPos = pThis->m_sheetSettings.GetNVOrgBasePosition();
				if(nPos<=0 || nPos >= 5)
				{
					strErrMsg.Format(_T("%s orignal base position [%d] is not correct,must be [1-4]!\nPlease check the production configure file!"),strID,nPos);
					_tcscpy(pbj->szErrorMsg,strErrMsg);
					return E_FAIL;
				}
				PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
				_tcscpy(pBMFileInfo2->szFileName,W2T( (LPWSTR)bstrFileName));
				pBMFileInfo2->bLoadCodeFromFile = TRUE;
				pBMFileInfo2->bChangeCode = FALSE;

				strID.MakeUpper();
				CString strFileIDNew;
				strFileIDNew.Format(_T("_ORG_%s"),strID);
				_tcscpy(pBMFileInfo2->szFileID,strFileIDNew);
				//_tcscpy(pBMFileInfo2->szFileID,_T("NVOriginal"));
				_tcscpy(pBMFileInfo2->szFileType, pThis->m_sheetSettings.HasPartitionInfo()?_T("CODE2"):_T("CODE"));
				_tcscpy(pBMFileInfo2->szRepID,pFileInfo->arrBlock[nPos].szRepID);
			
				pBMFileInfo2->llBase = pFileInfo->arrBlock[nPos].llBase;
				CString strDes = strFileIDNew;
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
				nInfoCount++;
			}

			*lpdwFlag = 1;
			*lpBMFileInfoCount =nInfoCount;

			break;
		}

		strID = pFileInfo->szID;	
		if(strID.CompareNoCase(_T("FDL")) == 0 && !pThis->m_sheetSettings.IsNandFlash())
		{		
			if ( g_theApp.m_bCMDFlash )
			{
				CString strInfo;
				strInfo.Format(_T("%s flash=NOR"),EZ_MODE_REPORT_FLAG);
				g_theApp.ReportToPipe(strInfo);
			}
			// For FDL
			_tcscpy(pBMFileInfo->szFileName,W2T( (LPWSTR)bstrFileName));
			_tcscpy(pBMFileInfo->szFileID,pFileInfo->szID);
			_tcscpy(pBMFileInfo->szRepID,pFileInfo->arrBlock[0].szRepID);
			pBMFileInfo->bChangeCode = FALSE;
			pBMFileInfo->bLoadCodeFromFile = TRUE;
			pBMFileInfo->llBase = pFileInfo->arrBlock[0].llBase;	
			_tcscpy(pBMFileInfo->szFileType, pFileInfo->szType); 
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strID);
			
			int nInfoCount = 1;
			//////////////////////////////////////////////////////////////////////////
			//  set ExtTable info
			if(pThis->m_sheetSettings.HasExtTblInfo())
			{
				// added ExtTable function
				PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
				pBMFileInfo2->bLoadCodeFromFile = FALSE;
				_tcscpy(pBMFileInfo2->szFileType, _T("EXTTABLE"));
				_tcscpy(pBMFileInfo2->szFileID,_T("_EXTTABLE_"));
				CString strDes = _T("_EXTTABLE_");
				pBMFileInfo2->lpCode = pThis->m_pExtTblData;
				pBMFileInfo2->llCodeSize = pThis->m_dwExtTblSize;	
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
				++nInfoCount;
			}

			
			//  read chip ID
			if( pThis->m_sheetSettings.IsEnableMultiFileBuf())
			{
				// add ReadChipType function
				PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
				pBMFileInfo2->bLoadCodeFromFile = FALSE;
				_tcscpy(pBMFileInfo2->szFileType, _T("READ_CHIPID")); 
				_tcscpy(pBMFileInfo2->szFileID,_T("ReadChipID"));
				CString strDes = _T("ReadChipID");
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
				nInfoCount++;			
			}
		
			if( pThis->m_sheetSettings.IsCheckMCPType())
			{
				// add ReadFlashType function
				PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
				pBMFileInfo2->bLoadCodeFromFile = FALSE;
				_tcscpy(pBMFileInfo2->szFileType, _T("CHECK_MCPTYPE")); 
				_tcscpy(pBMFileInfo2->szFileID,_T("CheckMcpType"));
				CString strDes = _T("CheckMcpType");
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
				nInfoCount++;			
			}
            
            //  read chip UID
            if( E_CHIPUID == pThis->m_nDUTID || pThis->m_sheetSettings.IsReadChipUID())
            {
                // add ReadFlashType function
                PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
                pBMFileInfo2->bLoadCodeFromFile = FALSE;
                _tcscpy(pBMFileInfo2->szFileType, _T("READ_CHIPUID")); 
                _tcscpy(pBMFileInfo2->szFileID,_T("ReadChipUID"));
                CString strDes = _T("ReadChipUID");
                ((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
                nInfoCount++;			
            }	

#ifndef _FACTORY
			if(!pThis->AddBackupFiles(pBMFileInfo,nInfoCount,pbj,pFileInfo))
			{
				return E_FAIL;
			}
#endif
            if (pThis->m_bPacHasKey || pThis->m_bEnableWriteFlash || pThis->m_sheetSettings.IsEnableRFChipType() )
            {
                PBMFileInfo pBMFileInfoCur = pBMFileInfo+nInfoCount;
                _tcscpy(pBMFileInfoCur->szFileID,_T("_EnableFlash_"));
                _tcscpy(pBMFileInfoCur->szFileType, _T("Enable_Flash"));	
                pBMFileInfoCur->llBase = 0x0;
                pBMFileInfoCur->llOprSize = 0x0;
                pBMFileInfoCur->bChangeCode = FALSE;
                pBMFileInfoCur->bLoadCodeFromFile = FALSE;

                CString strFileID = pBMFileInfoCur->szFileID;
                ((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);

                ++nInfoCount;
            }

			if( pThis->m_sheetSettings.IsEraseAll() && pThis->AddEraseAll(pBMFileInfo + nInfoCount))
			{		
				nInfoCount++;
			}
			
			*lpdwFlag = 1;
			*lpBMFileInfoCount =nInfoCount;	
			break;
		}

		strID = pFileInfo->szID;
		if(strID.CompareNoCase(_T("FDL")) == 0 && pThis->m_sheetSettings.IsNandFlash() && pThis->m_bPortSecondEnum)
		{					
			_tcscpy(pBMFileInfo->szFileID,_T("CheckBaud"));		
			pBMFileInfo->bChangeCode = FALSE;
			pBMFileInfo->bLoadCodeFromFile = FALSE;
			pBMFileInfo->llBase = 0;	
			_tcscpy(pBMFileInfo->szFileType, _T("CheckBaud")); 
			CString strDes = _T("CheckBaud");
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);

			int nInfoCount = 1;

			// For FDL
			PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
			_tcscpy(pBMFileInfo2->szFileName,W2T( (LPWSTR)bstrFileName));
			_tcscpy(pBMFileInfo2->szFileID,_T("FDLA"));
			_tcscpy(pBMFileInfo2->szRepID,pFileInfo->arrBlock[0].szRepID);
			pBMFileInfo2->bChangeCode = FALSE;
			pBMFileInfo2->bLoadCodeFromFile = TRUE;
			pBMFileInfo2->llBase = pFileInfo->arrBlock[0].llBase;	
			_tcscpy(pBMFileInfo2->szFileType, _T("FDL1")); 
			strDes = _T("FDL1");
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
			nInfoCount++;

			*lpdwFlag = 1;
			*lpBMFileInfoCount =nInfoCount;	
			break;
		}
		
		strID = pFileInfo->szID;
		if(strID.CompareNoCase(_T("Fdl2")) == 0 && pThis->m_sheetSettings.IsNandFlash())
		{
			int nInfoCount = 0;

			if(pThis->m_sheetSettings.IsKeepCharge())
			{
				PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
				pBMFileInfo2->bLoadCodeFromFile = FALSE;
				_tcscpy(pBMFileInfo2->szFileType, _T("KeepCharge"));
				_tcscpy(pBMFileInfo2->szFileID,_T("KeepCharge"));
				CString strDes = _T("KeepCharge");
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
				nInfoCount++;
			}

            if(pThis->m_sheetSettings.IsDdrCheck())
            {
                PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
                pBMFileInfo2->bLoadCodeFromFile = FALSE;
                _tcscpy(pBMFileInfo2->szFileType, _T("DDRCheck"));
                _tcscpy(pBMFileInfo2->szFileID,_T("DDRCheck"));
                CString strDes = _T("DDRCheck");
                ((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
                nInfoCount++;
            }

            if(pThis->m_sheetSettings.IsSelfRefresh())
            {
                PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
                pBMFileInfo2->bLoadCodeFromFile = FALSE;
                _tcscpy(pBMFileInfo2->szFileType, _T("SelfRefresh"));
                _tcscpy(pBMFileInfo2->szFileID,_T("SelfRefresh"));
                CString strDes = _T("Self Refresh");
                ((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
                nInfoCount++;
            }

			// For Fdl2
			if( pThis->m_bPortSecondEnum)
			{

                PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
                _tcscpy(pBMFileInfo2->szFileID,_T("2ndCheckBaud"));		
                pBMFileInfo2->bChangeCode = FALSE;
                pBMFileInfo2->bLoadCodeFromFile = FALSE;
                pBMFileInfo2->llBase = 0;	
                _tcscpy(pBMFileInfo2->szFileType, _T("CheckBaud")); 
                CString strDes = _T("CheckBaud");
                ((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
                nInfoCount++;

				PBMFileInfo pBMFileInfo3 = pBMFileInfo + nInfoCount;
				_tcscpy(pBMFileInfo3->szFileName,W2T( (LPWSTR)bstrFileName));
				_tcscpy(pBMFileInfo3->szFileID,_T("FDLB"));
				pBMFileInfo3->bChangeCode = FALSE;
				pBMFileInfo3->bLoadCodeFromFile = TRUE;
				pBMFileInfo3->llBase = pFileInfo->arrBlock[0].llBase;
				_tcscpy(pBMFileInfo3->szFileType, _T("FDL2")); 
				_tcscpy(pBMFileInfo3->szRepID,pFileInfo->arrBlock[0].szRepID);
				strDes = _T("FDL2");
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strID);
				nInfoCount++;
			}
			else
			{
				PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
				_tcscpy(pBMFileInfo2->szFileName,W2T( (LPWSTR)bstrFileName));
				_tcscpy(pBMFileInfo2->szFileID,pFileInfo->szID);
				pBMFileInfo2->bChangeCode = FALSE;
				pBMFileInfo2->bLoadCodeFromFile = TRUE;
				pBMFileInfo2->llBase = pFileInfo->arrBlock[0].llBase;
				_tcscpy(pBMFileInfo2->szFileType, pFileInfo->szType); 
				_tcscpy(pBMFileInfo2->szRepID,pFileInfo->arrBlock[0].szRepID);

				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strID);
				nInfoCount++;
			}
#ifndef _FACTORY
            //Read Uboot for check match
            if (pThis->m_sheetSettings.IsCheckMatch())
            {
                PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
                pBMFileInfo2->bLoadCodeFromFile = FALSE;
                _tcscpy(pBMFileInfo2->szFileType, pThis->m_sheetSettings.HasPartitionInfo()?_T("ReadFlash2"):_T("ReadFlash"));
                _stprintf(pBMFileInfo2->szFileID,_T("ReadDUTInfo"));
                _tcscpy(pBMFileInfo2->szRepID,pThis->m_strKeyPartID);
                pBMFileInfo2->llOprSize = pThis->m_dwMaxUbootLen;
                CString strDes = _T("ReadDUTInfo");
                ((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
                ++nInfoCount;
            }
#endif

            // read flash block rate
            if( pThis->m_sheetSettings.m_pageCustomization.GetBadFlashRate() >= 0)
            {
                PBMFileInfo pBMFileInfo2 = pBMFileInfo+nInfoCount;
                pBMFileInfo2->bLoadCodeFromFile = FALSE;
                _tcscpy(pBMFileInfo2->szFileID,_T("READ_NANDBLOCKINFO"));
                _tcscpy(pBMFileInfo2->szFileType, _T("ReadNandBlockInfo"));		
                CString strFileID = pBMFileInfo2->szFileID;
                ((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);
                nInfoCount++;
            }

            //  read chip UID
            if( E_CHIPUID == pThis->m_nDUTID || pThis->m_sheetSettings.IsReadChipUID())
            {
                // add ReadFlashType function
                PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
                pBMFileInfo2->bLoadCodeFromFile = FALSE;
                _tcscpy(pBMFileInfo2->szFileType, _T("READ_CHIPUID")); 
                _tcscpy(pBMFileInfo2->szFileID,_T("ReadChipUID"));
                CString strDes = _T("ReadChipUID");
                ((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
                nInfoCount++;			
            }	

			//  set ExtTable info
			if(pThis->m_sheetSettings.HasExtTblInfo())
			{
				// added ExtTable function
				PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
				pBMFileInfo2->bLoadCodeFromFile = FALSE;
				_tcscpy(pBMFileInfo2->szFileType, _T("EXTTABLE"));
				_tcscpy(pBMFileInfo2->szFileID,_T("_EXTTABLE_"));
				CString strDes = _T("_EXTTABLE_");
				pBMFileInfo2->lpCode = pThis->m_pExtTblData;
				pBMFileInfo2->llCodeSize = pThis->m_dwExtTblSize;	
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
				++nInfoCount;
			}
			
			if( pThis->m_sheetSettings.IsMapPBFileBuf())
			{
				// add ReadFlashInfo function
				PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
				pBMFileInfo2->bLoadCodeFromFile = FALSE;
				_tcscpy(pBMFileInfo2->szFileType, _T("READ_FLASHINFO"));
				_tcscpy(pBMFileInfo2->szFileID,_T("ReadFlashInfo"));
				CString strDes = _T("ReadFlashInfo");
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
				
				nInfoCount++;	
			}

			if (pThis->m_sheetSettings.IsEnableRFChipType())
			{
				// add Read RF Chip type function
				PBMFileInfo pBMFileInfo3 = pBMFileInfo + nInfoCount;
				pBMFileInfo3->bLoadCodeFromFile = FALSE;
				_tcscpy(pBMFileInfo3->szFileType, _T("READ_RF_CHIP_TYPE"));
				_tcscpy(pBMFileInfo3->szFileID,_T("ReadRFChipID"));
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(_T("ReadRFChipID"));
				++nInfoCount;	
			}

#ifdef _SPUPGRADE
			if( pThis->m_sheetSettings.IsSharkNand())
			{
				// add dluid function
				PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
				pBMFileInfo2->bLoadCodeFromFile = FALSE;
				_tcscpy(pBMFileInfo2->szFileType, _T("ReadFlash2"));
				_tcscpy(pBMFileInfo2->szFileID,_T("ReadDluid"));
				_tcscpy(pBMFileInfo2->szRepID,DLUID_SECTION);
				pBMFileInfo2->llOprSize = DLUID_SIZE;
				CString strDes = _T("_READ_DLUID_");
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
				
				nInfoCount++;	
			}
#endif
#ifndef _FACTORY
			if (!g_theApp.m_bReadFixNV)
			{
				if(!pThis->AddBackupFiles(pBMFileInfo,nInfoCount,pbj,pFileInfo))
				{
					return E_FAIL;
				}
			}	
#endif
			if(pThis->m_sheetSettings.IsReadFlashInFDL2())
			{
				CStringArray agNVID;
				int nNVCount = pThis->m_sheetSettings.GetDLNVID(agNVID);
				
				for(int i =0 ; i< nNVCount; i++)
				{
					CString strFileID = agNVID.GetAt(i);
					PFILE_INFO_T pBackupFileInfo = NULL;
					if(pThis->m_sheetSettings.GetFileInfo(strFileID,(LPDWORD)&pBackupFileInfo) != -1
						&& pBackupFileInfo != NULL 
						&& _tcsnicmp(pBackupFileInfo->szType,_T("NV"),2) == 0 )
					{				

						PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
						pBMFileInfo2->bLoadCodeFromFile = FALSE;
						_tcscpy(pBMFileInfo2->szFileType, pThis->m_sheetSettings.HasPartitionInfo()?_T("ReadFlash2"):_T("ReadFlash"));
						_stprintf(pBMFileInfo2->szFileID,_T("_BKF_%s"),strFileID);

						if (g_theApp.m_bReadFixNV)
						{
							_stprintf(pBMFileInfo2->szFileID,_T("R_%s"),strFileID);
							_tcscpy(pBMFileInfo2->szFileType, pThis->m_sheetSettings.HasPartitionInfo()?_T("ReadFlashAndDirectSave2"):_T("ReadFlashAndDirectSave"));
							_tcscpy(pBMFileInfo2->szFileName,g_theApp.m_strRFixNV.operator LPCTSTR());
						}

						_tcscpy(pBMFileInfo2->szRepID,pBackupFileInfo->arrBlock[0].szRepID);
						pBMFileInfo2->llBase = pBackupFileInfo->arrBlock[0].llBase;
						pBMFileInfo2->llOprSize = pBackupFileInfo->arrBlock[0].llSize? pBackupFileInfo->arrBlock[0].llSize : pThis->m_dwMaxNVLength;
						
						CString strDes = pBMFileInfo2->szFileID;
						((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);				
						nInfoCount++;
					}
				}			
			}


			if( pThis->m_sheetSettings.IsCheckMCPType())
			{
				// add ReadFlashType function
				PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
				pBMFileInfo2->bLoadCodeFromFile = FALSE;
				_tcscpy(pBMFileInfo2->szFileType, _T("CHECK_MCPTYPE"));
				_tcscpy(pBMFileInfo2->szFileID,_T("CheckMcpType"));
				CString strDes = _T("CheckMcpType");
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
						
				nInfoCount++;	
			}	

#ifdef _FACTORY
			if(pThis->m_sheetSettings.IsSharkNand())
			{
				PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
				_tcscpy(pBMFileInfo2->szFileName,W2T( (LPWSTR)bstrFileName));
				_tcscpy(pBMFileInfo2->szFileID,_T("WriteDluid"));				
				pBMFileInfo2->bChangeCode = FALSE;	
				_tcscpy(pBMFileInfo2->szFileType, _T("CODE2")); 		
				_tcscpy(pBMFileInfo2->szRepID,DLUID_SECTION);
				
				pBMFileInfo2->bLoadCodeFromFile = FALSE;
				pBMFileInfo2->llBase = 0;
				pBMFileInfo2->llCodeSize = DLUID_SIZE;	
				pBMFileInfo2->lpCode = NULL;		
				CString strDes = _T("_WRITE_DLUID_");
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
				nInfoCount++;
				
			}
#endif

            if (pThis->m_bPacHasKey || pThis->m_bEnableWriteFlash || pThis->m_sheetSettings.IsEnableRFChipType()) 
            {
                PBMFileInfo pBMFileInfoCur = pBMFileInfo+nInfoCount;
                _tcscpy(pBMFileInfoCur->szFileID,_T("_EnableFlash_"));
                _tcscpy(pBMFileInfoCur->szFileType, _T("Enable_Flash"));	
                pBMFileInfoCur->llBase = 0x0;
                pBMFileInfoCur->llOprSize = 0x0;
                pBMFileInfoCur->bChangeCode = FALSE;
                pBMFileInfoCur->bLoadCodeFromFile = FALSE;

                CString strFileID = pBMFileInfoCur->szFileID;
                ((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strFileID);

                ++nInfoCount;
            }

            if( pThis->m_sheetSettings.IsEraseAll() && pThis->AddEraseAll(pBMFileInfo + nInfoCount))
            {		
                nInfoCount++;
            }

#ifdef _FACTORY
			if( (pThis->m_sheetSettings.GetRepartitionFlag() == REPAR_STRATEGY_ALWAYS)
				|| pThis->m_sheetSettings.IsEraseAll())
#else
			if( pThis->m_sheetSettings.GetRepartitionFlag() == REPAR_STRATEGY_ALWAYS)
#endif
			{
				PBMFileInfo pBMFileInfo2 = pBMFileInfo + nInfoCount;
				pBMFileInfo2->bLoadCodeFromFile = FALSE;
				if(pThis->m_sheetSettings.HasPartitionInfo())
				{
#ifdef _SPUPGRADE
					_tcscpy(pBMFileInfo2->szFileType, _T("REPARTITION2"));	
#else
					_tcscpy(pBMFileInfo2->szFileType, _T("FORCE_REPARTITION2"));
#endif
					pBMFileInfo2->lpCode = pThis->m_pPartitionData;
					pBMFileInfo2->llCodeSize = pThis->m_dwPartitionSize;				
				}
				else
				{
#ifdef _FACTORY
					if(pThis->m_sheetSettings.IsEraseAll())
					{
						_tcscpy(pBMFileInfo2->szFileType, _T("FORCE_REPARTITION"));
					}
					else
#endif
					{
						_tcscpy(pBMFileInfo2->szFileType, _T("REPARTITION"));
					}
					
				}
				 
				_tcscpy(pBMFileInfo2->szFileID,_T("_REPARTITION_"));
				CString strDes = _T("_REPARTITION_");
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strDes);
				nInfoCount++;
			}


			*lpdwFlag = 1;
			*lpBMFileInfoCount =nInfoCount;	

			break;
			
		}	

		strID = pFileInfo->szID;
		BOOL bUpgrade = FALSE;
		if ( pThis->m_sheetSettings.IsLoadFromPac(strFileName.operator LPCTSTR()))
		{
			
			DATA_INFO_T dataInfo = pThis->m_sheetSettings.GetDataInfo(strFileName.operator LPCTSTR());
			if( 0 == dataInfo.llSize || 0 == dataInfo.llOffset )
			{
				return E_FAIL;
			}
			_tcscpy(pBMFileInfo->szFileName,pThis->m_strPacketPath.operator LPCTSTR());
			_tcscpy(pBMFileInfo->szFileID,pFileInfo->szID);
			_tcscpy(pBMFileInfo->szRepID,pFileInfo->arrBlock[0].szRepID);
			pBMFileInfo->bChangeCode		= FALSE;
			pBMFileInfo->bLoadCodeFromFile	= TRUE;
			pBMFileInfo->llCodeSize			= dataInfo.llSize;
			pBMFileInfo->llCodeOffset		= dataInfo.llOffset;
			pBMFileInfo->llBase				= pFileInfo->arrBlock[0].llBase;	
			_tcscpy(pBMFileInfo->szFileType, pFileInfo->szType); 


			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strID);
			*lpdwFlag = 1;
			*lpBMFileInfoCount = 1;	

			break;
		}
		
#ifdef _SPUPGRADE	
		bUpgrade = TRUE;
		if(!pThis->m_sheetSettings.IsSharkNand())
		{
			PFILE_INFO_T pTmpFileInfo = NULL;
			if(pThis->m_sheetSettings.GetFileInfo(strID,(LPDWORD)&pTmpFileInfo) != -1
				&& pTmpFileInfo != NULL)
			{			
				// UpgradeDownload omit the prodnv and miscdata for shark emc flash
                
				if( 
                    pThis->m_sheetSettings.GetBackupProdnvMiscdataType() == 0    &&
                    (
                        _tcsicmp(pTmpFileInfo->arrBlock[0].szRepID,_T("miscdata")) == 0 ||
					    _tcsicmp(pTmpFileInfo->arrBlock[0].szRepID,_T("prodnv")) == 0	
                    )
                  )
				{
					*lpdwFlag = 1;
					*lpBMFileInfoCount =0;
					break;
				}
                else if( 
                    pThis->m_sheetSettings.GetBackupProdnvMiscdataType() == 2    &&
                    (
                    _tcsicmp(pTmpFileInfo->arrBlock[0].szRepID,_T("prodnv")) == 0 
                    )
                    )
                {
                    *lpdwFlag = 1;
                    *lpBMFileInfoCount =0;
                    break;
                }
			}
		}
#endif 
		if( strID.CompareNoCase(_T("PhaseCheck"))==0 )
		{
			if((pThis->m_bNeedPhaseCheck && !bUpgrade) || 
				pThis->m_sheetSettings.IsBackupFile(_T("PhaseCheck")) != -1)
			{
				_tcscpy(pBMFileInfo->szFileName,W2T( (LPWSTR)bstrFileName));
                _tcscpy(pBMFileInfo->szFileID,_T("PhaseCheck"));
				
				pBMFileInfo->bChangeCode = FALSE;	
				_tcscpy(pBMFileInfo->szFileType, pFileInfo->szType); 		
				_tcscpy(pBMFileInfo->szRepID, pFileInfo->arrBlock[0].szRepID);
				
				unsigned __int64 llPhaseCheckSize = pFileInfo->arrBlock[0].llSize;
				if( pFileInfo->arrBlock[0].llSize == 0 ||
					pFileInfo->arrBlock[0].llSize > PRODUCTION_INFO_SIZE)
				{
					llPhaseCheckSize = PRODUCTION_INFO_SIZE;
				}
				
				pBMFileInfo->bLoadCodeFromFile = FALSE;
				pBMFileInfo->llBase = pFileInfo->arrBlock[0].llBase;
				pBMFileInfo->llCodeSize = llPhaseCheckSize;	
				pBMFileInfo->lpCode = NULL;
				
				*lpdwFlag = 1;
				*lpBMFileInfoCount =1;
				strID = pFileInfo->szID;
				((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strID);
			}
			else
			{
				*lpdwFlag = 1;
				*lpBMFileInfoCount =0;	
			}

			break;
		}
		
		strID = pFileInfo->szID;
		// deal with backup items that are not checked by the user, except "phasecheck"
		{
			PFILE_INFO_T pFileInfo = NULL;
			if(strID.CompareNoCase(_T("PhaseCheck")) != 0
				&& pThis->m_sheetSettings.GetFileInfo(strID,(LPDWORD)&pFileInfo) != -1)
			{
			
				if(	pFileInfo->isBackup==1 && strFileName.IsEmpty())
				{
	#ifndef _FACTORY	
					if(pThis->m_sheetSettings.IsBackupFile(strID) == -1)
	#endif
					{
						*lpdwFlag = 1;
						*lpBMFileInfoCount =0;
						break;
					}
				}
			}
		}

		
		if( strFileType.CompareNoCase(_T("UDISK_IMG"))==0 || strFileType.CompareNoCase(_T("UDISK_IMG2"))==0 )
		{
			_tcscpy(pBMFileInfo->szFileName,W2T( (LPWSTR)bstrFileName));
			_tcscpy(pBMFileInfo->szFileID,strID);
			_tcscpy(pBMFileInfo->szFileType,strFileType);
			_tcscpy(pBMFileInfo->szRepID,pFileInfo->arrBlock[0].szRepID);
			pBMFileInfo->bChangeCode = TRUE;		
			pBMFileInfo->bLoadCodeFromFile = TRUE;
			pBMFileInfo->llBase = pFileInfo->arrBlock[0].llBase;		
			
			*lpdwFlag = 1;
			*lpBMFileInfoCount =1;
			strID = pFileInfo->szID;
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strID);
			break;
		}

		if( strFileType.CompareNoCase(_T("PAGE"))==0 || strFileType.CompareNoCase(_T("PAGE2"))==0 ||
			strFileType.CompareNoCase(_T("PAGE_OOB"))==0 ||strFileType.CompareNoCase(_T("PAGE_OOB2"))==0)
		{
			_tcscpy(pBMFileInfo->szFileName,W2T( (LPWSTR)bstrFileName));
			_tcscpy(pBMFileInfo->szFileID,strID);
			_tcscpy(pBMFileInfo->szFileType,strFileType);	
			_tcscpy(pBMFileInfo->szRepID,pFileInfo->arrBlock[0].szRepID);
			pBMFileInfo->bChangeCode = FALSE;		
			pBMFileInfo->bLoadCodeFromFile = FALSE;
			pBMFileInfo->llBase = pFileInfo->arrBlock[0].llBase;		
			
			*lpdwFlag = 1;
			*lpBMFileInfoCount =1;
			strID = pFileInfo->szID;
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strID);
			break;
		}

		strID = pFileInfo->szID;
		if( strID.CompareNoCase(_T("DSPCode")) == 0 )
		{
			_tcscpy(pBMFileInfo->szFileName,W2T( (LPWSTR)bstrFileName));
			_tcscpy(pBMFileInfo->szFileID,strID);
			_tcscpy(pBMFileInfo->szFileType,strFileType);	
			_tcscpy(pBMFileInfo->szRepID,pFileInfo->arrBlock[0].szRepID);
			pBMFileInfo->bChangeCode = FALSE;		
			pBMFileInfo->bLoadCodeFromFile = TRUE;
			pBMFileInfo->llBase = pFileInfo->arrBlock[0].llBase;	
			
			if(pThis->m_sheetSettings.IsEnableChipDspMap())
			{
				_tcscpy(pBMFileInfo->szFileType,_T("CHIP_DSP"));
				pBMFileInfo->bChangeCode = TRUE;
			}
			if(pThis->m_sheetSettings.IsEnableMultiFileBuf())
			{
				pBMFileInfo->bChangeCode = TRUE;
			}
			
			*lpdwFlag = 1;
			*lpBMFileInfoCount =1;
			strID = pFileInfo->szID;
			((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strID);
			break;
		}

	} while(0);

    // use for GUI display
	if(*lpdwFlag == 0 )
	{
		strID = pFileInfo->szID;
		if (strID.CompareNoCase(_T("FDL")) && strID.CompareNoCase(_T("FDL2")))
		{
			bExistFlashOpr = TRUE;
		}
		
		((CDLoaderView *)(pThis->GetActiveView()))->AddStepDescription(strID);
	}
	pFileInfo = NULL;
	pBMFileInfo = NULL;
	
	return S_OK;
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CFrameWnd::WindowProc(message, wParam, lParam);
	// TODO: Add your specialized code here and/or call the base class
}

/* 为了防止线程调用回调函数时使用AfxMessageBox发生阻塞
* 该函数主要在回调函数中使用PostMessage调用
* lpParam为对话框显示的文本字符串指针
*/
LRESULT CMainFrame::OnWarnMessageBox(WPARAM wParam,LPARAM lpParam)
{
	if(!m_bFailedMsgboxShowed)
	{
		DWORD dwPort = wParam;
		CString strWarnMsg;
		if(lpParam!= NULL)
		{
			strWarnMsg = (LPCTSTR)lpParam;
		}
		else
		{
			strWarnMsg.Format(_T("Port[%d] is failed, please check!!!"),dwPort);
		}
		m_bFailedMsgboxShowed = TRUE;	
		MessageBox(strWarnMsg,_T("Warning"),MB_OK | MB_ICONWARNING);
		m_bFailedMsgboxShowed = FALSE;
		
	}

	return 0L;
}

BOOL CMainFrame::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	UNUSED_ALWAYS(pHelpInfo);
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	//return CFrameWnd::OnHelpInfo(pHelpInfo);
}

void CMainFrame::OnLoadPacket() 
{
	// TODO: Add your command handler code here	
	if(g_theApp.m_bNeedPassword)
	{
		CDlgPassword dlgPW;
		if(dlgPW.DoModal() == IDCANCEL)
		{
			return;
		}
	}

	static _TCHAR BASED_CODE szFilter[] = _T("Packet Files (*.pac)|*.pac|All Files(*.*)|*.*||");
    CString strFile = _T("");
    CFileDialog dlg(TRUE, _T(".pac"), strFile,OFN_NOCHANGEDIR |OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter, NULL);
	
	if(dlg.DoModal() == IDCANCEL)
		return; 
	
	m_strPacketPath = dlg.GetPathName();

	if(!LoadPac())
	{
		DeletePacTmpDir();
		return;
	}
	
	_TCHAR szFilePath[_MAX_PATH]={0};
    if(g_theApp.GetIniFilePath(szFilePath))
    {
		DWORD dwAttr = GetFileAttributes(szFilePath);
		if(MAXDWORD != dwAttr)
		{
			dwAttr &= ~FILE_ATTRIBUTE_READONLY;
			::SetFileAttributes(szFilePath,dwAttr);
		}
		WritePrivateProfileString(_T("Download_Packet"),_T("packet"),m_strPacketPath,szFilePath);		
		WritePrivateProfileString(_T("GUI"),_T("PacketMode"),_T("1"),szFilePath);
	}
}

void CMainFrame::OnUpdateLoadPacket(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
#if defined( _FACTORY ) || defined(_SPUPGRADE)
	pCmdUI->Enable(!m_bPacketOpen || !m_bStarted); //lint !e730
#else // _DLOADERR
    pCmdUI->Enable(!m_bStarted); //lint !e730
#endif
}
CString CMainFrame::GetSpecConfigFile()
{
	return m_strSpecConfig;
}
void CMainFrame::SetPrdVersion(LPCTSTR lpszSpdVer)
{
	if(lpszSpdVer!= NULL)
	{
		m_wndToolBar.m_stcWnd.SetText(lpszSpdVer);
		m_wndToolBar.m_stcWnd.Invalidate();
	}
}

DWORD CMainFrame::Buf2TString(LPBYTE pBuf,DWORD dwSize,LPTSTR szStr,DWORD dwStrLen)
{
	if(dwStrLen < dwSize * 2 )
		return 0;
	for(UINT i=0;i<dwSize;i++)
	{
		_stprintf(&(szStr[2*i]), _T("%02X"), *pBuf);	
		++pBuf;
	}
	return dwSize*2;
}
void  CMainFrame::ParseChipUID(DWORD dwBlk0,DWORD dwBlk1,char* szStr,DWORD dwStrLen)
{
    if (NULL == szStr || 0 == dwStrLen)
    {
        return;
    }
    char szChipID[MAX_PATH] = {0};
    DWORD dwLotID[6] = {0};
    DWORD dwWaferID  = 0;
    DWORD dwX        = 0;
    DWORD dwY        = 0;
    dwLotID[5] = ((dwBlk0 >>18) & 0x3F) + 48;   //LOTID-5: blk0, bit18 ~ bit23
    dwLotID[4] = ((dwBlk0 >>12) & 0x3F) + 48;   //LOTID-4: blk0, bit12 ~ bit17
    dwLotID[3] = ((dwBlk0 >>6 ) & 0x3F) + 48;   //LOTID-3: blk0, bit6  ~ bit11
    dwLotID[2] = (dwBlk0 & 0x3F) +48;           //LOTID-2: blk0, bit0  ~ bit5
    dwLotID[1] = ((dwBlk1 >>25) & 0x3F) + 48;   //LOTID-1: blk1, bit25 ~ bit30
    dwLotID[0] = ((dwBlk1 >>19) & 0x3F) + 48;   //LOTID-0: blk1, bit19 ~ bit24
    dwWaferID  = (dwBlk1 >>14) & 0x1F;          //WaferID: blk1, bit14 ~ bit18
    dwX        = (dwBlk1 >>7) & 0x7F;           //X:       blk1, bit7  ~ bit13
    dwY        =  dwBlk1 & 0x7F;                //Y:       blk1, bit0  ~ bit6

    sprintf(szChipID,"%c%c%c%c%c%c%02d%03d%03d",
        dwLotID[5],dwLotID[4],dwLotID[3],dwLotID[2],dwLotID[1],dwLotID[0],
        dwWaferID,dwX,dwY);

    strncpy(szStr,szChipID,dwStrLen);
}   
DWORD CMainFrame::BCDToWString(LPBYTE pBcd,DWORD dwSize,LPTSTR szStr,DWORD dwStrLen)
{
	if(dwStrLen < dwSize * 2 )
		return 0;
	
	_TCHAR szValue[ 4 ] = { 0 };
	
	for(UINT i=0;i<dwSize;i++)
	{
		_stprintf(szValue, _T("%02x"), *pBcd);
		if(i==0)
		{
			szStr[i]=szValue[0];
		}
		else
		{
			szStr[2*i-1]=szValue[1];
			szStr[2*i]=szValue[0];
		}
		
		pBcd++;
	}
	return dwSize*2-1;
}


LRESULT CMainFrame::OnInitalPacket(WPARAM wParam,LPARAM lpParam)
{
	UNUSED_ALWAYS(wParam);
	UNUSED_ALWAYS(lpParam);
	
	_TCHAR szIniFilePath[_MAX_PATH]={0};
	if(!g_theApp.GetIniFilePath(szIniFilePath))
	{
		g_theApp.ReportToPipe(_T("Failed to load ini file!"));
		return 0;
	}

	BOOL bPacketMode = GetPrivateProfileInt(_T("GUI"),_T("PacketMode"),0,szIniFilePath);

#if defined(_SPUPGRADE) || defined(_FACTORY)
	bPacketMode = TRUE;
#endif
	if(!bPacketMode) //lint !e774
	{
        if(g_theApp.m_bScriptCtrl)
        {
            ::PostMessage(m_hWnd,WM_COMMAND,ID_START,0);
        }
		return 0;
	}	
	
	_TCHAR szPacPath[_MAX_PATH]={0};
	if(g_theApp.m_bCMDFlash)
	{
		swprintf(szPacPath, _T("%s"), g_theApp.m_strInPacFile);
	}
	else if(g_theApp.m_bCMDPackage)
	{
		swprintf(szPacPath, _T("%s"), g_theApp.m_strDoPacPath);
	}
	else
	{
        DWORD dwRet =  GetPrivateProfileString(_T("Download_Packet"),_T("packet"),_T(""),szPacPath,_MAX_PATH,szIniFilePath);
        if(dwRet == 0 )
            return 0;
		
	}

	TCHAR szDirve[_MAX_DRIVE];	
	_tsplitpath( szPacPath,szDirve,NULL,NULL,NULL);	
	CString strAbsolutePac;
	if(szDirve[0] != _T('\0') || (_tcslen(szPacPath)>2 && szPacPath[0] == _T('\\') && szPacPath[1] == _T('\\')))
	{		
		strAbsolutePac = szPacPath;
	}
	else
	{
		TCHAR szCurrentDir[MAX_PATH] = {0};
		GetCurrentDirectory(MAX_PATH,szCurrentDir);
		strAbsolutePac.Format(_T("%s\\%s"),szCurrentDir,szPacPath);
		_tcscpy(szPacPath,strAbsolutePac.operator LPCTSTR());
	}
	
	if(_tcslen(szPacPath) == 0)
	{
		g_theApp.ReportToPipe(_T("PacPath is empty"));
		return 0;	
	}
	
	CFileFind finder;
	if(!finder.FindFile(szPacPath))
	{
		g_theApp.ReportToPipe(_T("PAC file does not exist"));
		return 0;
	}

	BOOL bConfirmMode = GetPrivateProfileInt(_T("Download_Packet"),_T("ShowConfirmDialog"),0,szIniFilePath);

	if (!g_theApp.m_bScriptCtrl && bConfirmMode && !g_theApp.m_bCMDPackage )
	{
		CString strComfirm;
		strComfirm.Format(_T("Load latest pac file?\n\n%s"),szPacPath);
		if(AfxMessageBox(strComfirm.operator LPCTSTR(),MB_YESNO|MB_ICONINFORMATION) == IDNO)
		{
			return 0;
		}
	}
	
	m_strPacketPath = szPacPath;
    if(g_theApp.m_bNeedPassword)
    {
        CDlgPassword dlgPW;
        if(dlgPW.DoModal() == IDCANCEL)
        {
            return 0;
        }
    }
    g_theApp.ReportToPipe(_T("Loading pac file ..."),FALSE);
	if(!LoadPac())
	{
		DeletePacTmpDir();
		g_theApp.ReportToPipe(_T("Failure to load PAC file!"));
	}
    else
    {   
		if(g_theApp.m_bCMDFlash && g_theApp.m_bCMDPackage)
		{
			::PostMessage(GetSafeHwnd(),WM_COMMAND,ID_SETTINGS,0);
		}
        else if( g_theApp.m_bScriptCtrl )
        {
            CString strInfo;
            g_theApp.ReportToPipe(_T("Load PAC file successfully!"),FALSE);
            ::PostMessage(m_hWnd,WM_COMMAND,ID_START,0);
            strInfo.Format(_T("Detecting download device [COM%d] ..."),g_theApp.m_dwPort);
            g_theApp.ReportToPipe(strInfo,FALSE);
        }
        
    }

	return 0;		
}

void CMainFrame::DoReport(DWORD dwOprCookie,BOOL bSuccess /*= TRUE*/)
{
	//wei.song 20140901
	if (g_theApp.m_bEnableFlow)
	{
		EnterCriticalSection( &m_csProcessFlow);
		
		Write2Sql(dwOprCookie, bSuccess);

		LeaveCriticalSection( &m_csProcessFlow);
	}

	if (g_theApp.m_bSprdMESEnable)
	{
		EnterCriticalSection( &m_csProcessFlow);

		MESWriteResult(dwOprCookie, bSuccess);

		LeaveCriticalSection( &m_csProcessFlow);
	}
	
	if(NULL == m_pReportFile && !g_theApp.m_bScriptCtrl)
		return;
	
	EnterCriticalSection( &m_csReportFile);

	USES_CONVERSION;

	_BMOBJ * pbj				= NULL;
	char szSN[X_SN_LEN+1]		= {0};
	char szIMEI[X_SN_LEN+1]		= {0};
    char szChipUID[X_SN_LEN+1]	= {0};
	char szFlashUID[MAX_PATH]	= {0};

	if(m_mapBMObj.Lookup(dwOprCookie,pbj) && pbj != NULL )
	{
		strcpy(szSN,pbj->szSN);	
		strcpy(szIMEI,pbj->szIMEI);
		strcpy(szFlashUID,pbj->szFlashUID);
        strcpy(szChipUID,pbj->szChipUID);
	}

	// do success report
	if(m_pReportFile != NULL)
	{	
		char szReport[MAX_PATH*2] = {0};
		if(m_nReportType == REPORT_NORMAL)
		{
			CTime timeCur = CTime::GetCurrentTime();
#if defined _FACTORY
            if (E_CHIPUID == m_nDUTID)
            {
                sprintf(szReport,"%4d-%02d-%02d %02d:%02d:%02d[COM%d]\tCHIPUID:%-16s\t%s\r\n",
                    timeCur.GetYear(),
                    timeCur.GetMonth(),
                    timeCur.GetDay(),
                    timeCur.GetHour(),
                    timeCur.GetMinute(),
                    timeCur.GetSecond(),
                    dwOprCookie,	
                    strlen(szChipUID) == 0 ? " ": szChipUID,	
                    bSuccess?"PASS":"FAIL");	
            }
            else //SN
            {
                sprintf(szReport,"%4d-%02d-%02d %02d:%02d:%02d[COM%d]\tSN:%-24s\t%s\r\n",
                    timeCur.GetYear(),
                    timeCur.GetMonth(),
                    timeCur.GetDay(),
                    timeCur.GetHour(),
                    timeCur.GetMinute(),
                    timeCur.GetSecond(),
                    dwOprCookie,
                    strlen(szSN) == 0 ? " ": szSN,	
                    bSuccess ? "PASS" : "FAIL");
            }
#else
            if (E_SN == m_nDUTID)
            {
                sprintf(szReport,"%4d-%02d-%02d %02d:%02d:%02d[COM%d]\tSN:%-24s\t%s\r\n",
                    timeCur.GetYear(),
                    timeCur.GetMonth(),
                    timeCur.GetDay(),
                    timeCur.GetHour(),
                    timeCur.GetMinute(),
                    timeCur.GetSecond(),
                    dwOprCookie,	
                    strlen(szSN) == 0 ? " ": szSN,	
                    bSuccess?"PASS":"FAIL");	
            }
            else if (E_CHIPUID == m_nDUTID)
            {
                sprintf(szReport,"%4d-%02d-%02d %02d:%02d:%02d[COM%d]\tCHIPUID:%-16s\t%s\r\n",
                    timeCur.GetYear(),
                    timeCur.GetMonth(),
                    timeCur.GetDay(),
                    timeCur.GetHour(),
                    timeCur.GetMinute(),
                    timeCur.GetSecond(),
                    dwOprCookie,	
                    strlen(szChipUID) == 0 ? " ": szChipUID,	
                    bSuccess?"PASS":"FAIL");	
            }
            else //E_IMEI
            {
                sprintf(szReport,"%4d-%02d-%02d %02d:%02d:%02d[COM%d]\tIMEI:%-20s\t%s\r\n",
                    timeCur.GetYear(),
                    timeCur.GetMonth(),
                    timeCur.GetDay(),
                    timeCur.GetHour(),
                    timeCur.GetMinute(),
                    timeCur.GetSecond(),
                    dwOprCookie,	
                    strlen(szIMEI) == 0 ? " ": szIMEI,
                    bSuccess?"PASS":"FAIL");	
            }
			
#endif	
		}
		else if(m_nReportType == REPORT_PRESS)
		{
			STATE_INFO_T info = {0};
			CTime timeCur = CTime::GetCurrentTime();
			((CDLoaderView *)(GetActiveView()))->GetInfo(dwOprCookie,info);
			char szDutInfo[MAX_PATH] = {0};
			if (E_SN == m_nDUTID)
			{
				sprintf(szDutInfo,"SN:%s",strlen(szSN) == 0 ? " ": szSN);	
			}
			else if (E_CHIPUID == m_nDUTID)
			{
				sprintf(szDutInfo,"CHIPUID:%s",strlen(szChipUID) == 0 ? " ": szChipUID);
			}
			else //E_IMEI
			{
				sprintf(szDutInfo,"IMEI:%s",strlen(szIMEI) == 0 ? " ": szIMEI);
			}
			if(bSuccess)
			{
				int nSeconds = _ttoi(info.szUsedTime);
				double dPacSize = (double)(pbj->llCodeSize/(1024*1024));
				if (nSeconds)
				{
					float fSpeed = (float)(dPacSize/nSeconds);
					sprintf(szReport,"com%d,NA,NA,%s,%s,%.2fM/s,pass,%4d-%02d-%02d,%02d:%02d:%02d,%.0lfMB\n",dwOprCookie,T2A(info.szUsedTime),szDutInfo,fSpeed,
						timeCur.GetYear(),
						timeCur.GetMonth(),
						timeCur.GetDay(),
						timeCur.GetHour(),
						timeCur.GetMinute(),
						timeCur.GetSecond(),
						dPacSize);
				}
				else
				{
					sprintf(szReport,"com%d,NA,NA,%s,%s,NA,pass,%4d-%02d-%02d,%02d:%02d:%02d,%.0lfMB\n",dwOprCookie,T2A(info.szUsedTime),szDutInfo, 
						timeCur.GetYear(),
						timeCur.GetMonth(),
						timeCur.GetDay(),
						timeCur.GetHour(),
						timeCur.GetMinute(),
						timeCur.GetSecond(),
						dPacSize);
				}
				
			}
			else
			{
				sprintf(szReport,"com%d,%s,\"%s\",%s,%s,NA,fail,%4d-%02d-%02d,%02d:%02d:%02d,NA\n",dwOprCookie,T2A(info.szCurFile),T2A(pbj->szErrorMsg),T2A(info.szUsedTime),szDutInfo,
					timeCur.GetYear(),
					timeCur.GetMonth(),
					timeCur.GetDay(),
					timeCur.GetHour(),
					timeCur.GetMinute(),
					timeCur.GetSecond());
			}			
		}
		else if ( REPORT_SOFTSIM == m_nReportType ) 
		{
			CString strSIMID = GetStrFileTitle(pbj->tSoftSim.szFile);
			if(bSuccess)
			{
				sprintf(szReport,"%s\t\t%s\t\tpass\r\n",szFlashUID,T2A(strSIMID.operator LPCTSTR()));
			}
			else
			{
				sprintf(szReport,"%s\t\t%s\t\tfail\r\n",szFlashUID,T2A(strSIMID.operator LPCTSTR()));
			}
		}

		if(strlen(szReport))
		{
			fwrite(szReport,strlen(szReport),1,m_pReportFile);			
			fflush(m_pReportFile);
		}		
	}
	//[[ for auto download
	if(g_theApp.m_bScriptCtrl)
	{
		CString strResultPath = g_theApp.m_strResultPath;
		if(g_theApp.m_bResultPathWithPort)
		{
			CString strCom;
			strCom.Format(_T("_com%d.txt"),dwOprCookie);
			strResultPath.Replace(_T(".txt"),strCom);
		}
		FILE *pFile = _tfopen(strResultPath,_T("wt"));
		
		CString strResult = _T("success");
		if(!bSuccess)
		{
			if(pbj != NULL && _tcslen(pbj->szErrorMsg)!=0 )
			{
				strResult = pbj->szErrorMsg;		
			}
			else
			{
				strResult = _T("unknown errors occur in downloading");
			}
		}

		_ftprintf(pFile,strResult);
		fclose(pFile);
	}
	//]]

	LeaveCriticalSection( &m_csReportFile);
}

PORT_DATA* CMainFrame::CreatePortData( DWORD dwPort )
{
	PORT_DATA * pPortData = NULL;
	if(m_mapPortData.Lookup(dwPort,pPortData)&&pPortData)
	{
		ResetEvent(pPortData->hSNEvent);
		return pPortData;
	}

    HANDLE hEvent = CreateEvent( NULL,TRUE,FALSE,NULL );
	
    EnterCriticalSection( &m_csPortData );
	pPortData = new PORT_DATA;
	pPortData->dwPort = dwPort;
	pPortData->hSNEvent = hEvent;
	pPortData->lpPhaseCheck = NULL;
	memset(pPortData->szSN,0,sizeof(pPortData->szSN));
    m_mapPortData.SetAt(dwPort, pPortData);
    LeaveCriticalSection( &m_csPortData );
    
    return pPortData;
}

PORT_DATA * CMainFrame::GetPortDataByPort(int nPort)
{
	PORT_DATA * pPortData = NULL;
	m_mapPortData.Lookup(nPort, pPortData);
	return pPortData;
}

_BMOBJ * CMainFrame::GetBMObj(int nPort)
{
	_BMOBJ * pObj = NULL;
	m_mapBMObj.Lookup(nPort, pObj);
	return pObj;
}

LRESULT CMainFrame::OnStopOnePort(WPARAM wParam,LPARAM lpParam)
{
	UNUSED_ALWAYS(lpParam);

	DWORD dwPort = wParam;
	StopOnePortWork(dwPort);
	return 0;
}


DWORD WINAPI CMainFrame::GetThreadFunc(LPVOID lpParam)
{
	DWORD *pStruct = (DWORD *)lpParam;
	CMainFrame * This = (CMainFrame * )(pStruct[0]);

	DWORD dwPort = pStruct[1];
	return This->ClosePortFunc(dwPort,pStruct[2]);
}

DWORD CMainFrame::ClosePortFunc(DWORD dwPort,BOOL bSuccess)
{
	m_pBMAF->BMAF_StopOneWork(dwPort);
	if(m_bShowFailedMsgbox && !bSuccess)
	{
		::PostMessage(this->GetSafeHwnd(),WM_WARN_MESSAGEBOX,dwPort,0);
	}

	return 0L;
}

BOOL CMainFrame::CheckDLFiles()
{
	CWaitCursor wait;
	CFileFind finder;	
	CString strTmp = _T("");
	for(int i=0; i< m_nFileCount; i++)
	{
		CString strFile = m_arrFile.GetAt(i);
		if(!strFile.IsEmpty() && strFile.CompareNoCase(FILE_OMIT)!=0 &&
			!m_sheetSettings.IsSoftSimFile(strFile.operator LPCTSTR()))
		{
			if(!finder.FindFile(strFile))
			{
				strTmp += strFile;
				strTmp += _T("\n");
			}
			else
			{
			   finder.FindNextFile();	   
			   if(finder.GetLength() == 0 && !m_sheetSettings.IsLoadFromPac(strFile.operator LPCTSTR()))
			   {
				   strTmp += strFile;
				   strTmp += _T("\n");
			   }
			   finder.Close();
			}
		}
	}

	if(!strTmp.IsEmpty())
	{
		CString strError;
		strError.Format(IDS_ERR_FILES_NOT_EXIST,strTmp.operator LPCTSTR());
        g_theApp.MessageBox(strError);
		return FALSE;
	}

	return TRUE;
}
BOOL CMainFrame::InitUDiskBufMap()
{
	CWaitCursor wait;
	/*clear UDISK_IMG map*/
	ClearExtImgMap(m_mapUDiskIMg);

	BOOL bOK				= TRUE;
	PFILE_INFO_T pFileInfo	= NULL;
    int nFileNameLen		= UDSIK_IMG_NAME_LEN; // udisk_img_000
	CString strUDiskImgFile;
	int nIdx = m_sheetSettings.GetFileInfo(_T("UDISK_IMG"),(LPDWORD)&pFileInfo);
	if(nIdx != -1 && pFileInfo != NULL)
	{
		strUDiskImgFile = m_arrFile.GetAt(nIdx);
		if(strUDiskImgFile.IsEmpty() || strUDiskImgFile.CompareNoCase(FILE_OMIT)==0)
		{
			// not select UDISK_IMG
			return TRUE;
		}
		CString strDir = strUDiskImgFile;		
		int nFind = strDir.ReverseFind(_T('\\'));
		strDir = strDir.Left(nFind);
		CString strFind = strDir + _T("\\udisk_img_*.bin*");	//*.bin.flag

		WIN32_FIND_DATA wfd = { 0 };
		HANDLE hFind = ::FindFirstFile( strFind, &wfd );
		BOOL bFind = TRUE;
		while( INVALID_HANDLE_VALUE != hFind  && bFind)
		{
			CString strFileName;
			CString strExt;
			CStringArray agFilePathInfo;
			GetFilePathInfo(wfd.cFileName,agFilePathInfo);
			strExt = agFilePathInfo.GetAt(2);
			if (m_sheetSettings.IsLoadFromPac(wfd.cFileName))
			{
				CStringArray agTmpFilePathInfo;
				GetFilePathInfo(agFilePathInfo.GetAt(1),agTmpFilePathInfo);
				strExt = agTmpFilePathInfo.GetAt(2)+ agFilePathInfo.GetAt(2);
				nFileNameLen += MAX_FLAG_LEN;
				
			}
			strFileName =((CString) (wfd.cFileName)).Left(((CString) (wfd.cFileName)).GetLength() - strExt.GetLength());
			
			if( strFileName.GetLength() == nFileNameLen)
			{
				int nSectorSize = 0;
				CString strSectorSize = strFileName.Right(3);
				_stscanf(strSectorSize,_T("%03d"),&nSectorSize);
				nSectorSize *= 1024;
				CString strFile;
				strFile.Format(_T("%s\\%s"),strDir.operator LPCTSTR(),wfd.cFileName);
				DATA_INFO_T dataInfo;
				if (m_sheetSettings.IsLoadFromPac(strFile.operator LPCTSTR()))
				{
					dataInfo = m_sheetSettings.GetDataInfo(strFile.operator LPCTSTR());
					strFile = m_strPacketPath;
				}

				EXT_IMG_INFO_PTR pUDisk = new EXT_IMG_INFO;
				if (pUDisk)
				{
					_tcscpy(pUDisk->szFilePath,strFile);
					/*
					CFile file(strFile,CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary);
					pUDisk->dwSize = (DWORD)file.GetLength();
					if(pUDisk->dwSize  != 0 )
					{	
						pUDisk->pBuf = new BYTE[pUDisk->dwSize];
						file.Read(pUDisk->pBuf,pUDisk->dwSize);
					}				
					file.Close();*/
					if (!LoadFileFromLocal(pUDisk->szFilePath,pUDisk->pBuf,pUDisk->dwSize,dataInfo.llSize,dataInfo.llOffset))
					{
						bOK = FALSE;
					}

					m_mapUDiskIMg.SetAt(nSectorSize,pUDisk); //lint !e729
				}
				else
				{
					bOK = FALSE;
				}
				/*lint -save -e429*/
			}
			nFileNameLen = UDSIK_IMG_NAME_LEN;
            /*lint -restore */
			bFind = ::FindNextFile(hFind, &wfd);
			
		}
		if(INVALID_HANDLE_VALUE != hFind)
		{
			::FindClose(hFind);	
		}
	}

	if (!bOK)
	{
		ClearExtImgMap(m_mapUDiskIMg);
		g_theApp.MessageBox(_T("InitUDiskBufMap fail!"));
	}

	return bOK;
}

BOOL CMainFrame::InitNVBuffer()
{
	CWaitCursor wait;
	
	CStringArray agNVID;
	int nNVCount = m_sheetSettings.GetDLNVID(agNVID);

	if(nNVCount == 0)
	{
		return TRUE;
	}

	if(m_sheetSettings.IsReadFlashInFDL2() || m_sheetSettings.IsHasLang() )
	{
		for(int i =0; i< nNVCount; i++)
		{
			CString strNVFileType;
			CString strNVFileName;
			PFILE_INFO_T pNVFile = NULL;
			CString strID = agNVID.GetAt(i);

			BACKUP_INFO_PTR pNVInfo = new BACKUP_INFO;
			
			int nIdx = m_sheetSettings.GetFileInfo(strID,(LPDWORD)&pNVFile);
			if(nIdx != -1 && pNVFile != NULL)
			{
				strNVFileName = m_arrFile.GetAt(nIdx);
			}
			if ( 0 == strNVFileName.CompareNoCase(FILE_OMIT))
			{
				continue;
			}
			
			if(strNVFileName.IsEmpty() || strNVFileName.CompareNoCase(FILE_OMIT) == 0)
			{
				SAFE_DELETE(pNVInfo);
				return FALSE;
			}

			CFile file(strNVFileName,CFile::modeRead | CFile::shareDenyWrite);
			pNVInfo->dwSize = (DWORD)file.GetLength();
			if(pNVInfo->dwSize != 0 )
			{	
				pNVInfo->pBuf = new BYTE[pNVInfo->dwSize];
				file.Read(pNVInfo->pBuf,pNVInfo->dwSize);
			}			
			file.Close();
			m_mapNVFileInfo.SetAt(strID,pNVInfo);

			if(m_sheetSettings.IsHasLang() && pNVInfo->dwSize != 0 && pNVInfo->pBuf != NULL )
			{
				DWORD dwOffset = 0;
				DWORD dwLength = 0;
				BOOL  bBigEndian = TRUE;
				if(XFindNVOffsetEx(NV_MULTI_LANG_ID,pNVInfo->pBuf,pNVInfo->dwSize,dwOffset,dwLength,bBigEndian,FALSE) &&
					dwLength == sizeof(NV_MULTI_LANGUE_CFG))
				{
					// "m_pNVFileBuf+dwOffset" not overflow is guaranteed by XFindNVOffsetEx
					NV_MULTI_LANGUE_CFG *pObj = (NV_MULTI_LANGUE_CFG *)(pNVInfo->pBuf+dwOffset); //lint !e613
					for(int k = 0; k<NV_MAX_LANG_NUM;k++)
					{
						BYTE flag = (BYTE)m_sheetSettings.m_pageMultiLang.m_agLangFlag.GetAt(k);
						pObj->flag[k] = flag;
					}
				}
				else
				{
					g_theApp.MessageBox(_T("Not find multi-lang item in NV file!"));
					return FALSE;
				}
				
				if(m_sheetSettings.IsEnableMultiFileBuf())
				{
					CStringArray agChipName;
					CUIntArray   agChipID;
					int nCount = m_sheetSettings.GetAllChipName(agChipName,agChipID);				
					for(int i = 0; i< nCount; i++)
					{
						DWORD dwChipID = agChipID[i];
						EXT_IMG_INFO_PTR pImg = NULL;
						pImg = m_mapMultiFileBuf[std::make_pair(dwChipID,strID)];
						if(pImg != NULL && pImg->pBuf != NULL && pImg->dwSize != 0)
						{
							if(XFindNVOffsetEx(NV_MULTI_LANG_ID,pImg->pBuf,pImg->dwSize,dwOffset,dwLength,bBigEndian,FALSE) &&
								dwLength == sizeof(NV_MULTI_LANGUE_CFG))
							{
								// "pImg->pBuf+dwOffset" not overflow is guaranteed by XFindNVOffsetEx
								NV_MULTI_LANGUE_CFG *pObj = (NV_MULTI_LANGUE_CFG *)(pImg->pBuf+dwOffset); //lint !e613
								for(int k = 0; k<NV_MAX_LANG_NUM;k++)
								{
									BYTE flag = (BYTE)m_sheetSettings.m_pageMultiLang.m_agLangFlag.GetAt(k);
									pObj->flag[k] = flag;
								}
							}
						}
						
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL  CMainFrame::InitMultiNVBuffer()
{
	CWaitCursor wait;

	if(!m_sheetSettings.IsEnableRFChipType())
	{
		return TRUE;
	}
	CStringArray agChipName;
	CUIntArray   agChipID;
	CString strErr;
	m_sheetSettings.GetAllRFChipName(agChipName,agChipID);				

	CMap<CString, LPCTSTR,CString, LPCTSTR> mapMultiNVInfo;

	PFILE_INFO_T pFileInfo = NULL;
	int nCount = m_sheetSettings.GetAllFileInfo((LPDWORD)&pFileInfo);

	for(int i = 0; i< nCount; ++i)
	{	
		CString strFileType = pFileInfo[i].szType;
		CString strID = pFileInfo[i].szID;
		CString strFilePath = m_sheetSettings.GetDownloadFilePath(strID);
		if( strFilePath.IsEmpty() || 
			strFilePath.CompareNoCase(FILE_OMIT) == 0 ||
			pFileInfo[i].isSelByRf != 1 )
		{
			continue;
		}
		if (_tcsnicmp(strID.operator LPCTSTR(),_T("NV"),2))
		{
			strErr.Format(_T("This version just support multi nv,But this is %s file."),strID);
			g_theApp.MessageBox(strErr);
			return FALSE;
		}

		CStringArray agFilePathInfo;
		GetFilePathInfo(strFilePath,agFilePathInfo);
		CString strFileName = agFilePathInfo[1];
		CString strFilePre;	//prefix_RFNAME_*nvitem.bin ->prefix
		CString strRFChips;
		strFilePre.Empty();
		strRFChips.Empty();

		for(int i = 0; i< agChipName.GetSize(); i++)
		{
			CString strChipName = _T("_") + agChipName.GetAt(i) + _T("_");

			int nIndex = strFileName.Find(strChipName);
			if (nIndex != -1 && -1 != strFileName.Find(g_sz_NVITEM))
			{
				strFilePre = strFileName.Left(nIndex);
				break;
			}
		}
		if(strFilePre.IsEmpty())
		{
			CString strTmp = _T("");
			for(int j = 0; j< agChipName.GetSize(); j++)
			{
				strTmp += agChipName.GetAt(j);
				strTmp += _T("\n");
			}

			strErr.Format(_T("There are no nv files matching with \"prefix_rfname_*nvitem.bin\":\n%s"),strTmp);
			g_theApp.MessageBox(strErr);
			return FALSE;	
		}

		CString strExt = agFilePathInfo[2];
		CString strFind;
		strFind.Format(_T("%s\\%s_*_*%s%s"),agFilePathInfo[0],strFilePre,g_sz_NVITEM,strExt);

		WIN32_FIND_DATA wfd = { 0 };
		HANDLE hFind = ::FindFirstFile( strFind, &wfd );
		BOOL bFind = TRUE;
		while( INVALID_HANDLE_VALUE != hFind  && bFind)
		{
			//prefix_RFNAME_*nvitem.bin ->RFNAME
			CString strRFName =((CString) (wfd.cFileName)).Left(((CString) (wfd.cFileName)).GetLength() - strExt.GetLength()-_tcslen(g_sz_NVITEM));
			strRFName = strRFName.Right(strRFName.GetLength()-strFilePre.GetLength()-1); //-1 : _
			if (-1 == strRFName.Find(_T("_")))
			{
				continue;
			}
			strRFName = strRFName.Left(strRFName.Find(_T("_")));
			DWORD dwRFChipID = 0;
			if (m_sheetSettings.GetRFChipID(strRFName,dwRFChipID))
			{		

				MAP_MULTI_NVFILE::iterator it = m_mapMultiNVInfo.find(std::make_pair(dwRFChipID,strID));
				if (m_mapMultiNVInfo.end() != it)
				{
					strErr.Format(_T("%s can't have multiple nv file."),strRFName.operator LPCTSTR());
					g_theApp.MessageBox(strErr);
					return FALSE;
				}

				CString strFile;
				strFile.Format(_T("%s\\%s"),agFilePathInfo[0].operator LPCTSTR(),wfd.cFileName);

				BACKUP_INFO_PTR pNVInfo = new BACKUP_INFO;
				if (NULL == pNVInfo)
				{
					g_theApp.MessageBox(_T("New buf fail in InitMultiNVBuffer."));
					return FALSE;	
				}
				if (0 == _tcsnicmp(strID.operator LPCTSTR(),_T("NV"),2) && m_sheetSettings.GetNvBkpItemCount(strID.operator LPCTSTR()))
				{
					ClearCalibrationFlag(strID,strFile.operator LPCTSTR());
				}
				if (!LoadFileFromLocal(strFile.operator LPCTSTR(),pNVInfo->pBuf,pNVInfo->dwSize) )
				{
					SAFE_DELETE(pNVInfo);
					strErr.Format(_T("Load nv file fail [%s]."),strFile.operator LPCTSTR());
					g_theApp.MessageBox(strErr);
					return FALSE;
				}
				_tcscpy(pNVInfo->szNVFile,strFile.operator LPCTSTR());
				m_mapMultiNVInfo[std::make_pair(dwRFChipID,strID)] = pNVInfo;
				if (strRFChips.IsEmpty())
				{
					strRFChips = strRFName;
				}
				else
				{
					strRFChips += _T(",");
					strRFChips += strRFName;
				}

			}
			bFind = ::FindNextFile(hFind, &wfd);

		}

		if(INVALID_HANDLE_VALUE != hFind)
		{
			::FindClose(hFind);	
		}			
		mapMultiNVInfo.SetAt(strID,strRFChips);

	}

	/*POSITION pos = mapMultiNVInfo.GetStartPosition();
	while(NULL != pos)
	{
		CString strKey;
		CString strRFChips;
		CStringArray agRFChips;
		mapMultiNVInfo.GetNextAssoc( pos, strKey, strRFChips );

		int nCountFiles = SplitStr(strRFChips,agRFChips,_T(','));
		if(nCountFiles != agChipName.GetSize())
		{
			strErr.Format(_T("%s only have %d image files.\nIt must be equal to %d image files."),
				strKey,nCountFiles,agChipName.GetSize());
			g_theApp.MessageBox(strErr.operator LPCTSTR());
			return FALSE;
		}
	}*/
	return TRUE;
}

BOOL CMainFrame::InitReportInfo()
{
	CWaitCursor wait;
	CFileFind finder;
	CString strErrMsg;
	_TCHAR szConfigPath[_MAX_PATH]={0};
	_TCHAR szDefaultPath[_MAX_PATH]={0};
	g_theApp.GetIniFilePath(szConfigPath);
	_tcscpy(szDefaultPath,szConfigPath);
	
	LPTSTR pResult = _tcsrchr(szDefaultPath,_T('\\'));
    *pResult = 0;
	if(m_nReportType == REPORT_PRESS)
	{
		_tcscat(szDefaultPath,_T("\\download_result.csv"));	
	}
	else
	{
		_tcscat(szDefaultPath,_T("\\download_result.txt"));
	}

	if(m_bDoReport)
	{	
		if(finder.FindFile(szDefaultPath))
		{
			DWORD dwAtt = 0;
			dwAtt = ::GetFileAttributes(szDefaultPath);
			dwAtt &= ~FILE_ATTRIBUTE_READONLY;
			::SetFileAttributes(szDefaultPath,dwAtt);
		}
		
		m_pReportFile = _tfopen(szDefaultPath,_T("a+"));
		
		if(m_pReportFile == NULL)
		{
			strErrMsg.Format(_T("Open download report file [%s] fail!"),szDefaultPath);
			g_theApp.MessageBox(strErrMsg);
			return FALSE;
		}
	}

	return TRUE;
}

void CMainFrame::ClearExtImgMap(MAP_EXTIMG &mapExtImg)
{
	POSITION pos = mapExtImg.GetStartPosition();
	while(NULL != pos)
	{
		DWORD dwKey= 0;
        EXT_IMG_INFO_PTR pExtImgInfo = NULL;
      
        mapExtImg.GetNextAssoc( pos, dwKey, pExtImgInfo );

		if(pExtImgInfo != NULL)
		{
			SAFE_DELETE_ARRAY(pExtImgInfo->pBuf);	
			delete pExtImgInfo;
		}
	}
	mapExtImg.RemoveAll();
}

BOOL CMainFrame::InitChipDspBufmap()
{
	CWaitCursor wait;
	/*clear UDISK_IMG map*/
	ClearExtImgMap(m_mapChipDsp);

	if(!m_sheetSettings.IsEnableChipDspMap())
	{
		// need not chip-dsp map
		return TRUE;
	}

	BOOL bOK			= TRUE;
	CString strErrMsg	=_T("");
	PFILE_INFO_T pFileInfo = NULL;
	CString strChipDspFile;
	int nIdx = m_sheetSettings.GetFileInfo(_T("DSPCode"),(LPDWORD)&pFileInfo);
	if(nIdx != -1 && pFileInfo != NULL)
	{
		strChipDspFile = m_arrFile.GetAt(nIdx);
		if(strChipDspFile.IsEmpty() || strChipDspFile.CompareNoCase(FILE_OMIT)==0)
		{
			// not select "DSPCode"
			return TRUE;
		}

		CFileFind finder;
		
		_TCHAR szConfigPath[_MAX_PATH]={0};
		::GetModuleFileName(g_theApp.m_hInstance,szConfigPath,_MAX_PATH);
		LPTSTR pResult = _tcsrchr(szConfigPath,_T('\\'));
		*pResult = 0;
		_tcscat(szConfigPath,_T("\\BMFileType.ini"));
		
		_TCHAR szKeyValue[ MAX_BUF_SIZE ]={0}; 
		DWORD dwSize = GetPrivateProfileSection( _T("ChipDSPMap"), szKeyValue, MAX_BUF_SIZE, szConfigPath );
		if(dwSize == 0)
		{
			g_theApp.MessageBox(_T("Not found ChipID-DSPName map setting in BMFileType.ini!"));
			return FALSE;
		}
		CStringArray arrKeyData;    
		UINT nFileCount = (UINT)EnumKeys(szKeyValue,&arrKeyData);
		if(nFileCount == 0)
		{
			g_theApp.MessageBox(_T("Not found ChipID-DSPName map setting in BMFileType.ini!"));
			return FALSE;
		}
		
		CString strDir = strChipDspFile;
		
		int nFind = strDir.ReverseFind(_T('\\'));
		strDir = strDir.Left(nFind);

		for(UINT i= 0; i< nFileCount; i++)
		{		
			DWORD dwChipID = 0;
			CString strChipID = arrKeyData[2*i];
			strChipID.MakeUpper();
			_stscanf(strChipID,_T("0X%08x"),&dwChipID);		
			CString strFile;
			strFile.Format(_T("%s\\%s"),strDir.operator LPCTSTR(),arrKeyData[2*i+1].operator LPCTSTR());
			if(!finder.FindFile(strFile))
			{
				CString strFlagFile;
				strFlagFile = strFile + FLAG_FILE_SUFFIX;
				if (!finder.FindFile(strFlagFile))
				{
					bOK = FALSE;
					strErrMsg += strFile;
					strErrMsg += _T("\r\n");
					continue;
				}
				strFile = strFlagFile;
				
			}
			finder.Close();
			EXT_IMG_INFO_PTR pChipDsp = new EXT_IMG_INFO;
			if (pChipDsp)
			{
				DATA_INFO_T dataInfo;
				if (m_sheetSettings.IsLoadFromPac(strFile.operator LPCTSTR()))
				{
					_tcscpy(pChipDsp->szFilePath,m_strPacketPath.operator LPCTSTR());
					dataInfo = m_sheetSettings.GetDataInfo(strFile.operator LPCTSTR());
				}
				else
				{
					_tcscpy(pChipDsp->szFilePath,strFile.operator LPCTSTR());
				}
				if (!LoadFileFromLocal(pChipDsp->szFilePath,pChipDsp->pBuf,pChipDsp->dwSize,dataInfo.llSize,dataInfo.llOffset))
				{
					bOK = FALSE;
				}
				/*CFile file(strFile,CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary);
				pChipDsp->dwSize = (DWORD)file.GetLength();
				if(pChipDsp->dwSize  != 0 )
				{	
				pChipDsp->pBuf = new BYTE[pChipDsp->dwSize];
				file.Read(pChipDsp->pBuf,pChipDsp->dwSize);
				}				
				file.Close();*/		
				m_mapChipDsp.SetAt(dwChipID,pChipDsp); //lint !e429	
				/*lint -save -e429*/
			}
			else
			{
				bOK = FALSE;
			}
			
		}
		/*lint -restore */
	}

	if(!strErrMsg.IsEmpty() || !bOK)
	{
		strErrMsg.Insert(0,_T("Not found bellow DSP file:\r\n"));
		strErrMsg += _T("\r\nPlease check BMFileType.ini, [ChipDspMap]!");
		g_theApp.MessageBox(strErrMsg);
		ClearExtImgMap(m_mapChipDsp);
		return FALSE;
	}

	return bOK;
}

void CMainFrame::GetOprErrorCodeDescription(
                                           DWORD dwErrorCode,
                                           LPTSTR lpszErrorDescription, 
                                           int nSize )
{
    if( lpszErrorDescription == NULL || nSize == 0 )
        return;
    
    _TCHAR szKey[ 10 ];
    _itot( dwErrorCode, szKey, 10 );
    
    GetPrivateProfileString(  _T("ErrorDescription"), szKey,
        _T("Unknown Error"), lpszErrorDescription, 
        nSize, m_strOprErrorConfigFile );  
    
    return;
}


void CMainFrame::InitCodeChipID()
{
	USES_CONVERSION;
	CWaitCursor wait;
	int nIdx = m_sheetSettings.GetFileInfo(_T("FDL2"),NULL);
	if(nIdx == -1)
	{
		return;
	}

	CString strFDL2File = m_arrFile[nIdx];

	CFileFind finder;
	if(!finder.FindFile(strFDL2File))
	{
		return;
	}

	CString strError;

	CFile file;
    CFileException fe;
    BOOL bRet = file.Open( strFDL2File,CFile::modeRead|CFile::shareDenyWrite,&fe );

    if( !bRet )
    {
        // Can not open file
        LPTSTR p = strError.GetBuffer( _MAX_PATH );
        fe.GetErrorMessage( p,_MAX_PATH );
        strError.ReleaseBuffer();
		CString strTmp;
		strTmp.Format(_T("Can not open file [%s]!\n(%s)"),strFDL2File.operator LPCTSTR(),strError.operator LPCTSTR());
		g_theApp.MessageBox(strTmp);
        return;
    }

    LPBYTE lpContent = NULL;
    int nLen = 0;

    try
    {
        nLen = (int)file.GetLength();
        lpContent = new BYTE[nLen];
        file.Read( lpContent,nLen );
    }
    catch( CFileException &ex )
    {
        LPTSTR p = strError.GetBuffer( _MAX_PATH );
        ex.GetErrorMessage( p,_MAX_PATH );
        strError.ReleaseBuffer();
		CString strTmp;
		strTmp.Format(_T("Can not open file [%s]!\n(%s)"),strFDL2File.operator LPCTSTR(),strError.operator LPCTSTR());
		g_theApp.MessageBox(strTmp);
		if(lpContent != NULL)
		{
			 delete []lpContent;
		}
        return;
    }
   
    const char szChipIDPrefix[] = "#*CHIP_VER_";

#define MAX_CHIP_VER_LEN (100)

    BYTE* lpPos = NULL;
	lpPos = std::search( lpContent,lpContent + nLen,szChipIDPrefix,szChipIDPrefix + strlen( szChipIDPrefix ) - 1 );
    if( lpPos == lpContent + nLen )
    {
        // not find the version string
        delete []lpContent;
        return;
    }
	else
	{
		char szChipVer[MAX_CHIP_VER_LEN+1] = {0};
		lpPos += strlen( szChipIDPrefix );
		int nMin = MAX_CHIP_VER_LEN < (nLen-(lpPos-lpContent)) ? MAX_CHIP_VER_LEN : (nLen-(lpPos-lpContent));

		for(int i=0;i<(nMin-1); i++)
		{
			if(*(lpPos+i) != '*' && *(lpPos+i+1) != '#' )
			{
				szChipVer[i] = *(lpPos+i);
			}
			else
			{
				break;
			}
		}
#if defined(UNICODE) || defined(_UNICODE)
		m_strCodeChipID = A2W(szChipVer);  //lint !e733
#else
		m_strCodeChipID = szChipVer;
#endif
		delete []lpContent;    
		return;
	}   
}

LRESULT CMainFrame::OnPowerManage(WPARAM wParam,LPARAM lpParam)
{
	UNUSED_ALWAYS(wParam);
	UNUSED_ALWAYS(lpParam);
    CString strData;
    if(lpParam != 0)
    {
        strData.Format(_T("DOWNLOAD:POWER-MANAGE:%s"), lpParam);
    }
    else
    {
        strData = _T("DOWNLOAD:POWER-MANAGE");
    }
    
	COPYDATASTRUCT  CopyDataStruct;
    CopyDataStruct.dwData = 0;
    CopyDataStruct.cbData = (strData.GetLength()+1) * sizeof(TCHAR);
    CopyDataStruct.lpData = (PVOID)(strData.operator LPCTSTR());

	EnterCriticalSection( &m_csPowerManage);
    // Find the window
    g_hLayer1TesterHwnd = NULL;
    ::EnumWindows(EnumWindowsProc, 0);
    if (NULL != g_hLayer1TesterHwnd)
    {
		::SendMessage(g_hLayer1TesterHwnd, WM_COPYDATA, (WPARAM)GetSafeHwnd(), (long)&CopyDataStruct);
    }
	LeaveCriticalSection( &m_csPowerManage);

	return 0;
}

void CMainFrame::SetDLTitle()
{
	_TCHAR szTitle[_MAX_PATH]={0};
	_TCHAR szFilePath[_MAX_PATH]={0};
	BOOL bShowVer = TRUE;
	BOOL bShowPac = FALSE;
    if(g_theApp.GetIniFilePath(szFilePath))
    {
        GetPrivateProfileString(_T("GUI"),_T("Title"),_T(""),szTitle,_MAX_PATH,szFilePath);
		bShowVer = GetPrivateProfileInt(_T("GUI"),_T("ShowVer"),1,szFilePath);      
		bShowPac = GetPrivateProfileInt(_T("GUI"),_T("ShowPacPath"),0,szFilePath);
    }

	CString strVersion;

	if(bShowVer)
	{
		CXVerInfo ver;
		strVersion = ver.GetSPRDVersionString();
	}

	CString strTile;
#ifdef _SPUPGRADE
	if(_tcslen(szTitle) != 0)
	{
		strTile.Format(_T("UpgradeDownload [%s]"),szTitle);
	}
	else
	{
		strTile = _T("UpgradeDownload");
	}	
#elif defined _FACTORY
	if(_tcslen(szTitle) != 0)
	{
		strTile.Format(_T("FactoryDownload [%s]"),szTitle);
	}
	else
	{
		strTile = _T("FactoryDownload");
	}
#else
	if(_tcslen(szTitle) != 0)
	{
		strTile.Format(_T("ResearchDownload [%s]"),szTitle);
	}
	else
	{
		strTile = _T("ResearchDownload");
	}
#endif

	if(bShowVer)
	{
#ifdef _DEBUG
		strTile += _T(" - D");
#else 
		strTile += _T(" - R");
#endif
		strTile += strVersion;	
	}

	if(bShowPac && !m_strPacketPath.IsEmpty() && m_bPacketOpen)
	{
		CString strTmp;
		strTmp.Format(_T(" - [%s]"),m_strPacketPath);
		strTile += strTmp;
	}

	SetWindowText(strTile); 	
}

static BOOL CheckLCDConfig(VEC_LCD_CFIG &vLcdCfig1,VEC_LCD_CFIG &vLcdCfig2)
{
	if(vLcdCfig1.size() != vLcdCfig2.size())
		return FALSE;

	int nSize = vLcdCfig1.size();
	for(int i = 0; i< nSize; i++)
	{
		if(vLcdCfig1[i].dwFlagOffset != vLcdCfig2[i].dwFlagOffset)
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL  CMainFrame::InitPSFile()
{
	CString strPSFileName;

	int nIdx = m_sheetSettings.GetFileInfo(_T("PS"),NULL);
	if(nIdx == -1)
	{
		nIdx = m_sheetSettings.GetFileInfo(_T("UserImg"),NULL);		
	}
	if(nIdx == -1)
	{
		return TRUE;
	}

	strPSFileName = m_arrFile.GetAt(nIdx);
	
	if(m_sheetSettings.IsHasLCD() && !strPSFileName.IsEmpty() && strPSFileName.CompareNoCase(FILE_OMIT) != 0)
	{	
		//////////////////////////////////////////////////////////////////////////
		// check if select LCD config
		int nVSize = m_sheetSettings.m_pageLCDCfig.m_vLCDCfig.size();
		if(nVSize == 0)
		{
			return TRUE;
		}
		else
		{
			BOOL bSelLCD = FALSE;
			for(int i = 0 ;i< nVSize; i++)
			{
				if( m_sheetSettings.m_pageLCDCfig.m_vLCDCfig[i].dwFlag != 0)
				{
					bSelLCD = TRUE;
					break;
				}
			}
			if(!bSelLCD)
			{
                g_theApp.MessageBox(_T("Must select one of LCD configure at least!"));
				return FALSE;
			}
		}

		CFileFind finder;
		if(finder.FindFile(strPSFileName))
		{
			DWORD dwAtt = GetFileAttributes(strPSFileName);
			dwAtt &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributes(strPSFileName,dwAtt);
		}
		else
		{
			return TRUE;
		}

		HANDLE hFile = INVALID_HANDLE_VALUE;
		hFile = ::CreateFile(strPSFileName,
							 GENERIC_READ,
							 FILE_SHARE_READ,
							 NULL,
							 OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL,
							 NULL);

		if(hFile == INVALID_HANDLE_VALUE)
		{
			CString strFormatted;
			strFormatted.Format(_T("Can not open file [%s]."),strPSFileName.operator LPCTSTR());
			g_theApp.MessageBox(strFormatted);
			return FALSE;
		}

		DWORD dwSize = GetFileSize(hFile,NULL);
		BYTE *pBuf = NULL;
		if(dwSize != 0 )
		{	
			pBuf = new BYTE[dwSize];
			DWORD dwRealRead =  0;
			ReadFile(hFile,pBuf,dwSize,&dwRealRead,NULL);
		}			
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;

		if(dwSize != 0)
		{
			VEC_LCD_CFIG vLcdCfig;
			if(m_sheetSettings.FindLCDItem(strPSFileName,vLcdCfig))
			{
				if(CheckLCDConfig(vLcdCfig,m_sheetSettings.m_pageLCDCfig.m_vLCDCfig))
				{
					int nSize = vLcdCfig.size();
					for(int i = 0; i<nSize; i++)
					{
						*(pBuf+vLcdCfig[i].dwFlagOffset) = (BYTE)m_sheetSettings.m_pageLCDCfig.m_vLCDCfig[i].dwFlag; //lint !e613
					}

					if(nSize > 0)
					{
						hFile = ::CreateFile(strPSFileName,
								 GENERIC_WRITE,
								 FILE_SHARE_READ,
								 NULL,
								 CREATE_ALWAYS,
								 FILE_ATTRIBUTE_NORMAL,
								 NULL);

						if(hFile == INVALID_HANDLE_VALUE)
						{
							CString strFormatted;
							strFormatted.Format(_T("Can not create file [%s]."),strPSFileName.operator LPCTSTR());
							g_theApp.MessageBox(strFormatted);
							SAFE_DELETE_ARRAY(pBuf);
							return FALSE;
						}
						else
						{
							DWORD dwWrite= 0;
							WriteFile(hFile,pBuf,dwSize,&dwWrite,NULL);
							CloseHandle(hFile);
							hFile = INVALID_HANDLE_VALUE;
							SAFE_DELETE_ARRAY(pBuf);
						}
					}
				}
				else
				{
					CString strFormatted;
					strFormatted.Format(_T("PS/USER_IMG file [%s] is changed,please set LCD configure again."),strPSFileName.operator LPCTSTR());
					g_theApp.MessageBox(strFormatted);
					SAFE_DELETE_ARRAY(pBuf);
					return FALSE;
				}
			}					
		}	

		SAFE_DELETE_ARRAY(pBuf);
	}

	return TRUE;
}

BOOL CMainFrame::InitIMEIID()
{
	CWaitCursor wait;

	m_aIMEIID.RemoveAll();
	m_aIMEIID.Add(GSM_IMEI_ITEM_ID);

	_TCHAR szAppPath[_MAX_PATH]={0};
	GetModuleFilePath(g_theApp.m_hInstance,szAppPath);
	CFileFind finder;
	
	CString strBMFileType;
	strBMFileType.Format(_T("%s\\BMFileType.ini"),szAppPath);	
	if(!finder.FindFile(strBMFileType))
	{
		g_theApp.MessageBox(_T("Not found BMFileType.ini!"));
		return FALSE;		
	}

	_TCHAR szKeyValue[ MAX_BUF_SIZE ]={0}; 
	DWORD dwSize = GetPrivateProfileSection( _T("IMEI_ID"), szKeyValue, MAX_BUF_SIZE, strBMFileType );
	if(dwSize == 0)
	{
		return TRUE;
	}
	CStringArray arrKeyData;    
	UINT nIDCount = (UINT)EnumKeys(szKeyValue,&arrKeyData);
	if(nIDCount == 0)
	{		
		return TRUE;
	}	

	for(UINT i= 0;i< nIDCount; i++)
	{
		CString strTmp = arrKeyData[i*2+1];
		UINT uID = 0;
		strTmp.MakeLower();
		if(strTmp.Find(_T("0x")) == 0)
		{
			_stscanf(strTmp.operator LPCTSTR(),_T("0x%X"),&uID);
		}
		else
		{
			uID = _ttoi(strTmp.operator LPCTSTR());
		}
		if(uID > 0 && uID<0xFFFF)
		{
			m_aIMEIID.Add(uID);
		}
	}
	
	return TRUE;

}

void CMainFrame::ClearMultiFileBuf()
{
	MAP_FILEBUF::iterator it;
	for(it = m_mapMultiFileBuf.begin(); it != m_mapMultiFileBuf.end(); it++)
	{
		EXT_IMG_INFO_PTR pImg = it->second;
		if(pImg != NULL)
		{
			pImg->clear();
			delete pImg;
			it->second = NULL;
		}
	}

	m_mapMultiFileBuf.clear();
}

BOOL CMainFrame::InitMultiFileBuf()
{
	CWaitCursor wait;
	/*clear m_mapMultiFileBuf map*/
	ClearMultiFileBuf();

	if(!m_sheetSettings.IsEnableMultiFileBuf())
	{
		return TRUE;
	}

	int nStartFile = 1;

	if(m_sheetSettings.IsNandFlash())
	{
		nStartFile = 2;
	}

	CStringArray agChipName;
	CUIntArray   agChipID;
	m_sheetSettings.GetAllChipName(agChipName,agChipID);

	CStringArray agDLFiles;
	m_sheetSettings.GetDownloadFile(agDLFiles);
	int k=0;
	// check chip prefix
	CString strChipPre;
	BOOL bOK = TRUE;
	CString strErrChipPre = _T("");
 	for(k = nStartFile; k< agDLFiles.GetSize(); k++)
	{	
		CString strFilePath = agDLFiles.GetAt(k);
		if(strFilePath.IsEmpty() || strFilePath.CompareNoCase(FILE_OMIT) == 0 )
		{
			continue;
		}

		int nIndx = strFilePath.ReverseFind('\\');
		CString strFileName = strFilePath;
		if(nIndx != -1)
		{
			strFileName = strFilePath.Right(strFilePath.GetLength()-nIndx -1);
			strFilePath = strFilePath.Left(nIndx);
		}
		else
		{
			strFilePath.Empty();
		}
	
		if(strChipPre.IsEmpty())
		{
			for(int i = 0; i< agChipName.GetSize(); i++)
			{
				CString strChipName = agChipName.GetAt(i);
				strChipName += _T("_");
				if(strFileName.Left(strChipName.GetLength()).CompareNoCase(strChipName) == 0)
				{					
					strChipPre = strChipName;
					strChipPre.MakeUpper();
					break;
				}
			}
			if(strChipPre.IsEmpty())
			{
				CString strTmp = _T("");
				for(int j = 0; j< agChipName.GetSize(); j++)
				{
					strTmp += agChipName.GetAt(j);
					strTmp += _T("\n");
				}

				CString strErr;
				strErr.Format(_T("There are no download files matching with chip name prefix:\n%s."),strTmp);
				g_theApp.MessageBox(strErr);
				return FALSE;	
			}
		}
		else
		{
			if(strFileName.Left(strChipPre.GetLength()).CompareNoCase(strChipPre) != 0)
			{					
				bOK = FALSE;
				strErrChipPre += agDLFiles.GetAt(k);
				strErrChipPre += _T("\n");
			}
		}
	}

	if(!bOK)
	{
		CString strErr;
		strErr.Format(_T("Following files not match with chip name prefix \"%s\":\n%s."),strChipPre,strErrChipPre);
		g_theApp.MessageBox(strErr);
		return FALSE;
	}


	CStringArray agDLFileID;
	m_sheetSettings.GetAllFileID(agDLFileID);

	int i = 0;
	int j = 0;

	CString strErrNoFile = _T("");
	CString strErrMkMaster = _T("");

	bOK = TRUE;

	for(j = nStartFile; j< agDLFiles.GetSize(); j++)
	{		
		CString strFilePath = agDLFiles.GetAt(j);
		if(strFilePath.IsEmpty() || strFilePath.CompareNoCase(FILE_OMIT) == 0 )
		{
			continue;
		}

		CString strFileID = agDLFileID.GetAt(j);

		int nIndx = strFilePath.ReverseFind('\\');
		CString strFileName = strFilePath;
		if(nIndx != -1)
		{
			strFileName = strFilePath.Right(strFilePath.GetLength()-nIndx -1);
			strFilePath = strFilePath.Left(nIndx);
		}
		else
		{
			strFilePath.Empty();
		}

		CString strFileTitle;

// 		for(i = 0;  i< agChipName.GetSize(); i++)
// 		{
// 			CString strChipName = agChipName.GetAt(i);
// 			strChipName += _T("_");
// 			if(strFileName.Left(strChipName.GetLength()).CompareNoCase(strChipName) == 0)
// 			{
// 				strFileTitle = strFileName.Right(strFileName.GetLength() -strChipName.GetLength());
// 				break;
// 			}		
// 		}	

		strFileTitle = strFileName.Right(strFileName.GetLength() - strChipPre.GetLength());

// 		if(strFileTitle.IsEmpty())
// 		{
// 			//need not deal with error, becaude checked file size before.
// 		}

		for(i = 0;  i< agChipName.GetSize(); i++)
		{
			DWORD   dwChipID = agChipID.GetAt(i);
			CString strChipName = agChipName.GetAt(i);
			CString strCurFilePath;
			strCurFilePath.Format(_T("%s\\%s_%s"),strFilePath,strChipName,strFileTitle);

			CFileFind finder;
			if(!finder.FindFile(strCurFilePath))		
			{
				bOK = FALSE;
				strErrNoFile += strCurFilePath;
				strErrNoFile += _T("\n");
			}
			else
			{

				DATA_INFO_T dataInfo;				
				EXT_IMG_INFO_PTR pImg = new EXT_IMG_INFO;
				if( pImg == NULL ) 
				{
					bOK = FALSE;
					strErrNoFile += _T("new opr fail\n");
					break;                        
				}
				if (m_sheetSettings.IsLoadFromPac(strCurFilePath.operator LPCTSTR()))
				{
					_tcscpy(pImg->szFilePath,m_strPacketPath.operator LPCTSTR());
					dataInfo = m_sheetSettings.GetDataInfo(strCurFilePath.operator LPCTSTR());
				}
				else
				{
					_tcscpy(pImg->szFilePath,strCurFilePath.operator LPCTSTR());
				}
				
				/*CFile file(strCurFilePath,CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary);
				pImg->dwSize = (DWORD)file.GetLength();			
				if(pImg->dwSize  != 0 )
				{	
					pImg->pBuf = new BYTE[pImg->dwSize];
					file.Read(pImg->pBuf ,pImg->dwSize);
				}				
				file.Close();*/
				LoadFileFromLocal(pImg->szFilePath,pImg->pBuf,pImg->dwSize,dataInfo.llSize,dataInfo.llOffset);

				if(pImg->pBuf != NULL)
				{
					PFILE_INFO_T pfi = NULL;
					
					m_sheetSettings.GetFileInfo(strFileID,(LPDWORD)&pfi);
					if( pfi != NULL)
					{
						CString strFileType = pfi->szType;
						
						if( strFileType.CompareNoCase(_T("MasterImage"))==0 )
						{		
							DWORD dwSize = 0;
							LPBYTE pBuf = NULL;
							IMAGE_PARAM imageparam;						
							memset( &imageparam,0,sizeof( IMAGE_PARAM ) );
							_tcscpy( imageparam.szPath,pImg->szFilePath );
							CMasterImgGen mig;
							pBuf = (LPBYTE)mig.MakeMasterImageSingle( &dwSize,1,&imageparam,m_sheetSettings.GetFlashPageType(),dataInfo.llSize,dataInfo.llOffset );
							
							if(pBuf == NULL)
							{
								//error;
								strErrMkMaster += strCurFilePath;
								strErrMkMaster += _T("\n");
								bOK = FALSE;
							}	
							else
							{
								delete [] pImg->pBuf;
								pImg->pBuf  = pBuf;
								pImg->dwSize = dwSize;
							}							
						}
					}

					m_mapMultiFileBuf[std::make_pair(dwChipID,strFileID)] = pImg;

					// NV may have two files
					if(strFileID.Find(_T("NV")) == 0 && m_sheetSettings.IsNVOrgDownload())
					{
						CString strExtID /*= _T("NVOriginal")*/;
						strExtID.Format(_T("_ORG_%s"),strFileID);
						EXT_IMG_INFO_PTR pImg2 = new EXT_IMG_INFO;
						if( pImg2 == NULL ) 
						{
							bOK = FALSE;
							strErrNoFile += _T("new opr fail\n");
							break;                        
						}
						_tcscpy(pImg2->szFilePath,pImg->szFilePath);
						pImg2->dwSize = pImg->dwSize;
						if(pImg2->dwSize != 0)
						{
							pImg2->pBuf = new BYTE[pImg2->dwSize];
							if (pImg2->pBuf)
							{
								memcpy(pImg2->pBuf,pImg->pBuf,pImg2->dwSize);
							}
							else
							{
								bOK = FALSE;
							}

						}
						m_mapMultiFileBuf[std::make_pair(dwChipID,strExtID)] = pImg;

					}
				}
				else
				{
					
					bOK = FALSE;
					strErrNoFile += _T("LoadFileFromLocal fail\n");                        
				}

				
			}//if(!finder.FindFile(strCurFilePath)) else		
		}
	}

	if(bOK)
	{
		return TRUE;
	}
	else
	{
		ClearMultiFileBuf();

		CString strTotalErr1;
		if(strErrNoFile.GetLength()!=0)
		{
			strTotalErr1.Format(_T("Not found following files:\n%s"),strErrNoFile);
		}
		CString strTotalErr2;
		if(strErrMkMaster.GetLength()!=0)
		{
			strTotalErr2.Format(_T("Make master image for following files failed:\n%s"),strErrMkMaster);
		}

		CString strTotalErr;
		strTotalErr += strTotalErr1;
		strTotalErr += strTotalErr2;
		g_theApp.MessageBox(strTotalErr);
		return FALSE;
	}	
}

BOOL CMainFrame::InitMapPBFileBuf()
{
	CWaitCursor wait;
	/*clear m_mapPBFileBuf map*/
	ClearMapPBFileBuf();

	if(!m_sheetSettings.IsMapPBFileBuf())
	{
		return TRUE;
	}

	PFILE_INFO_T pFileInfo = NULL;
	int nCount = m_sheetSettings.GetAllFileInfo((LPDWORD)&pFileInfo);

	int i=0;
	CString strErr = _T("");
	int nPBFileCount = 0;
 	for(i = 0; i< nCount; i++)
	{	
		CString strFileType = pFileInfo[i].szType;
		CString strFileID = pFileInfo[i].szID;
		CString strFilePath = m_sheetSettings.GetDownloadFilePath(strFileID);
		if( strFilePath.IsEmpty() || 
			strFilePath.CompareNoCase(FILE_OMIT) == 0 ||
			pFileInfo[i].isSelByFlashInfo != 1 )
		{
			continue;
		}

		CStringArray agFilePathInfo;
		GetFilePathInfo(strFilePath,agFilePathInfo);
		CString strFileName = agFilePathInfo[1];
		int  nIndex = strFileName.Find(_T("_b"));
		if(nIndex == -1)
		{
			strErr.Format(_T("The name of file [%s] is invalid!\nFile name must be \"xxx_bnk_pmk.yyy\", n and m is a number."),strFilePath);
			g_theApp.MessageBox(strErr);
			ClearMapPBFileBuf();
			return FALSE;
		}

		nPBFileCount++;

		CString strExt = agFilePathInfo[2];
		CString strFilePre = strFileName.Left(nIndex);
		CString strFind;

 		if (m_sheetSettings.IsLoadFromPac(strFilePath.operator LPCTSTR()))  //*.bin.flag
 		{
			CStringArray agTmpFilePathInfo;
 			GetFilePathInfo(strFileName,agTmpFilePathInfo);
			strExt = agTmpFilePathInfo.GetAt(2)+ agFilePathInfo.GetAt(2);
 		}
		strFind.Format(_T("%s\\%s_b*%s"),agFilePathInfo[0],strFilePre,strExt);

		WIN32_FIND_DATA wfd = { 0 };
		HANDLE hFind = ::FindFirstFile( strFind, &wfd );
		BOOL bFind = TRUE;
		while( INVALID_HANDLE_VALUE != hFind  && bFind)
		{
			CString strPBInfo =((CString) (wfd.cFileName)).Left(((CString) (wfd.cFileName)).GetLength() - strExt.GetLength());
			strPBInfo = strPBInfo.Right(strPBInfo.GetLength()-strFilePre.GetLength());
			
			CString strFile;
			strFile.Format(_T("%s\\%s"),agFilePathInfo[0].operator LPCTSTR(),wfd.cFileName);
			EXT_IMG_INFO_PTR pImg = LoadPageBlockFile(strFile);

			if(pImg == NULL)
			{
				strErr.Format(_T("Load file [%s] failed!"),strFile);
				g_theApp.MessageBox(strErr);
				::FindClose(hFind);	
				ClearMapPBFileBuf();	
				return FALSE;
			}

			strPBInfo.MakeLower();

			m_mapPBFileBuf[std::make_pair(strPBInfo,strFileID)] = pImg;

			bFind = ::FindNextFile(hFind, &wfd);
			
			DWORD dwPBCount = 0;
			
			if(m_mapPBInfo.Lookup(strPBInfo,dwPBCount))
			{
				dwPBCount += 1;				
			}
			else
			{
				dwPBCount = 1;				
			}
			m_mapPBInfo.SetAt(strPBInfo,dwPBCount);
		}

		if(INVALID_HANDLE_VALUE != hFind)
		{
			::FindClose(hFind);	
		}			
	}

	POSITION pos = m_mapPBInfo.GetStartPosition();
	while(NULL != pos)
	{
		CString strKey;
		DWORD dwPBCount = 0;
      
        m_mapPBInfo.GetNextAssoc( pos, strKey, dwPBCount );

		if(dwPBCount != (DWORD)nPBFileCount)
		{
			strErr.Format(_T("Block-Page [%s] only have %d image files.\nIt must be equal to %d."),
				          strKey,dwPBCount,nPBFileCount);
			g_theApp.MessageBox(strErr);
			ClearMapPBFileBuf();
			return FALSE;
		}
	}

	return TRUE;
}

void CMainFrame::ClearMapPBFileBuf()
{
	MAP_FILEBUF_PB::iterator it;
	for(it = m_mapPBFileBuf.begin(); it != m_mapPBFileBuf.end(); it++)
	{
		EXT_IMG_INFO_PTR pImg = it->second;
		if(pImg != NULL)
		{
			pImg->clear();
			delete pImg;
			it->second = NULL;
		}
	}
	
	m_mapPBFileBuf.clear();
	m_mapPBInfo.RemoveAll();
}

BOOL CMainFrame::LoadPac()
{
	CWaitCursor wait;
    m_bPacLoading   = TRUE;
	DeletePacTmpDir();
	m_bPacketOpen = FALSE;	
	
	CFileFind finder;
	if(finder.FindFile(m_strPacketPath))
	{
		finder.FindNextFile();
		finder.GetLastWriteTime(m_tmPacModifyTime);		
	}
	finder.Close();	
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        pSysMenu->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND|MF_DISABLED);
    }
	
	if(!m_sheetSettings.LoadPacket(m_strPacketPath))
	{
		
        if (pSysMenu != NULL)
        {
            pSysMenu->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND|MF_ENABLED);
        }
		CString strInfo;
		strInfo.LoadString(IDS_ERR_LOAD_PACKET_FAIL);
		g_theApp.MessageBox(strInfo);
        m_bPacLoading   = FALSE;
        DeletePacTmpDir();
		if(g_theApp.m_bCMDFlash)
		{
			PostMessage(WM_CLOSE);
		}
		return FALSE;	
	}
    m_bPacLoading   = FALSE;
    if (pSysMenu != NULL)
    {
        pSysMenu->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND|MF_ENABLED);
    }
	
	m_bPacketOpen = TRUE;
	
	if(!LoadSettings())
    {
		m_bPacketOpen = FALSE;
		if(!m_strLoadSettingMsg.IsEmpty())
		{
			CString strError;
			strError.LoadString(IDS_LOAD_SETTINGS_FAIL);
			strError += m_strLoadSettingMsg;
			g_theApp.MessageBox(strError,MB_ICONSTOP);
		}

        DeletePacTmpDir();
		if(g_theApp.m_bCMDFlash)
		{
			CString strInfo;
			strInfo.Format(_T("%s fail:CRC Error! PAC file may be damaged!"),EZ_MODE_STATUS_FLAG);
			g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
			PostMessage(WM_CLOSE);
		}
        return FALSE;
    }	
	
#ifdef _FACTORY
	m_sheetSettings.m_pageCalibration.m_lstBackup.m_bBackNV = FALSE;
	m_sheetSettings.m_pageMultiLang.m_bBackupLang = FALSE;
#endif	

	SetDLTitle();

	return TRUE;
}

void  CMainFrame::InitDebugData()
{
	m_tDebugData.Clear();
	_TCHAR szConfigPath[_MAX_PATH]={0};
	if (m_sheetSettings.IsEnableDebugMode() && g_theApp.GetIniFilePath(szConfigPath))
	{
		USES_CONVERSION;
		_TCHAR szBuf[_MAX_PATH*2] = {0};
		GetPrivateProfileString(_T("Debug"),_T("Data"),_T(""),szBuf,_MAX_PATH,szConfigPath);
		CHAR* pData = T2A(szBuf);
		DWORD dwSize = (strlen(pData)+1)/2*2;
		if (0 == dwSize)
		{
			return;
		}
		m_tDebugData.pBuf = new BYTE[dwSize];
		if (m_tDebugData.pBuf)
		{
			m_tDebugData.dwSize = dwSize;
			memset(m_tDebugData.pBuf,0,dwSize);
			memcpy(m_tDebugData.pBuf,pData,strlen(pData));
		}
		
	}
	
}

BOOL CMainFrame::IsValidVComDriverVer(CString& strError)
{
	if (!g_theApp.m_bCheckDriverVer)
	{
		return TRUE;
	}

    if (g_bValidVComDriverVer)
    {
        return g_bValidVComDriverVer;
    }
	strError.Empty();

    TCHAR szDriverFile[MAX_PATH] = {0};
    if (!GetSystemDirectory(szDriverFile, MAX_PATH))
    {
        return TRUE;
    }
    BOOL bOK = FALSE;

    _tcscat(szDriverFile,_T("\\drivers\\sprdvcom.sys"));
    CXVerInfo Ver;

    if (!Ver.Init(szDriverFile))
    {
        //strError = _T("Please install the download driver first!!!");
		//Just check sprdvcom.sys
		return TRUE;
    }
   
	CString strVersion=Ver.GetProductVersion();
	strVersion.Replace( _T(","),_T(".") );
	strVersion.Replace(_T(" "),_T("") ); 	

	CStringArray agVer;
	int nCount = SplitStr(strVersion,agVer,_T('.'));
	if (nCount ==4 )
	{
		UINT nDstVer = (2<<24) + 131;
		UINT nCurVer = (_ttoi(agVer[0])<<24) + (_ttoi(agVer[1])<<16) + (_ttoi(agVer[2])<<8) +_ttoi(agVer[3]);
		if (nCurVer >= nDstVer)
		{
			bOK = TRUE;
		}	
	}
	else
	{
		strError.Format(_T("Your driver version is %s, it is too low,\nPlease upgrade download driver !!!"),strVersion);          
	}

    g_bValidVComDriverVer = bOK;
    return bOK;

}

BOOL CMainFrame::IsSupportCU()
{
    return m_bSupportCU;
}

BOOL CMainFrame::IsSupportCheckRoot()
{
    return m_bSupportCheckRoot;
}

BOOL CMainFrame::IsSupportCheckInproduction()
{
    return m_bSupportCheckInproduction;
}

LRESULT CMainFrame::OnStartDownload(WPARAM wParam,LPARAM lpParam)
{
	UNUSED_ALWAYS(wParam);
	UNUSED_ALWAYS(lpParam);

    

	CWaitCursor wait;

	CString strErrMsg;
    if( FAILED( CoInitialize(NULL) ) )
    {
        strErrMsg.LoadString( IDS_ATL_INIT_FAIL );
		g_theApp.MessageBox(strErrMsg);
		m_bStarted = FALSE;
        return 0;
    }    

#if defined( _FACTORY ) || defined(_SPUPGRADE)
#else
	if(m_sheetSettings.IsEraseAll())
	{
        //strErrMsg = _T("You have selected [Erase All Flash], this will clear all data stored in the flash including calibration.IMEI will be erased too and cannot be restored,If continue?");
		if(AfxMessageBox( m_strEraseAllFlashMsg,MB_YESNO )==IDNO)
		{
			m_bStarted = FALSE;
			return 0;
		}
	}
    else if (m_sheetSettings.IsEraseIMEI())
    {
        
        //strErrMsg = _T("You don't select to backup IMEI item,IMEI will be erased and cannot be restored,If continue?");
        if(AfxMessageBox( m_strEraseIMEIMsg,MB_YESNO )==IDNO)
        {
            m_bStarted = FALSE;
            return 0;
        }
    }

	if(m_bPacketOpen && !m_strPacketPath.IsEmpty())
	{
		CFileFind finder;
		if(finder.FindFile(m_strPacketPath))
		{
			CTime tm;
			finder.FindNextFile();
			finder.GetLastWriteTime(tm);
			if(tm != m_tmPacModifyTime)
			{
				if(AfxMessageBox( _T("Pac file is updated, do want load the new pac file?"),MB_YESNO )==IDYES)
				{
					 if(!LoadPac())
					 {
						 DeletePacTmpDir();
						 m_bStarted = FALSE;
						 return 0;
					 }
				}
			}
		}
	}
#endif

	wait.Restore();
    m_arrFile.RemoveAll();
	m_sheetSettings.GetDownloadFile(m_arrFile);
	m_nFileCount = m_arrFile.GetSize();
	ClearNVMap();
	ClearMultiNVMap();

	m_strCodeChipID = _T("");

#if defined( _FACTORY ) || defined(_SPUPGRADE)

    _TCHAR szConfigIniFile[MAX_PATH];    
    GetModuleFilePath( g_theApp.m_hInstance, szConfigIniFile );
    _tcscat( szConfigIniFile,  _T("\\BMFileType.ini") );
    BOOL bEnableCrcCfgCheck = GetPrivateProfileInt( _T("Misc") , _T("EnableCrcCfgCheck"), FALSE, szConfigIniFile);  

    if(bEnableCrcCfgCheck)
    {
        CString strCrcFailFileId;
        if(!m_sheetSettings.CheckCrcDLFiles(strCrcFailFileId))
        {
            CString strError;
            strError.Format(_T("The CRC check for pac is failed at %s"),strCrcFailFileId.operator LPCTSTR());
            g_theApp.MessageBox(strError);
            m_bStarted = FALSE;
            return 0;
        }
    }

#endif

	//////////////////////////////////////////////////////////////////////////
	//  check files
	if(!CheckDLFiles())
	{
		m_bStarted = FALSE;
		return 0;
	}	

	InitDebugData();

	//////////////////////////////////////////////////////////////////////////	
	// get multi-files for different chip name.
	if(!InitMultiFileBuf())
	{
		m_bStarted = FALSE;
		return 0;
	}
	
	//////////////////////////////////////////////////////////////////////////
	//  initialize report success infomation
	if(!InitReportInfo())
	{
		m_bStarted = FALSE;
		return 0;
	}
	
	//////////////////////////////////////////////////////////////////////////
	if (!InitMultiNVBuffer())
	{
		m_bStarted = FALSE;
		return 0;
	}
	
	// init NV buffer
	if(!InitNVBuffer())
	{
		m_bStarted = FALSE;
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////	
	// init UDiskIMG buffers
	if(!InitUDiskBufMap())
	{
		m_bStarted = FALSE;
		return 0;
	}
	
	//////////////////////////////////////////////////////////////////////////	
	// init DSP files buffers
	if(!InitChipDspBufmap())
	{
		m_bStarted = FALSE;
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////	
	// get chip ID string from FDL2
	InitCodeChipID();
	
	//////////////////////////////////////////////////////////////////////////	
	// get IMEI ID from BMFileType.ini
	if(!InitIMEIID())
	{
		m_bStarted = FALSE;
		return 0;
	}
  
	if(!InitPartitionData())
	{
		m_bStarted = FALSE;
		return 0;
	} 

	// added ExtTable @ polo.jiang on 20141001
	if (!InitExtTblData())
	{
		m_bStarted = FALSE;
		return 0;
	}

	//Added softsim function
	if (!InitSoftSIM())
	{
		m_bStarted = FALSE;
		return 0;
	}

	if(!InitMapPBFileBuf())
	{
		m_bStarted = FALSE;
		return 0;
	} 
    if (!CheckPacKey())
    {
        m_bStarted = FALSE;
        return 0;
    }

#ifdef _SPUPGRADE
	if(m_sheetSettings.IsSharkNand())
	{
		if(!CreateBKFileFolder())
		{
			m_bStarted = FALSE;
			return 0;
		}
	}
#endif

	CheckPort2ndEnum();
    //InitDownloadLog();

	((CDLoaderView *)(this->GetActiveView()))->StartDLTimer();
		
    StartWork();

	return 0;
}
void CMainFrame::InitDownloadLog()
{
    TCHAR szPath[MAX_PATH] = {0};
    TCHAR szChannelLog[MAX_PATH] = {0};
    TCHAR szBMLog[MAX_PATH] = {0};
    GetModuleFilePath(NULL,szPath);
    _stprintf(szChannelLog,_T("%s\\Channel.ini"),szPath);
    _stprintf(szBMLog,_T("%s\\BMTimeout.ini"),szPath);

    if(m_sheetSettings.IsEnableLog())
    {
        WritePrivateProfileString( _T("Log"),_T("Level"),_T("5"),szChannelLog ); 
        WritePrivateProfileString( _T("Log"),_T("Enable"),_T("1"),szBMLog ); 
    }
    else
    {
        WritePrivateProfileString( _T("Log"),_T("Level"),_T("0"),szChannelLog ); 
        WritePrivateProfileString( _T("Log"),_T("Enable"),_T("0"),szBMLog ); 
    }

}

BOOL CMainFrame::SaveBackupFileToLocal(BACKUP_FILE_TYPE eBackupType,_BMOBJ *pbj, BYTE *pBuf, DWORD dwSize)
{
	USES_CONVERSION;
	CString strErrMsg;
	CString strLocalPath = m_sheetSettings.GetNVSavePath();	
	if(strLocalPath.IsEmpty())
	{
		strErrMsg.Format(_T("%s save to local: the path is empty."),g_szBackupFileType[eBackupType]);
		_tcscpy(pbj->szErrorMsg,strErrMsg);
		return FALSE;
	}	
	CString strFullPath;
	HANDLE hFile = INVALID_HANDLE_VALUE;				
	SYSTEMTIME timeCur;
	::GetLocalTime(&timeCur);
	CString strPrefixed;
	TCHAR* pTSN = A2T(pbj->szSN);
	TCHAR* pTIMEI = A2T(pbj->szIMEI);
	if (m_bNameWithSN)
	{
		strPrefixed.Format(_T("%s%s_%s"),(pTSN && _tcslen(pTSN)) ? _T("SN") : _T("IMEI"),(pTSN && _tcslen(pTSN)) ? pTSN : pTIMEI,g_szBackupFileType[eBackupType] );
	}
	else
	{
		strPrefixed.Format(_T("%s%s_%s"),_T("IMEI"),pTIMEI,g_szBackupFileType[eBackupType]);
	}
	strFullPath.Format(_T("%s\\%s_%4d-%02d-%02d_%02d-%02d-%02d-%03d"),strLocalPath.operator LPCTSTR(),
		strPrefixed.operator LPCTSTR(),
		timeCur.wYear,timeCur.wMonth,timeCur.wDay,
		timeCur.wHour,timeCur.wMinute,timeCur.wSecond,timeCur.wMilliseconds);

	CFileFind finder;
	int ext = 1;
	while(finder.FindFile(strFullPath+_T(".bin")))
	{
		CString strExt;
		strExt.Format(_T("(%d)"),ext++);
		strFullPath += strExt;
	}
	strFullPath += _T(".bin");
	finder.Close();
	hFile = CreateFile(strFullPath,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile != INVALID_HANDLE_VALUE)						
	{
		DWORD dwWritten=0;
		WriteFile(hFile,pBuf,dwSize,&dwWritten,NULL);
		CloseHandle(hFile);	
	}
	else
	{
		strErrMsg.Format(_T("[save %s to local] Create file [%s] fail."),g_szBackupFileType[eBackupType],strFullPath.operator LPCTSTR());
		_tcscpy(pbj->szErrorMsg,strErrMsg);
		return FALSE;
	}	
	return TRUE;
}

LRESULT CMainFrame::OnDevHound(WPARAM wParam,LPARAM lpParam)
{
	if( m_bAutoStart)
    {
		unsigned int nPort = (unsigned int)wParam;
		_TCHAR szName[DEV_NAME_MAX_LEN] = {0};
		//m_usbMoniter.GetPortName(nPort,szName);
        m_pUsbMoniter->GetPortName(nPort,szName);


		CString strError;
		CString strPortName = szName;
		unsigned int nSelPort = (unsigned int)m_sheetSettings.GetComPort();
		BMOBJ_PTR pbj = NULL;
        switch(lpParam)
        {
        case 1: //DBT_DEVICEARRIVAL            
			if(nSelPort != 0 && nPort != nSelPort)
			{
				break;
			}

			if(m_dwSinglePort != 0 && nPort != m_dwSinglePort)
			{
				break;
			}

			if(g_theApp.m_bFilterPort)
			{
				DWORD dwValue = 0;
				if(g_theApp.m_mapFilterPort.Lookup((DWORD)nPort,dwValue))
				{
					break;
				}
			}  	
			
			if(m_sheetSettings.IsNeedRebootByAT())
			{
				// Is AT port?
				if(IsATPort(strPortName))
				{
					SendAT2Reboot(nPort);
					break;
				}
			}
			
			if(!IsDLPort(strPortName,(DWORD)nPort)) // If not DL port, continue
			{
				break;
			}
			
            if(strPortName.CompareNoCase(_T("SPRD U2S Diag")) == 0)
            {
                if(!IsValidVComDriverVer(strError))
                {
                    m_pUsbMoniter->Stop();
                    g_theApp.MessageBox(strError);
                    PostMessage(WM_COMMAND, ID_STOP, 0);
                    return 0;
                }
            }

#ifndef _FACTORY
            DL_STAGE stage;
            if( ((CDLoaderView*)GetActiveView())->GetStatus( nPort,stage ) )
            {
                if( DL_FINISH_STAGE != stage && DL_NONE_STAGE != stage && DL_UNPLUGGED_STAGE != stage)
                {
                    // Last download process is not finished.
                    break;
                }
            }
#endif
			// The port plugged is regarded as USB
            CreatePortData( nPort );
			if(g_theApp.m_bManual)
			{
				((CDLoaderView*)GetActiveView())->AddProg( nPort, TRUE ); 
			}
			else
			{
				if (StartOnePortWork(nPort,FALSE))
				{
					if (m_bNeedCompare)
					{
						m_dwSinglePort = nPort;
					}
				}
					
			}
    
            break;
        case 0: //DBT_DEVICEREMOVECOMPLETE
            
			if(nSelPort != 0 && nPort != nSelPort)
			{
				break;
			}

			if(g_theApp.m_bFilterPort)
			{
				DWORD dwValue = 0;
				if(g_theApp.m_mapFilterPort.Lookup((DWORD)nPort,dwValue))
				{
					break;
				}
			}				
#ifndef _FACTORY
			DL_STAGE _stage;
            if( ((CDLoaderView*)GetActiveView())->GetStatus( nPort,_stage ) )
            {
                if( DL_NONE_STAGE == _stage)
                {
                    // Last download process is not finished.
                    break;
                }
            }
#endif
			
			m_mapBMObj.Lookup((DWORD)nPort, pbj);
			
			// CDLoaderView displays [Removed].
			((CDLoaderView*)GetActiveView())->SetResult( nPort,0,pbj,2);   
			StopOnePortWork((DWORD)nPort,TRUE);
            break;
        default:
            break;
        }
    }	
	return 0L;
}

BOOL CMainFrame::SendAT2Reboot(UINT nPort)
{
	ICommChannel *pChannel = NULL;

	g_theApp.m_pfCreateChannel(&pChannel,CHANNEL_TYPE_COM);

	char szCmd[] = "AT+SPREF=\"AUTODLOADER\"\r\n";

	CHANNEL_ATTRIBUTE ca;
	ca.ChannelType = CHANNEL_TYPE_COM;
	ca.Com.dwPortNum = nPort;
	ca.Com.dwBaudRate = 115200;

	if(pChannel->Open(&ca))
	{
		pChannel->Write(szCmd,strlen(szCmd));
		pChannel->Write(szCmd,strlen(szCmd));
		pChannel->Write(szCmd,strlen(szCmd));
	}

	pChannel->Close();
	g_theApp.m_pfReleaseChannel(pChannel);

	return TRUE;
}

BOOL CMainFrame::IsATPort(LPCTSTR lpszPort)
{
	if(m_agATPort.GetSize()==0)
		return FALSE;

	CString strPortName = lpszPort;

	for(int i = 0; i< m_agATPort.GetSize(); i++)
	{
		CString strATPort = m_agATPort[i];
		if( strPortName.Find(strATPort) != -1)
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CMainFrame::IsDLPort(LPCTSTR lpszPort,DWORD dwPort)
{
	BOOL bNameIsDLPort		= FALSE;
	BOOL bNumberIsDLPort	= FALSE;
	int	 nCount			= 0;

	//Check Port  Name
	nCount = m_agDLPort.GetSize();
	if ( 0 == nCount )
	{
		bNameIsDLPort = TRUE;
	}
	else
	{
		CString strPortName = lpszPort;

		for(int i = 0; i< nCount; i++)
		{
			CString strDLPort = m_agDLPort[i];
			if( strPortName.Find(strDLPort) != -1)
			{
				bNameIsDLPort = TRUE;
				break;
			}
		}
	}

	//Check Port Number
	nCount =  g_theApp.m_mapFixPort.GetCount();
	if ( 0 == nCount )
	{
		bNumberIsDLPort	= TRUE;
	}
	else
	{
		DWORD dwValue = 0;
		if(g_theApp.m_mapFixPort.Lookup(dwPort,dwValue))
		{
			bNumberIsDLPort	= TRUE;
		}
		if (g_theApp.m_bCMDFlash && g_theApp.m_dwMultiCount>1)
		{
			bNumberIsDLPort	= TRUE;
		}

	}
	
	return (bNameIsDLPort && bNumberIsDLPort) ;
}

void CMainFrame::ClearNVMap()
{
	CString strID;
	BACKUP_INFO_PTR pBkInfo = NULL;
	POSITION pos = m_mapNVFileInfo.GetStartPosition();
    while( pos )
    {
		pBkInfo = NULL;
        m_mapNVFileInfo.GetNextAssoc( pos, strID, pBkInfo );	
		if(pBkInfo)
		{
			pBkInfo->Clear();
			delete pBkInfo;
			pBkInfo = NULL;
		}		
    }
	m_mapNVFileInfo.RemoveAll();
}

void CMainFrame::ClearMultiNVMap()
{

	MAP_MULTI_NVFILE::iterator it;
	for(it = m_mapMultiNVInfo.begin(); it != m_mapMultiNVInfo.end(); it++)
	{
		BACKUP_INFO_PTR pBkInfo = it->second;
		if(pBkInfo != NULL)
		{
			pBkInfo->Clear();
			delete pBkInfo;
			pBkInfo = NULL;
			it->second = NULL;
		}
	}

	m_mapMultiNVInfo.clear();
}

BOOL  CMainFrame::InitExtTblData()
{
	if(!m_sheetSettings.HasExtTblInfo())
	{
		return TRUE;
	}
	SAFE_DELETE_ARRAY(m_pExtTblData);
	m_dwExtTblSize = 0;
	
	m_pExtTblData = m_sheetSettings.GetExtTblData(m_dwExtTblSize);
	
	return (m_pExtTblData != NULL);
}
void CMainFrame::RemoveLstSoftSim()
{
	std::list<TCHAR*>::iterator iter = m_lstSoftSim.begin();
	for( ;iter != m_lstSoftSim.end();)
	{
		if (*iter)
		{
			delete[] (*iter);
		}
		m_lstSoftSim.erase(iter++);	
	}
	m_lstSoftSim.clear();
}
BOOL	CMainFrame::FindSoftSim(LPCTSTR lpSrcDir,std::list<TCHAR*>& lstFile,LPCTSTR lpPostfix)   
{   
	
	HANDLE  hResult			= NULL;   
	TCHAR szFind[MAX_PATH]	= {0};
	TCHAR szFile[MAX_PATH]	= {0};
    WIN32_FIND_DATA FindFileData;
	if (NULL == lpSrcDir)
	{
		return FALSE;
	}
    _tcscpy(szFind,lpSrcDir);
    _tcscat(szFind, _T("\\*"));
    
	hResult  = FindFirstFile(szFind,&FindFileData);   
	
	if(hResult  ==  INVALID_HANDLE_VALUE)   
	{   
		return  FALSE;   
	}   
    
	if(_tcscmp(FindFileData.cFileName,_T(".")) && _tcscmp(FindFileData.cFileName ,_T("..")))   
	{   
		_tcscpy(szFile,lpSrcDir);   
		_tcscat(szFile,_T("\\"));   
		_tcscat(szFile,FindFileData.cFileName);   
		
		if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)   		
		{    
			FindSoftSim(szFile,lstFile,lpPostfix);  
		}   
		else   
		{   
			CString strPostfix = GetFilePostfix(szFile);
			if ( 0 == strPostfix.CompareNoCase(lpPostfix) )
			{
				int nLen = _tcslen(szFile)+1;
				TCHAR* pFindFile = new TCHAR[nLen];
				if (pFindFile)
				{
					_tcscpy(pFindFile,szFile);
					lstFile.push_back(pFindFile);
				}
			}
			
		}   
	}   
	while(1)   
	{   
		if(!FindNextFile(hResult,&FindFileData))   
			break;   
		
		if(_tcscmp(FindFileData.cFileName , _T(".")) && _tcscmp(FindFileData.cFileName   ,_T("..")) )   
		{   
			_tcscpy(szFile,lpSrcDir);   
			_tcscat(szFile,_T("\\"));   
			_tcscat(szFile,FindFileData.cFileName);  
			
			if((FindFileData.dwFileAttributes) == FILE_ATTRIBUTE_DIRECTORY)   // & FILE_ATTRIBUTE_DIRECTORY		
			{   
				FindSoftSim(szFile,lstFile,lpPostfix);     
			}   
			else   
			{   	
				CString strPostfix = GetFilePostfix(szFile);
				if ( 0 == strPostfix.CompareNoCase(lpPostfix) )
				{
					int nLen = _tcslen(szFile)+1;
					TCHAR* pFindFile = new TCHAR[nLen];
					if (pFindFile)
					{
						_tcscpy(pFindFile,szFile);
						lstFile.push_back(pFindFile);
					}		
				}
				
			}   
		}   
	}   
	FindClose(hResult);
	return   TRUE;   
} 
BOOL  CMainFrame::InitSoftSIM()
{
	BOOL bRet = FALSE;
	CString strErr;
	if(!m_sheetSettings.IsSoftSimPrj())
	{
		return TRUE;
	}

	do 
	{
		RemoveLstSoftSim();	
		
		CString strSoftSimFile = m_sheetSettings.GetSoftSimFile();
		CString strSoftSimDir = GetStrFilePath(strSoftSimFile.operator LPCTSTR());
		if (!m_sheetSettings.IsValidSoftSIM(strSoftSimDir.operator LPCTSTR()))
		{
			strErr.Format(_T("SoftSim\'file directory can't same as application and other download file\'s directory!\n%s"),
				_T("Please put SoftSim\'s file in independent directory!"));
			break;
		}

		if (!strSoftSimDir.IsEmpty())
		{
			CString strPostFix = GetFilePostfix(strSoftSimFile.operator LPCTSTR());
			FindSoftSim(strSoftSimDir.operator LPCTSTR(),m_lstSoftSim,strPostFix.operator LPCTSTR());
		}
		bRet = (m_lstSoftSim.size() != 0 )? TRUE :FALSE;
		
		strErr.Format(_T("There are no download files matching with \'SoftSIM\'."));
	} while(0);
	
	if (!bRet)
	{
		g_theApp.MessageBox(strErr);
	}
	return bRet;
}
BOOL CMainFrame::InitPartitionData()
{
	if(!m_sheetSettings.HasPartitionInfo())
	{
		return TRUE;
	}

	SAFE_DELETE_ARRAY(m_pPartitionData);
	m_dwPartitionSize = 0;

	m_pPartitionData = m_sheetSettings.GetPartitionData(m_dwPartitionSize);

	return (m_pPartitionData != NULL);
}

BOOL CMainFrame::AddReadSN(PBMFileInfo pBMFileInfo)
{
	PFILE_INFO_T pPhaseCheckFileInfo = NULL;
	if(   m_sheetSettings.GetFileInfo(_T("PhaseCheck"),(LPDWORD)&pPhaseCheckFileInfo) != -1
		&& pPhaseCheckFileInfo != NULL)
	{
		// add Read SN function for saving NV to local as the name of NV file	
		pBMFileInfo->bLoadCodeFromFile = FALSE;
		_tcscpy(pBMFileInfo->szFileID,_T("ReadSN"));
		_tcscpy(pBMFileInfo->szRepID, pPhaseCheckFileInfo->arrBlock[0].szRepID);
		if(m_sheetSettings.HasPartitionInfo())
		{
			_tcscpy(pBMFileInfo->szFileType, _T("ReadSN2")); 
		}
		else
		{
			_tcscpy(pBMFileInfo->szFileType, _T("ReadSN")); 
		}		
		
		CString strDes = _T("ReadSN");
		((CDLoaderView *)(GetActiveView()))->AddStepDescription(strDes);			
		
		unsigned __int64 llPhaseCheckSize = pPhaseCheckFileInfo->arrBlock[0].llSize;
		if( pPhaseCheckFileInfo->arrBlock[0].llSize == 0 ||
			pPhaseCheckFileInfo->arrBlock[0].llSize > PRODUCTION_INFO_SIZE)
		{
			llPhaseCheckSize = PRODUCTION_INFO_SIZE;
		}
		
		pBMFileInfo->llBase = pPhaseCheckFileInfo->arrBlock[0].llBase;
		pBMFileInfo->llOprSize = llPhaseCheckSize;

		return TRUE;
	}

	return FALSE;
}

BOOL CMainFrame::AddEraseAll(PBMFileInfo pBMFileInfo)
{
	_tcscpy(pBMFileInfo->szFileID,_T("ERASE_ALL"));
	_tcscpy(pBMFileInfo->szFileType, m_sheetSettings.HasPartitionInfo()?_T("EraseFlash2"):_T("EraseFlash"));
	_tcscpy(pBMFileInfo->szRepID, _T("erase_all"));
	pBMFileInfo->llBase = 0x0;
	pBMFileInfo->llOprSize = 0xFFFFFFFF;
	pBMFileInfo->bChangeCode = FALSE;
	pBMFileInfo->bLoadCodeFromFile = FALSE;
	
	CString strFileID = pBMFileInfo->szFileID;
	((CDLoaderView *)(GetActiveView()))->AddStepDescription(strFileID);
	
	return TRUE;
}
BOOL CMainFrame::AddBackupFiles(PBMFileInfo pBMFileInfo, int &nCount,_BMOBJ * pbj, PFILE_INFO_T pFileInfo)
{
	BOOL bReadPhaseCheck = FALSE;
	
	CString strErrMsg;
	CStringArray agBackupFileID;
	m_sheetSettings.GetBackupFiles(agBackupFileID);

    //将PhaseCheck调整到第一个位置，因为需要读取SN号用于Save files to local这个功能中文件的命名
    if(agBackupFileID.GetSize())
    {
		if (0 == agBackupFileID.GetAt(0).CompareNoCase(_T("PhaseCheck")))
		{
			bReadPhaseCheck = TRUE;
		}
		else
		{
			CString strFileIDFirst, strFileIDPhaseCheck;
			strFileIDFirst = agBackupFileID[0];
			int nIndexPhaseCheck = 0;
			for(int i = 0; i< agBackupFileID.GetSize(); i++)
			{
				CString strFileID = agBackupFileID[i];
				if(strFileID.CompareNoCase(_T("PhaseCheck")) == 0)
				{
					nIndexPhaseCheck = i;
					bReadPhaseCheck = TRUE;
					break;
				}
			}
			agBackupFileID.SetAt(0, _T("PhaseCheck"));
			agBackupFileID.SetAt(nIndexPhaseCheck, strFileIDFirst);
		}
    }
  
	// only research and upgrade can read SN.Using SN to name nv backup file
	if( 
		!bReadPhaseCheck && 
		( (E_SN == m_nDUTID) || (m_sheetSettings.IsNVSaveToLocal() && m_bNameWithSN) || (g_theApp.m_bNeedUpRefInfo && m_sheetSettings.HasPartitionInfo()) ) &&
		AddReadSN(pBMFileInfo + nCount)
		)
	{
		++nCount;
	}

	for(int i = 0; i< agBackupFileID.GetSize(); i++)
	{
		CString strFileID = agBackupFileID[i];
		PFILE_INFO_T pBackupFileInfo = NULL;
		if( m_sheetSettings.GetFileInfo(strFileID,(LPDWORD)&pBackupFileInfo) != -1
			&& pBackupFileInfo != NULL)
		{
			PBMFileInfo pBMFileInfo2 = pBMFileInfo + nCount;
			pBMFileInfo2->bLoadCodeFromFile = FALSE;
			_tcscpy(pBMFileInfo2->szFileType, m_sheetSettings.HasPartitionInfo()?_T("ReadFlash2"):_T("ReadFlash"));
			_stprintf(pBMFileInfo2->szFileID,_T("_BKF_%s"),strFileID);
			_tcscpy(pBMFileInfo2->szRepID,pBackupFileInfo->arrBlock[0].szRepID);
			pBMFileInfo2->llBase = pBackupFileInfo->arrBlock[0].llBase;
			pBMFileInfo2->llOprSize = pBackupFileInfo->arrBlock[0].llSize;
			if( pBMFileInfo2->llOprSize == 0 )
			{
				if(strFileID.CompareNoCase(_T("PhaseCheck")) == 0)
				{
					pBMFileInfo2->llOprSize = PRODUCTION_INFO_SIZE;
				}
				else
				{
					strErrMsg.Format(_T("[%s] Backup size is zero!"),pFileInfo->szID);
					_tcscpy(pbj->szErrorMsg,strErrMsg);
					return FALSE;
				}					
			}
			CString strDes = pBMFileInfo2->szFileID;
			((CDLoaderView *)(GetActiveView()))->AddStepDescription(strDes);				
			nCount++;

		}		
	}

	return TRUE;
}

void  CMainFrame::WaitForStopedObj()
{
	
	POSITION pos = m_mapBMObj.GetStartPosition();
    while( pos )
    {
		DWORD dwPort		= 0 ;
		BMOBJ_PTR  pStruct	= NULL;
        m_mapBMObj.GetNextAssoc( pos, dwPort, pStruct );	
		if (pStruct && pStruct->bStoping)
		{
			WaitForSingleObject(pStruct->hStopEvent,INFINITE);
			ResetEvent(pStruct->hStopEvent);
			pStruct->bStoping = FALSE;
		}
    }

}
BMOBJ_PTR CMainFrame::InitBMObj(DWORD dwPort, BOOL bUart)
{
	CString   strErrMsg;
	BMOBJ_PTR pStruct = NULL;
	if( m_mapBMObj.Lookup(dwPort, pStruct) && pStruct )
    {
		pStruct->Clear();
		pStruct->InitRefInfo();
		pStruct->dwRFChipType = (DWORD)-1;
		pStruct->hStopEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    }
	else
	{
		pStruct = new _BMOBJ;
		if( pStruct == NULL ) //lint !e774
        {
            strErrMsg.LoadString( IDS_NEWOPR_FAIL );
            AfxMessageBox( strErrMsg );
            return NULL;                        
        }
		pStruct->InitRefInfo();
		pStruct->dwRFChipType = (DWORD)-1;
		pStruct->dwCookie = dwPort;
		pStruct->dwIsUart = bUart;

		pStruct->bStoping = FALSE;
		pStruct->hStopEvent = CreateEvent(NULL,TRUE,FALSE,NULL);	
		m_mapBMObj.SetAt(dwPort,pStruct);
	}

#ifdef _FACTORY

	if(m_sheetSettings.IsSharkNand())
	{
		CreateDluidInfo(pStruct->dluid);
	}

#endif
	
	if(m_sheetSettings.IsReadFlashInFDL2() || m_sheetSettings.IsHasLang() )
	{
		CStringArray agNVID;		
		int nNVCount = m_sheetSettings.GetDLNVID(agNVID);
		for(int i = 0; i<nNVCount; i++)
		{
			CString strNVID = agNVID[i];
			BACKUP_INFO_PTR pNVInfo = NULL;
			if(m_mapNVFileInfo.Lookup(strNVID,pNVInfo) && pNVInfo!= NULL)
			{
				if(pNVInfo->dwSize == 0 || pNVInfo->pBuf == NULL)
				{	
					strErrMsg.Format(_T("%s file is empty"),strNVID);
					g_theApp.MessageBox(strErrMsg);
					return NULL;
				}
				pStruct->tNVBackup[i].dwSize = pNVInfo->dwSize;				
				pStruct->tNVBackup[i].pBuf = new BYTE[pNVInfo->dwSize];
				
				if(pStruct->tNVBackup[i].pBuf != NULL)
				{
					memcpy(pStruct->tNVBackup[i].pBuf, pNVInfo->pBuf,pNVInfo->dwSize);
				}
				else
				{
					AfxMessageBox( _T("Memory full!") );			
					return NULL;
				}
			}
		}
	}	

	return pStruct;
}

void CMainFrame::SetStatusBarText(LPCTSTR pStr)
{
	if(pStr != NULL)
	{
		m_wndStatusBar.SetPaneText(0,pStr,TRUE);
		m_strPacVerison = pStr;
	}
}
EXT_IMG_INFO_PTR CMainFrame::LoadPageBlockFile(LPCTSTR lpszFile)
{
	if( lpszFile == NULL )
		return NULL;

	CString strFile;
	GetAbsolutePath( strFile,lpszFile );
	if( strFile.IsEmpty() )
	{
		return NULL;
	}
	EXT_IMG_INFO_PTR pImg = new EXT_IMG_INFO;
	if(pImg)
	{
		_tcscpy(pImg->szFilePath,strFile);
	}
	return pImg; 
}
#if 0 //mem
EXT_IMG_INFO_PTR CMainFrame::LoadPageBlockFile(LPCTSTR lpszFile)
{
    if( lpszFile == NULL )
        return NULL;
    
    CString strFile;
    GetAbsolutePath( strFile,lpszFile );
    if( strFile.IsEmpty() )
    {
        return NULL;
    }
    CString strMapName = strFile ;
    strMapName.Replace('\\','.');
    strMapName = "Local\\" + strMapName;
    
    HANDLE hCode = INVALID_HANDLE_VALUE;
    HANDLE hCodeMapView = NULL;
    
    hCode = CreateFile(strFile.operator LPCTSTR (),
        GENERIC_READ,
        FILE_SHARE_READ,           //Exclusive Open
        NULL,                      //Can't Be inherited
        OPEN_EXISTING,             //If not existing then failed
        FILE_ATTRIBUTE_READONLY,   //Read Only
        NULL);
    if( hCode == INVALID_HANDLE_VALUE)
    {   
        return FALSE;
    }
    
    DWORD dwCodeSize = GetFileSize( hCode, NULL);
    if( dwCodeSize == INVALID_FILE_SIZE)
    {
        CloseHandle( hCode);
        return NULL;
    }

	DWORD dwFirstMapSize = MAX_MAP_SIZE;
	if(dwCodeSize < MAX_MAP_SIZE)
    {
		dwFirstMapSize = dwCodeSize;
	}

    hCodeMapView = CreateFileMapping( hCode,   //The Handle of Opened File
        NULL,          //Security
        PAGE_READONLY, //Read Only Access
        0,             //Max Size
        0,             //Min Size
        strMapName);   //Object Name
    
    if( hCodeMapView == NULL)
    {
        CloseHandle( hCode);
        return NULL;
    }
    
    void* lpCode = ::MapViewOfFile( hCodeMapView, FILE_MAP_READ,  0, 0, dwFirstMapSize);    
    if( lpCode == NULL)
    {
        CloseHandle( hCode );
        CloseHandle( hCodeMapView );
        return NULL;
    }      
    
	
	EXT_IMG_INFO_PTR pImg = new EXT_IMG_INFO;
	if(pImg == NULL)
	{
		UnmapViewOfFile(lpCode);
		CloseHandle( hCode );
        CloseHandle( hCodeMapView );
	}
	
	pImg->bIsFileMap = TRUE;
	pImg->hFile = hCode;
	pImg->hFileMap = hCodeMapView;
	pImg->pBuf = (LPBYTE)lpCode;
	pImg->dwSize = dwCodeSize;
	pImg->dwFirstMapSize = dwFirstMapSize;
	_tcscpy(pImg->szFilePath,strFile);
    
    return pImg; 
}
#endif

BOOL CMainFrame::CreateDluidInfo(DLUID_INFO &dluid)
{
	memset(&dluid,0,sizeof(dluid));
	dluid.dwSize = DLUID_SIZE;
	dluid.pBuf = new BYTE[DLUID_SIZE];
	if(dluid.pBuf == NULL)
		return FALSE;

	memset(dluid.pBuf,0xFF,DLUID_SIZE);
	BYTE uid[DLUID_LEN+2] = {0};

	CXRandom rd;
	rd.GetRandomBytes(uid,DLUID_LEN);

	WORD wCRC = 0;
	wCRC = crc16(wCRC,uid,DLUID_LEN);
	*(WORD*)(uid+DLUID_LEN) = wCRC;

	memcpy(dluid.pBuf,uid,DLUID_LEN+2);
	memcpy(dluid.pBuf+DLUID_SIZE/2,uid,DLUID_LEN+2);

	return TRUE;
}

BOOL CMainFrame::GetDluidInfo(DLUID_INFO &dluid)
{
	BYTE uid[DLUID_LEN+2] = {0};
	WORD wCRC = 0;
		
	wCRC = crc16(wCRC,dluid.pBuf,DLUID_LEN+2);
	if(memcmp(dluid.pBuf,uid,DLUID_LEN+2)==0 || wCRC != 0)
	{
		wCRC = 0;
		wCRC = crc16(wCRC,dluid.pBuf+DLUID_SIZE/2,DLUID_LEN+2);
		if(memcmp(dluid.pBuf+DLUID_SIZE/2,uid,DLUID_LEN+2)==0 || wCRC != 0)
			return FALSE;
		else
			memcpy(uid,dluid.pBuf+DLUID_SIZE/2,DLUID_LEN);
	}
	else
	{
		memcpy(uid,dluid.pBuf,DLUID_LEN);
	}

	for(int i = 0; i<DLUID_LEN; i++)
	{
		_stprintf(dluid.szUID+i*2, _T("%02X"),uid[i]);
	}

	return TRUE;
}

BOOL CMainFrame::CreateBKFileFolder()
{
	BOOL bOK = FALSE;
	do 
	{	
		_TCHAR szConfigPath[_MAX_PATH]={0};
		g_theApp.GetIniFilePath(szConfigPath);

		CString strFolder = szConfigPath;
		int nFind = strFolder.ReverseFind('\\');
		if(nFind == -1)
			break;

		strFolder = strFolder.Left(nFind);
		strFolder += IMG_BACKUP_FOLDER;
		if(CreateDeepDirectory(strFolder))
		{
			m_strLocalBKFolder = strFolder;
			bOK = TRUE; 
		}
	} while(0);

	if(!bOK)
	{
		g_theApp.MessageBox(_T("Create imgstore folder failed."));
	}

	return bOK;
}

void CMainFrame::RemoveLocalFile(LPCTSTR pszDluid)
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\%s*.bin"),m_strLocalBKFolder,pszDluid);

	CFileFind findfile;  
	BOOL bfind = findfile.FindFile(strFilePath);
	while (bfind)
	{  
	   bfind = findfile.FindNextFile();
	   if(!findfile.IsDots() && !findfile.IsDirectory()) 
	   {  		   
			DeleteFile(findfile.GetFilePath()); 		    
	   }  
	   else   
	   {  
		   continue;  
	   }  	    
	}  
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_F1)
		{
			PostMessage(WM_USER_HELP);
		}
	}
	
	return CFrameWnd::PreTranslateMessage(pMsg);
}
LRESULT CMainFrame::OnHelp(WPARAM wParam,LPARAM lpParam)
{
	UNUSED_ALWAYS(wParam);
	UNUSED_ALWAYS(lpParam);
	TCHAR szPath[MAX_PATH] = {0};
	::GetModuleFileName(NULL,szPath,MAX_PATH);
    LPTSTR pResult = _tcsrchr(szPath,_T('\\'));
	*pResult = '\0';

#ifdef _SPUPGRADE
	_tcscat(szPath,_T("\\..\\Doc\\UpgradeDownload User Guide (en).docx"));
#elif defined _FACTORY 
	_tcscat(szPath,_T("\\..\\Doc\\FactoryDownload User Guide (en).docx"));
#else
	_tcscat(szPath,_T("\\..\\Doc\\Research Download User Guide (en).docx"));
#endif
	CFileFind finder;
	if(finder.FindFile(szPath))
	{
		ShellExecute(this->GetSafeHwnd(),_T("open"),szPath,NULL,NULL,SW_SHOW);
	}

	return 0L;
}

//wei.song 20140901
void CMainFrame::Write2Sql(DWORD dwOprCookie, BOOL bSuccess /*= TRUE*/)
{
	USES_CONVERSION;

	_BMOBJ * pbj = NULL;

	if(m_mapBMObj.Lookup(dwOprCookie,pbj) && pbj != NULL )
	{
		if ( ((int)strlen(pbj->szSN)) == g_theApp.m_nSnLength)
		{
			char szSN[X_SN_LEN+1] = {0};
			TCHAR szErrorMsg[_MAX_PATH*2] = {0};
			TCHAR szStatusType[MAX_STATUS_LENGTH+1] = {0};
			TCHAR szInformation[MAX_INFORMATION_LENGTH+1] = {0};
			TCHAR szDateTime[MAX_TIME_LENGTH+1] = {0};
			TCHAR szToolVersion[MAX_TOOLVERSION_LENGTH+1] = {0};
			TCHAR szErrType[MAX_ERRTYPE_LENGTH+1] = {0};
			TCHAR szErrCode[MAX_ERRCODE_LENGTH+1] = {0};
			TCHAR szElapseTime[MAX_ELAPSE_LENGTH+1] = {0};
			TCHAR szSwVersion[MAX_SWVERSION_LENGTH+1] = {0};
			char szMsg[1028] = {0};
			
			STATE_INFO_T info;
			ZeroMemory(&info, sizeof(info));			
			
			strcpy(szSN,pbj->szSN);

			((CDLoaderView*)GetActiveView())->GetInfo(dwOprCookie, info);
			_tcscpy(szElapseTime, info.szUsedTime);

			if (!bSuccess)
			{
				_tcscpy(szErrType, info.szCurFile);
				_tcscpy(szErrorMsg, pbj->szErrorMsg);
			}

			_tcscpy(szStatusType, bSuccess? _T("Pass") : _T("Fail"));
			_stprintf(szInformation, _T("COM[%d]"), dwOprCookie);
			CTime mTime = CTime::GetCurrentTime();
			_tcscpy(szDateTime, mTime.Format("%Y-%m-%d"));
			_tcscpy(szToolVersion, g_theApp.m_strVersion);
			
			ZeroMemory(szMsg,sizeof(szMsg));
			if (!ConnectDB(szMsg))
			{
				_tcscat(pbj->szErrorMsg, A2W(szMsg));
				return;
			}

			_tcsncpy(szSwVersion,m_strPacVerison,MAX_SWVERSION_LENGTH);
			
			ZeroMemory(szMsg,sizeof(szMsg));
			if (!WriteDataToFlow((LPCTSTR)szSN, (LPCTSTR)W2A(szInformation), (LPCTSTR)W2A(szStatusType), 
				(LPCTSTR)W2A(szDateTime), (LPCTSTR)W2A(szElapseTime), (LPCTSTR)W2A(szErrType),
				(LPCTSTR)W2A(szErrCode), (LPCTSTR)W2A(szErrorMsg), (LPCTSTR)W2A(szSwVersion),
				(LPCTSTR)W2A(szToolVersion), (LPTSTR)szMsg))
			{
				_tcscat(pbj->szErrorMsg, A2W(szMsg));
				return;
			}
			
			DisconnectDB();
		}
	}

}

//wei.song 20151020
void CMainFrame::MESWriteResult(DWORD dwOprCookie, BOOL bSuccess)
{
	USES_CONVERSION;

	_BMOBJ * pbj = NULL;

	if(m_mapBMObj.Lookup(dwOprCookie,pbj) && pbj != NULL )
	{
		if (((int)strlen(pbj->szSN)) == g_theApp.m_BatchInfo.nSN1_Length)
		{
			TEST_RESULT_T TestResult;
			STATE_INFO_T info;
			ZeroMemory(&info, sizeof(info));
			((CDLoaderView*)GetActiveView())->GetInfo(dwOprCookie, info);

			strcpy(TestResult.szSN, pbj->szSN);
			strcpy(TestResult.szBatchName, g_theApp.m_szBatchName);
			TestResult.nResult = bSuccess;
			TestResult.nTestTime = atoi(W2A(info.szUsedTime));
			//sprintf(TestResult.szInformation, "COM[%d]", dwOprCookie);
			//TestResult.nTestResultID = pbj->nTestResultID;
			strcpy(TestResult.szTestResultGUID, pbj->szTestResultGUID);
			if (0 != strlen(pbj->szChipUID))
			{
				strcpy(TestResult.szInformation, pbj->szChipUID);
			}
			
#ifdef _SPUPGRADE
			sprintf(TestResult.szToolsVersion, "UpgradeDownload_");
			strcat(TestResult.szToolsVersion, W2A(g_theApp.m_strVersion));
#elif defined _FACTORY 
			sprintf(TestResult.szToolsVersion, "FactoryDownload_");
			strcat(TestResult.szToolsVersion, W2A(g_theApp.m_strVersion));
#else
			sprintf(TestResult.szToolsVersion, "ResearchDownload_");
			strcat(TestResult.szToolsVersion, W2A(g_theApp.m_strVersion));
#endif
			if (!bSuccess)
			{
				strcpy(TestResult.szErrType, W2A(info.szCurFile));
				strcpy(TestResult.szErrMsg, W2A(pbj->szErrorMsg));
			}

			//MES_InitTest(STATION_DOWNLOAD, TestResult.szSN, TestResult.szBatchName, &TestResult.nTestResultID);
			int nStatus = MES_WriteTestResult(&TestResult);
			if (nStatus != MES_SUCCESS)
			{
				char szError[1024]={0};
				MES_GetLastError(szError);
				_tcscat(pbj->szErrorMsg, A2W(szError));
				//return S_FALSE;
			}
		}
	}
}

BOOL CMainFrame::CheckCalibration(LPCTSTR lpszNVID,LPBYTE lpPhoBuf,DWORD dwPhoSize,CString &strErr)
{
	BOOL bRet = TRUE;
	do 
	{
		if(!m_bCheckCali)						break;
		if (m_sheetSettings.IsHostProType())	break;
		if (NULL == lpszNVID || NULL == lpPhoBuf)
		{
			strErr.Format(_T("NULL == lpszNVID || NULL == lpPhoBuf"));
			bRet = FALSE;
			break;
		}
		if( _tcsnicmp( lpszNVID, _T("NV"), 2 ))
		{
			strErr.Format(_T("[%] is not NV ID!"));
			bRet = FALSE;
			break;
		}

		PFILE_INFO_T pFileInfo = NULL;
		m_sheetSettings.GetFileInfo(lpszNVID,(LPDWORD)&pFileInfo);
		
		if( NULL == pFileInfo )
		{
			strErr.Format(_T("GetFileInfo [%s] fail,"),lpszNVID);
			bRet = FALSE;
			break;
		}
        MAP_CALIFLAG mapCaliInfo;
        GetNeedCheckCaliID(lpszNVID,pFileInfo->byCaliFlag,mapCaliInfo);

        //check cali flag
        POSITION pos = mapCaliInfo.GetStartPosition();
        while( pos )
        {
            int nCaliType;	
            WORDArray vecCaliID;
            mapCaliInfo.GetNextAssoc( pos, nCaliType, vecCaliID );	 
            int nCount = vecCaliID.size();
            for(int i=0; i<nCount && nCaliType<E_CALI_COUNT; ++i)
            {
                bRet = CalibrationCheck(vecCaliID[i],lpPhoBuf,dwPhoSize,strErr,nCaliType);
                if (!bRet)
                {
                    break;
                }
            }
            if (!bRet)
            {
                break;
            }
        }
        mapCaliInfo.RemoveAll();	
	
	} while(0);

	return bRet;
}
void  CMainFrame::GetNeedCheckCaliID(LPCTSTR lpszNVID,BYTE byteCali,MAP_CALIFLAG& mapCaliInfo)
{
    mapCaliInfo.RemoveAll();
    int nCount = m_sheetSettings.GetNvBkpItemCount(lpszNVID);
    PNV_BACKUP_ITEM_T pNvBkpItem = NULL;
    
    for (int j=0; j<E_CALI_COUNT;++j)
    {
        WORDArray vecCaliID;
        BOOL bCheck = (byteCali>>j) & 0x01;

        for(int k=0;bCheck && k<nCount;++k)
        {
            pNvBkpItem = m_sheetSettings.GetNvBkpItemInfo(k,lpszNVID);

            if( 
                pNvBkpItem && pNvBkpItem->wIsBackup &&
                _tcsnicmp(pNvBkpItem->szItemName,g_CaliFlagTable[j].szCaliKeyWord,_tcslen(g_CaliFlagTable[j].szCaliKeyWord))==0
               )
            {
                WORD wNVItemID = (WORD)(pNvBkpItem->dwID&0xFFFF);
                if(wNVItemID == 0xFFFF)
                {
                    wNVItemID = g_CaliFlagTable[j].wDefCheckID;              
                }
                vecCaliID.push_back(wNVItemID);
            }

        }
        mapCaliInfo.SetAt(j,vecCaliID);
    }
}

LRESULT CMainFrame::OnProgressMsg(WPARAM wParam, LPARAM lParam)
{
    CProgressCtrl & prgCtrl = m_wndStatusBar.m_prgrsCtrl;
    switch(wParam)
    {
    case PROG_BEGIN:
        prgCtrl.ShowWindow(SW_SHOW);
        prgCtrl.SetRange32(0, 100);
        prgCtrl.SetPos(0);
        break;
    case PROG_PROCEED:
        prgCtrl.SetPos((DWORD)lParam);
        break;
    case PROG_END:
        if(lParam == 100)
        {
            int nLower = 0;
            int nUpper = 0;
            prgCtrl.GetRange(nLower,nUpper);
            prgCtrl.SetPos(nUpper);
        }
        prgCtrl.ShowWindow(SW_HIDE);
        break;
    default:
        break;
    }
    return 0;
}

void CMainFrame::CheckPort2ndEnum()
{	
	m_bPortSecondEnum = FALSE;
	int nIdx = m_sheetSettings.GetFileInfo(_T("FDL"),NULL);

	if(nIdx == -1)
		return;

	CString strFile = m_arrFile[nIdx];
	CFileFind finder;
	if(!finder.FindFile(strFile))
	{
		return;
	}

	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = ::CreateFile(strFile,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	
	DWORD dwSize = GetFileSize(hFile,NULL);

	if(dwSize == 0 || dwSize == 0xFFFFFFFF)
	{
        CloseHandle(hFile);
		return;
	}

	BYTE *pBuf = new BYTE[dwSize];

	DWORD dwRealRead =  0;
	ReadFile(hFile,pBuf,dwSize,&dwRealRead,NULL);

	CloseHandle(hFile);

	const char szFlag[] = "##PORT-SECOND-ENUM##";

	BYTE* lpPos = NULL;
	lpPos = std::search( pBuf,pBuf + dwSize,szFlag,szFlag + strlen( szFlag ));
	if(lpPos < (pBuf + dwSize) )
	{
		m_bPortSecondEnum = TRUE;
	}		
    delete [] pBuf;
}
BOOL CMainFrame::GetDUTKeyInfo(LPBYTE pBuf, DWORD dwSize,DUT_KEY_T& stDutKey)
{
    BOOL bRet = FALSE;
    const char szStartFlag[] = "###DUT_KEY_BEGIN$$$";
    const char szEndFlag[]   = "###DUT_KEY_END$$$";
    ZeroMemory(&stDutKey,sizeof(DUT_KEY_T));
    if (NULL == pBuf || dwSize < sizeof(DUT_KEY_T))
    {
        return FALSE;
    }
    BYTE* lpPos = NULL;
    BYTE* lpBeginPos = NULL;
    BYTE* lpEndPos = NULL;
    BYTE* lpTmpPos = pBuf;

    do 
    {
        lpPos = std::search( lpTmpPos,lpTmpPos + dwSize,szStartFlag,szStartFlag + strlen( szStartFlag ) );
        if(lpPos < (lpTmpPos + dwSize) )
        {
            lpBeginPos = lpPos;
            lpPos = std::search( lpBeginPos,lpBeginPos + (dwSize-(lpBeginPos-pBuf)),szEndFlag,szEndFlag + strlen( szEndFlag )  );
            if(lpPos < (lpBeginPos + (dwSize-(lpBeginPos-pBuf))))
            {
                lpEndPos = lpPos;
                if (lpEndPos - lpBeginPos < sizeof(DUT_KEY_T)+ strlen(szStartFlag))
                {
                    break;
                }
                memcpy(&stDutKey,lpBeginPos+strlen( szStartFlag ),sizeof(DUT_KEY_T));
                bRet = TRUE;
                break;

            }
        }
    } while(lpPos < (lpTmpPos + dwSize));

    return bRet;
}
BOOL CMainFrame::CheckPacKey()
{
    BOOL bRet    = TRUE;
    LPBYTE pBuf  = NULL;  
    DWORD dwSize = 0;
    DUT_KEY_T stDutKey;
    m_bPacHasKey = FALSE;
    memset(m_szPacKey,0,sizeof(m_szPacKey));
    do 
    {
        CString strFile = m_sheetSettings.GetDownloadFilePath(_T("UBOOTLoader"),FALSE);
        CFileFind finder;
        if(!finder.FindFile(strFile))
        {
            break;
        }
        if (!LoadFileFromLocal(strFile,pBuf,dwSize))
        {
            break;
        }
        if (GetDUTKeyInfo(pBuf,dwSize,stDutKey))
        {
            CString strError;
            if ('1' == stDutKey.ver[0] && 0 == stDutKey.ver[1] && 0 == stDutKey.ver[2] && 0 == stDutKey.ver[3] && strlen(stDutKey.szDUTKey) )
            {
                m_bPacHasKey = TRUE;
                strncpy(m_szPacKey,stDutKey.szDUTKey,MAX_PATH);
            }
            else
            {
                bRet = FALSE;
                strError.Format(_T("This download tool just support 0.0.0.1 DUT key version,but this DUT key version is %c.%c.%c.%c,Please upgrade download tool."),
                    stDutKey.ver[3],stDutKey.ver[2],stDutKey.ver[1],stDutKey.ver[0]);
                g_theApp.MessageBox(strError);
            }
        }
    } while (0);
    
    SAFE_DELETE_ARRAY(pBuf);
    return bRet;
    
}

LRESULT CMainFrame::OnStopAutoDloader(WPARAM wParam,LPARAM lpParam)
{
    UNUSED_ALWAYS(lpParam);
    if(g_theApp.m_bScriptCtrl)
    {
        DWORD dwOprCookie = (DWORD)wParam;
        _BMOBJ * pbj = NULL;
        if( !m_mapBMObj.Lookup(dwOprCookie,pbj) ||  NULL == pbj )	
        {
            return E_FAIL;
        }
        _tcscpy(pbj->szErrorMsg,g_theApp.m_strEnumPortErrorMsg.operator LPCTSTR());
        DoReport((DWORD)wParam,FALSE);
        PostMessage(WM_COMMAND, ID_STOP, 0 );
        PostMessage(WM_CLOSE, 0, 0 );
    }
    return 0;
}


BOOL CMainFrame::CheckKeyMatch(const DUT_KEY_T& stDUTKey,LPTSTR szErrorMsg)
{
    USES_CONVERSION;
    BOOL bOK = FALSE;
    BOOL bHasDUTKey = strlen(stDUTKey.szDUTKey) ? TRUE : FALSE;
    if (NULL == szErrorMsg)
    {
        return FALSE;
    }

    if (m_bPacHasKey)
    {
        if (bHasDUTKey)
        {
            if (0 == stricmp(m_szPacKey,stDUTKey.szDUTKey))
            {
                bOK = TRUE;
            }
            else
            {
                _stprintf(szErrorMsg,_T("SW version is not matched,DUTKey is %s,But PACKey is %s"),
                    A2T(stDUTKey.szDUTKey),
                    A2T(m_szPacKey)
                    );
            }
        }
        else
        {
            if (0 == m_dwCheckMatchPolicy)
            {
                _stprintf(szErrorMsg,_T("PACKey exist,but DUTKey don't exist,Not allow to downloaded."));
            }
            else
            {
                bOK = TRUE;
            }
            
            
        }
    }
    else
    {
        if (bHasDUTKey)
        {
            _stprintf(szErrorMsg,_T("PACKey don't exist,Not allow to downloaded."));
        }
        else
        {
            bOK = TRUE;
        }
    }
    return bOK;
}

BOOL CMainFrame::ClearCalibrationFlag(LPCTSTR lpNVID,LPCTSTR lpNVFile)
{
    if (!g_theApp.m_bClearCaliFlag)
    {
		return TRUE;
    }
    CString strNVID(lpNVID);
    int nCount = m_sheetSettings.GetNvBkpItemCount(strNVID.operator LPCTSTR());	
    if(0 == nCount)
    {
        return TRUE;
    }
    CString strErr,strErrMsg;
    PNV_BACKUP_ITEM_T pNvBkpItem = NULL;
    LPBYTE lpNVBuf  = NULL;
    DWORD dwNVSize  = 0;
    BOOL bOK        = FALSE;
    if (!LoadFileFromLocal(lpNVFile,lpNVBuf,dwNVSize))
    {
        return FALSE;
    }
    for(int k=0;k<nCount;k++)
    {
        pNvBkpItem = m_sheetSettings.GetNvBkpItemInfo(k,strNVID.operator LPCTSTR());
        if(NULL == pNvBkpItem)
        {
            continue;	
        }

        if(_tcsnicmp(pNvBkpItem->szItemName,g_CaliFlagTable[E_GSM_CALI].szCaliKeyWord,_tcslen(g_CaliFlagTable[E_GSM_CALI].szCaliKeyWord))==0)      //_T("Calibration")        
        {
            WORD wNVItemID = (WORD)(pNvBkpItem->dwID&0xFFFF);
            if(wNVItemID == 0xFFFF)
            {
                wNVItemID = g_CaliFlagTable[E_GSM_CALI].wDefNvID;//GSM_CALI_ITEM_ID;
            }
            ClearGSMCaliFlag(wNVItemID,lpNVBuf,dwNVSize);
						
        }
        else if(_tcsnicmp(pNvBkpItem->szItemName,g_CaliFlagTable[E_WCDMA_CALI].szCaliKeyWord,_tcslen(g_CaliFlagTable[E_WCDMA_CALI].szCaliKeyWord))==0)       //_T("W_Calibration")     
        {					
            WORD wNVItemID = (WORD)(pNvBkpItem->dwID&0xFFFF);
            if(wNVItemID == 0xFFFF)
            {
                wNVItemID = g_CaliFlagTable[E_WCDMA_CALI].wDefNvID;//WCDMA_CALI_ITEM_ID;
            }
            ClearWCDMACaliFlag(wNVItemID,lpNVBuf,dwNVSize);
        }
        else if(_tcsnicmp(pNvBkpItem->szItemName,g_CaliFlagTable[E_LTE_CALI].szCaliKeyWord,_tcslen(g_CaliFlagTable[E_LTE_CALI].szCaliKeyWord))==0)       //_T("LTE_Calibration")     
        {					
            WORD wNVItemID = (WORD)(pNvBkpItem->dwID&0xFFFF);
            if(wNVItemID == 0xFFFF)
            {
                wNVItemID = g_CaliFlagTable[E_LTE_CALI].wDefNvID;//LTE_CALI_ITEM_ID;
            }//else is 0xA05
            ClearLTECaliFlag(wNVItemID,lpNVBuf,dwNVSize);
        }
    }

    bOK = SaveFileToLocal(lpNVFile,lpNVBuf,dwNVSize);
    SAFE_DELETE_ARRAY(lpNVBuf);
    return bOK;
}

CString CMainFrame::GetMD5(LPCTSTR lpszFile,__int64 llOffset/* = 0 */,__int64 llSize /*= 0*/)
{

	HANDLE hFile = CreateFile(lpszFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0, 0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return _T("");
	}

	MD5_CTX ctx = {0};
	BYTE hash[16] = {0};
	_TCHAR szBuf[33]= {0};

	MD5Init(&ctx);	

	LARGE_INTEGER liFileSize;
	GetFileSizeEx(hFile,&liFileSize);

	DWORD dwBufSize = 10*1204*1024; // 10M

	LPBYTE pBuf = new BYTE[dwBufSize];
	if (NULL == pBuf)
	{
		return _T("");
	}
	DWORD dwMaxLen = dwBufSize;

	__int64 llLeft = liFileSize.QuadPart;
	if (llSize)
	{
		llLeft = llSize;
	}
	if (llOffset)
	{
		LARGE_INTEGER li;
		li.QuadPart = llOffset;
		if (::SetFilePointer(hFile, li.LowPart, &li.HighPart, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			return  _T("");
		}
		llLeft -= llOffset;
	}
	DWORD dwReaded = 0;
	int i=0;
	while(llLeft > 0)
	{
		if(llLeft >= dwBufSize)
		{
			dwMaxLen = dwBufSize;
		}
		else
		{
			dwMaxLen = (DWORD)llLeft;
		}
		if(!ReadFile(hFile,pBuf,dwMaxLen,&dwReaded,NULL) || dwReaded != dwMaxLen )
		{
			goto FILE_ERROR;
		}
		MD5Update(&ctx,pBuf,dwMaxLen);
		llLeft -= dwMaxLen;
	}

	MD5Final(hash,&ctx);
	delete [] pBuf;
	CloseHandle(hFile);
	for(i=0; i<16;i++)
	{
		_stprintf(szBuf+2*i,_T("%02X"),hash[i]);
	}
	return szBuf;
FILE_ERROR:
	CloseHandle(hFile);
	delete[] pBuf;
	MD5Final(hash,&ctx);
	return _T("");

}

BOOL CMainFrame::CompareFile(LPCTSTR lpFile1,LPCTSTR lpFile2,__int64 llOffset/* = 0*/)
{
	BOOL bRet = FALSE;
	__int64 llFile1Size = GetFileSizeEx(lpFile1);
	__int64 llFile2Size = GetFileSizeEx(lpFile2);
	__int64 llSize = min(llFile1Size,llFile2Size);
	CString strFile1Md5 = GetMD5(lpFile1,llOffset,llSize);
	CString strFile2Md5 = GetMD5(lpFile2,llOffset,llSize);
	if (!strFile1Md5.IsEmpty() && !strFile2Md5.IsEmpty() && 0 == strFile1Md5.CompareNoCase(strFile2Md5))
	{
		bRet = TRUE;
	}
	return bRet;
}

BOOL CMainFrame::AddCompareFileInfo(const PBMFileInfo pBMFileInfoCur)
{
#ifndef _RESEARCH
	UNUSED_ALWAYS( pBMFileInfoCur );
	return TRUE;
#endif
	DWORD dwCompareType = m_sheetSettings.GetComparePolicy();
	if (dwCompareType == E_NOT_COMPARE || NULL == pBMFileInfoCur || !m_sheetSettings.HasPartitionInfo())
	{
		return TRUE;
	}

	if( 0 ==_tcsnicmp(pBMFileInfoCur->szFileType,_T("ReadFlashAndDirectSave"),_tcslen(_T("ReadFlashAndDirectSave"))) )
	{
		if(m_sheetSettings.GetUnPacPolicy())
		{
			MessageBox(_T("Auto compare function does not enable fast load pac function.\r\nPlease modify LoadPolicy=0 in BinPack.ini and reload pac."),_T("Error"));
			return FALSE;
		}
		COMPARE_FILE_INFO tCmpInfo;
		_stprintf(tCmpInfo.szFile1,_T("%s"),m_sheetSettings.GetDownloadFilePathByPartitionName(pBMFileInfoCur->szRepID));
		_tcscpy(tCmpInfo.szFile2,pBMFileInfoCur->szFileName);
		m_mapCmpInfo.SetAt(pBMFileInfoCur->szFileID,tCmpInfo);

		
	}
	return TRUE;

}

BOOL CMainFrame::VerifyIntegrity(LPCTSTR lpFileID,LPCTSTR lpPartName)
{	
	COMPARE_FILE_INFO tCmpInfo;
	if(!m_mapCmpInfo.Lookup(lpFileID,tCmpInfo))
	{
		return FALSE;
	}
	__int64 llOffset = 0;
	if (0 == _tcsicmp(lpPartName,_T("splloader")))
	{
		llOffset = 512;
	}
	else if (_tcsstr(lpPartName,_T("fixnv1")))
	{
		llOffset = 4;
	}
	return CompareFile(tCmpInfo.szFile1,tCmpInfo.szFile2,llOffset);
}

void CMainFrame::AutoCompare(DWORD /*dwOprCookie*/,_BMOBJ* pbj)
{
#ifndef _RESEARCH
	UNUSED_ALWAYS( pbj );
	return;
#endif

	DWORD dwCompareType = m_sheetSettings.GetComparePolicy();
	if (dwCompareType == E_NOT_COMPARE || NULL == pbj || !m_sheetSettings.HasPartitionInfo() || 0 == m_mapCmpInfo.GetSize())
	{
		return ;
	}
	CString strError;
	strError.Empty();
	switch(dwCompareType)
	{
	case E_BUILT_IN_COMPARE:
		BuiltinComparison(strError);
		break;
	case E_3RD_PROGRAM_COMPARE:
		ThirdPartyComparison(strError);
		break;
	case E_BUILT_IN_PLUS_3RD_PROGRAM_COMPARE:
		BuiltinComparison(strError);
		ThirdPartyComparison(strError);
		break;
	default:
		break;

	}
	if (!strError.IsEmpty())
	{
		_tcscpy(pbj->szErrorMsg,strError.operator LPCTSTR());
	}

}

void CMainFrame::BuiltinComparison(CString& strError)
{
	POSITION pos = m_mapCmpInfo.GetStartPosition();
	while( pos )
	{
		CString strFileID;
		COMPARE_FILE_INFO tCmpInfo;
		m_mapCmpInfo.GetNextAssoc( pos, strFileID, tCmpInfo );	
		CString strPartName(strFileID);
		strPartName.Replace(_T("R_"),_T(""));
		if (!VerifyIntegrity(strFileID,strPartName))
		{
			if (!strError.IsEmpty())
			{
				strError += _T(",");
			}
			strError += strPartName;
		}

	}
	if (!strError.IsEmpty())
	{
		strError += _T(" partition data is abnormal.");
	}
}

void CMainFrame::ThirdPartyComparison(CString& strError)
{
	CString strLaunchPara;
	strLaunchPara.Format(m_sheetSettings.GetCompareParameter().operator LPCTSTR(), m_aPacReleaseDir.GetAt(m_aPacReleaseDir.GetSize()-1),GetReadFlashPath());

	if(!ShellExecute(NULL, _T("open"), m_sheetSettings.GetCompareApp().operator LPCTSTR(), strLaunchPara, NULL, SW_SHOWNORMAL))
	{
		strError = _T("Unable to execute third party comparison program.");
	}
}

CString CMainFrame::GetReadFlashPath()
{
	CUIntArray agFlashOprFiles;
	CString strReadFlashPath;
	int nFlashOprCount = m_sheetSettings.GetFlashOprFileInfo(agFlashOprFiles);
	for(int i= 0; i< nFlashOprCount && m_sheetSettings.HasPartitionInfo(); i++)
	{
		PFILE_INFO_T pFileInfo = (PFILE_INFO_T)agFlashOprFiles[i];
		if(0 == _tcsnicmp(pFileInfo->szID,_T("R_"),2))
		{
			strReadFlashPath = GetStrFilePath(pFileInfo->szFilePath);
			if (!strReadFlashPath.IsEmpty())
			{
				break;
			}

		}
	}
	return strReadFlashPath;

}

BOOL CMainFrame::NeedCompare()
{
	BOOL bOK = FALSE;
	DWORD dwCompareType = m_sheetSettings.GetComparePolicy();
	CString strReadFlashPath = GetReadFlashPath();
	if(dwCompareType && !strReadFlashPath.IsEmpty())
	{
		bOK = TRUE;
	}
	return bOK;
}
