// DLoader.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DLoader.h"
#include "UsageStatistics.h"

#include "MainFrm.h"
#include "DLoaderDoc.h"
#include "DLoaderView.h"
#include "XAboutDlg.h"
#include "LiveUpdatesApi.h"

//wei.song 20140901
#include <atlconv.h>
#include "ProcessFlowLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CDLCmdLine : public CCommandLineInfo
{
public:
	CDLCmdLine() 
	{ 
		m_strPrdVersion = _T("");
		m_nParaCount = 0; 
		m_nMaxParaCount = 3; 
		m_bWait = FALSE;
	}
	virtual void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );

	CString m_strPrdVersion;

	BOOL    m_bWait;

protected:
	int m_nParaCount;
	int m_nMaxParaCount;
};



void CDLCmdLine::ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast )
{
	UNREFERENCED_PARAMETER(bLast);
//	if( bFlag )
//	{
		// Do not support any flag now
//		return;
//	}

//	if( m_nParaCount > m_nMaxParaCount )
//	{
//		// Only support three parameters
//		return;
//	}

	CString strParam = lpszParam;
	strParam.TrimLeft();
	strParam.TrimRight();

	if( 0 == m_nParaCount )
	{
		m_strFileName = strParam;
	}
	else if( 1 == m_nParaCount )
	{
		m_strPrdVersion = strParam;
	}
	else
	{
		if(bFlag && strParam.CompareNoCase(_T("w")) == 0)
		{
			m_bWait = TRUE;
		}	
	}
	m_nParaCount++;
}



/////////////////////////////////////////////////////////////////////////////
// CDLoaderApp

BEGIN_MESSAGE_MAP(CDLoaderApp, CWinApp)
	//{{AFX_MSG_MAP(CDLoaderApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLoaderApp construction

CDLoaderApp::CDLoaderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_pBMAFramework = NULL;
	m_bScriptCtrl = FALSE;
	m_bResultPathWithPort = FALSE;

#if defined(_SPUPGRADE) || defined(_FACTORY)
	m_bShowOtherPage = FALSE;
	m_bShowMcpTypePage = FALSE;
#else
	m_bShowOtherPage = TRUE;
	m_bShowMcpTypePage = TRUE;
#endif

	m_bDeleteTmpDir = TRUE;
	
	m_bColorFlag = FALSE;
	m_bClosePortFlag = TRUE;
	m_bResultHolding = TRUE;
	m_bManual = FALSE;

	m_bCMDPackage       = FALSE;

    m_bCMDFlash         = FALSE;
	m_bEZMode			= FALSE;
	m_bReadFixNV		= FALSE;
	m_dwSumCMDFlash		= 0;
	m_dwMultiCount		= 1;
	m_strDoPacPath      = _T("");
	m_strPrdVersion     = _T("");
	m_strInPacFile      = _T("");
	m_bKeepPacNVState   = FALSE;
	m_bNeedPassword     = FALSE;
	m_bClearCaliFlag    = FALSE;

	m_bFilterPort       = TRUE;

	m_strFileFilter     = _T("");

	m_nGSMCaliVaPolicy  = FALSE;
    m_bStopDownloadIfOldMemory = FALSE;
	m_bNeedUpRefInfo	= FALSE;
	m_bCheckDriverVer	= TRUE;

	m_pfCreateXmlConfigParse    = NULL;
	m_pfCreateBMAFramework      = NULL;
	m_hBMAFrameLib              = NULL;
    m_nEnumPortTimeOut          = 60000;
    m_dwPort                    = 0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDLoaderApp object

CDLoaderApp g_theApp;

/////////////////////////////////////////////////////////////////////////////
// CDLoaderApp initialization

