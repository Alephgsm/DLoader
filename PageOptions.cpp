// PageOptions.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "PageOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageOptions property page

IMPLEMENT_DYNCREATE(CPageOptions, CPropertyPage)

CPageOptions::CPageOptions() : CPropertyPage(CPageOptions::IDD)
, m_bTmpReadChipUID(FALSE)
, m_bTmpCheckMatch(FALSE)
, m_bTmpSecureBoot(FALSE)
, m_bTmpDebugMode(FALSE)
, m_bTmpUartDownload(FALSE)
, m_bTmpAutoCompare(FALSE)
, m_strTmpCompareApp(_T(""))
{
	//{{AFX_DATA_INIT(CPageOptions)
	m_bTmpRepart        = FALSE;
	m_bTmpReset         = FALSE;
	m_bTmpReadMcpType   = FALSE;
	m_bTmpKeepCharge    = FALSE;
    m_bTmpDdrCheck      = FALSE;
    m_bTmpSelfRefresh   = FALSE;
    m_bTmpOldMemoryCheck= FALSE;
    m_bTmpEmmcCheck     = FALSE;
	m_bXmlPowerOff      = FALSE;
	m_bIniPowerOff      = FALSE;
	m_bUIPowerOff		= FALSE;
    m_bTmpEnableLog     = FALSE;
	//}}AFX_DATA_INIT

	m_bReadMcpType      = m_bTmpReadMcpType;
	m_bRepart           = m_bTmpRepart;
	m_bReset            = m_bTmpReset;
	m_bPowerOff         = m_bUIPowerOff;
	m_bKeepCharge       = m_bTmpKeepCharge;
	m_bUartDownload		= m_bTmpUartDownload;
	m_bAutoCompare		= m_bTmpAutoCompare;
	m_strCompareApp		= m_strTmpCompareApp;
    m_bDdrCheck			= m_bTmpDdrCheck;
    m_bSelfRefresh      = m_bTmpSelfRefresh;
    m_bOldMemoryCheck   = m_bTmpOldMemoryCheck;
    m_bEmmcCheck		= m_bTmpEmmcCheck;
    m_bReadChipUID      = m_bTmpReadChipUID;
    m_bCheckMatch       = m_bTmpCheckMatch;
    m_bEnableLog        = m_bTmpEnableLog;
    m_bEnableSecureBoot = m_bTmpSecureBoot;
	m_bEnableDebugMode	= m_bTmpDebugMode;
	m_nFlashPageType    = 0;
	m_bSharkNandOption  = FALSE;
    m_bEnableEndProcess = FALSE;
	m_dwComparePolicy	= 0;
	m_strCompareParameter = _T("\"%s\" \"%s\"");

}

CPageOptions::~CPageOptions()
{
}

void CPageOptions::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPageOptions)
    DDX_Check(pDX, IDC_DOP_REPART, m_bTmpRepart);
    DDX_Check(pDX, IDC_DOP_RESET, m_bTmpReset);
    DDX_Check(pDX, IDC_FOD_CHK_READ_MCPTYPE, m_bTmpReadMcpType);
    DDX_Check(pDX, IDC_DOP_CHK_KEEP_CHARGE, m_bTmpKeepCharge);
    DDX_Check(pDX, IDC_CHK_DDR_CHECK, m_bTmpDdrCheck);
    DDX_Check(pDX, IDC_CHK_SELF_REFRESH, m_bTmpSelfRefresh);
    DDX_Check(pDX, IDC_CHK_OLD_MEMORY, m_bTmpOldMemoryCheck);
    DDX_Check(pDX, IDC_CHK_EMMC_CHECK, m_bTmpEmmcCheck);
    DDX_Check(pDX, IDC_DOP_POWER_OFF, m_bUIPowerOff);
    DDX_Check(pDX, IDC_CHECK_READ_CHIPUID, m_bTmpReadChipUID);
    DDX_Check(pDX, IDC_CHK_CHECK_MATCH, m_bTmpCheckMatch);
    DDX_Check(pDX, IDC_CHK_ENABLE_DL_LOG, m_bTmpEnableLog);
    DDX_Check(pDX, IDC_CHK_ENABLE_SECUREBOOT, m_bTmpSecureBoot);   
	DDX_Check(pDX, IDC_CHK_DEBUG_MODE, m_bTmpDebugMode);
	DDX_Check(pDX, IDC_CHECK_UART_MODE, m_bTmpUartDownload);
	DDX_Check(pDX, IDC_CHECK_AUTO_COMPARE, m_bTmpAutoCompare);
	DDX_Text(pDX, IDC_EDIT_COMPARE_APP, m_strTmpCompareApp);
	DDV_MaxChars(pDX, m_strTmpCompareApp, 256);
    //}}AFX_DATA_MAP    
}


BEGIN_MESSAGE_MAP(CPageOptions, CPropertyPage)
	//{{AFX_MSG_MAP(CPageOptions)
	ON_BN_CLICKED(IDC_DOP_RESET, OnReset)
	ON_BN_CLICKED(IDC_DOP_POWER_OFF, OnPowerOff)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_COMPARE_APP, &CPageOptions::OnSelectCompareApp)
	ON_BN_CLICKED(IDC_CHECK_AUTO_COMPARE, &CPageOptions::OnAutoCompare)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageOptions message handlers

BOOL CPageOptions::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bTmpRepart        = m_bRepart;
	m_bTmpReset         = m_bReset;
	m_bUIPowerOff		= m_bPowerOff;
	m_bTmpKeepCharge    = m_bKeepCharge;
	m_bTmpUartDownload	= m_bUartDownload;
	m_bTmpAutoCompare	= m_bAutoCompare;
	m_strTmpCompareApp	= m_strCompareApp;
    m_bTmpDdrCheck      = m_bDdrCheck;
    m_bTmpSelfRefresh   = m_bSelfRefresh;
    m_bTmpOldMemoryCheck= m_bOldMemoryCheck;
    m_bTmpEmmcCheck     = m_bEmmcCheck;
	m_bTmpReadMcpType   = m_bReadMcpType;
    m_bTmpReadChipUID   = m_bReadChipUID;
    m_bTmpCheckMatch    = m_bCheckMatch;
    m_bTmpEnableLog     = m_bEnableLog;
    m_bTmpSecureBoot    = m_bEnableSecureBoot;
	m_bTmpDebugMode		= m_bEnableDebugMode;
	EnableAutoCompareCtrl(m_bTmpAutoCompare);

	UpdateData(FALSE);

#if defined(_SPUPGRADE) || defined(_FACTORY)	
	if(!g_theApp.m_bShowOtherPage)
	{
		GetDlgItem(IDC_DOP_REPART)->EnableWindow(FALSE);
		GetDlgItem(IDC_DOP_CHK_KEEP_CHARGE)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHK_DDR_CHECK)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHK_SELF_REFRESH)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHK_OLD_MEMORY)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHK_EMMC_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_UART_MODE)->EnableWindow(FALSE);
        //GetDlgItem(IDC_DOP_RESET)->EnableWindow(FALSE);
        //GetDlgItem(IDC_DOP_POWER_OFF)->EnableWindow(FALSE);
		GetDlgItem(IDC_FOD_CHK_READ_MCPTYPE)->EnableWindow(FALSE);       
        GetDlgItem(IDC_CHECK_READ_CHIPUID)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHK_CHECK_MATCH)->EnableWindow(FALSE); 
        GetDlgItem(IDC_CHK_ENABLE_SECUREBOOT)->EnableWindow(FALSE); 
		GetDlgItem(IDC_CHK_DEBUG_MODE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_AUTO_COMPARE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_COMPARISON_TEXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_COMPARE_APP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_SELECT_COMPARE_APP)->ShowWindow(SW_HIDE);
	}