BOOL CDLoaderApp::InitInstance()
{
	AfxEnableControlContainer();

	OleInitialize( NULL );

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	/*
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	*/

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Spreadtrum"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    //@ Liu Kai 2004-8-23
    GetVersion();
	
	//wei.song 20140901
	if (!ProcessFlowInit())
	{
		MessageBox(_T("Init ProcessFlow fail."));
		return FALSE;
	}
	
	//wei.song 20151020
	if (!SprdMESInit())
	{
		return FALSE;
	}

	_TCHAR szConfigPath[_MAX_PATH]={0};
	if(!GetIniFilePath(szConfigPath))
	{
		if(_tcslen(szConfigPath) > 0)
		{			
			CString strError;
			strError.Format(_T("Can not find configure file \"%s\"!"),szConfigPath);			
			MessageBox(strError);
		}
		else
		{
			MessageBox(_T("Load configure ini file failed!"));
		}		
		return FALSE;
	}  

#if defined(_SPUPGRADE) || defined(_FACTORY)	
	m_bShowOtherPage = GetPrivateProfileInt(_T("GUI"),_T("ShowOtherPage"),0,szConfigPath);
	m_bManual = GetPrivateProfileInt(_T("GUI"),_T("Manual"),0,szConfigPath);
	m_bResultHolding = GetPrivateProfileInt(_T("GUI"),_T("ResultHolding"),1,szConfigPath);
	m_bShowMcpTypePage = GetPrivateProfileInt(_T("GUI"),_T("ShowMcpTypePage"),0,szConfigPath);
#else
	m_bShowOtherPage = GetPrivateProfileInt(_T("GUI"),_T("ShowOtherPage"),1,szConfigPath);
	m_bShowMcpTypePage = GetPrivateProfileInt(_T("GUI"),_T("ShowMcpTypePage"),1,szConfigPath);
	m_bDeleteTmpDir = GetPrivateProfileInt(_T("GUI"),_T("DeleteTmpDir"),1,szConfigPath);
#endif	

	m_bColorFlag = GetPrivateProfileInt(_T("GUI"),_T("MarkLastState"),0,szConfigPath);
	m_bClosePortFlag = GetPrivateProfileInt(_T("GUI"),_T("ClosePortFlag"),1,szConfigPath);

	m_bScriptCtrl = GetPrivateProfileInt(_T("GUI"),_T("ScriptControl"),0,szConfigPath);
	m_bCMDPackage = GetPrivateProfileInt(_T("GUI"),_T("CmdPackage"),0,szConfigPath);
#if defined(_SPUPGRADE) 
	m_bKeepPacNVState = GetPrivateProfileInt(_T("GUI"),_T("KeepPacNVState"),0,szConfigPath);
#endif
	m_bNeedPassword = GetPrivateProfileInt(_T("GUI"),_T("NeedPassword"),0,szConfigPath);

#ifdef _FACTORY
	m_bClearCaliFlag = GetPrivateProfileInt(_T("Settings"),_T("ClearCaliFlag"),TRUE,szConfigPath);
#endif
	m_dwWaitTimeForNextChip = GetPrivateProfileInt(_T("Settings"),_T("WaitTimeForNextChip"),0,szConfigPath);
	m_bCheckDriverVer = GetPrivateProfileInt(_T("Settings"),_T("CheckDriverVer"),1,szConfigPath);

	m_bResultPathWithPort = GetPrivateProfileInt(_T("GUI"),_T("ResultPathWithPort"),0,szConfigPath);
	m_strResultPath = szConfigPath;
	int nFind = m_strResultPath.ReverseFind('\\');
	m_strResultPath = m_strResultPath.Left(nFind);
	CString strBMFileTypeConfig = m_strResultPath;
    strBMFileTypeConfig+= _T("\\BMFileType.ini");

    CString strBMTimeOutConfig = m_strResultPath;
    strBMTimeOutConfig+= _T("\\BMTimeout.ini");

	m_strResultPath += _T("\\result.txt");

	_TCHAR szBuf[_MAX_PATH] = {0};
    m_nEnumPortTimeOut  = GetPrivateProfileInt(_T("SecondEnumPort"),_T("EnumPortTimeOut"),60000,strBMTimeOutConfig.operator LPCTSTR());
    GetPrivateProfileString(_T("SecondEnumPort"),_T("EnumPortErrorMsg"),_T("The second enumemration port timeout"),szBuf,_MAX_PATH,strBMTimeOutConfig.operator LPCTSTR());
	m_strEnumPortErrorMsg = szBuf;

    GetPrivateProfileString(_T("GUI"),_T("FileFilter"),_T(""),szBuf,_MAX_PATH,szConfigPath);
	m_strFileFilter = szBuf;	

	LoadFilterPortSetting(szConfigPath);

	m_nGSMCaliVaPolicy = GetPrivateProfileInt(_T("DownloadNV"),_T("GSMCaliVaPolicy"),0,strBMFileTypeConfig);
	if(m_nGSMCaliVaPolicy <0 || m_nGSMCaliVaPolicy>3)
	{
		MessageBox(_T("GSMCaliVaPolicy configure error in BMFileType.ini!\n It must be 0,1,2,3."));
		return FALSE;
	}
    
    m_bStopDownloadIfOldMemory = GetPrivateProfileInt( _T("Misc") , _T("StopDownloadIfOldMemory"), 0, strBMFileTypeConfig);

    if ( __argc > 1 && !m_bCMDPackage)
    {
        if (!ParseCMDParam(__argc,__targv))
        {
            return FALSE;
        }
        m_bCMDFlash     = TRUE;
        m_bScriptCtrl   = TRUE;
        m_cPipeSrv.Init(m_dwPort);
        m_mapFixPort.RemoveAll();
        m_mapFixPort.SetAt(m_dwPort,m_dwPort);
		if(DO_PACKET_PORT ==  m_dwPort)
		{
			m_bCMDPackage = TRUE;
			m_strDoPacPath.Format(_T("%s\\%s_%s.pac"),GetStrFilePath(m_strInPacFile.operator LPCTSTR()),GetStrFileTitle(m_strInPacFile.operator LPCTSTR()),g_szDoPackFlag);
		}
    }
	if (!m_bCMDFlash && !m_bScriptCtrl && !m_bCMDPackage)
	{
		CheckToolVerUpdate(TRUE);
	}
	
    if(!InitBMAFrameLib())
    {
        MessageBox(_T("InitBMAFrameLib fail."));
        return FALSE;
    }

    if(!InitChannelLib())
    {
        MessageBox(_T("InitChannelLib fail."));
        return FALSE;
    }	

    if(!m_pfCreateBMAFramework( &m_pBMAFramework))
    {
        MessageBox(_T("Create BMAFramework fail."));
        return FALSE;
    } 

#ifdef _RESEARCH
	CUsageStatistics cUS;
	m_bNeedUpRefInfo = cUS.PingSrv(); 
#endif
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDLoaderDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CDLoaderView));
	AddDocTemplate(pDocTemplate);


	UNUSED_ALWAYS(IDR_DLOADETYPE);

	// Parse command line for standard shell commands, DDE, file open
	CDLCmdLine cmdInfo;
	ParseCommandLine(cmdInfo);
    
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.	
	if(m_bCMDPackage || m_bCMDFlash)
	{
		m_pMainWnd->ShowWindow(SW_HIDE);
	}
	else
	{
		m_pMainWnd->ShowWindow(SW_SHOW);
	}
	m_pMainWnd->UpdateWindow();


	if(!m_bCMDFlash && m_bCMDPackage)
	{
		m_strDoPacPath = cmdInfo.m_strFileName;
		m_strPrdVersion = cmdInfo.m_strPrdVersion;
		if(!m_strDoPacPath.IsEmpty() && !m_strPrdVersion.IsEmpty())
		{
			PostMessage(m_pMainWnd->m_hWnd,WM_COMMAND,ID_SETTINGS,0);
		}
		else
		{
			AfxMessageBox(_T("Usage: ResearchDownload.exe <pac path> <product version>"));
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDLoaderApp::ParseCMDParam(int argc, TCHAR** argv)
{
    BOOL bRet = FALSE;
	m_bEZMode = FALSE;
	m_bReadFixNV = FALSE;
	m_dwMultiCount = 1;
	m_dwSumCMDFlash= 0;
    if (!m_bCMDPackage && argc>1)
    {
        for(int i=1; i<argc; ++i)
        {
            if( 0 == _tcsicmp(argv[i],_T("-pac")) )   
            {
                if((i+1)<argc)
                {
                    m_strInPacFile = argv[++i];
                }
            }
            else if( 0 == _tcsicmp(argv[i],_T("-port")) )   
            {
                if((i+1)<argc)
                {
                    m_dwPort = _ttoi(argv[++i]);
                }
            }
			else if( 0 == _tcsicmp(argv[i],_T("-count")) )   
			{
				if((i+1)<argc)
				{
					m_dwMultiCount = _ttoi(argv[++i]);
				}
			}
			else if (0 == _tcsicmp(argv[i],_T("-EZMode")))
			{
				m_bEZMode = TRUE;
			}
			else if (0 == _tcsicmp(argv[i],_T("-ReadNV")))
			{
				if((i+1)<argc)
				{
					m_bReadFixNV = TRUE;
					m_strRFixNV = argv[++i];
				}
			}

        }
        bRet = TRUE;
        if ( m_strInPacFile.IsEmpty()|| 0 == m_dwPort || 0 == m_dwMultiCount)
        {
            _tprintf(_T("Invalid arguments\n"));

            //ShowUsage();
            bRet = FALSE;
            
        }
    }

    return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX }; //lint !e30
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDLoaderApp::OnAppAbout()
{
	CXAboutDlg aboutDlg;

	TCHAR szPath[MAX_PATH] = {0};
	::GetModuleFileName(m_hInstance,szPath,MAX_PATH);
    LPTSTR pResult = _tcsrchr(szPath,_T('\\'));
	*pResult = '\0';

#ifdef _SPUPGRADE
	aboutDlg.SetProductName(_T("UpgradeDownload"));
	_tcscat(szPath,_T("\\..\\Doc\\UpgradeDownload User Guide (en).docx"));
#elif defined _FACTORY 
	aboutDlg.SetProductName( _T("FactoryDownload"));
	_tcscat(szPath,_T("\\..\\Doc\\FactoryDownload User Guide (en).docx"));
#else
	aboutDlg.SetProductName( _T("ResearchDownload"));
	_tcscat(szPath,_T("\\..\\Doc\\Research Download User Guide (en).docx"));
#endif
	CFileFind finder;
	if(finder.FindFile(szPath))
	{
		aboutDlg.SetHelpFile(szPath);
	}
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CDLoaderApp message handlers


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    //@ Liu Kai 2004-08-23
    GetDlgItem( IDC_STATIC_VERSION )->SetWindowText( g_theApp.m_strVersion );
    GetDlgItem( IDC_STATIC_BUILD )->SetWindowText( g_theApp.m_strBuild );
#ifdef _SPUPGRADE
	GetDlgItem( IDC_STATIC_PRODUCT_NAME )->SetWindowText(_T("Spreadtrum UpgradeDownload"));
	SetWindowText(_T("About Spreadtrum UpgradeDownload")); 
#elif defined _FACTORY
    GetDlgItem( IDC_STATIC_PRODUCT_NAME )->SetWindowText(_T("Spreadtrum FactoryDownload"));
	SetWindowText(_T("About Spreadtrum FactoryDownload")); 
#else
	GetDlgItem( IDC_STATIC_PRODUCT_NAME )->SetWindowText(_T("Spreadtrum ResearchDownload"));
	SetWindowText(_T("About Spreadtrum ResearchDownload")); 
#endif
	
	return TRUE;
}

int CDLoaderApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_pBMAFramework != NULL)
	{
		m_pBMAFramework->BMAF_Release();   
	}

	if (g_theApp.m_bEnableFlow)
	{
		ExitProcessFlow(); //wei.song 20140901
	}

	if (g_theApp.m_bSprdMESEnable)
	{
		MES_Disconnect();
		MES_Release();
	}
	
	FreeLib();
	OleUninitialize();
	return CWinApp::ExitInstance();
}