#endif 
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPageOptions::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData();

	m_bReadMcpType  = m_bTmpReadMcpType;
	m_bRepart       = m_bTmpRepart;
	m_bReset        = m_bTmpReset;
	m_bPowerOff     = m_bUIPowerOff;
	m_bKeepCharge   = m_bTmpKeepCharge;
	m_bUartDownload = m_bTmpUartDownload;
	m_bAutoCompare	= m_bTmpAutoCompare;
	m_strCompareApp	= m_strTmpCompareApp;
    m_bDdrCheck     = m_bTmpDdrCheck;
    m_bSelfRefresh  = m_bTmpSelfRefresh;
    m_bOldMemoryCheck = m_bTmpOldMemoryCheck;
    m_bEmmcCheck    = m_bTmpEmmcCheck;
    m_bReadChipUID  = m_bTmpReadChipUID;
    m_bCheckMatch   = m_bTmpCheckMatch;
    m_bEnableLog    = m_bTmpEnableLog;
    m_bEnableSecureBoot = m_bTmpSecureBoot;
	m_bEnableDebugMode	= m_bTmpDebugMode;

	if(!m_strIniFile.IsEmpty())
	{
		DWORD dwAttr = GetFileAttributes(m_strIniFile);
		if(MAXDWORD != dwAttr)
		{
			dwAttr &= ~FILE_ATTRIBUTE_READONLY;
			::SetFileAttributes(m_strIniFile,dwAttr);
		}
		SaveSettings(m_strIniFile);
	}

	CPropertyPage::OnOK();
}


BOOL CPageOptions::LoadSettings(LPCTSTR pFileName,CString &strErrorMsg)
{
	//	UNUSED_ALWAYS(strErrorMsg);
    ASSERT(NULL != pFileName);
    if(NULL == pFileName)
    {
		strErrorMsg += _T("Configure file is empty!\n");
        return FALSE;
    }

	_TCHAR szBmFileTypeIniFile[MAX_PATH] = {0};    
    GetModuleFilePath( g_theApp.m_hInstance, szBmFileTypeIniFile );
    _tcscat( szBmFileTypeIniFile,  _T("\\BMFileType.ini") );

    m_strIniFile        = pFileName;	
	m_bRepart           = GetPrivateProfileInt( _T("Options"),_T("Repartition"),1,pFileName );
	m_bReset            = GetPrivateProfileInt( _T("Options"),_T("Reset"),0,pFileName );
	m_bIniPowerOff		= GetPrivateProfileInt( _T("Options"),_T("PowerOff"),0,pFileName );
	m_nFlashPageType    = GetPrivateProfileInt( _T("Options"),_T("FlashPageType"),0,pFileName );
	m_bReadMcpType      = GetPrivateProfileInt( _T("Options"),_T("ReadMCPType"),0,pFileName );
	m_bKeepCharge       = GetPrivateProfileInt( _T("Options"),_T("KeepCharge"),0,pFileName );
	m_bUartDownload     = GetPrivateProfileInt( _T("Options"),_T("UartDownload"),0,pFileName );
	m_bAutoCompare		= GetPrivateProfileInt( _T("Comparison"),_T("Enable"),0,pFileName );
	m_dwComparePolicy	= GetPrivateProfileInt( _T("Comparison"),_T("Policy"),0,pFileName );
    m_bDdrCheck         = GetPrivateProfileInt( _T("Options"),_T("DDRCheck"),0,pFileName );
    m_bSelfRefresh      = GetPrivateProfileInt( _T("Options"),_T("SelfRefresh"),0,pFileName );
    m_bOldMemoryCheck   = GetPrivateProfileInt( _T("Misc"),_T("NeedCheckOldMemory"),0,szBmFileTypeIniFile );
    m_bEmmcCheck        = GetPrivateProfileInt( _T("Misc"),_T("DoCheckSum"),0,szBmFileTypeIniFile );
	m_bSharkNandOption  = GetPrivateProfileInt( _T("Options"),_T("SharkNandOption"),0,pFileName );
    m_bReadChipUID      = GetPrivateProfileInt( _T("Options"),_T("ReadChipUID"),0,pFileName );
    m_bEnableLog     	= GetPrivateProfileInt( _T("Options"),_T("EnableLod"),0,pFileName );
    m_bEnableSecureBoot = GetPrivateProfileInt( _T("Options"),_T("EnableSecureBoot"),0,pFileName );
    m_bEnableEndProcess = GetPrivateProfileInt( _T("Options"),_T("EnableEndProcess"),0,pFileName );
	m_bEnableDebugMode  = GetPrivateProfileInt( _T("Debug"),_T("EnableDebugMode"),0,pFileName );
#ifndef _FACTORY
    m_bCheckMatch      = GetPrivateProfileInt( _T("Options"),_T("CheckMatch"),0,pFileName );
#endif

#ifdef _RESEARCH
	TCHAR szBuf[MAX_PATH] = {0};
	memset(szBuf,0,sizeof(szBuf));
	GetPrivateProfileString(_T("Comparison"),_T("Program"),_T(""),szBuf,_MAX_PATH,pFileName);
	m_strCompareApp = szBuf;

	memset(szBuf,0,sizeof(szBuf));
	GetPrivateProfileString(_T("Comparison"),_T("Parameter"),_T("\"%s\" \"%s\""),szBuf,_MAX_PATH,pFileName);
	m_strCompareParameter = szBuf;
#endif
	
	if(m_bReset)
	{
		m_bPowerOff = FALSE;
	}
	else
	{
		m_bPowerOff = m_bXmlPowerOff;
		if (!m_bXmlPowerOff)
		{		
			m_bPowerOff = m_bIniPowerOff;
		}
	}

	m_bTmpRepart        = m_bRepart;
	m_bTmpReset         = m_bReset;
	m_bUIPowerOff		= m_bPowerOff;
	m_bTmpReadMcpType   = m_bReadMcpType;
	m_bTmpKeepCharge    = m_bKeepCharge;
	m_bTmpUartDownload	= m_bUartDownload;
	m_bTmpAutoCompare	= m_bAutoCompare;
	m_strTmpCompareApp	= m_strCompareApp;
    m_bTmpDdrCheck      = m_bDdrCheck;
    m_bTmpSelfRefresh   = m_bSelfRefresh;
    m_bTmpOldMemoryCheck= m_bOldMemoryCheck;
    m_bTmpEmmcCheck     = m_bEmmcCheck;
    m_bTmpReadChipUID   = m_bReadChipUID;
    m_bTmpCheckMatch    = m_bCheckMatch;
    m_bTmpEnableLog     = m_bEnableLog;
    m_bTmpSecureBoot    = m_bEnableSecureBoot;
	m_bTmpDebugMode		= m_bEnableDebugMode;

    return TRUE;
}

BOOL CPageOptions::SaveSettings(LPCTSTR pFileName)
{
    ASSERT(NULL != pFileName);
    if(NULL == pFileName)
    {
        return FALSE;
    }

    _TCHAR szBmFileTypeIniFile[MAX_PATH];    
    GetModuleFilePath( g_theApp.m_hInstance, szBmFileTypeIniFile );
    _tcscat( szBmFileTypeIniFile,  _T("\\BMFileType.ini") );
	
    CString strTemp;
    strTemp.Format( _T("%d"),m_bRepart );
    WritePrivateProfileString(  _T("Options"),_T("Repartition"),strTemp,pFileName );
	
    strTemp.Format( _T("%d"),m_bReset );
    WritePrivateProfileString( _T("Options"),_T("Reset"),strTemp,pFileName );

	strTemp.Format( _T("%d"),m_bKeepCharge );
    WritePrivateProfileString( _T("Options"),_T("KeepCharge"),strTemp,pFileName );

	strTemp.Format( _T("%d"),m_bUartDownload );
	WritePrivateProfileString( _T("Options"),_T("UartDownload"),strTemp,pFileName );

#ifdef _RESEARCH
	strTemp.Format( _T("%d"),m_bAutoCompare );
	WritePrivateProfileString( _T("Comparison"),_T("Enable"),strTemp,pFileName );
	WritePrivateProfileString( _T("Comparison"),_T("Program"),m_strCompareApp,pFileName );
#endif

    strTemp.Format( _T("%d"),m_bDdrCheck );
    WritePrivateProfileString( _T("Options"),_T("DDRCheck"),strTemp,pFileName );

    strTemp.Format( _T("%d"),m_bSelfRefresh );
    WritePrivateProfileString( _T("Options"),_T("SelfRefresh"),strTemp,pFileName );

    strTemp.Format( _T("%d"),m_bOldMemoryCheck );
    WritePrivateProfileString( _T("Misc"),_T("NeedCheckOldMemory"),strTemp,szBmFileTypeIniFile );

    strTemp.Format( _T(" %d"),m_bEmmcCheck );
    WritePrivateProfileString( _T("Misc"),_T("DoCheckSum"),strTemp,szBmFileTypeIniFile );

    strTemp.Format( _T("%d"),m_bIniPowerOff );
    WritePrivateProfileString( _T("Options"),_T("PowerOff"),strTemp,pFileName ); 

    strTemp.Format( _T("%d"),m_bReadChipUID );
    WritePrivateProfileString( _T("Options"),_T("ReadChipUID"),strTemp,pFileName ); 

    strTemp.Format( _T("%d"),m_bCheckMatch );
    WritePrivateProfileString( _T("Options"),_T("CheckMatch"),strTemp,pFileName ); 

	strTemp.Format( _T("%d"),m_bEnableDebugMode );
	WritePrivateProfileString( _T("Debug"),_T("EnableDebugMode"),strTemp,pFileName ); 

    return TRUE;
}