void CDLoaderApp::LoadFilterPortSetting(LPCTSTR pszConfigFile)
{
	_TCHAR szBuf[_MAX_PATH] = {0};
	m_bFilterPort = GetPrivateProfileInt(_T("PortSetting"),_T("EnableFilter"),TRUE,pszConfigFile);	

	//Get FilterPort 
	GetPrivateProfileString(_T("PortSetting"),_T("FilterPort"),_T("1,2"),szBuf,_MAX_PATH,pszConfigFile);
	FetchPort(szBuf,m_mapFilterPort);

	//Get FixPort 
	GetPrivateProfileString(_T("PortSetting"),_T("FixPort"),_T(""),szBuf,_MAX_PATH,pszConfigFile);
	FetchPort(szBuf,m_mapFixPort);

}

void CDLoaderApp::FetchPort(LPTSTR lpszStr,CMap<DWORD,DWORD,DWORD,DWORD>& mapPort,TCHAR chSeparate/* = _T(',')*/)
{
	DWORD dwPort = 0;
	mapPort.RemoveAll();
	if(_tcslen(lpszStr) != 0)
	{	
		LPTSTR  lpBuf  = lpszStr;
		LPTSTR  lpFind = _tcschr(lpBuf, chSeparate);
		while(lpFind != NULL)
		{
			*lpFind = _T('\0');

			_stscanf(lpBuf,_T("%d"),&dwPort);
			mapPort.SetAt(dwPort,dwPort);
			lpBuf = lpFind + 1;
			lpFind = _tcschr(lpBuf, chSeparate);
		}		
		_stscanf(lpBuf,_T("%d"),&dwPort);
		mapPort.SetAt(dwPort,dwPort);	
	}

}
BOOL CDLoaderApp::GetIniFilePath(LPTSTR pName)
{
    if(pName == NULL)
        return FALSE;
	
    DWORD dwRet = ::GetModuleFileName(AfxGetApp()->m_hInstance,pName,_MAX_PATH);
    if(dwRet == 0)
    {
        pName[0] = 0;
        return FALSE;
    }
	
    LPTSTR pResult = _tcsrchr(pName,_T('\\'));
    if(pResult == NULL)
    {
        pName[0] = 0;
        return FALSE;
    }
	
    *pResult = 0;
	
#if defined(_FACTORY)
    _tcscat(pName,_T("\\FactoryDownload.ini"));
#elif defined(_SPUPGRADE)
	_tcscat(pName,_T("\\UpgradeDownload.ini"));
#else
	_tcscat(pName,_T("\\ResearchDownload.ini"));
#endif

	CFileFind finder;
	if(!finder.FindFile(pName))
	{		
		return FALSE;
	}
	
    return TRUE;
}

//wei.song 20140901
void CDLoaderApp::GetVersion()
{
    CXVerInfo Ver;
    Ver.Init();
    m_strVersion=Ver.GetProductVersion();
    m_strVersion.Replace( _T(","),_T(".") );
    m_strVersion.Replace(_T(" "),_T("") ); 
}

BOOL CDLoaderApp::ProcessFlowInit()
{
	USES_CONVERSION;

	m_bEnableFlow = CheckProcessEnable();
	if (m_bEnableFlow)
	{
		BOOL bStatus = FALSE;
		char szMsg[1024] = {0};
		
		bStatus = InitProcessFlow(NULL, (LPTSTR)m_szOrderNum, &m_nSnLength, (LPTSTR)szMsg);
		if (!bStatus)
		{
			CString strTemp;
			strTemp.Format(_T("%s%s"), A2W(szMsg), _T("\nProgram will exit soon!"));
			AfxMessageBox(strTemp);
			return FALSE;
		}
// 		bStatus = ConnectDB(szMsg);
// 		if (!bStatus)
// 		{
// 			CString strTemp;
// 			strTemp.Format(_T("%s%s"), A2W(szMsg), _T("\nProgram will exit soon!"));
// 			AfxMessageBox(strTemp);
// 			return FALSE;
// 		}
// 		DisconnectDB();
	}
	
	return TRUE;
}