int CPageOptions::GetGUINandRepartStrategy()
{
	if(::IsWindow(this->GetSafeHwnd()))
	{
		UpdateData(FALSE);
	}
	
	return (m_bTmpRepart ? REPAR_STRATEGY_ALWAYS : REPAR_STRATEGY_STOP);
}

int CPageOptions::GetNandRepartStrategy()
{
	return (m_bRepart ? REPAR_STRATEGY_ALWAYS : REPAR_STRATEGY_STOP);
}

void CPageOptions::OnReset() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_bTmpReset && m_bUIPowerOff)
	{
		m_bUIPowerOff = FALSE;
		m_bIniPowerOff = m_bUIPowerOff;
		UpdateData(FALSE);
	}
}

void CPageOptions::OnPowerOff() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_bIniPowerOff = m_bUIPowerOff;
	if(m_bTmpReset && m_bUIPowerOff)
	{
		m_bTmpReset = FALSE;
		UpdateData(FALSE);
	}
}

void CPageOptions::OnSelectCompareApp()
{
	// TODO: Add your control notification handler code here 
	UpdateData();
	static _TCHAR BASED_CODE szFilter[] = _T("Programs (*.exe)|*.exe|All Files(*.*)|*.*||");
	CString strFile = m_strTmpCompareApp.IsEmpty() ? _T("C:\\Program Files (x86)\\Beyond Compare 3\\BCompare.exe") : m_strTmpCompareApp;
	CFileDialog dlg(TRUE, _T(".exe"), strFile,OFN_NOCHANGEDIR |OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter, NULL);

	if(dlg.DoModal() == IDCANCEL)
		return; 

	m_strTmpCompareApp = dlg.GetPathName();
	UpdateData(FALSE);
}

void CPageOptions::OnAutoCompare()
{
	// TODO: Add your control notification handler code here 
	UpdateData();
	EnableAutoCompareCtrl(m_bTmpAutoCompare);
}

void CPageOptions::EnableAutoCompareCtrl(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_COMPARE_APP)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_SELECT_COMPARE_APP)->EnableWindow(bEnable);
}

DWORD CPageOptions::GetUnPacPolicy()
{
	_TCHAR szBinPackIniFile[MAX_PATH] = {0};    
	GetModuleFilePath( g_theApp.m_hInstance, szBinPackIniFile );
	_tcscat( szBinPackIniFile,  _T("\\BinPack.ini") );
	return  GetPrivateProfileInt( _T("Setting"),_T("LoadPolicy"),0,szBinPackIniFile );

}