BOOL CDLoaderApp::SprdMESInit()
{
	USES_CONVERSION;
	int nStatus = MES_Create();
	if (nStatus != MES_SUCCESS)
	{
		return FALSE;
	}
	m_bSprdMESEnable = MES_EnableCheck();
	MES_GetBatchName(m_szBatchName);
	MES_GetBatchInfo(m_szBatchName, &m_BatchInfo);
	return TRUE;
}

BOOL CDLoaderApp::InitBMAFrameLib()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFilePath(NULL,szPath);

#if _MSC_VER < 1300  //VC 6.0
#ifdef _DEBUG
	_tcscat(szPath,_T("\\BMAFrameD.dll"));
#else
	_tcscat(szPath,_T("\\BMAFrame.dll"));
#endif
#else
#ifdef _DEBUG
	_tcscat(szPath,_T("\\BMAFrame9D.dll"));
#else
	_tcscat(szPath,_T("\\BMAFrame9.dll"));
#endif
#endif

	m_hBMAFrameLib = LoadLibrary(szPath);

	if(m_hBMAFrameLib == NULL)
	{
		CString strErr;
		strErr.Format(_T("Cannot load BMAFrame DLL (%s),ErrorCode=0x%X."),szPath,GetLastError());
		AfxMessageBox(strErr);
		return FALSE;
	}

	m_pfCreateXmlConfigParse = (pfCreateXmlConfigParse)GetProcAddress(m_hBMAFrameLib,"CreateXmlConfigParse");
	m_pfCreateBMAFramework = (pfCreateBMAFramework)GetProcAddress(m_hBMAFrameLib,"CreateBMAFramework");

	if(m_pfCreateXmlConfigParse == NULL || m_pfCreateBMAFramework == NULL )
	{
		CString strErr;
		strErr.Format(_T("Cannot load BMAFrame function (%s),ErrorCode=0x%X."),szPath,GetLastError());
		AfxMessageBox(strErr);
		return FALSE;
	}

	return TRUE;
}

BOOL CDLoaderApp::InitChannelLib()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFilePath(NULL,szPath);

#if _MSC_VER < 1300  //VC 6.0
#ifdef _DEBUG
	_tcscat(szPath,_T("\\ChannelD.dll"));
#else
	_tcscat(szPath,_T("\\Channel.dll"));
#endif
#else
#ifdef _DEBUG
	_tcscat(szPath,_T("\\Channel9.dll"));
#else
	_tcscat(szPath,_T("\\Channel9.dll"));
#endif
#endif

	m_hChannelLib = LoadLibrary(szPath);

	if(m_hChannelLib == NULL)
	{
		CString strErr;
		strErr.Format(_T("Cannot load Channel DLL (%s),ErrorCode=0x%X."),szPath,GetLastError());
		AfxMessageBox(strErr);
		return FALSE;
	}

	m_pfCreateChannel = (pfCreateChannel)GetProcAddress(m_hChannelLib,"CreateChannel");
	m_pfReleaseChannel = (pfReleaseChannel)GetProcAddress(m_hChannelLib,"ReleaseChannel");

	if(m_pfCreateChannel == NULL || m_pfReleaseChannel == NULL )
	{
		CString strErr;
		strErr.Format(_T("Cannot load Channel function (%s),ErrorCode=0x%X."),szPath,GetLastError());
		AfxMessageBox(strErr);
		return FALSE;
	}

	return TRUE;
}

void CDLoaderApp::FreeLib()
{
	if(m_hBMAFrameLib)
	{
		FreeLibrary(m_hBMAFrameLib);
		m_hBMAFrameLib = NULL;
	}

	if(m_hChannelLib)
	{
		FreeLibrary(m_hChannelLib);
		m_hChannelLib = NULL;
	}
}

void CDLoaderApp::ReportToPipe(CString sLogContent,BOOL isError)
{ 
    if (g_theApp.m_bCMDFlash)
    {
 		if (m_bEZMode && -1 == sLogContent.Find(EZ_MODE_STATUS_FLAG) && -1 == sLogContent.Find(EZ_MODE_REPORT_FLAG))
 		{
			return;
 		}
        CString strInfo(sLogContent);
        if (!m_bEZMode && isError)
        {
	        strInfo.Format(_T("[ERROR] %s"),sLogContent);
        }
        m_cPipeSrv.UpdateDataToPipe(strInfo);
    }
}

BOOL CDLoaderApp::IsSkipFile(LPCTSTR lpFileID) 
{ 
	BOOL bSkiped = FALSE;
	if (g_theApp.m_bCMDFlash && g_theApp.IsPartialFlash())
	{
		bSkiped = m_cPipeSrv.IsSkipFile(lpFileID);
	}
	return bSkiped;	
}

void CDLoaderApp::MessageBox(CString strInfo,UINT nType /*= MB_OK*/,BOOL isError /*= TRUE*/)
{
	
	if ( !g_theApp.m_bCMDFlash )
	{
		AfxMessageBox(strInfo,nType);
	}
    else
    {
        ReportToPipe(strInfo,isError);
    }
}