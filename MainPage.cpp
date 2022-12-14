// MainPage.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "MainPage.h"
#include "SettingsSheet.h"
#include "EdtBtnCtrl.h"
#include "DlgPacketSetting.h"
#include "MainFrm.h"
#include "Calibration.h"
#include "Calibration_Struct.h"
#include <atlconv.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef _lint // bad pc-lint
extern BOOL g_bInitSheet; 
#endif

#define ALL_PORT _T("All")

// Serial ports section
static _TCHAR g_sz_SP[]			= _T("Serial port");
static _TCHAR g_sz_BR[]			= _T("Baud rate");
static _TCHAR g_sz_PORT[]		= _T("Port");

// GUI main page
static _TCHAR g_sz_SEC_GUI[]	= _T("GUI");
static _TCHAR g_sz_KEY_SEL[]	= _T("CanSelect");
static _TCHAR g_sz_KEY_EDT[]	= _T("CanEdit");
static _TCHAR g_sz_KEY_BRS[]	= _T("BaudRates");
static _TCHAR g_sz_BR_DEFAULT[]	= _T("57600,115200,230400,460800,921600");


static _TCHAR g_sz_SEC_NV[]		= _T("NV");
static _TCHAR g_sz_KEY_LNII[]   = _T("LangNVItemID");

static _TCHAR g_szUA[]            = _T("OmadmFota");
static _TCHAR g_szEDU[]           = _T("EraseDU");
static _TCHAR g_szPreload[]       = _T("Preload");
static _TCHAR g_szKernelImg2[]    = _T("KernelImg2");
static _TCHAR g_szRomDisk[]		  = _T("RomDisk");

static _TCHAR g_sz_SEC_CHIPDSPMAP[]	 = _T("ChipDspMap");
static _TCHAR g_sz_KEY_CDME[]		 = _T("enable");


#define CMD_PAC_EXIT \
{\
	if(g_theApp.m_bCMDPackage)\
{\
	GetParent()->PostMessage(WM_COMMAND, IDCANCEL);\
	AfxGetMainWnd()->PostMessage(WM_CLOSE);\
}\
}

/////////////////////////////////////////////////////////////////////////////
// CMainPage property page

IMPLEMENT_DYNCREATE(CMainPage, CPropertyPage) 

CMainPage::CMainPage() : CPropertyPage(CMainPage::IDD)
{
	//{{AFX_DATA_INIT(CMainPage)
	m_bAllFiles = FALSE;
	m_bChkWriteSN = FALSE;
	//}}AFX_DATA_INIT
	m_pXmlConfigParse = NULL;
	m_bInit = FALSE;
	m_strIniFile = _T("");
	m_pCurProductInfo = NULL;
	m_nBaudRate = -1;
	m_nComPort = -1;
	m_nCurProduct = -1;
	
	m_strCurProduct = _T("");
	
	m_nTmpProduct = -1;
	m_pTmpProductInfo = NULL;
	m_strTempProduct = _T("");
	
	
	m_nProductCount = 0;
	
	m_pCaliPage = NULL;
	m_pOptionsPage = NULL;
	m_pMultiLangPage = NULL;
	m_pLCDCfigPage = NULL;
	m_pOptionsPage = NULL;
	m_pVolFreqPage = NULL;
    m_pUartPortSwitchPage = NULL;
	m_pFlashOptPage = NULL;
    m_pCustomizationPage = NULL;

	m_strPrdVersion = _T("");
	m_mapPrdCfg.RemoveAll();
	
	m_wLangNVItemID = NV_MULTI_LANG_ID;
	
#if defined(_SPUPGRADE) || defined(_FACTORY)
	m_bCanEdit = FALSE;
	m_bCanSel = FALSE;
#else
	m_bCanEdit = TRUE;
	m_bCanSel = TRUE;
#endif	
	
	m_bOmaDM = TRUE;
	m_bTmpOmaDM = TRUE;
	
	m_bPreload = TRUE;
	m_bTmpPreload = TRUE;
	
	m_bKernelImg2 = TRUE;
	m_bTmpKernelImg2 = TRUE;
	
	m_bRomDisk = TRUE;
	m_bTmpRomDisk = TRUE;
	
	m_bEnableChipDspMap = FALSE;
	
	m_bWriteSN = m_bChkWriteSN;

	m_bListLock = FALSE;

	m_bShowChinese = FALSE;
	ZeroMemory(&m_ftPacOrg,sizeof(m_ftPacOrg));
}

CMainPage::~CMainPage()
{
	/*lint -save -e1551 */
	if(m_pXmlConfigParse != NULL)
	{
		m_pXmlConfigParse->Release();
		m_pXmlConfigParse = NULL;
	}
	m_pCurProductInfo = NULL;
	m_pTmpProductInfo = NULL;
	
	ClearProdInfoVector();
	m_arrProductName.RemoveAll();
	
	m_pCaliPage =  NULL;
	m_pOptionsPage = NULL;
	m_pLCDCfigPage = NULL;
	m_pMultiLangPage = NULL;
	m_pVolFreqPage = NULL;
    m_pUartPortSwitchPage = NULL;
    m_pCustomizationPage = NULL;

	m_mapDLFiles.RemoveAll();
	m_mapDLState.RemoveAll();
	m_mapDLSize.RemoveAll();
    m_mapLastTime.RemoveAll();
	
	/*lint -restore */
}

void CMainPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainPage)
	DDX_Control(pDX, IDC_EDT_PRD_VERSION, m_edtPrdVersion);
	DDX_Control(pDX, IDC_STC_PROD_COMMENT, m_stcProdComment);
	DDX_Control(pDX, IDC_LST_PRODUCT_INFO, m_lstProductInfo);
	DDX_Control(pDX, IDC_CMB_PRODUCT, m_cmbProduct);
	DDX_Control(pDX, IDC_CMB_PORT, m_cmbPort);
	DDX_Control(pDX, IDC_CMB_BAUDRATE, m_cmbBaudrate);
	DDX_Check(pDX, IDC_CHK_ALL, m_bAllFiles);
	DDX_Check(pDX, IDC_MPD_CHK_SN, m_bChkWriteSN);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMainPage, CPropertyPage)
//{{AFX_MSG_MAP(CMainPage)
ON_CBN_SELCHANGE(IDC_CMB_PRODUCT, OnSelchangeCmbProduct)
ON_BN_CLICKED(IDC_CHK_ALL, OnChkAll)
ON_NOTIFY(NM_DBLCLK, IDC_LST_PRODUCT_INFO, OnDblclkLstProductInfo)
ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LST_PRODUCT_INFO, OnEndlabeleditLstProductInfo)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LST_PRODUCT_INFO, OnItemchangedLstProductInfo)
ON_WM_CTLCOLOR()
ON_NOTIFY(NM_RCLICK, IDC_LST_PRODUCT_INFO, OnRclickLstProductInfo)
ON_NOTIFY(NM_CLICK, IDC_LST_PRODUCT_INFO, OnClickLstProductInfo)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainPage message handlers

BOOL CMainPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitPort();
	InitBaudrate();
	InitListCtrl();

	int i = 0;		
	m_bAllFiles = TRUE;
	
	if(m_nProductCount>0)
	{
		for(i=0;i<m_nProductCount;i++)
			m_cmbProduct.AddString(m_arrProductName[i]);
	}
	
	if(m_strCurProduct.IsEmpty())
	{
		m_nCurProduct = 0;
		m_strCurProduct = m_arrProductName.GetAt(0);		
		m_nTmpProduct = m_nCurProduct;
		m_strTempProduct = m_strCurProduct;
	}
	
    FillList(m_strCurProduct);
	
	m_cmbProduct.SetCurSel(m_nCurProduct);
	CMainFrame * pMF = (CMainFrame*)AfxGetMainWnd();
#if defined(_SPUPGRADE) || defined(_FACTORY)
	CStatic *pwnd = (CStatic *)GetDlgItem(IDC_STC_SEL_PRD);
	if(pwnd != NULL)
	{
		pwnd->SetWindowText(_T("          Product:"));
	}
	GetDlgItem(IDC_CMB_PRODUCT)->EnableWindow(FALSE);		
	
#ifdef _FACTORY	
	if(!pMF->m_bNeedPhaseCheck)
	{
		GetDlgItem(IDC_MPD_CHK_SN)->ShowWindow(SW_HIDE);
	}
#else 
	GetDlgItem(IDC_MPD_CHK_SN)->ShowWindow(SW_HIDE);
#endif // _FACTORY
#else
	if(!pMF->m_bNeedPhaseCheck)
	{
		GetDlgItem(IDC_MPD_CHK_SN)->ShowWindow(SW_HIDE);
	}
	GetDlgItem(IDC_EDT_PRD_VERSION)->ShowWindow(SW_HIDE);
#endif // defined(_SPUPGRADE) || defined(_FACTORY)
	
	m_bChkWriteSN = m_bWriteSN;	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
BOOL CMainPage::LoadSettings(LPCTSTR pFileName,CString &strErrorMsg)
{
	//ini first
	ASSERT(NULL != pFileName);
    if(NULL == pFileName)
    {
		strErrorMsg += _T("Configure file is empty!\n");
        return FALSE;
    }   
    m_strIniFile = pFileName;
	
	CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
	if(pMF!= NULL && pMF->m_bPacketOpen)
	{
		return LoadConfig(pFileName,strErrorMsg);
	}	
	
	if(!_LoadConfig(strErrorMsg))
	{
		return FALSE;
	}    
	
	LoadDLFiles(pFileName);	
	
	if(m_pCurProductInfo != NULL)
	{
		if(m_pCurProductInfo->dwOmaDMFlag != 0)
		{
			CString strFDL2 = _T("");
			m_mapDLFiles.Lookup(_T("FDL2"),strFDL2);
			m_bOmaDM = m_bTmpOmaDM = GetFdl2Flag(strFDL2,FDL2F_OMADM);
			m_bPreload = m_bTmpPreload = GetFdl2Flag(strFDL2,FDL2F_PRELOAD);
			m_bKernelImg2 = m_bTmpKernelImg2 = GetFdl2Flag(strFDL2,FDL2F_KERNELIMG2);
			m_bRomDisk = m_bTmpRomDisk = GetFdl2Flag(strFDL2,FDL2F_ROMDISK);
		}
	}
	
	InitCaliPage(m_pCurProductInfo);
	InitMultiLangPage();
	InitLCDCfigPage();

#ifdef _RESEARCH
	InitVolFreqPage();
    InitUartPortSwitchPage();
	InitFlashOptPage(m_pCurProductInfo,pFileName);
#endif

    return TRUE;
}

BOOL CMainPage::SaveSettings(LPCTSTR pFileName)
{
	ASSERT(NULL != pFileName);
    if(NULL == pFileName)
    {
        return FALSE;
    }
	
	CFileFind finder;
	if(finder.FindFile(pFileName))
	{
		DWORD dwAtt = GetFileAttributes(pFileName);
		dwAtt &= ~FILE_ATTRIBUTE_READONLY;
		SetFileAttributes(pFileName,dwAtt);
	}
	
    // Save baudrate 
    CString strTemp;
    strTemp.Format( _T("%d"), m_nComPort );
    ::WritePrivateProfileString( g_sz_SP,g_sz_PORT,strTemp,pFileName);
    strTemp.Format(_T("%d"),m_nBaudRate);
    ::WritePrivateProfileString( g_sz_SP,g_sz_BR,strTemp,pFileName);
    
	// save selected product
	::WritePrivateProfileString(g_sz_SELECT,g_sz_SELECT_PRODUCT,m_strCurProduct,pFileName);

	CMainFrame * pMF = (CMainFrame *)AfxGetMainWnd();
	if(m_strCurProduct.Find(_T("PAC_")) != 0)
	{
		pMF->m_bPacketOpen = FALSE;
	}	
	
	BOOL bPacketMode = FALSE;
    if(pMF != NULL)
	{
		if(pMF->m_bPacketOpen)
		{
			bPacketMode= TRUE;
		}
	}

	
	strTemp.Format( _T("%d"), bPacketMode );
	::WritePrivateProfileString(g_sz_SEC_GUI,_T("PacketMode"),strTemp,pFileName);
	
	BOOL bAutoGenerateSN = !m_bWriteSN;
	strTemp.Format( _T("%d"),  bAutoGenerateSN);
	::WritePrivateProfileString(_T("SN"),  _T("AutoGenerateSN"),strTemp,pFileName);
	
    // Save download files' names
	// Save omitted file also 	
	// first delete old files 
	::WritePrivateProfileString( m_strCurProduct,NULL,NULL,pFileName);
	CString strFilePath;
	CString strFileID;
	int nItem = m_lstProductInfo.GetItemCount();
	CString strCheckFile;
	if(m_pTmpProductInfo != NULL)
	{
		for(int i = 0; i<nItem && !m_strCurProduct.IsEmpty(); i++)
		{
			strFileID = m_pTmpProductInfo->pFileInfoArr[i].szID;
			strFilePath = m_lstProductInfo.GetItemText(i,1);
			strCheckFile.Format(_T("%d@%s"),m_lstProductInfo.GetCheck(i),strFilePath.operator LPCTSTR());
			WritePrivateProfileString( m_strCurProduct, strFileID,strCheckFile, pFileName );
#if defined(_SPUPGRADE)
			if(g_theApp.m_bKeepPacNVState)
			{
				if(strFileID.Find(_T("NV"))== 0)
				{
					WritePrivateProfileString( strFileID, strFileID,strCheckFile, pFileName );
				}
			}
#endif
		}
	}
	
    return TRUE;
}

void CMainPage::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
    
    CString strText;
	
    m_cmbPort.GetWindowText( strText );
    if( _tcscmp( strText, ALL_PORT ) == 0 )
    {
        m_nComPort = 0;
    }
    else
    {
        _stscanf( strText, _T("COM%d"), &m_nComPort );
    }
    
    m_cmbBaudrate.GetWindowText(strText);
    m_nBaudRate = _ttoi(strText);
	
	m_mapDLFiles.RemoveAll();
	m_mapDLState.RemoveAll();	
	m_cmbProduct.GetWindowText(m_strCurProduct);
	m_nCurProduct = m_cmbProduct.GetCurSel();
	
    if(m_nCurProduct != -1)
		m_pCurProductInfo = m_vctProductInfo[m_nCurProduct];

	int nCount = m_lstProductInfo.GetItemCount();
	if(m_pCurProductInfo != NULL)
	{
		for(int i=0;i<nCount;i++)
		{
			CString strID = m_pCurProductInfo->pFileInfoArr[i].szID;
			strID.MakeUpper();
			m_mapDLFiles.SetAt(strID,m_lstProductInfo.GetItemText(i,1));
			m_mapDLState.SetAt(strID,m_lstProductInfo.GetCheck(i));		
		}
	}
	
	if(CheckInitDLFiles())
	{
		m_bInit = TRUE;
	}
	else
	{
		m_bInit = FALSE;
	}	
	
	m_bOmaDM = m_bTmpOmaDM;
	m_bPreload = m_bTmpPreload;
	m_bKernelImg2 = m_bTmpKernelImg2;
	m_bRomDisk = m_bTmpRomDisk;
	m_bWriteSN = m_bChkWriteSN;
	
	CMainFrame * pMF = (CMainFrame *)AfxGetMainWnd();
	
	if(m_strCurProduct.Find(_T("PAC_")) != 0)
	{
		CString strTmp;
		strTmp.Format(_T("%s"),m_strCurProduct.operator LPCTSTR());
		if(pMF != NULL)
		{
			pMF->SetPrdVersion(strTmp);
			pMF->m_bPacketOpen = FALSE;
		}
	}
	
	//save setting
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

BOOL CMainPage::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	this->SetFocus();
	if( !g_bInitSheet)
	{
		CString strErr;		
		int nCount = m_lstProductInfo.GetItemCount();
		int nCheckCount = 0;
		for(int i = 0;i<nCount;i++)
		{
			if(m_lstProductInfo.GetCheck(i) )
			{
				nCheckCount++;
				if(DI_IS_NEED_FILE(m_lstProductInfo.GetItemData(i)))
				{
					if(m_lstProductInfo.GetItemText(i,1).IsEmpty())
					{
						AfxMessageBox(IDS_ERR_FILE_EMPTY);
						return FALSE;
					}
				}				
			}
			if(!m_lstProductInfo.GetCheck(i) && DI_IS_KEY_ITEM(m_lstProductInfo.GetItemData(i)) )
			{
				strErr.Format(_T("%s must be selected!"),m_lstProductInfo.GetItemText(i,0).operator LPCTSTR());
				AfxMessageBox(strErr.operator LPCTSTR()/*IDS_ERR_FDL_NOT_CHECKED*/);
				m_lstProductInfo.SetCheck(i);
				return FALSE;
			}
		}

		if(nCheckCount == 0)
		{	
			AfxMessageBox(_T("Must select at least one file!"));
			return FALSE;
		}
		
		CSettingsSheet* pParentWnd = NULL;
		pParentWnd = (CSettingsSheet*)GetParent();
		CString strCurProduct;
		m_cmbProduct.GetWindowText(strCurProduct);
		int nSel = m_cmbProduct.GetCurSel();
		
		PPRODUCT_INFO_T pProdInfo = m_vctProductInfo[nSel];
		
		if(m_pCaliPage != NULL && 
			( m_pCaliPage->m_strTempCurProduct.IsEmpty() || 
			m_pCaliPage->m_strTempCurProduct.CompareNoCase(strCurProduct)!=0 ))
		{
            pParentWnd->m_pageCalibration.m_lstBackup.InitNvBackupInfo(pProdInfo);
            pParentWnd->m_pageCalibration.m_lstBackup.FillList();

			pParentWnd->m_pageCalibration.m_strTempCurProduct = strCurProduct;

			pParentWnd->m_pageCalibration.AddBackupFiles(pProdInfo->pFileInfoArr,pProdInfo->dwFileCount,FALSE);
			
		}
		
		InitMultiLangPage(TRUE);
		InitLCDCfigPage(TRUE);
#ifdef _RESEARCH
		InitVolFreqPage(TRUE);
        InitUartPortSwitchPage(TRUE);
#endif 

	}
	
	return CPropertyPage::OnKillActive();
}

void CMainPage::OnSelchangeCmbProduct() 
{
    CString strProduct;
	m_cmbProduct.GetWindowText(strProduct);
	GetDlgItem(IDC_CHK_ALL)->EnableWindow(TRUE);
	m_bAllFiles = TRUE;
	FillList(strProduct);
	
	UpdateData(FALSE);
}

BOOL CMainPage::OnSetActive() 
{	
	return CPropertyPage::OnSetActive();
}

BOOL CMainPage::FillList(LPCTSTR lpcstrProduct)
{
	ASSERT(m_pXmlConfigParse != NULL);
	if(lpcstrProduct==NULL)
		return TRUE;
	
	int nProduct = GetProductIndex(lpcstrProduct);
	if(nProduct == -1)
		return FALSE;
	
	m_nTmpProduct = nProduct;
	
	m_strTempProduct = lpcstrProduct;
	
	PPRODUCT_INFO_T pProdInfo = m_vctProductInfo[nProduct];
	m_pTmpProductInfo = pProdInfo;
	
	DWORD dwCount=pProdInfo->dwFileCount;
    PFILE_INFO_T pFileInfo = pProdInfo->pFileInfoArr;
	
	
	CString strComment = pProdInfo->szComment;
	m_stcProdComment.SetText(strComment);
	
#if defined(_SPUPGRADE) || defined(_FACTORY)
	CString strVer = m_strPrdVersion;
	strVer.MakeUpper();
	m_edtPrdVersion.SetWindowText(strVer);
#endif
	
	m_lstProductInfo.DeleteAllItems();
	
	UINT i=0;
	UINT j=0;
	CString str;

	m_bListLock = TRUE;
	for(i=0;i<dwCount;i++)
	{
		m_lstProductInfo.InsertItem(i,pFileInfo[i].szIDAlias);

		CString strID = pFileInfo[i].szID;
		strID.MakeUpper();
		
        UINT nBlock = pFileInfo[i].dwBlockCount;	
		if(nBlock > 5)
			nBlock = 5;
		
		for(j=0;j<nBlock;j++)
		{
			str.Format(_T("0x%X"),pFileInfo[i].arrBlock[j].llBase);
			if(pProdInfo->dwPartitionCount>0 && _tcslen(pFileInfo[i].arrBlock[j].szRepID) != 0)
			{
				str = pFileInfo[i].arrBlock[j].szRepID;
			}
			
			m_lstProductInfo.SetItemText(i,2+j*2,str);

#if defined(_SPUPGRADE) || defined(_FACTORY)
			__int64 llFileSize;
			if(j==0 && pFileInfo[i].dwFlag != 0 &&  m_mapDLSize.Lookup(strID,llFileSize))
			{
				str.Format(_T("0x%I64X"),llFileSize);
			}
			else
#endif
			{
				if( 0 == j && pProdInfo->dwPartitionCount>0 && _tcslen(pFileInfo[i].arrBlock[j].szRepID) != 0)
				{
					__int64 llPartSize = m_pFlashOptPage->GetPartitionSize(pFileInfo[i].arrBlock[j].szRepID,pProdInfo);
					if (0 != llPartSize)
					{
						str.Format(_T("0x%I64X"),llPartSize);
					}
					else
					{
						str.Format(_T("0x%I64X"),pFileInfo[i].arrBlock[j].llSize);
					}
				}
				else
				{
					str.Format(_T("0x%I64X"),pFileInfo[i].arrBlock[j].llSize);
				}

			}

			if(str.CompareNoCase(_T("0x0"))==0)
			{
				str = _T("");
			}
			m_lstProductInfo.SetItemText(i,3+j*2,str);
		}
		m_lstProductInfo.SetItemText(i,12,pFileInfo[i].szType);
		DWORD dwItemData = pFileInfo[i].dwFlag;
		if(	pFileInfo[i].dwCheckFlag == 2 )
		{
			dwItemData = DI_SET_OMIT(dwItemData);
		}
		else if(pFileInfo[i].dwCheckFlag == 1)
		{
			dwItemData = DI_SET_KEY_ITEM(dwItemData);
		}

		m_lstProductInfo.SetItemData(i,dwItemData);
		m_lstProductInfo.SetCheck(i);	
	}
	m_bListLock = FALSE;
	m_bAllFiles = TRUE;
	UpdateData(FALSE);
	
	CString strFDL2Path;
	strFDL2Path.Empty();
	
	if(lpcstrProduct!=NULL && m_strCurProduct.CompareNoCase(lpcstrProduct)==0 ) //lint !e774
	{
		for(i=0;i<dwCount;i++)
		{
			CString strFileID = pFileInfo[i].szID;
			strFileID.MakeUpper();
			CString strFile = _T("");
			BOOL    bCheck = FALSE;
			m_mapDLFiles.Lookup(strFileID,strFile);
			m_mapDLState.Lookup(strFileID,bCheck);

			m_lstProductInfo.SetItemText(i,1,strFile);				
			
			if(m_pTmpProductInfo->dwOmaDMFlag != 0)
			{
				DWORD dwData = m_lstProductInfo.GetItemData(i);
				DWORD dwNewData = dwData;

				
				if(strFileID.CompareNoCase(_T("FDL2")) == 0)
				{
					strFDL2Path = strFile;
					m_bTmpOmaDM = GetFdl2Flag(strFDL2Path,FDL2F_OMADM);
					m_bTmpPreload = GetFdl2Flag(strFDL2Path,FDL2F_PRELOAD);
					m_bTmpKernelImg2 = GetFdl2Flag(strFDL2Path,FDL2F_KERNELIMG2);
					m_bTmpRomDisk = GetFdl2Flag(strFDL2Path,FDL2F_ROMDISK);
				}
				else if(strFileID.CompareNoCase(g_szUA) == 0 || strFileID.CompareNoCase(g_szEDU) == 0)
				{					
					if(!m_bTmpOmaDM)
					{
						dwNewData = DI_SET_GRAY(dwData);						
					}
					else
					{
						dwNewData = DI_UNSET_GRAY(dwData);
					}
					
					if( DI_IS_GRAY(dwData) != DI_IS_GRAY(dwNewData))
					{
						m_lstProductInfo.SetItemData(i,dwNewData);	
						m_lstProductInfo.SetCheck(i,FALSE);
						if(!DI_IS_GRAY(dwNewData))
						{
							m_bAllFiles = FALSE;							
						}
					}				
				}
				else if(strFileID.CompareNoCase(g_szPreload) == 0)
				{			
					if(!m_bTmpPreload)
					{				
						dwNewData = DI_SET_GRAY(dwData);		
					}
					else
					{
						dwNewData = DI_UNSET_GRAY(dwData);
					}
					
					if( DI_IS_GRAY(dwData) != DI_IS_GRAY(dwNewData))
					{
						m_lstProductInfo.SetItemData(i,dwNewData);	
						m_lstProductInfo.SetCheck(i,FALSE);
						if(!DI_IS_GRAY(dwNewData))
						{
							m_bAllFiles = FALSE;							
						}
					}				
				}
				else if(strFileID.CompareNoCase(g_szKernelImg2) == 0)
				{				
					if(!m_bTmpKernelImg2)
					{				
						dwNewData = DI_SET_GRAY(dwData);		
					}
					else
					{
						dwNewData = DI_UNSET_GRAY(dwData);
					}
					
					if( DI_IS_GRAY(dwData) != DI_IS_GRAY(dwNewData))
					{
						m_lstProductInfo.SetItemData(i,dwNewData);	
						m_lstProductInfo.SetCheck(i,FALSE);
						if(!DI_IS_GRAY(dwNewData))
						{
							m_bAllFiles = FALSE;							
						}
					}	
				}
				else if(strFileID.CompareNoCase(g_szRomDisk) == 0)
				{				
					if(!m_bTmpRomDisk)
					{				
						dwNewData = DI_SET_GRAY(dwData);			
					}
					else
					{
						dwNewData = DI_UNSET_GRAY(dwData);
					}
					
					if( DI_IS_GRAY(dwData) != DI_IS_GRAY(dwNewData))
					{
						m_lstProductInfo.SetItemData(i,dwNewData);
						m_lstProductInfo.SetCheck(i,FALSE);
						if(!DI_IS_GRAY(dwNewData))
						{
							m_bAllFiles = FALSE;							
						}
					}						
				}
			}

			if(bCheck)
			{			
				m_lstProductInfo.SetCheck(i);	
			}
			else
			{
				if(pFileInfo[i].dwCheckFlag == 1) // must be selected
				{
					m_lstProductInfo.SetCheck(i);
				}
				else
				{					
					m_lstProductInfo.SetCheck(i,FALSE);
					DWORD dwData = m_lstProductInfo.GetItemData(i);
					if(!DI_IS_GRAY(dwData))
					{
						m_bAllFiles = FALSE;							
					}			
				}
			}
		}
		UpdateData(FALSE);
	}
	return TRUE;
}
CString CMainPage::GetXmlFile()
{
    _TCHAR szInstancePath[_MAX_PATH]={0};   
    CFileFind finder;
    CString strXmlFile;
    GetModuleFilePath(NULL,szInstancePath);
    strXmlFile.Format(_T("%s\\config\\BMAConfig.xml"),szInstancePath);	
    if(!finder.FindFile(strXmlFile))
    {
        strXmlFile.Format(_T("%s\\BMAConfig.xml"),szInstancePath);		
    }
    return strXmlFile;
}

BOOL CMainPage::LoadAllProduct(LPCTSTR lpszXMLConfig)
{
	if(lpszXMLConfig == NULL)
		return FALSE;
	
   	if(m_pXmlConfigParse == NULL)
	{
		if(!g_theApp.m_pfCreateXmlConfigParse(&m_pXmlConfigParse))
		{
			return FALSE;
		}
	}
	
	CString strXmlFile = lpszXMLConfig;
    if(strXmlFile.IsEmpty())
		return FALSE;
	if(!m_pXmlConfigParse->Init(strXmlFile))
		return FALSE;
	
	CString strSpecConfig = _T("");
	
	CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
	if(pMF != NULL)
	{
		strSpecConfig = pMF->GetSpecConfigFile();
	}
	
	//if(!m_pXmlConfigParse->Init(strXmlFile))
	//	return FALSE;
	
    if(!strSpecConfig.IsEmpty())
	{
		if(!m_pXmlConfigParse->Init(strSpecConfig,1)) // for special configure
			return FALSE;
	}
	
	m_arrProductName.RemoveAll();
	m_nProductCount = m_pXmlConfigParse->GetProductCount();
	
	if(m_nProductCount == 0)
		return FALSE;
	
	DWORD dwSize = 2000;
	DWORD dwRealSize = 0;
	_TCHAR *psz = new _TCHAR[dwSize];
	memset(psz,0,sizeof(_TCHAR)*dwSize);
	m_pXmlConfigParse->GetProductNameList(psz,dwSize,dwRealSize);
    _TCHAR *pNext = NULL;
	_TCHAR *pCur = psz;
	for(int j = 0;j<m_nProductCount;j++)
	{
		_TCHAR sz[200] = {0};
		pNext = _tcschr(pCur,_T(';'));
		memcpy(sz,pCur,(pNext-pCur)*sizeof(_TCHAR));
		pCur = pNext+1;
		m_arrProductName.Add(sz);
	}
	delete []psz;
	
	
	ClearProdInfoVector();
	m_mapPrdCfg.RemoveAll();
	
	PPRODUCT_INFO_T pProdInfo = NULL;
	PPRODUCT_INFO_T pTmpProdInfo = NULL;
	FILE_INFO_ARR arrFileInfo;

	UINT i=0;
	for(i=0; i<(UINT)m_nProductCount; i++)
	{		
		pTmpProdInfo = NULL;
		pTmpProdInfo = m_pXmlConfigParse->GetProdInfo(m_arrProductName[i]);
		_ASSERTE(pTmpProdInfo != NULL);
		
		pProdInfo = new PRODUCT_INFO_T;
		
		DeepCopyProductInfo(pProdInfo,pTmpProdInfo);
		
		
		m_vctProductInfo.push_back(pProdInfo);
		CString strCfgFile = m_pXmlConfigParse->GetConfigFile(m_arrProductName[i]);
		m_mapPrdCfg.SetAt(m_arrProductName[i],strCfgFile);
	}
	
	return TRUE;
}


int CMainPage::GetProductIndex(LPCTSTR lpcstrProuct)
{
	int nCount = m_arrProductName.GetSize();
	int i=0;
	for(i=0;i<nCount;i++)
	{
		if(m_arrProductName.GetAt(i).Compare(lpcstrProuct) == 0)
			break;
	}
	if(i>=nCount)
		return -1;
	else
		return i;
}

void CMainPage::OnChkAll() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	BOOL bCheck = m_bAllFiles;
	m_bListLock = TRUE;	
	int nProduct = GetProductIndex(m_strTempProduct);
	PPRODUCT_INFO_T pProdInfo = m_vctProductInfo[nProduct];
    PFILE_INFO_T pFileInfo = pProdInfo->pFileInfoArr;
	
	int nCount = m_lstProductInfo.GetItemCount();
	CString strID;
	for(int i =0; i<nCount; i++)
	{
		strID = m_pTmpProductInfo->pFileInfoArr[i].szID;

		DWORD dwItemData = m_lstProductInfo.GetItemData(i);
		if(DI_IS_GRAY(dwItemData))
		{
			continue;
		}
		
		if(pFileInfo[i].dwCheckFlag != 1) // maybe 0, 2
		{
#ifdef _SPUPGRADE	
			if(strID.CompareNoCase(_T("FLASH"))== 0)
			{
				continue;
			}
#endif
			m_lstProductInfo.SetCheck(i,bCheck);
		}		
	}

	m_bListLock = FALSE;
}

void CMainPage::OnDblclkLstProductInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(!m_bCanEdit)
	{
		*pResult = 0;
		return;
	}
	
	LPNMITEMACTIVATE pnia = (LPNMITEMACTIVATE)pNMHDR;
	
	CEdtBtnCtrl *pCtrl = NULL;
	int nItem = pnia->iItem;
	int nSubItem = pnia->iSubItem;
	if(nItem == -1 || nSubItem != 1)
		return;
	if(DI_IS_NEED_FILE(m_lstProductInfo.GetItemData(nItem)) == 0)
		return;
	
	pCtrl = new CEdtBtnCtrl();
	
	if(NULL != pCtrl)
	{
		pCtrl->SetFileFilterString(g_theApp.m_strFileFilter);
		pCtrl->m_nItem = nItem;
		pCtrl->m_nSubItem = nSubItem;
		pCtrl->m_strText = m_lstProductInfo.GetItemText(nItem,nSubItem);
		if(!pCtrl->Create(IDD_EDT_BTN_DLG,FromHandle(m_lstProductInfo.m_hWnd)))
		{
			AfxMessageBox(_T("Error to create EdtBtnCtrl"));
			return;
		}
	}
	else
	{
		return;
	}
	
	CRect rect;
	m_lstProductInfo.GetSubItemRect(nItem,nSubItem,LVIR_BOUNDS,rect);
	CRect rtList;
	m_lstProductInfo.GetClientRect(&rtList);
	if(rect.right>rtList.right)
		rect.right = rtList.right;
	if(rect.left < rtList.left)
		rect.left = rtList.left;
	if(rect.bottom>rtList.bottom)
		rect.bottom = rtList.bottom;
	if(rect.top < rtList.top)
		rect.top = rtList.top;
	
	pCtrl->SetWindowPos(&wndTop,rect.left,rect.top-1,rect.right-rect.left,rect.bottom-rect.top,NULL);
	
    pCtrl->ShowWindow(SW_SHOW);
	
	*pResult = 0;
	
}

void CMainPage::OnEndlabeleditLstProductInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_bListLock = TRUE;
	m_lstProductInfo.SetItemText(pDispInfo->item.iItem,pDispInfo->item.iSubItem,pDispInfo->item.pszText);
	m_bListLock = FALSE;
	if(m_pTmpProductInfo != NULL && m_pTmpProductInfo->dwOmaDMFlag != 0)
	{		
		CString fileID = m_pTmpProductInfo->pFileInfoArr[pDispInfo->item.iItem].szID;
		if(fileID.CompareNoCase(_T("FDL2"))== 0)
		{
			m_bTmpOmaDM= GetFdl2Flag(pDispInfo->item.pszText,FDL2F_OMADM);
			m_bTmpPreload= GetFdl2Flag(pDispInfo->item.pszText,FDL2F_PRELOAD);
			m_bTmpKernelImg2 = GetFdl2Flag(pDispInfo->item.pszText,FDL2F_KERNELIMG2);
			m_bTmpRomDisk = GetFdl2Flag(pDispInfo->item.pszText,FDL2F_ROMDISK);			
			ModifyItemGray();			
		}
	}	
	
	*pResult = 0;
}


void CMainPage::OnItemchangedLstProductInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_bListLock)
		return;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem < 0 || pNMListView->iSubItem != 0 )
		return;
	// TODO: Add your control notification handler code here
	if(pNMListView->uNewState == 0x00001000 ) // uncheck
	{
		DWORD dwItemData = m_lstProductInfo.GetItemData(pNMListView->iItem);
		if(!DI_IS_GRAY(dwItemData))
		{
			m_bAllFiles = FALSE;
		}
	}
	else if(pNMListView->uNewState == 0x00002000) // check
	{
		m_bAllFiles = TRUE;
		int nItem = m_lstProductInfo.GetItemCount();
		int i = 0;
		for(i = 0; i< nItem; i++)
		{
			if(!m_lstProductInfo.GetCheck(i))
			{
				DWORD dwItemData = m_lstProductInfo.GetItemData(i);
				if(!DI_IS_GRAY(dwItemData))
				{
					m_bAllFiles = FALSE;
					break;
				}
			}
		}
	}

	GetDlgItem(IDC_CHK_ALL)->UpdateData(FALSE);
	
	if(m_bCanSel)
	{
		int nProduct = GetProductIndex(m_strTempProduct);
		PPRODUCT_INFO_T pProdInfo = m_vctProductInfo[nProduct];
		PFILE_INFO_T pFileInfo = pProdInfo->pFileInfoArr;
		
		CString strFileID;
		strFileID = pFileInfo[pNMListView->iItem].szID;
		CString strText = m_lstProductInfo.GetItemText(pNMListView->iItem,0);

#ifdef _SPUPGRADE
		if(strFileID.CompareNoCase(_T("FLASH"))== 0 && pNMListView->uNewState == 0x00001000 )
		{
			AfxMessageBox(_T("Must select flash!!!"));
			m_lstProductInfo.SetCheck(pNMListView->iItem);
		}
#endif
		if(pFileInfo[pNMListView->iItem].dwCheckFlag == 1 && pNMListView->uNewState == 0x00001000)
		{
			CString strWarning;
			strWarning.Format(_T("Must select %s!!!"),strText.operator LPCTSTR());
			AfxMessageBox(strWarning);
			m_lstProductInfo.SetCheck(pNMListView->iItem);
		}

		DWORD dwItemData = m_lstProductInfo.GetItemData(pNMListView->iItem);
		if(DI_IS_GRAY(dwItemData))
		{
			m_lstProductInfo.SetCheck(pNMListView->iItem,FALSE);
		}	
	}
	
	
    UpdateData(FALSE);
	*pResult = 0;
}

void CMainPage::InitFlashOptPage(PPRODUCT_INFO_T pCurProduct,LPCTSTR pFileName)
{
	ASSERT(m_pFlashOptPage != NULL);
	ASSERT(pCurProduct != NULL);
	if(m_pFlashOptPage == NULL || pCurProduct == NULL)
	{
		return;
	}
	m_pFlashOptPage->InitReadFlashCfg(pCurProduct,pFileName, m_mapDLFiles);
}

void CMainPage::InitCaliPage(PPRODUCT_INFO_T pCurProduct)
{
    ASSERT(m_pCaliPage != NULL);
	ASSERT(pCurProduct != NULL);
	if(m_pCaliPage == NULL || pCurProduct == NULL)
	{
		return;
	}
	
    m_pCaliPage->m_strCurProduct = m_strCurProduct;
	m_pCaliPage->m_strTempCurProduct = m_strCurProduct;
	
	//	m_pCaliPage->m_lstBackup.DeleteAll();
    m_pCaliPage->m_lstBackup.InitNvBackupInfo(pCurProduct);	
	m_pCaliPage->AddBackupFiles(pCurProduct->pFileInfoArr,pCurProduct->dwFileCount,TRUE);
}

void CMainPage::InitNandPage(PPRODUCT_INFO_T pCurProduct)
{
	if(m_pOptionsPage)
	{
		m_pOptionsPage->m_nFlashPageType = m_bph.dwNandPageType;	
		m_pOptionsPage->m_bRepart = (m_bph.dwNandStrategy == REPAR_STRATEGY_ALWAYS)?TRUE:FALSE;
	}

	if (pCurProduct)
	{
		m_pOptionsPage->m_bXmlPowerOff = pCurProduct->bPowerOff;
	}
}

void CMainPage::InitMultiLangPage(BOOL bUpdateWnd)
{
	CUIntArray agLangFlag;
	int nNVItem = GetFileInfo(_T("NV"),NULL,bUpdateWnd);
	CString strNVFile;
	BOOL bNVCheck = TRUE;
	FILETIME ft;
	memset(&ft,0,sizeof(FILETIME));
	
	if(bUpdateWnd)
	{
		if(nNVItem != -1 && nNVItem < m_lstProductInfo.GetItemCount())
		{
			strNVFile = m_lstProductInfo.GetItemText(nNVItem,1);
			bNVCheck = m_lstProductInfo.GetCheck(nNVItem);
		}
		else
		{
			bNVCheck = FALSE;
			strNVFile.Empty();
		}
	}
	else
	{
		m_mapDLState.Lookup(_T("NV"),bNVCheck);
		if(bNVCheck)
		{
			m_mapDLFiles.Lookup(_T("NV"),strNVFile);
		}	

	}
	if(nNVItem != -1 && !strNVFile.IsEmpty() && bNVCheck
		&& FindLangNVItem(strNVFile,agLangFlag,&ft) )
	{
		m_pMultiLangPage->SetLangFlag(TRUE,agLangFlag,&ft,bUpdateWnd);	
	}
	else
	{	
		m_pMultiLangPage->SetLangFlag(FALSE,agLangFlag,&ft,bUpdateWnd);
	}
}

void CMainPage::InitLCDCfigPage(BOOL bUpdateWnd)
{
	BOOL bPS = TRUE;
	VEC_LCD_CFIG vLCDCfig;
	int nPSItem = GetFileInfo(_T("PS"),NULL,bUpdateWnd);
	if(nPSItem == -1 )
	{
		bPS = FALSE;
		nPSItem = GetFileInfo(_T("UserImg"),NULL,bUpdateWnd);
	}
	CString strPSFile;
	BOOL bPSCheck = TRUE;
	FILETIME ft;
	memset(&ft,0,sizeof(FILETIME));
	
	if(bUpdateWnd)
	{
		if(nPSItem != -1 && nPSItem < m_lstProductInfo.GetItemCount())
		{
			strPSFile = m_lstProductInfo.GetItemText(nPSItem,1);
			bPSCheck = m_lstProductInfo.GetCheck(nPSItem);
		}
		else
		{
			bPSCheck = FALSE;
			strPSFile.Empty();
		}
	}
	else
	{
		if(nPSItem == -1)
		{
			bPSCheck = FALSE;
			strPSFile.Empty();
		}
		else
		{
			m_mapDLState.Lookup(bPS?_T("PS"):_T("USERIMG"),bPSCheck);
			if(bPSCheck)
			{
				m_mapDLFiles.Lookup(bPS?_T("PS"):_T("USERIMG"),strPSFile);
			}
		}

	}
	if(nPSItem != -1 && !strPSFile.IsEmpty() && bPSCheck
		&& FindLCDItem(strPSFile,vLCDCfig,&ft) )
	{
		m_pLCDCfigPage->SetLCDCfig(TRUE,vLCDCfig,&ft,bUpdateWnd,strPSFile);	
	}
	else
	{	
		m_pLCDCfigPage->SetLCDCfig(FALSE,vLCDCfig,&ft,bUpdateWnd,strPSFile);
	}
}

void CMainPage::InitVolFreqPage(BOOL bUpdateWnd)
{
	//BOOL bPS = TRUE;
	VEC_LCD_CFIG vLCDCfig;
	
	int nSPLItem = GetFileInfo(_T("SPLLoader"),NULL,bUpdateWnd);

	CString strSPLFile;
	BOOL bSPLCheck = TRUE;	

	if(nSPLItem != -1 )
	{
		if(bUpdateWnd )
		{
			if(nSPLItem < m_lstProductInfo.GetItemCount())
			{
				bSPLCheck = m_lstProductInfo.GetCheck(nSPLItem);
				if(bSPLCheck)
				{
					strSPLFile = m_lstProductInfo.GetItemText(nSPLItem,1);
				}
			}	
			else
			{
				bSPLCheck = FALSE;
				strSPLFile.Empty();
			}
		}
		else
		{
			CString strID = _T("SPLLoader");
			strID.MakeUpper();
			m_mapDLState.Lookup(strID,bSPLCheck);
			if(bSPLCheck)
			{
				m_mapDLFiles.Lookup(strID,strSPLFile);
			}
		}		
	}
	else
	{
		bSPLCheck = FALSE;
		strSPLFile.Empty();
	}

	m_pVolFreqPage->SetConfig(bUpdateWnd,strSPLFile);

}

void CMainPage::InitUartPortSwitchPage(BOOL bUpdateWnd)
{
    int nUbootItem = GetFileInfo(_T("UBOOTLoader"),NULL,bUpdateWnd);

    CString strUbootFile;
    BOOL bUbootCheck = TRUE;	

    if(nUbootItem != -1 )
    {
        if(bUpdateWnd )
        {
            if(nUbootItem < m_lstProductInfo.GetItemCount())
            {
                bUbootCheck = m_lstProductInfo.GetCheck(nUbootItem);
                if(bUbootCheck)
                {
                    strUbootFile = m_lstProductInfo.GetItemText(nUbootItem,1);
                }
            }	
            else
            {
                bUbootCheck = FALSE;
                strUbootFile.Empty();
            }
        }
        else
        {
            CString strID = _T("UBOOTLoader");
            strID.MakeUpper();
            m_mapDLState.Lookup(strID,bUbootCheck);
            if(bUbootCheck)
            {
                m_mapDLFiles.Lookup(strID,strUbootFile);
            }
        }	
        m_pUartPortSwitchPage->SetConfig(bUpdateWnd,strUbootFile);
    }
    else
    {
        bUbootCheck = FALSE;
        strUbootFile.Empty();
    }
}

void CMainPage::GetLastModifyTime(LPCTSTR lpszFile,FILETIME& ft)
{
    ZeroMemory(&ft,sizeof(ft));
    if (lpszFile && _tcslen(lpszFile))
    {
        WIN32_FIND_DATA fd={0};
        HANDLE hFind = NULL;
        hFind = FindFirstFile(lpszFile,&fd);
        if (hFind)
        {
            FindClose(hFind);
            ft = fd.ftLastWriteTime;
        } 
    }
}

BOOL CMainPage::LoadPacket(LPCTSTR lpszPacketName)
{
	_ASSERTE(lpszPacketName != NULL);
	if(lpszPacketName == NULL)
	{
		return FALSE;
	}

	CWaitCursor wc;
	
    CMainFrame * pMF= (CMainFrame *)AfxGetMainWnd();
	pMF->SetPrdVersion(_T(""));
	
	pMF->SetStatusBarText(_T("Loading packet file..."));

	ZeroMemory(&m_ftPacOrg,sizeof(m_ftPacOrg));
	GetLastModifyTime(lpszPacketName,m_ftPacOrg);
	
    IBinPack* pBinPack = NULL;
    if (!CreateSecPacParse(&pBinPack))
    {
        pMF->SetStatusBarText(_T("Ready"));
        return FALSE;
    }
    //pBinPack->SetSafeKey(DEFAULE_SAFE_PAC_KEY); 
    pBinPack->SetReceiver((DWORD)(pMF->m_hWnd),WM_BINPAC_PROG_MSG,FALSE);
    
	if(!pBinPack->Open(lpszPacketName,TRUE))
	{
		pMF->SetStatusBarText(_T("Ready"));
		pBinPack->Close();
        pBinPack->Release();
		return FALSE;
	}
	
	BIN_PACKET_HEADER_T bph;
	
	FILE_T *paFile = NULL;
    TCHAR  szPacXmlFile[MAX_PATH] = {0};
    TCHAR  szReleaseTmpDir[MAX_PATH] = {0};
    UINT nRet = 0;

	((CMainFrame *)AfxGetMainWnd())->m_wndToolBar.EnableWindow(FALSE);	
#if defined(_FACTORY)
    nRet = pBinPack->Unpacket(((CMainFrame *)AfxGetMainWnd())->m_strPacDirBase,bph,&paFile,TRUE);
#else
    nRet = pBinPack->Unpacket(((CMainFrame *)AfxGetMainWnd())->m_strPacDirBase,bph,&paFile,FALSE);
#endif

	((CMainFrame *)AfxGetMainWnd())->m_wndToolBar.EnableWindow(TRUE);
    pBinPack->GetReleaseDir(szReleaseTmpDir,_countof(szReleaseTmpDir)); 
    pBinPack->GetConfigFilePath(szPacXmlFile,_countof(szPacXmlFile));
    ((CMainFrame *)AfxGetMainWnd())->m_aPacReleaseDir.Add(szReleaseTmpDir);
    ((CMainFrame *)AfxGetMainWnd())->m_strSpecConfig = szPacXmlFile;
	pBinPack->Close();
	if( nRet || NULL == paFile)
	{
        if (nRet)
        {
            TCHAR  szErrorDes[MAX_PATH] = {0};
            pBinPack->GetErrString(nRet,szErrorDes,MAX_PATH); 

			if ( g_theApp.m_bCMDFlash )
			{
				CString strInfo;
				strInfo.Format(_T("%s fail:%s"),EZ_MODE_STATUS_FLAG,szErrorDes);
				g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
			}
			else
			{
				g_theApp.MessageBox(szErrorDes);
			}
        }
		pMF->SetStatusBarText(_T("Ready"));
        pBinPack->ReleaseMem(paFile);
        pBinPack->Release();
		return FALSE;
    }
	m_arrProductName.RemoveAll();
	
	m_mapDLFiles.RemoveAll();
	m_mapDLState.RemoveAll();
	m_mapDLSize.RemoveAll();
	m_mapDataInfo.RemoveAll();
	m_mapLastTime.RemoveAll();
	
	
	m_strCurProduct = bph.szPrdName;
	m_arrProductName.Add(m_strCurProduct);
	m_nProductCount = 1;	
    m_strPrdVersion = bph.szPrdVersion;
	//wei.song 20151020
	if (g_theApp.m_bSprdMESEnable)
	{
		USES_CONVERSION;
		if (NULL ==strstr(W2A(bph.szPrdVersion), g_theApp.m_BatchInfo.szSWVer))
		{
			pBinPack->ReleaseMem(paFile);
            pBinPack->Release();
			AfxMessageBox(_T("PAC mismatch"));
			return FALSE;
		}
	}
	
	CString strAlias= bph.szPrdAlias;
	
	FILE_T * pFT = NULL;	
	
	BOOL bNvFileSelected = FALSE;
	BOOL bFlashSelected = FALSE;
	BOOL bPhaseCheckSelected = FALSE;
	BOOL bExistPhaseCheck = FALSE;
	BOOL bExistFlash = FALSE;
	
	int nAllowOmit = 0;
	int nRealFileCount = 0;
	
	CStringArray agNVID;

	CString strFileEmptyIDs = _T("");
	CString strUncheckFileIDs = _T("");
    CString strCuFile = _T("");
	
	for(int i = 0; i< bph.nFileCount; i++)
	{
		pFT = paFile + i;
		CString strID = pFT->szFileID;
		CString strFileName = GetStrFileTitle(pFT->szFileName,TRUE,TRUE);
		strID.MakeUpper();

        FILETIME ft;
		LARGE_INTEGER liTmpSize;
		LARGE_INTEGER liTmpOffset;
		liTmpSize.HighPart = pFT->dwHiFileSize;
		liTmpSize.LowPart = pFT->dwLoFileSize;

		liTmpOffset.HighPart = pFT->dwHiDataOffset;
		liTmpOffset.LowPart = pFT->dwLoDataOffset;

        GetLastModifyTime(pFT->szFileName,ft);
		m_mapDLFiles.SetAt(strID,pFT->szFileName);
		m_mapDLState.SetAt(strID,pFT->nCheckFlag);
		m_mapDLSize.SetAt(strID,liTmpSize.QuadPart);
		if (!strFileName.IsEmpty())
		{
			DATA_INFO_T dataInfo;
			dataInfo.llSize		= liTmpSize.QuadPart;
			dataInfo.llOffset	= liTmpOffset.QuadPart;
			m_mapDataInfo.SetAt(strFileName,dataInfo);
			m_mapLastTime.SetAt(strFileName,ft);
		}
		
		if(pFT->dwCanOmitFlag == 1)
		{
			nAllowOmit++;
		}

		if(strID.Find(_T("NV")) == 0)
		{
			agNVID.Add(strID);
		}
		else if(strID.CompareNoCase(_T("PhaseCheck")) == 0)
		{
			bExistPhaseCheck = TRUE;
		}
		else if(strID.Find(_T("FLASH")) == 0)
		{
			bExistFlash = TRUE;
		}
        else if(strID.Find(_T("CUREF")) == 0)
        {
            strCuFile = pFT->szFileName;
        }

		if(pFT->nCheckFlag && (_tcslen(pFT->szFileName) != 0 || !DI_IS_NEED_FILE(pFT->nFileFlag) ))
		{
			nRealFileCount++;
			
			if(strID.Find(_T("NV")) == 0)
			{
				bNvFileSelected = TRUE;
			}
			else if(strID.Find(_T("FLASH")) == 0)
			{
				bFlashSelected = TRUE;
			}
			else if(strID.CompareNoCase(_T("PhaseCheck")) == 0)
			{
				bPhaseCheckSelected = TRUE;
			}
			else if(strID.CompareNoCase(_T("FDL2")) == 0)
			{		
				m_bOmaDM = GetFdl2Flag(pFT->szFileName,FDL2F_OMADM);
				m_bPreload = GetFdl2Flag(pFT->szFileName,FDL2F_PRELOAD);
				m_bKernelImg2 = GetFdl2Flag(pFT->szFileName,FDL2F_KERNELIMG2);
				m_bRomDisk = GetFdl2Flag(pFT->szFileName,FDL2F_ROMDISK);
			}
			
			if(pFT->dwCanOmitFlag == 1)
			{
				nAllowOmit--;
			}			
		}
		else
		{
			m_mapDLFiles.SetAt(strID,_T(""));	

			if( pFT->dwCanOmitFlag == 0 )
			{
				if(!strFileEmptyIDs.IsEmpty())
				{
					strFileEmptyIDs += _T(",");
				}
				strFileEmptyIDs += strID;

				if(!pFT->nCheckFlag)
				{
					if(!strUncheckFileIDs.IsEmpty())
					{
						strUncheckFileIDs += _T(",");
					}
					strUncheckFileIDs += strID;
				}
			}
				 
		}
	}
	
    pBinPack->ReleaseMem(paFile);
	
	BOOL bAllFileDown = FALSE;
	
	if( (bph.nFileCount - nRealFileCount) == nAllowOmit )
	{
		bAllFileDown = TRUE;
	}	

    if(pMF->IsSupportCU())
    {
        if(strCuFile.IsEmpty())
        {
            AfxMessageBox(_T("There is no curef.bin file, please check the pac."));
			pMF->SetStatusBarText(_T("Ready"));
			pBinPack->Release();
			return FALSE;
        }
        else
        {
            ParseCuRef(strCuFile);
        }
    }
	
	BOOL bRepartition = FALSE;
	if (bph.dwNandStrategy == REPAR_STRATEGY_ALWAYS || bph.dwNandStrategy ==REPAR_STRATEGY_DO)
		bRepartition = TRUE;
	
#if defined(_SPUPGRADE) || defined(_FACTORY) 
	BOOL bNand = (BOOL)(bph.dwFlashType);
#endif
	
	if(bExistFlash && !bFlashSelected)
	{
		if ( g_theApp.m_bCMDFlash )
		{
			CString strInfo;
			strInfo.Format(_T("%s fail:Load Pac Error<have not selected to earase running-nv>"),EZ_MODE_STATUS_FLAG);
			g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
		}
		else
		{
			AfxMessageBox(_T("Configure dangerouse: have not selected to earase running-nv.\nPlease select right packet!"));
		}
		
		pMF->SetStatusBarText(_T("Ready"));
        pBinPack->Release();
		return FALSE;
	}
	
#if defined(_SPUPGRADE)
	BOOL bBackupNV = (BOOL)(bph.dwIsNvBackup);
	
	//if(!bNvFileSelected)
	//{
	//	if ( g_theApp.m_bCMDFlash )
	//	{
	//		CString strInfo;
	//		strInfo.Format(_T("%s fail:Load Pac Error<have not selected NV file to dwonload>"),EZ_MODE_STATUS_FLAG);
	//		g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
	//	}
	//	else
	//	{
	//		AfxMessageBox(_T("Configure dangerouse: have not selected NV file to dwonload.\nPlease select right packet!"));
	//	}
	//	
	//	pMF->SetStatusBarText(_T("Ready"));
 //       pBinPack->Release();
	//	return FALSE;
	//}
	
	/* bNvFileSelected = TRUE */
	if( !bBackupNV && bNvFileSelected)
	{
		if ( g_theApp.m_bCMDFlash )
		{
			CString strInfo;
			strInfo.Format(_T("%s fail:Load Pac Error<have not selected any NV item to backup>"),EZ_MODE_STATUS_FLAG);
			g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
		}
		else
		{
			AfxMessageBox(_T("Configure dangerouse: have not selected any NV item to backup.\nPlease select right packet!"));
		}
		
		pMF->SetStatusBarText(_T("Ready"));
        pBinPack->Release();
		return FALSE;
	}
	
	
	if(!bAllFileDown)
	{
		if(bNand && bRepartition)
		{
			if ( g_theApp.m_bCMDFlash )
			{
				CString strInfo;
				strInfo.Format(_T("%s fail:Load Pac Error<Not selected all files to dwonload>"),EZ_MODE_STATUS_FLAG);
				g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
			}
			else
			{
				CString strMsg;
				strMsg.Format(_T("Configure dangerouse: have selected repartition,\nbut not selected all files to download.\n[%s]\nPlease select right packet!"),strUncheckFileIDs);
				AfxMessageBox(strMsg);
			}
			
			pMF->SetStatusBarText(_T("Ready"));
            pBinPack->Release();
			return FALSE;
		}
		else
		{
			// allow to download part files. but prompt the warning.
			if ( g_theApp.m_bCMDFlash )
			{
				CString strInfo;
				strInfo.Format(_T("%s fail:Load Pac Error<have not selected all files to dwonload>"),EZ_MODE_STATUS_FLAG);
				g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
			}
			else
			{
				CString strMsg;
				strMsg.Format(_T("Configure dangerouse: have not selected all files to dwonload.\n[%s]"),strUncheckFileIDs);
				AfxMessageBox(strMsg);
			}
			
			wc.Restore();
		}
	}
	else
	{
		if(bNand && bRepartition && !bBackupNV)
		{
			if ( g_theApp.m_bCMDFlash )
			{
				CString strInfo;
				strInfo.Format(_T("%s fail:Load Pac Error<Not selected all files to download>"),EZ_MODE_STATUS_FLAG);
				g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
			}
			else
			{
				CString strMsg;
				strMsg.Format(_T("Configure dangerouse: have selected repartition,\nbut  not selected all files to download.\n[%s]\nPlease select right packet!"),strUncheckFileIDs);
				AfxMessageBox(strMsg);
			}
						
			pMF->SetStatusBarText(_T("Ready"));
            pBinPack->Release();
			return FALSE;
		}
	}
	
#endif	
	
#if defined(_FACTORY)
	/* bph.dwIsNvBackup must be false(0) */
	_ASSERTE(bph.dwIsNvBackup == 0 );
    if (pBinPack->IsExistEmptyFile() && !bAllFileDown)
    {
		if ( g_theApp.m_bCMDFlash )
		{
			CString strInfo;
			strInfo.Format(_T("%s fail:Load Pac Error<%s is empty or not selected>"),EZ_MODE_STATUS_FLAG,strFileEmptyIDs);
			g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
		}
		else
		{
			CString strMsg;
			strMsg.Format(_T("Configure dangerouse: exist empty file in package which should not occur in production download.\n[%s] is empty or not selected.\nPlease select right packet!"),strFileEmptyIDs);
			AfxMessageBox(strMsg);	
		}
		pMF->SetStatusBarText(_T("Ready"));
        pBinPack->Release();
		return FALSE;
    }
	if (bExistPhaseCheck && !bPhaseCheckSelected)
	{
		if ( g_theApp.m_bCMDFlash )
		{
			CString strInfo;
			strInfo.Format(_T("%s fail:Load Pac Error<no PhaseCheck information in package>"),EZ_MODE_STATUS_FLAG);
			g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
		}
		else
		{
			AfxMessageBox(_T("Configure dangerouse: no PhaseCheck information in package which should not occur in production download.\nPlease select right packet!"));
		}
		
		pMF->SetStatusBarText(_T("Ready"));
        pBinPack->Release();
		return FALSE;
	}

	if(!bAllFileDown)
	{
		if(bNand && bRepartition)
		{
			if ( g_theApp.m_bCMDFlash )
			{
				CString strInfo;
				strInfo.Format(_T("%s fail:Load Pac Error<Not selected all files to dwonload>"),EZ_MODE_STATUS_FLAG);
				g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
			}
			else
			{
				CString strMsg;
				strMsg.Format(_T("Configure dangerouse: have selected repartition,\nbut not selected all files to download.\n[%s]\nPlease select right packet!"),strUncheckFileIDs);
				AfxMessageBox(strMsg);
				pMF->SetStatusBarText(_T("Ready"));
				pBinPack->Release();
			}
			
			return FALSE;
		}
		else
		{
			// allow to download part files. but prompt the warning.
			if ( g_theApp.m_bCMDFlash )
			{
				CString strInfo;
				strInfo.Format(_T("%s fail:Load Pac Error<have not selected all files to dwonload>"),EZ_MODE_STATUS_FLAG);
				g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
			}
			else
			{
				CString strMsg;
				strMsg.Format(_T("Configure dangerouse: have not selected all files to dwonload.\n[%s]"),strUncheckFileIDs);
				AfxMessageBox(strMsg);	
				wc.Restore();
			}
			
		}
	}
	
#endif
	
	memcpy(&m_bph,&bph,sizeof(bph));

    double dPacSize = (double)GetFileSizeEx(lpszPacketName)/(1024*1024);
	
	CString strTmp;
	if(strAlias.IsEmpty())
		strTmp.Format(_T("%s : %s (PACKAGE SIZE = %0.3fMB)"),m_strCurProduct.operator LPCTSTR(),m_strPrdVersion.operator LPCTSTR(), dPacSize);	
	else
		strTmp.Format(_T("%s : %s (PACKAGE SIZE = %0.3fMB)"),strAlias.operator LPCTSTR(),m_strPrdVersion.operator LPCTSTR(), dPacSize);	
	
	pMF->SetPrdVersion(strTmp);	


#if defined(_SPUPGRADE)
	_TCHAR szConfigPath[_MAX_PATH]={0};
	if(g_theApp.GetIniFilePath(szConfigPath))
	{		
		if( agNVID.GetSize() > 0 && g_theApp.m_bKeepPacNVState)
		{
			for(int i = 0; i<agNVID.GetSize(); i++)
			{
				_TCHAR szBuf[512] = {0};
				DWORD dwRet = GetPrivateProfileString(_T("NV"),agNVID[i],_T(""),szBuf,512,szConfigPath);
				if(dwRet != 0)
				{
					CString strNVState = szBuf;
					strNVState.TrimLeft();
					strNVState.TrimRight();
					int nFind = strNVState.Find(_T('@'));
					if(nFind != -1)
					{
						CString strState = strNVState.Left(nFind);
						CString strPath = strNVState.Right(strNVState.GetLength()-nFind-1);
						m_mapDLFiles.SetAt(agNVID[i],strPath);
						m_mapDLState.SetAt(agNVID[i],_ttoi(strState.operator LPCTSTR()));
					}
				}
			}			
		}
	}	
#endif

	_TCHAR szPath[_MAX_PATH]={0};
	if(g_theApp.GetIniFilePath(szPath))
	{
		CString strID;
		CString strCheckFile;
		POSITION pos = m_mapDLFiles.GetStartPosition();
		while(pos)
		{
			CString strPath;
			m_mapDLFiles.GetNextAssoc(pos,strID,strPath);
			BOOL bSel = FALSE;
			m_mapDLState.Lookup(strID,bSel);

			strCheckFile.Format(_T("%d@%s"),bSel,strPath.operator LPCTSTR());
			WritePrivateProfileString( m_strCurProduct, strID,strCheckFile, szPath );
		}

		WritePrivateProfileString(g_sz_SELECT,g_sz_SELECT_PRODUCT,m_strCurProduct,szPath);
	}
	
    PUMP_MESSAGES();
	pMF->SetStatusBarText(_T("Ready"));
    pBinPack->Release();
	m_strPacketFile = lpszPacketName;
	return TRUE;
}

//如果错误消息已经用对话框弹出，则不用填充strErrorMsg
BOOL CMainPage::LoadConfig(LPCTSTR pFileName,CString &strErrorMsg)
{
	//ini first
	ASSERT(NULL != pFileName);
    if(NULL == pFileName)
    {
		strErrorMsg += _T("Configure file is empty!\n");
        return FALSE;
    }   
    m_strIniFile = pFileName;
	
	if(!_LoadConfig(strErrorMsg))
	{
		return FALSE;
	}

	InitProdInfo();

	if(CheckInitDLFiles())
	{
		m_bInit = TRUE;
	}
	else
	{
		m_bInit = FALSE;
	}
	
	m_bTmpOmaDM = m_bOmaDM;
	m_bTmpPreload = m_bPreload;
	m_bTmpKernelImg2 = m_bKernelImg2;
	m_bTmpRomDisk = m_bRomDisk;
	
	InitCaliPage(m_pTmpProductInfo);
	InitNandPage(m_pTmpProductInfo);
	InitMultiLangPage();
	InitLCDCfigPage();
#ifdef _RESEARCH
	InitVolFreqPage();
    InitUartPortSwitchPage();
	InitFlashOptPage(m_pTmpProductInfo,pFileName);

#endif
	
#ifdef _SPUPGRADE	
    CString strTmpNotChkItemName = m_pCaliPage->m_lstBackup.GetTmpNotChkItemName();
	if(!strTmpNotChkItemName.IsEmpty())
	{
		CString strNVItemNames = strTmpNotChkItemName;
		strNVItemNames += _T(",");
		strNVItemNames.Replace(_T("Calibration,"),_T(""));
		strNVItemNames.Replace(_T("TD_Calibration,"),_T(""));
		strNVItemNames.Replace(_T("IMEI,"),_T(""));
		strNVItemNames.TrimLeft(_T(","));
		if(strNVItemNames.IsEmpty())
		{
			return TRUE;
		}
		CString strWarn;
		strWarn.Format(_T("Not selected \"%s\" to backup in pac file!\nIf continue?"),strTmpNotChkItemName.operator LPCTSTR());
		if(m_bShowChinese)
		{
			CString strTemp;
			strTemp.Format(_T("\n\n包文件中没有选择 \"%s\" 备份项！\n是否继续？"),strTmpNotChkItemName.operator LPCTSTR());
			strWarn += strTemp;
		}
		if(AfxMessageBox(strWarn,MB_YESNO) == IDNO)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
#endif 
    return TRUE;
}

HBRUSH CMainPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	if(pWnd->GetDlgCtrlID() == IDC_EDT_PRD_VERSION)
	{
		pDC->SetTextColor(RGB(0,128,64));
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

//return FALSE always, this is for user clicking packet button repeatly
BOOL CMainPage::DoPacket() 
{
	CWaitCursor wait;

	if(m_pTmpProductInfo == NULL || m_pOptionsPage == NULL || m_pCaliPage == NULL)
	{
		CMD_PAC_EXIT;
		return FALSE;
	}
	if(g_theApp.m_bCMDPackage)
	{

		CString strInfo;
		strInfo.Format(_T("%s ongoing"),EZ_MODE_STATUS_FLAG);
		g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);

	}

	CSettingsSheet* pParentWnd = NULL;
	pParentWnd = (CSettingsSheet*)GetParent();
	
    //检查是否选择了文件，并且选择的文件应该输入文件路径的，必须已输入文件路径
	BOOL bNvFileSelected = FALSE;
	BOOL bFlashSelected = FALSE;
	BOOL bFlashIDExist = FALSE;
	BOOL bNVIDExist = FALSE;
	
	BOOL bUDiskImgSelected = FALSE;	
	BOOL bUDiskImgIDExist = FALSE;
	CString strUDiskImgFile;
	
	BOOL bDspImgSelected = FALSE;
	BOOL bDspImgIDExist = FALSE;
	CString strDspImgFile;

	int nItemCount = m_lstProductInfo.GetItemCount();
	if(nItemCount == 0)
	{
		AfxMessageBox(_T("Please select files to download!"));
		CMD_PAC_EXIT;
		return FALSE;		
	}

	CString strChipPrex;
	BOOL bMultiFiles = FALSE;
	int nStartFile = 1;
	if(m_pTmpProductInfo->dwFlashType == 1)
	{
		nStartFile = 2;
	}
	if(m_pTmpProductInfo->tChips.bEnable && m_pTmpProductInfo->tChips.dwCount != 0)
	{
		bMultiFiles = TRUE;	
		// get chip prefix
		for(int j = nStartFile; j < m_lstProductInfo.GetItemCount(); j++)
		{		
			if(!m_lstProductInfo.GetCheck(j))
			{
				continue;
			}
			CString strFilePath = m_lstProductInfo.GetItemText(j,1);
			if(strFilePath.IsEmpty())
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

			CString strFileTitle = strFileName;	
			// Chip category
			for(UINT i = 0; i< m_pTmpProductInfo->tChips.dwCount; i++)
			{
				CString strChipName = m_pTmpProductInfo->tChips.pChips[i].szName;
				strChipName += _T("_");
				if(strFileName.Left(strChipName.GetLength()).CompareNoCase(strChipName) == 0)
				{
					strFileTitle = strFileName.Right(strFileName.GetLength() -strChipName.GetLength());
					strChipPrex = strChipName;
					strChipPrex.MakeUpper();
					break;
				}
			}
			break;
		}

		if(strChipPrex.IsEmpty())
		{
			CString strTmp = _T("");
			for(UINT j = 0; j< m_pTmpProductInfo->tChips.dwCount; j++)
			{
				strTmp += m_pTmpProductInfo->tChips.pChips[j].szName;
				strTmp += _T("\n");
			}
			CString strErr;
			strErr.Format(_T("There are no download files matching with chip name prefix:\n%s."),strTmp);
			AfxMessageBox(strErr);	
			CMD_PAC_EXIT;
			return FALSE;	
		}
	}

	CString strTmp = _T("");
	CString strTmp2 = _T("");
    int i = 0;
	int nRealDownFile = 0;
	int nAllowOmit = 0;
	for(i = 0;i<nItemCount;i++)
	{
		CString strFileID = m_pTmpProductInfo->pFileInfoArr[i].szID;
		CString strFileIDUpper = strFileID;
		strFileIDUpper.MakeUpper();
		if(strFileID.Find(_T("FLASH")) == 0)
		{
			bFlashIDExist = TRUE;
		}
		else if(strFileIDUpper.Find(_T("NV")) == 0)
		{
			bNVIDExist = TRUE;
		}
		else if(strFileID.CompareNoCase(_T("UDISK_IMG")) == 0)
		{
			bUDiskImgIDExist = TRUE;
		}
		else if(strFileID.CompareNoCase(_T("DSPCode")) == 0)
		{
			bDspImgIDExist = TRUE;
		}

		DWORD dwItemData = m_lstProductInfo.GetItemData(i);	
		if(m_lstProductInfo.GetCheck(i) )
		{
			if(LOWORD(dwItemData)!=0)
			{
				CString strDLFile = m_lstProductInfo.GetItemText(i,1);
				
				if(!CheckFileValidate(strDLFile))
				{
					strTmp += strFileID;
					strTmp += _T(": ");
					strTmp += strDLFile;
					strTmp += _T("\n");						
				}
				if(bMultiFiles && i>= nStartFile && !CheckFileChipPrefix(strDLFile,strChipPrex))
				{
					strTmp2 += strFileID;
					strTmp2 += _T(": ");
					strTmp2 += strDLFile;
					strTmp2 += _T("\n");						
				}
			
				//检查是否选择了NV文件				
				if(strFileIDUpper.Find(_T("NV")) == 0)
				{
					bNvFileSelected = TRUE;
				}
				else if(strFileID.CompareNoCase(_T("UDISK_IMG")) == 0)
				{
					bUDiskImgSelected = TRUE;
					strUDiskImgFile = m_lstProductInfo.GetItemText(i,1);
				}
				else if(strFileID.CompareNoCase(_T("DSPCode")) == 0)
				{
					bDspImgSelected = TRUE;
					strDspImgFile = m_lstProductInfo.GetItemText(i,1);
				}					
			}
			else
			{
				//检查是否选择了擦除Flash
				if(strFileID.Find(_T("FLASH")) == 0)
				{
					bFlashSelected = TRUE;
				}		
			}

			nRealDownFile++;			
		}
		else
		{
			if(DI_IS_GRAY(dwItemData) || DI_IS_OMIT(dwItemData))
			{
				nAllowOmit++;
			}
		}
	
	}
	if(!strTmp.IsEmpty())
	{
		CString strError;
		strError.Format(IDS_ERR_FILES_NOT_EXIST,strTmp.operator LPCTSTR());
		AfxMessageBox(strError);
		CMD_PAC_EXIT;
		return FALSE;
	}

	if(bMultiFiles && !strTmp2.IsEmpty())
	{
		CString strError;
		strError.Format(_T("Following files not match the chip prefix \"%s\":\n%s"),strChipPrex.operator LPCTSTR(),strTmp2.operator LPCTSTR());
		AfxMessageBox(strError);
		CMD_PAC_EXIT;
		return FALSE;
	}

	CStringArray agMultiFiles;
	int nMultiFiles = 0;
	CString strChipPreErr;
	if(bMultiFiles)
	{
		nMultiFiles = GetMultiFiles(agMultiFiles,strChipPreErr);
		if(!strChipPreErr.IsEmpty())
		{
			CString strError;
			strError.Format(_T("Follow files not found:\n%s"),strChipPreErr);
			AfxMessageBox(strError);
			CMD_PAC_EXIT;
			return FALSE;
		}
	}

	CStringArray agPageBlockFiles;
	if(pParentWnd->IsMapPBFileBuf())
	{
		if(GetPageBlockFiles(agPageBlockFiles) == -1)
		{
			CMD_PAC_EXIT;
			return FALSE;
		}
	}

	BOOL bCaseOK = TRUE;

	CString strWarning = _T("There are warnings:\n");


	CStringArray agMultiNVFiles;
	if(m_pTmpProductInfo->tRfChips.bEnable && m_pTmpProductInfo->tRfChips.dwCount != 0)
	{
		CString strTmpWarning;
		if(GetMultiNVFiles(agMultiNVFiles,strTmpWarning) == -1)
		{
			CMD_PAC_EXIT;
			return FALSE;
		}
		if (!strTmpWarning.IsEmpty())
		{
			bCaseOK = FALSE;
			strWarning += strTmpWarning;	
		}
	}

    BOOL bAllFileDown = TRUE;
	if( nAllowOmit != ( nItemCount - nRealDownFile) )
	{
		bAllFileDown = FALSE;
	}

    //检查NandFlash分区策略
	BOOL bNand = m_pTmpProductInfo->dwFlashType;	

	int nRepStrategy = m_pOptionsPage->GetGUINandRepartStrategy();

	BOOL bRepartition = FALSE;
	if(nRepStrategy == REPAR_STRATEGY_ALWAYS || nRepStrategy == REPAR_STRATEGY_DO)
	{
		bRepartition = TRUE;
	}
	
	
	//not select flash
	if(bFlashIDExist && !bFlashSelected)
	{
		bCaseOK = FALSE;
		strWarning += _T("Not selected FLASH to earase flash;\n");	
	}

	//not select NV
	if(bNVIDExist && !bNvFileSelected)
	{
		bCaseOK = FALSE;
		strWarning += _T("Not selected NV file;\n");	
	}

    //检查选择了备份NV
	m_pCaliPage->m_lstBackup.UpdateTemp();
	BOOL bBackupNV = TRUE;
	bBackupNV = m_pCaliPage->m_lstBackup.m_bTempBackNV;
	//如果选择了NV文件，但没有选择备份任何NV项
	/*if(bNvFileSelected)
	{
        CString strTmpNotChkItemName = m_pCaliPage->m_lstBackup.GetTmpNotChkItemName();
		if(!bBackupNV)
		{
			bCaseOK = FALSE;
			strWarning += _T("Not selected any NV item to backup;\n");	
		}
		else if(!strTmpNotChkItemName.IsEmpty())
		{
			bCaseOK = FALSE;
			CString strWarn;
			strWarn.Format(_T("Not selected \"%s\" to backup;\n"),strTmpNotChkItemName.operator LPCTSTR());
			strWarning += strWarn;
		}
	}*/	

	//如果没有选择所有文件
	if(!bAllFileDown)
	{
		bCaseOK = FALSE;
		strWarning += _T("Not selected all files;\n");
	}
	
	if(bNand && bRepartition && !bAllFileDown)
	{
		bCaseOK = FALSE;
		strWarning += _T("Selected repartition,but not select all files to download;\n");	
	}
	
	if(bNand && bRepartition && !bBackupNV && bNvFileSelected)
	{
		bCaseOK = FALSE;
		strWarning += _T("Selected repartition,but not any NV item to backup;\n");
	}

	if(bNand && bRepartition && bBackupNV && !bNvFileSelected && bNVIDExist)
	{
		bCaseOK = FALSE;
		strWarning += _T("Selected backup nv items,but not input the NV file;\n");	
	}

	if(!bCaseOK && !g_theApp.m_bCMDPackage)
	{
		strWarning += _T("\nIf continue?");
		if(AfxMessageBox(strWarning.operator LPCTSTR(),MB_YESNO | MB_ICONWARNING) == IDNO)
		{
			CMD_PAC_EXIT;
			return FALSE;
		}
	}

	CString strPrdName = m_strTempProduct;
	CString strPrdAlias = m_strTempProduct;
	CString strPacPath = g_theApp.m_strDoPacPath;
	CString strPrdVersion = g_theApp.m_strPrdVersion;
	if (g_theApp.m_bCMDFlash)
	{
		strPrdVersion = m_strPrdVersion;
	}
    BOOL    bEncryptPack = FALSE;
	BOOL	bExistFlagFile = FALSE;
	if(strPrdName.Find(_T("PAC_")) == 0)
	{
		strPrdName.Delete(0,4);
	}

	if(!g_theApp.m_bCMDPackage)
	{		
		CDlgPacketSetting dlg;
		dlg.m_strPrdName = strPrdName;
		dlg.m_strPrdAlias = strPrdName;

		if(dlg.DoModal() == IDCANCEL)
			return FALSE;

        PUMP_MESSAGES();

		strPrdName = dlg.m_strPrdName;
		strPrdAlias = dlg.m_strPrdAlias;
		strPacPath = dlg.m_strPath;
		strPrdVersion = dlg.m_strVersion;
	}

	int nFileNum = nItemCount;
	
	CStringArray agUDiskFiles;
	int nUDiskFiles = 0;
	if(bUDiskImgSelected)
	{
		nUDiskFiles = GetUDiskFiles(strUDiskImgFile,agUDiskFiles);
		nFileNum += nUDiskFiles;
	}

	CStringArray agDspFiles;
	int nDspFiles = 0;
	if(bDspImgSelected)
	{
		nDspFiles = GetDspFiles(strDspImgFile,agDspFiles);
		if(nDspFiles == -1)
		{
			CMD_PAC_EXIT;
			return FALSE;
		}

		nFileNum += nDspFiles;
	}

	nFileNum += nMultiFiles;
	nFileNum += agPageBlockFiles.GetSize();
	nFileNum += agMultiNVFiles.GetSize();
	

	int nCurIndex = 0;
	FILE_T * paFile = new FILE_T[nFileNum];
	FILE_T * pFT = NULL;
	for(i=0;i<nItemCount;i++)
	{		
        pFT = paFile + i;
		pFT->nCheckFlag = m_lstProductInfo.GetCheck(i);
		CString strFileName = m_lstProductInfo.GetItemText(i,1);
		CString strFileID = m_pTmpProductInfo->pFileInfoArr[i].szID;
		_tcscpy(pFT->szFileID,strFileID);
		if(pFT->nCheckFlag)
		{
			_tcscpy(pFT->szFileName,strFileName);
			if (IsLoadFromPac(pFT->szFileName))
			{
				DATA_INFO_T dataInfo = GetDataInfo(pFT->szFileName);
				bExistFlagFile	  = TRUE;
				pFT->dwHiDataOffset = (dataInfo.llOffset>>32) & 0xFFFFFFFF;
				pFT->dwLoDataOffset = dataInfo.llOffset & 0xFFFFFFFF;
				pFT->dwHiFileSize = (dataInfo.llSize>>32) & 0xFFFFFFFF;
				pFT->dwLoFileSize = dataInfo.llSize & 0xFFFFFFFF;
			}
		}		
		DWORD dwItemData = m_lstProductInfo.GetItemData(i);
		pFT->nFileFlag = LOWORD(dwItemData);
		if(DI_IS_GRAY(dwItemData) || DI_IS_OMIT(dwItemData))
		{
			pFT->dwCanOmitFlag = 1;
		}
		
		pFT->dwAddrNum = m_pTmpProductInfo->pFileInfoArr[i].dwBlockCount;
		for(UINT k = 0; k< pFT->dwAddrNum; k++)
		{
			pFT->dwAddr[k] = (DWORD)(m_pTmpProductInfo->pFileInfoArr[i].arrBlock[k].llBase);
		}

		nCurIndex ++;

	}

	for(i=0;i<nMultiFiles;i++)
	{
		pFT = paFile + nCurIndex;
		pFT->nCheckFlag = 1;
		_tcscpy(pFT->szFileName,agMultiFiles.GetAt(i));
		if (IsLoadFromPac(pFT->szFileName))
		{	
			DATA_INFO_T dataInfo = GetDataInfo(pFT->szFileName);
			bExistFlagFile	  = TRUE;
			pFT->dwHiDataOffset = (dataInfo.llOffset>>32) & 0xFFFFFFFF;
			pFT->dwLoDataOffset = dataInfo.llOffset & 0xFFFFFFFF;
			pFT->dwHiFileSize = (dataInfo.llSize>>32) & 0xFFFFFFFF;
			pFT->dwLoFileSize = dataInfo.llSize & 0xFFFFFFFF;
		}
		nCurIndex++;
	}

	for(i=0;i<nUDiskFiles;i++)
	{
		pFT = paFile + nCurIndex;
		pFT->nCheckFlag = 1;
		_tcscpy(pFT->szFileName,agUDiskFiles.GetAt(i));
		if (IsLoadFromPac(pFT->szFileName))
		{
			DATA_INFO_T dataInfo = GetDataInfo(pFT->szFileName);
			bExistFlagFile	  = TRUE;
			pFT->dwHiDataOffset = (dataInfo.llOffset>>32) & 0xFFFFFFFF;
			pFT->dwLoDataOffset = dataInfo.llOffset & 0xFFFFFFFF;
			pFT->dwHiFileSize = (dataInfo.llSize>>32) & 0xFFFFFFFF;
			pFT->dwLoFileSize = dataInfo.llSize & 0xFFFFFFFF;
		}
		nCurIndex++;
	}
	for(i=0;i<nDspFiles;i++)
	{
		pFT = paFile + nCurIndex;
		pFT->nCheckFlag = 1;
		_tcscpy(pFT->szFileName,agDspFiles.GetAt(i));
		if (IsLoadFromPac(pFT->szFileName))
		{
			DATA_INFO_T dataInfo = GetDataInfo(pFT->szFileName);
			bExistFlagFile	  = TRUE;
			pFT->dwHiDataOffset = (dataInfo.llOffset>>32) & 0xFFFFFFFF;
			pFT->dwLoDataOffset = dataInfo.llOffset & 0xFFFFFFFF;
			pFT->dwHiFileSize = (dataInfo.llSize>>32) & 0xFFFFFFFF;
			pFT->dwLoFileSize = dataInfo.llSize & 0xFFFFFFFF;
		}
		nCurIndex++;
	}

	for(i=0;i<agPageBlockFiles.GetSize();i++)
	{
		pFT = paFile + nCurIndex;
		pFT->nCheckFlag = 1;
		_tcscpy(pFT->szFileName,agPageBlockFiles.GetAt(i));

		if (IsLoadFromPac(pFT->szFileName))
		{
			DATA_INFO_T dataInfo = GetDataInfo(pFT->szFileName);
			bExistFlagFile	  = TRUE;
			pFT->dwHiDataOffset = (dataInfo.llOffset>>32) & 0xFFFFFFFF;
			pFT->dwLoDataOffset = dataInfo.llOffset & 0xFFFFFFFF;
			pFT->dwHiFileSize = (dataInfo.llSize>>32) & 0xFFFFFFFF;
			pFT->dwLoFileSize = dataInfo.llSize & 0xFFFFFFFF;
		}
		nCurIndex++;
	}

	for(i=0;i<agMultiNVFiles.GetSize();i++)
	{
		pFT = paFile + nCurIndex;
		pFT->nCheckFlag = 1;
		_tcscpy(pFT->szFileName,agMultiNVFiles.GetAt(i));

		if (IsLoadFromPac(pFT->szFileName))
		{
			DATA_INFO_T dataInfo = GetDataInfo(pFT->szFileName);
			bExistFlagFile	  = TRUE;
			pFT->dwHiDataOffset = (dataInfo.llOffset>>32) & 0xFFFFFFFF;
			pFT->dwLoDataOffset = dataInfo.llOffset & 0xFFFFFFFF;
			pFT->dwHiFileSize = (dataInfo.llSize>>32) & 0xFFFFFFFF;
			pFT->dwLoFileSize = dataInfo.llSize & 0xFFFFFFFF;
		}
		nCurIndex++;
	}

	if (bExistFlagFile && 0 == m_strPacketFile.CompareNoCase(strPacPath))
	{
		CString strErr;
		strErr.Format(_T("Don't allow to replace the original pac file [%s]"),m_strPacketFile);
		AfxMessageBox(strErr);
		SAFE_DELETE_ARRAY(paFile);
		CMD_PAC_EXIT;
		return FALSE;
	}

    IBinPack* pBinPack = NULL;
    if (!CreateSecPacParse(&pBinPack))
    {
        SAFE_DELETE_ARRAY(paFile);
        CMD_PAC_EXIT;
        return FALSE;
    }
    //pBinPack->SetSafeKey(DEFAULE_SAFE_PAC_KEY);
	if (g_theApp.m_bCMDPackage)
	{
		::DeleteFile(strPacPath.operator LPCTSTR());
	}
	if(!pBinPack->Open(strPacPath))
	{
		SAFE_DELETE_ARRAY(paFile);
		CMD_PAC_EXIT;
        pBinPack->Release();
		return FALSE;
	}
	CString strCfgFile;
	m_mapPrdCfg.Lookup(m_strTempProduct,strCfgFile);

	BIN_PACKET_HEADER_T bph;
	_tcscpy(bph.szPrdName,strPrdName);
    _tcscpy(bph.szPrdVersion,strPrdVersion);

	if(g_theApp.m_bCMDPackage && strPrdAlias.Find(_T("PAC_")) == 0)
	{
		strPrdAlias.Delete(0,4);
	}
	strPrdAlias.TrimLeft();
	strPrdAlias.TrimRight();
	if(!strPrdAlias.IsEmpty())
	{
		_tcscpy(bph.szPrdAlias,strPrdAlias);
	}

	bph.nFileCount = nFileNum;
	bph.dwFlashType = m_pTmpProductInfo->dwFlashType;
	bph.dwMode = m_pTmpProductInfo->dwMode;
	bph.dwNandStrategy = m_pOptionsPage->GetGUINandRepartStrategy();
	bph.dwNandPageType = m_pOptionsPage->GetGUIFlashPageType();

	BOOL bBackupNv = m_pCaliPage->m_lstBackup.m_bTempBackNV;
	//int  nNBICount = m_pCaliPage->m_lstBackup.m_nTempNvBkpItmCount;

	bph.dwIsNvBackup = (DWORD)bBackupNv;

	bph.dwOmaDmProductFlag = m_pTmpProductInfo->dwOmaDMFlag;
	bph.dwIsOmaDM = m_bTmpOmaDM;
	bph.dwIsPreload = m_bTmpPreload;

	_X_NV_BACKUP_ITEM_T *pnvi = NULL;
    /*
    int  nNBICount = m_pCaliPage->m_lstBackup.m_nTempNvBkpItmCount;
	if(nNBICount != 0)
	{
		pnvi = new _X_NV_BACKUP_ITEM_T[m_pCaliPage->m_lstBackup.m_nTempNvBkpItmCount];

	    memcpy(pnvi,m_pCaliPage->m_lstBackup.m_pTempNvBkpItmArray,sizeof(_X_NV_BACKUP_ITEM_T)*nNBICount);
	}
	
	BOOL bOK = bp.Packet(&bph,strCfgFile,paFile,bBackupNV,pnvi,nNBICount,0);
    */
    UINT nRet = pBinPack->Packet(&bph,strCfgFile,paFile,m_strPacketFile.operator LPCTSTR(),bEncryptPack);
    TCHAR  szErrorDes[MAX_PATH] = {0};
    if (nRet)
    {
         pBinPack->GetErrString(nRet,szErrorDes,MAX_PATH);
    }
	pBinPack->Close();    

	SAFE_DELETE_ARRAY(pnvi);
	SAFE_DELETE_ARRAY(paFile);
	

    pBinPack->Release();
	if(g_theApp.m_bCMDPackage)
	{
		
		CString strInfo;
		if (nRet)
		{
			strInfo.Format(_T("%s fail"),EZ_MODE_STATUS_FLAG);
		}
		else
		{
			strInfo.Format(_T("%s pass"),EZ_MODE_STATUS_FLAG);
		}
		g_theApp.ReportToPipe(strInfo.operator LPCTSTR(),FALSE);
		GetParent()->PostMessage(WM_COMMAND, IDCANCEL);
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return FALSE;
	}
	else
	{
		if(nRet)
        {
            CString strErr;
            strErr.Format(_T("Packeting failed! %s"),szErrorDes);
			AfxMessageBox(strErr);
        }
		else
        {
            AfxMessageBox(_T("Packeting complete!"));
        }
	}

	return FALSE;
}
BOOL CMainPage::DeepCopyProductInfo( PPRODUCT_INFO_T pDst, const PPRODUCT_INFO_T pSrc)
{
	_ASSERTE(pDst != NULL && pSrc != NULL);
	if(pDst == NULL || pSrc == NULL)
	{
		return FALSE;
	}

	BOOL bOK =  pDst->DeepCopy(pSrc);

	return bOK;
/*lint -save -e429*/
}/*lint -restore*/
void CMainPage::ClearProdInfoVector()
{
	UINT i=0;
	for(i=0;i<m_vctProductInfo.size();i++)
	{
		m_vctProductInfo[i]->Clear();
		delete m_vctProductInfo[i];
	}
	m_vctProductInfo.clear();
}
BOOL CMainPage::CheckFileValidate(LPCTSTR lpszFile)
{
	if(lpszFile == NULL)
		return FALSE;

	BOOL bOK = FALSE;

	CFileFind finder;
	if(finder.FindFile(lpszFile))
	{
		finder.FindNextFile();
		if(finder.GetLength()!=0 || IsLoadFromPac(lpszFile))
		{
			bOK = TRUE;
		}
	}
	finder.Close();
	return bOK;
}

BOOL CMainPage::CheckFileChipPrefix( LPCTSTR lpszFile, CString &strChipPre )
{
	if(lpszFile == NULL)
		return FALSE;

	BOOL bOK = FALSE;
	CFileFind finder;

	if(!strChipPre.IsEmpty())
	{
		if(finder.FindFile(lpszFile))
		{		
			finder.FindNextFile();
			CString strFileTitle = finder.GetFileTitle();
			strFileTitle.MakeUpper();
			if(strFileTitle.Find(strChipPre) == 0)
			{
				bOK = TRUE;
			}
		}
	}

	return bOK;
}

BOOL CMainPage::FindLangNVItem(LPCTSTR lpszFilePath,CUIntArray &agLangFlag, FILETIME * pLastWriteTime)
{
	if(lpszFilePath == NULL || m_wLangNVItemID == 0xFFFF)
		return FALSE;

	CFileFind finder;
	if(!finder.FindFile(lpszFilePath))
	{
		return FALSE;
	}

	agLangFlag.RemoveAll();

	BOOL bRlt = FALSE;

	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = ::CreateFile(lpszFilePath,
		                 GENERIC_READ,
						 FILE_SHARE_READ,
						 NULL,
						 OPEN_EXISTING,
						 FILE_ATTRIBUTE_NORMAL,
						 NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if(pLastWriteTime != NULL)
	{
		::GetFileTime(hFile,NULL,NULL,pLastWriteTime);
	}

	DWORD dwSize = GetFileSize(hFile,NULL);

	if(dwSize == 0 || dwSize == 0xFFFFFFFF)
	{
		return FALSE;
	}

	BYTE *pBuf = new BYTE[dwSize];

	DWORD dwRealRead =  0;
	ReadFile(hFile,pBuf,dwSize,&dwRealRead,NULL);
	DWORD dwOffset=0;
	DWORD dwLength=0;
	BOOL  bBigEndian = TRUE;
	if(XFindNVOffsetEx(m_wLangNVItemID,pBuf,dwSize,dwOffset,dwLength,bBigEndian,FALSE) && 
		dwLength == sizeof(NV_MULTI_LANGUE_CFG))
	{
		bRlt = TRUE;
		NV_MULTI_LANGUE_CFG *pObj= (NV_MULTI_LANGUE_CFG *)(pBuf+dwOffset);
	
		for(int i=0;i< NV_MAX_LANG_NUM;i++)
		{
			agLangFlag.Add((UINT)pObj->flag[i]);
		}
	}	

	CloseHandle(hFile);
	
	delete [] pBuf;

	return bRlt;

}

BOOL CMainPage::FindLCDItem(LPCTSTR lpszFilePath,VEC_LCD_CFIG &vLcdCfig, FILETIME * pLastWriteTime)
{
	USES_CONVERSION;
	if(lpszFilePath == NULL)
		return FALSE;

	CFileFind finder;
	if(!finder.FindFile(lpszFilePath))
	{
		return FALSE;
	}
	finder.Close();

	vLcdCfig.clear();

	BOOL bRlt = FALSE;

	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = ::CreateFile(lpszFilePath,
		                 GENERIC_READ,
						 FILE_SHARE_READ,
						 NULL,
						 OPEN_EXISTING,
						 FILE_ATTRIBUTE_NORMAL,
						 NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if(pLastWriteTime != NULL)
	{
		::GetFileTime(hFile,NULL,NULL,pLastWriteTime);
	}

	DWORD dwSize = GetFileSize(hFile,NULL);

	if(dwSize == 0 || dwSize == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	BYTE *pBuf = new BYTE[dwSize];
	if (NULL == pBuf)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	DWORD dwRealRead =  0;
	ReadFile(hFile,pBuf,dwSize,&dwRealRead,NULL);

	const char szBeginFlag[] = "LCDTABLEBEGIN#*##";
	const char szEndFlag[] = "LCDTABLEEND#*##";
	const char szNameFlag[] = "LCDNAME_";

	BYTE* lpPos = NULL;
	BYTE* lpBeginPos = NULL;
	BYTE* lpEndPos = NULL;
	BYTE* lpTmpPos = NULL;
	lpPos = std::search( pBuf,pBuf + dwSize,szBeginFlag,szBeginFlag + strlen( szBeginFlag ) - 1 );

	LCD_ITEM li = {0};
	char szName[128] = {0};


	if(lpPos < (pBuf + dwSize) )
	{
		lpBeginPos = lpPos;

		lpPos = std::search( lpBeginPos,lpBeginPos + (dwSize-(lpBeginPos-pBuf)) ,szEndFlag,szEndFlag + strlen( szEndFlag ) - 1 );
		if(lpPos < (lpBeginPos + (dwSize-(lpBeginPos-pBuf))))
		{
			lpEndPos = lpPos;
			
			char szTmp[4] = {0};
			memcpy(szTmp,lpBeginPos+strlen( szBeginFlag ),2);
			int nNameLen = 0;
			sscanf(szTmp,"%d",&nNameLen);
			if(nNameLen > 0)
			{				
				lpTmpPos = lpBeginPos+strlen( szBeginFlag )+2;
				while(lpTmpPos < lpEndPos)
				{
					lpPos = std::search( lpTmpPos,lpEndPos ,szNameFlag,szNameFlag + strlen( szNameFlag ) - 1 );
					lpTmpPos = lpPos + nNameLen;
				
					if(lpPos < lpEndPos)
					{
						bRlt = TRUE;
						li.dwFlagOffset = lpTmpPos - pBuf;
						li.dwNameLen = nNameLen;
						li.dwFlag = (BYTE)(*lpTmpPos);
						memset(szName,0,sizeof(szName));
						memcpy(szName,lpPos,nNameLen);
						_tcscpy(li.szName,A2W(szName)); 
						vLcdCfig.push_back(li);
					}					
				}
			}
		}
	}	

	CloseHandle(hFile);
	
	delete [] pBuf;

	return bRlt;
}

int CMainPage::GetFileInfo(LPCTSTR lpszFileID, LPDWORD ppFileInfo, BOOL bWnd /*= FALSE*/)
{
	if(lpszFileID == NULL)
	{
		return -1;
	}

	int nCount = 0;
	PFILE_INFO_T pFileInfo = NULL;
	if(!bWnd)
	{
		if(m_pCurProductInfo == NULL)
		{
			return -1;
		}
		nCount = m_pCurProductInfo->dwFileCount;
		pFileInfo= m_pCurProductInfo->pFileInfoArr;
	}
	else
	{
		if(m_pTmpProductInfo == NULL)
		{
			return -1;
		}
		nCount = m_pTmpProductInfo->dwFileCount;
		pFileInfo= m_pTmpProductInfo->pFileInfoArr;
	}

	int i=0;
	CString strID;
	for(i = 0;i<nCount;i++)
	{
		strID = (pFileInfo+i)->szID;
		if( strID.CompareNoCase(lpszFileID)==0)
		{
			if(ppFileInfo!= NULL)
			    *ppFileInfo = (DWORD)(pFileInfo+i);
			break;
		}
	}
	
	if(i>=nCount)
		return -1;
	else
		return i;
}

int CMainPage::GetAllFileInfo(LPDWORD ppFileInfo, BOOL bWnd /*= FALSE*/)
{
	int nCount = 0;
	PFILE_INFO_T pFileInfo = NULL;
	if(!bWnd)
	{
		if(m_pCurProductInfo == NULL)
		{
			return -1;
		}
		nCount = m_pCurProductInfo->dwFileCount;
		pFileInfo= m_pCurProductInfo->pFileInfoArr;
	}
	else
	{
		if(m_pTmpProductInfo == NULL)
		{
			return -1;
		}
		nCount = m_pTmpProductInfo->dwFileCount;
		pFileInfo= m_pTmpProductInfo->pFileInfoArr;
	}

	if(ppFileInfo != NULL)
		*ppFileInfo = (DWORD)pFileInfo;
	
	return nCount;
}


BOOL CMainPage::GetFdl2Flag(LPCTSTR lpszFilePath,UINT nType)
{
	if(lpszFilePath == NULL || _tcslen(lpszFilePath) == 0)
	{
		return TRUE;
	}

	CFileFind finder;
	if(!finder.FindFile(lpszFilePath))
		return FALSE;

	CString strError;

	CFile file;
    CFileException fe;
    BOOL bRet = file.Open( lpszFilePath,CFile::modeRead|CFile::shareDenyWrite,&fe );

    if( !bRet )
    {
        // Can not open file
        LPTSTR p = strError.GetBuffer( _MAX_PATH );
        fe.GetErrorMessage( p,_MAX_PATH );
        strError.ReleaseBuffer();
		CString strTmp;
		strTmp.Format(_T("Can not open file [%s]!\n(%s)"),lpszFilePath,strError.operator LPCTSTR());
		AfxMessageBox(strTmp);
        return bRet;
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
		if(lpContent!=NULL)
		{
			delete []lpContent;
		}

		CString strTmp;
		strTmp.Format(_T("Can not open file [%s]!\n(%s)"),lpszFilePath,strError.operator LPCTSTR());
		AfxMessageBox(strTmp);		
        return FALSE;
    }
   
    const char szOmadm[] = "#$DEVICE_MANAGER$#";
	const char szPreload[] = "#*PRELOADSUPPORT*#";
	const char szKernelImg2[]="#*DEMANDPAGING*#";
	const char szRomDisk[]= "#*USBCOMAUTORUN*#";

    BYTE* lpPos = NULL;
	if(nType == FDL2F_OMADM)
	{
		lpPos = std::search( lpContent,lpContent + nLen,szOmadm,szOmadm + strlen( szOmadm ) - 1 );
	}
	else if(nType == FDL2F_PRELOAD)
	{
		lpPos = std::search( lpContent,lpContent + nLen,szPreload,szPreload + strlen( szPreload ) - 1 );
	}
	else if(nType == FDL2F_KERNELIMG2)
	{
		lpPos = std::search( lpContent,lpContent + nLen,szKernelImg2,szKernelImg2 + strlen( szKernelImg2 ) - 1 );
	}
	else if(nType == FDL2F_ROMDISK)
	{
		lpPos = std::search( lpContent,lpContent + nLen,szRomDisk,szRomDisk + strlen( szRomDisk ) - 1 );
	}
	else
	{
		delete []lpContent;
		return FALSE;
	}

    if( lpPos == lpContent + nLen )
    {
        // not find the version string
        delete []lpContent;
        return FALSE;
    }
	else
	{
		delete []lpContent;    
		return TRUE;
	}   
}

void CMainPage::OnRclickLstProductInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(!m_bCanEdit)
	{
		*pResult = 0;
		return;
	}
	
	LPNMITEMACTIVATE pnia = (LPNMITEMACTIVATE)pNMHDR;


	int nItem = pnia->iItem;
	int nSubItem = pnia->iSubItem;
	if(nItem == -1 || nSubItem != 1)
		return;
	//LOWORD is file flag
	//HIWORD is gray flag
	if(DI_IS_NEED_FILE(m_lstProductInfo.GetItemData(nItem)) == 0)
		return;
	
	static _TCHAR BASED_CODE szFilter[] = _T("All Files(*.*)|*.*|Binary Files (*.bin)|*.bin||");
    CString strFile=_T("");	
    CFileDialog dlg(TRUE, NULL, strFile, OFN_NOCHANGEDIR |OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter, NULL);
	
	if(dlg.DoModal() == IDOK)
    {
		CString strText = dlg.GetPathName();
		m_lstProductInfo.SetItemText(nItem,nSubItem,strText);
		if(m_pTmpProductInfo != NULL && m_pTmpProductInfo->dwOmaDMFlag != 0)
		{
			
			CString fileID = m_pTmpProductInfo->pFileInfoArr[nItem].szID;
			if(fileID.CompareNoCase(_T("FDL2"))== 0)
			{
				m_bTmpOmaDM= GetFdl2Flag(strText,FDL2F_OMADM);
				m_bTmpPreload= GetFdl2Flag(strText,FDL2F_PRELOAD);	
				m_bTmpKernelImg2= GetFdl2Flag(strText,FDL2F_KERNELIMG2);
				m_bTmpRomDisk = GetFdl2Flag(strText,FDL2F_ROMDISK);
				ModifyItemGray();
			}
		}
		
    }

	*pResult = 0;
}

void CMainPage::OnClickLstProductInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	LPNMITEMACTIVATE pnia = (LPNMITEMACTIVATE)pNMHDR;

	int nItem = pnia->iItem;
	int nSubItem = pnia->iSubItem;
	if(nItem == -1 || nSubItem != 1)
	{
		if(m_pTmpProductInfo != NULL)
		{
			m_stcProdComment.SetText(m_pTmpProductInfo->szComment);
		}
		return;
	}

	CString strFileDescript=_T("");
	CString strTips;
	if(m_pTmpProductInfo!= NULL)
	{
		if(_tcslen(m_pTmpProductInfo->pFileInfoArr[nItem].szFileDescript)>0)
		{
			strFileDescript.Format(_T("Note: %s"),m_pTmpProductInfo->pFileInfoArr[nItem].szFileDescript);
		}
	}

	CString strFile = m_lstProductInfo.GetItemText(nItem,nSubItem);
	if(DI_IS_NEED_FILE(m_lstProductInfo.GetItemData(nItem)) == 0 || strFile.IsEmpty())
	{
		strTips = strFileDescript;
		m_stcProdComment.SetText(strTips);
		
	}
	else
	{
		CFileFind finder;
		if(finder.FindFile(strFile))
		{
			finder.FindNextFile();
			CTime lwt;
			finder.GetLastWriteTime(lwt);
			__int64 llFileSize = finder.GetLength();
			if (IsLoadFromPac(strFile.operator LPCTSTR()))
			{
				DATA_INFO_T dataInfo = GetDataInfo(strFile);
				llFileSize = dataInfo.llSize;
			}
			strTips.Format(_T("Size: %-8I64d [0x%I64X] Modify Time:%04d-%02d-%02d %02d:%02d:%02d\n%s"),		
					      llFileSize,
						  llFileSize,
						  lwt.GetYear(),
						  lwt.GetMonth(),
						  lwt.GetDay(),
						  lwt.GetHour(),
						  lwt.GetMinute(),
						  lwt.GetSecond(),
						  strFileDescript.operator LPCTSTR());
						  
		}
		else
		{
			strTips.Format(_T("File Not Found!\n%s"),strFileDescript.operator LPCTSTR());

		}
		m_stcProdComment.SetText(strTips);

	}	
	*pResult = 0;
}

int CMainPage::GetUDiskFiles(LPCTSTR lpszBaseUDiskFile, CStringArray &agFiles)
{
	agFiles.RemoveAll();

	CString strUDiskImgFile = lpszBaseUDiskFile;
	int nFileNameLen = UDSIK_IMG_NAME_LEN;
	CString strDir = strUDiskImgFile;
	
	int nFind = strDir.ReverseFind(_T('\\'));
	strDir = strDir.Left(nFind);

	CString strFind = strDir + _T("\\udisk_img_*.bin*");	

	WIN32_FIND_DATA wfd = { 0 };
	HANDLE hFind = ::FindFirstFile( strFind, &wfd );
	BOOL bFind = TRUE;
	while( INVALID_HANDLE_VALUE != hFind  && bFind)
	{
		CString strExt;
		CStringArray agFilePathInfo;
		GetFilePathInfo(wfd.cFileName,agFilePathInfo);
		strExt = agFilePathInfo.GetAt(2);
		if (IsLoadFromPac(wfd.cFileName))
		{
			CStringArray agTmpFilePathInfo;
			GetFilePathInfo(agFilePathInfo.GetAt(1),agTmpFilePathInfo);
			strExt = agTmpFilePathInfo.GetAt(2)+ agFilePathInfo.GetAt(2);
			nFileNameLen += MAX_FLAG_LEN;
		}
		CString strFileName =((CString) (wfd.cFileName)).Left(((CString) (wfd.cFileName)).GetLength() - strExt.GetLength());
		if( strFileName.GetLength() == nFileNameLen)
		{
			CString strFile;
			strFile.Format(_T("%s\\%s"),strDir.operator LPCTSTR(),wfd.cFileName);
			if(strFile.CompareNoCase(strUDiskImgFile) != 0 )
			{
				agFiles.Add(strFile);
			}
		}
		nFileNameLen = UDSIK_IMG_NAME_LEN;
		bFind = ::FindNextFile(hFind, &wfd);			
	}
	if(INVALID_HANDLE_VALUE != hFind)
	{
		::FindClose(hFind);	
	}

	return agFiles.GetSize();
}

int  CMainPage::GetDspFiles(LPCTSTR lpszBaseDspFile, CStringArray &agFiles)
{
	agFiles.RemoveAll();
	if(!m_bEnableChipDspMap)
	{
		// need not chip-dsp map
		return 0;
	}
	
	_TCHAR szConfigPath[_MAX_PATH]={0};
	::GetModuleFileName(g_theApp.m_hInstance,szConfigPath,_MAX_PATH);
	LPTSTR pResult = _tcsrchr(szConfigPath,_T('\\'));
	*pResult = 0;
	_tcscat(szConfigPath,_T("\\BMFileType.ini"));
	
	_TCHAR szKeyValue[ MAX_BUF_SIZE ]={0}; 
	DWORD dwSize = GetPrivateProfileSection( _T("ChipDSPMap"), szKeyValue, MAX_BUF_SIZE, szConfigPath );
	if(dwSize == 0)
	{
		AfxMessageBox(_T("Not found ChipID-DSPName map setting in BMFileType.ini!"));
		return -1;
	}
	CStringArray arrKeyData;    
	UINT nFileCount = (UINT)EnumKeys(szKeyValue,&arrKeyData);
	if(nFileCount == 0)
	{
		AfxMessageBox(_T("Not found ChipID-DSPName map setting in BMFileType.ini!"));
		return -1;
	}
	
	CString strErrMsg =_T("");
	
	CString strDspFile = lpszBaseDspFile;
	CString strDir = strDspFile;	
	int nFind = strDir.ReverseFind(_T('\\'));
	strDir = strDir.Left(nFind);	
	CFileFind finder;
	for(UINT i= 0; i< nFileCount; i++)
	{				
		CString strFile;
		strFile.Format(_T("%s\\%s"),strDir.operator LPCTSTR(),arrKeyData[2*i+1].operator LPCTSTR());
		
		if(!finder.FindFile(strFile))
		{
			CString strFlagFile;
			strFlagFile = strFile + FLAG_FILE_SUFFIX;
			if (!finder.FindFile(strFlagFile))
			{
				strErrMsg += strFile;
				strErrMsg += _T("\r\n");
				continue;
			}
			else
			{
				if(strFlagFile.CompareNoCase(strDspFile)!= 0)
				{
					agFiles.Add(strFlagFile);
				}	
				finder.Close();
			}
			
		}
		else
		{
			if(strFile.CompareNoCase(strDspFile)!= 0)
			{
				agFiles.Add(strFile);
			}	
			finder.Close();
		}
	}	
	if(!strErrMsg.IsEmpty())
	{
		strErrMsg.Insert(0,_T("Not found bellow DSP file:\r\n"));
		strErrMsg += _T("\r\nPlease check BMFileType.ini, [ChipDspMap]!");
		AfxMessageBox(strErrMsg);
		return -1;
	}	
	return agFiles.GetSize();
}


int  CMainPage::GetMultiFiles(CStringArray &agFiles, CString &strErr)
{
	agFiles.RemoveAll();
	strErr.Empty();

	int nStartFile = 1;
	if(m_pTmpProductInfo->dwFlashType == 1)
	{
		nStartFile = 2;
	}

	CFileFind finder;
	// File number
	for(int j = nStartFile; j < m_lstProductInfo.GetItemCount(); j++)
	{		
		if(!m_lstProductInfo.GetCheck(j))
		{
			continue;
		}
		CString strFilePath = m_lstProductInfo.GetItemText(j,1);
		if(strFilePath.IsEmpty())
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

		CString strFileTitle = strFileName;	

		UINT nCurChip = (UINT)(-1);
		// Chip category
		for(UINT i = 0; i< m_pTmpProductInfo->tChips.dwCount; i++)
		{
			CString strChipName = m_pTmpProductInfo->tChips.pChips[i].szName;
			strChipName += _T("_");
			if(strFileName.Left(strChipName.GetLength()).CompareNoCase(strChipName) == 0)
			{
				strFileTitle = strFileName.Right(strFileName.GetLength() -strChipName.GetLength());
				nCurChip = i;
				break;
			}
		}

		for(UINT k = 0; k< m_pTmpProductInfo->tChips.dwCount; k++)
		{
			CString strChipName = m_pTmpProductInfo->tChips.pChips[k].szName;
			
			if( k != nCurChip )
			{
				CString strExtFile;
				strExtFile.Format(_T("%s\\%s_%s"),strFilePath,strChipName,strFileTitle);
				if(finder.FindFile(strExtFile))
				{
					agFiles.Add(strExtFile);
				}
				else
				{
					strErr += strExtFile;
					strErr += _T("\n");
				}
			}
		}
	}

	return agFiles.GetSize();
}

void CMainPage::ModifyItemGray()
{
	m_bListLock = TRUE;
	m_bAllFiles = TRUE;
	for(int i= 0; i< m_lstProductInfo.GetItemCount();i++)
	{	
		DWORD dwData = m_lstProductInfo.GetItemData(i);
		DWORD dwNewData = dwData;	
		CString fileID =  m_pTmpProductInfo->pFileInfoArr[i].szID;
		if(fileID.CompareNoCase(g_szUA)==0 || fileID.CompareNoCase(g_szEDU)==0 )
		{	
			if(!m_bTmpOmaDM)
			{						
				dwNewData = DI_SET_GRAY(dwData);	
			}
			else
			{							
				dwNewData = DI_UNSET_GRAY(dwData);
			}

			if( DI_IS_GRAY(dwData) != DI_IS_GRAY(dwNewData))
			{
				m_lstProductInfo.SetItemData(i,dwNewData);	
				m_lstProductInfo.SetCheck(i,FALSE);
				if(!DI_IS_GRAY(dwNewData))
				{
					m_bAllFiles = FALSE;					
				}
			}			
		}					
		else if(fileID.CompareNoCase(g_szPreload)==0)
		{		
			if(!m_bTmpPreload)
			{						
				dwNewData = DI_SET_GRAY(dwData);	
			}
			else
			{							
				dwNewData = DI_UNSET_GRAY(dwData);
			}

			if( DI_IS_GRAY(dwData) != DI_IS_GRAY(dwNewData))
			{
				m_lstProductInfo.SetItemData(i,dwNewData);	
				m_lstProductInfo.SetCheck(i,FALSE);
				if(!DI_IS_GRAY(dwNewData))
				{
					m_bAllFiles = FALSE;					
				}
			}
			
		}
		else if(fileID.CompareNoCase(g_szKernelImg2)==0)
		{			
			if(!m_bTmpKernelImg2)
			{						
				dwNewData = DI_SET_GRAY(dwData);	
			}
			else
			{							
				dwNewData = DI_UNSET_GRAY(dwData);
			}

			if( DI_IS_GRAY(dwData) != DI_IS_GRAY(dwNewData))
			{
				m_lstProductInfo.SetItemData(i,dwNewData);
				m_lstProductInfo.SetCheck(i,FALSE);
				if(!DI_IS_GRAY(dwNewData))
				{
					m_bAllFiles = FALSE;					
				}
			}
			
		}
		else if(fileID.CompareNoCase(g_szRomDisk)==0)
		{		
			if(!m_bTmpRomDisk)
			{						
				dwNewData = DI_SET_GRAY(dwData);	
			}
			else
			{							
				dwNewData = DI_UNSET_GRAY(dwData);
			}

			if( DI_IS_GRAY(dwData) != DI_IS_GRAY(dwNewData))
			{
				m_lstProductInfo.SetItemData(i,dwNewData);
				m_lstProductInfo.SetCheck(i,FALSE);
				if(!DI_IS_GRAY(dwNewData))
				{
					m_bAllFiles = FALSE;					
				}
			}			
		}
		else
		{
			if(!m_lstProductInfo.GetCheck(i))
			{
				m_bAllFiles = FALSE;
			}
		}
		
	}
	
	m_bListLock = FALSE;

	UpdateData(FALSE);
}

void CMainPage::InitBaudrate()
{
	//Load Baudrate default
	int i = 0;
	CString strText;
	m_arrBaudRateDefault.RemoveAll();
	CString strBaudRate;
	VERIFY( strBaudRate.LoadString(IDS_BAUDRATE_DEFAULT));
	int     nLen   = strBaudRate.GetLength();
	LPTSTR  lpBuf  = strBaudRate.GetBuffer(nLen);
	LPTSTR  lpFind = _tcschr(lpBuf, _T(','));
	while(lpFind != NULL)
	{
		*lpFind = _T('\0');
		
		m_arrBaudRateDefault.Add(lpBuf);
		lpBuf = lpFind + 1;
		lpFind = _tcschr(lpBuf, _T(','));
	}
	m_arrBaudRateDefault.Add(lpBuf);
	strBaudRate.ReleaseBuffer();
	
	CStringArray arrBaudRate;
	
	if(m_arrBaudRateConfig.GetSize()>0)
	{
		arrBaudRate.Copy(m_arrBaudRateConfig);
	}
	else
	{
		arrBaudRate.Copy(m_arrBaudRateDefault);
	}
    for(i=0;i<arrBaudRate.GetSize();i++)
	{
		m_cmbBaudrate.AddString(arrBaudRate.GetAt(i));
	}

	strText.Format(_T("%d"),m_nBaudRate);
	
	int iSelBaudRate = m_cmbBaudrate.FindString(0,strText);
	if(iSelBaudRate == CB_ERR)
	{
		// Default is 115200
		iSelBaudRate = 1;	
	}
    m_cmbBaudrate.SetCurSel(iSelBaudRate);

}

void CMainPage::InitPort()
{
	m_cmbPort.AddString( ALL_PORT );
	/*
	// Search valible serial ports and open them
	int i = 0;
    HKEY  hOpenKey;
    const int cLen = 255;
    DWORD dwValueLen;
    DWORD dwDataLen;
    _TCHAR szValueName[cLen];
    _TCHAR szData[cLen];
    DWORD dwType;
    DWORD dwRet;    
	
    if((dwRet =RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		_T("HARDWARE\\DEVICEMAP\\SERIALCOMM"),
		NULL,
		KEY_QUERY_VALUE | KEY_READ,
		&hOpenKey)) == ERROR_SUCCESS)
    {
		i = 0;
        for(;;)
        {
            dwValueLen = cLen;
            dwDataLen = cLen;
            szValueName[0] = 0;
            szData[0] = 0;
            
            dwRet = ::RegEnumValue(hOpenKey,
                i++,
                szValueName,
                &dwValueLen,
                NULL,
                &dwType,
                (BYTE*)szData,
                &dwDataLen
                );
            
            if(dwRet != ERROR_SUCCESS )
                break;
            
            int port;
            int nRet = _stscanf(szData,_T("COM%d"),&port);
            
            if(nRet == 0)
                continue;
			if(g_theApp.m_bFilterPort)
			{
				DWORD dwValue = 0;
				if(g_theApp.m_mapFilterPort.Lookup((DWORD)port,dwValue))
				{
					continue;
				}
			}
            
			if(m_cmbPort.FindString(0,szData) == CB_ERR)
			{
				m_cmbPort.AddString( szData );
			}
        }    
        RegCloseKey(hOpenKey);
    }
	*/

	CMainFrame * pMF = (CMainFrame*)AfxGetMainWnd();
	
	std::vector<DEV_INFO> lstPort;
	//pMF->m_usbMoniter.ScanPort(lstPort);
    DEV_INFO* pDevInfo = NULL;
    int nCount = pMF->m_pUsbMoniter->ScanPort(&pDevInfo);
    lstPort.clear();
    for(int j = 0; j<nCount; ++j)
    {
        lstPort.push_back(pDevInfo[j]);
    }

	for(UINT i = 0; i< lstPort.size(); i++)
	{
		if(g_theApp.m_bFilterPort)
		{
			DWORD dwValue = 0;
			if(g_theApp.m_mapFilterPort.Lookup((DWORD)lstPort[i].nPortNum,dwValue))
			{
				continue;
			}
		}
		
		if(m_cmbPort.FindString(0,lstPort[i].szPortName) == CB_ERR)
		{
			m_cmbPort.AddString( lstPort[i].szPortName );
		}
	}
	
    CString strText;  
    int iSelPort = 0;
    if( m_nComPort != 0 )
    {
        strText.Format(_T("COM%d"),m_nComPort );
        
        iSelPort = m_cmbPort.FindString(0,strText);
        if( iSelPort == CB_ERR)
        {
            // Default is all
            iSelPort = 0;	
        }        
    }
    m_cmbPort.SetCurSel( iSelPort );	
    pMF->m_pUsbMoniter->FreeMemory(pDevInfo);
}

void CMainPage::InitListCtrl()
{
	//m_lstProduct
	m_lstProductInfo.SetBkColor(RGB(232, 232, 232));	
	m_lstProductInfo.ModifyStyle(0, LVS_SHOWSELALWAYS);
	
	DWORD dwExStyle = m_lstProductInfo.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT;
	dwExStyle |= LVS_EX_GRIDLINES;	
	if(m_bCanSel)
	{
		dwExStyle |= LVS_EX_CHECKBOXES;
	}
	else
	{
		GetDlgItem(IDC_CHK_ALL)->ShowWindow(SW_HIDE);
	}	
	
	m_lstProductInfo.SetExtendedStyle(dwExStyle);
	
	//Load Column
	CString strClmn;
	VERIFY( strClmn.LoadString(IDS_PRODUCT_INFO_COLUMN) );	
	int nLen   = strClmn.GetLength();
	LPTSTR lpBuf  = strClmn.GetBuffer(nLen);
	LPTSTR lpFind = _tcschr(lpBuf, _T(','));
	int     nIndex = 0;
	while(lpFind != NULL)
	{
		*lpFind = _T('\0');
		if(nIndex == 1)
			m_lstProductInfo.InsertColumn(nIndex++,lpBuf,LVCFMT_LEFT,250);
		else
			m_lstProductInfo.InsertColumn(nIndex++,lpBuf,LVCFMT_LEFT,110);
		lpBuf = lpFind + 1;
		lpFind = _tcschr(lpBuf, _T(','));
	}
	m_lstProductInfo.InsertColumn(nIndex, lpBuf, LVCFMT_LEFT, 100 );
	strClmn.ReleaseBuffer();	

	m_lstProductInfo.EnableToolTips();
}

void CMainPage::LoadDLFiles(LPCTSTR lpszConfigFile)
{
    int nBufLength = 512*1024;
	_TCHAR*  szBuf = new _TCHAR[nBufLength];
    memset(szBuf,0,nBufLength*sizeof(_TCHAR));
    ::GetPrivateProfileString(g_sz_SELECT,g_sz_SELECT_PRODUCT,NULL,szBuf,nBufLength,lpszConfigFile);
    m_strCurProduct = szBuf;

	if(!InitProdInfo())
    {
        delete [] szBuf;
        szBuf = NULL;
        return;
    }

	memset(szBuf,0,nBufLength*sizeof(_TCHAR));
    GetPrivateProfileSection( m_strCurProduct, szBuf, nBufLength, lpszConfigFile );    
	m_mapDLFiles.RemoveAll();
	m_mapDLState.RemoveAll();
	m_mapDLSize.RemoveAll();
	m_mapDataInfo.RemoveAll();
	m_mapLastTime.RemoveAll();

	CStringArray arrKeyData;    
	UINT nFileCount = (UINT)EnumKeys(szBuf,&arrKeyData);
	UINT i=0;
	CString strCheckFile;
	CString strCheck;
	int nCheck;
	int nFind = -1;
	int nCheckCount = 0;
	CString strID;	
	for(i=0;i<nFileCount;i++)
	{
		strID = arrKeyData[i*2];
		strID.MakeUpper(); // ******
		strCheckFile = arrKeyData[i*2+1];
		strCheckFile.TrimLeft();
		if(strCheckFile.IsEmpty())
		{
			m_mapDLState.SetAt(strID,FALSE);
		}
		else
		{
			nFind = strCheckFile.Find(_T('@'));
			if(nFind == -1 || nFind > 1)
			{
				m_mapDLState.SetAt(strID,TRUE);
				nCheckCount++;
			}
			else
			{
				strCheck = strCheckFile.Left(nFind);
				nCheck = _ttoi(strCheck);
				if(nCheck != 0)
				{
					nCheck = 1;
					nCheckCount++;
				}
				m_mapDLState.SetAt(strID,nCheck);
				strCheckFile = strCheckFile.Right(strCheckFile.GetLength()-nFind-1);
			}
		}
		strCheckFile.TrimLeft();
		strCheckFile.TrimRight();
		m_mapDLFiles.SetAt(strID,strCheckFile);		
	}    

	//if(!m_strCurProduct.IsEmpty() && nCheckCount > 0  )
	//	m_bInit = TRUE;
	m_bInit = CheckInitDLFiles();
    delete [] szBuf;
    szBuf = NULL;
}

BOOL CMainPage::CheckInitDLFiles()
{
	if(m_pCurProductInfo == NULL)
		return FALSE;

	UINT nCount = m_pCurProductInfo->dwFileCount;

	int  nCheckCount = 0;
	BOOL bHasModifiedFile = FALSE;
	for(UINT i = 0;i<nCount;i++)
	{
		CString strID = m_pCurProductInfo->pFileInfoArr[i].szID;
		strID.MakeUpper();
		BOOL bCheck = FALSE;
		CString strFile;
		m_mapDLFiles.Lookup(strID,strFile);
		m_mapDLState.Lookup(strID,bCheck);
		if(bCheck)
		{
			nCheckCount++;
#ifdef _RESEARCH
			if (strID.CompareNoCase(_T("SPLLoader")) && IsModifiedDLFile(strFile.operator LPCTSTR()))
			{
				 bHasModifiedFile = TRUE;
			}
#endif
		}

		if( m_pCurProductInfo->pFileInfoArr[i].dwCheckFlag == 1 ) // key item
		{
			if(!bCheck)
			{
				return FALSE;
			}

			if(m_pCurProductInfo->pFileInfoArr[i].dwFlag == 1 &&  // need a file
			   strFile.IsEmpty())
			{
				return FALSE;
			}
			
		}

		if(bCheck && m_pCurProductInfo->pFileInfoArr[i].dwFlag == 1 && strFile.IsEmpty())
		{
			return FALSE;
		}		
	}

	CMainFrame * pMF= (CMainFrame *)AfxGetMainWnd();
	if (pMF)
	{
		if (bHasModifiedFile)
		{
			ZeroMemory(&(pMF->m_ftPacInfo),sizeof(FILETIME));
		}
		else
		{
			pMF->m_ftPacInfo = m_ftPacOrg;
		}	
	}
	
	if(nCheckCount == 0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

int  CMainPage::GetDLFiles(CStringArray &agFiles)
{
	agFiles.RemoveAll();
	if(m_pCurProductInfo == NULL)
		return 0;

	UINT nCount = m_pCurProductInfo->dwFileCount;
	for(UINT i = 0;i<nCount;i++)
	{
		CString strID = m_pCurProductInfo->pFileInfoArr[i].szID;
		strID.MakeUpper();
		BOOL bCheck = FALSE;
		CString strFile;
		m_mapDLFiles.Lookup(strID,strFile);
		m_mapDLState.Lookup(strID,bCheck);
        if( m_pOptionsPage->m_bCheckMatch && 0 == strID.CompareNoCase(_T("EraseUBOOT")))  //need to skip EraseUBOOT when Check matching 
        {
            bCheck = FALSE;
        }

		if(bCheck)
		{
			agFiles.Add(strFile);
		}
		else
		{
			agFiles.Add(FILE_OMIT);
		}		
	}
	return nCount;	
}

BOOL  CMainPage::CheckCrcDLFiles(CString& strCrcFailFileId)
{
    if(m_pCurProductInfo == NULL)
        return 0;

    _TCHAR szConfigIniFile[MAX_PATH];    
    GetModuleFilePath( g_theApp.m_hInstance, szConfigIniFile );
    _tcscat( szConfigIniFile,  _T("\\BMFileType.ini") );
    _TCHAR szBuf[MAX_PATH] = {0};
    GetPrivateProfileString(_T("Misc"),_T("CrcFile"),_T(""),szBuf,MAX_PATH,szConfigIniFile);
    
    _TCHAR szKeyValue[MAX_BUF_SIZE]={0}; 
    GetPrivateProfileSection( _T("Crc"), szKeyValue, MAX_BUF_SIZE, szBuf );
    CStringArray arrKeyData;    
    UINT nFileCount = (UINT)EnumKeys(szKeyValue,&arrKeyData);
    for(UINT i= 0; i< nFileCount; i++)
    {
        CString strText = arrKeyData[2*i];
        BOOL bExisted = FALSE;
        if(!strText.IsEmpty())
        {
            UINT nCount = m_pCurProductInfo->dwFileCount;
            for(UINT i = 0;i<nCount;i++)
            {
                CString strID = m_pCurProductInfo->pFileInfoArr[i].szID;
                if(strID.CompareNoCase(strText) == 0)
                {
                    strID.MakeUpper();
                    BOOL bCheck = FALSE;
                    m_mapDLState.Lookup(strID,bCheck);

                    if(bCheck)
                    {
                        bExisted = TRUE;
                    }
                }
            }
        }
        if(!bExisted)
        {
            strCrcFailFileId = strText;
            return FALSE;
        }
    }
    return TRUE;  	
}

BOOL CMainPage::InitProdInfo()
{
	BOOL bMatch = FALSE;
	int j = 0;
	for(j = 0; j< m_arrProductName.GetSize(); j++)
	{	
		if(m_arrProductName[j].CompareNoCase(m_strCurProduct) == 0)
		{
			m_nCurProduct = j;
			m_pCurProductInfo = m_vctProductInfo[j];
			bMatch = TRUE;			
			m_nTmpProduct = m_nCurProduct;
			m_pTmpProductInfo = m_pCurProductInfo;
			m_strTempProduct = m_strCurProduct;	
			break;
		}
	}
	if(!bMatch)
	{
		m_nCurProduct = 0;
		m_pCurProductInfo = m_vctProductInfo[0];
		m_strCurProduct = m_arrProductName[0];
		m_mapDLFiles.RemoveAll();
		m_mapDLSize.RemoveAll();
		m_mapDataInfo.RemoveAll();
		m_mapLastTime.RemoveAll();
		m_mapDLState.RemoveAll();
		m_bInit = FALSE;		
		m_nTmpProduct = m_nCurProduct;
		m_pTmpProductInfo = m_pCurProductInfo;
		m_strTempProduct = m_strCurProduct;			
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL CMainPage::_LoadConfig(CString &strErrorMsg)
{
	LPCTSTR pFileName = m_strIniFile.operator LPCTSTR();
	
    // Get Default Baudrates
    _TCHAR szBaudSection[MAX_PATH];
    memset(szBaudSection,0,MAX_PATH);
	::GetPrivateProfileString(g_sz_SEC_GUI,g_sz_KEY_BRS,g_sz_BR_DEFAULT,szBaudSection,MAX_PATH,pFileName);
	
	CString strBaudRate;
	strBaudRate = szBaudSection;
	if(!strBaudRate.IsEmpty())
    {
		m_arrBaudRateConfig.RemoveAll();
		int     nLen   = strBaudRate.GetLength();
		LPTSTR  lpBuf  = strBaudRate.GetBuffer(nLen);
		LPTSTR  lpFind = _tcschr(lpBuf, _T(','));
		while(lpFind != NULL)
		{
			*lpFind = _T('\0');
			
			m_arrBaudRateConfig.Add(lpBuf);
			lpBuf = lpFind + 1;
			lpFind = _tcschr(lpBuf, _T(','));
		}
		m_arrBaudRateConfig.Add(lpBuf);
		strBaudRate.ReleaseBuffer();
    }
	
    // Get baudrate
    m_nBaudRate = ::GetPrivateProfileInt(g_sz_SP,g_sz_BR,115200,pFileName);
    m_nComPort = ::GetPrivateProfileInt( g_sz_SP, g_sz_PORT, 0, pFileName );

	
#ifdef _SPUPGRADE
	BOOL bShowChinese = FALSE;
	m_bCanSel = ::GetPrivateProfileInt( g_sz_SEC_GUI, g_sz_KEY_SEL, 0, pFileName );
	m_bCanEdit = ::GetPrivateProfileInt( g_sz_SEC_GUI, g_sz_KEY_EDT, 0, pFileName );
	m_bShowChinese = ::GetPrivateProfileInt( g_sz_SEC_GUI, _T("ShowChinese"), 1, pFileName );
#else
	m_bCanSel = ::GetPrivateProfileInt( g_sz_SEC_GUI, g_sz_KEY_SEL, 1, pFileName );
	m_bCanEdit = ::GetPrivateProfileInt( g_sz_SEC_GUI, g_sz_KEY_EDT, 1, pFileName );
#endif
	
#ifdef _FACTORY
	m_bCanSel = ::GetPrivateProfileInt( g_sz_SEC_GUI, g_sz_KEY_SEL, 0, pFileName );
	m_bCanEdit = ::GetPrivateProfileInt( g_sz_SEC_GUI, g_sz_KEY_EDT, 0, pFileName );
#endif
	
	m_wLangNVItemID = (WORD)::GetPrivateProfileInt( g_sz_SEC_NV, g_sz_KEY_LNII, NV_MULTI_LANG_ID, pFileName );	
	m_bEnableChipDspMap = ::GetPrivateProfileInt( g_sz_SEC_CHIPDSPMAP, g_sz_KEY_CDME, FALSE, pFileName );	
	m_bWriteSN = !((BOOL)::GetPrivateProfileInt(_T("SN"),  _T("AutoGenerateSN"), 1, pFileName));
	m_bChkWriteSN = m_bWriteSN;
	
	CString strXMLConfig = GetXmlFile();
	if(!LoadAllProduct(strXMLConfig))
	{
		CString strErr;
		strErr.Format(_T("Load XML configure file [%s] failed!\n"),strXMLConfig.operator LPCTSTR());
		strErrorMsg += strErr;
		return FALSE;
	}

    return TRUE;
}

int CMainPage::GetPageBlockFiles(CStringArray &agFiles)
{
	agFiles.RemoveAll();
	
	PFILE_INFO_T pFileInfo = m_pTmpProductInfo->pFileInfoArr;
	int nCount = (int)m_pTmpProductInfo->dwFileCount;
	CMap<CString, LPCTSTR,DWORD,DWORD> mapPBInfo;

	int i=0;
	CString strErr = _T("");
	int nPBFileCount = 0;
 	for(i = 0; i< nCount; i++)
	{	
		CString strFileType = pFileInfo[i].szType;
		CString strFileID = pFileInfo[i].szID;
		CString strFilePath = m_lstProductInfo.GetItemText(i,1);
		if( strFilePath.IsEmpty() || 
			!m_lstProductInfo.GetCheck(i) ||
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
			AfxMessageBox(strErr);
			agFiles.RemoveAll();
			return -1;
		}

		nPBFileCount++;

		CString strExt = agFilePathInfo[2];
		CString strFilePre = strFileName.Left(nIndex);
		CString strFind;
		strFind.Format(_T("%s\\%s_b*%s"),agFilePathInfo[0],strFilePre,strExt);

		WIN32_FIND_DATA wfd = { 0 };
		HANDLE hFind = ::FindFirstFile( strFind, &wfd );
		BOOL bFind = TRUE;
		while( INVALID_HANDLE_VALUE != hFind  && bFind)
		{
			CString strPBInfo =((CString) (wfd.cFileName)).Left(((CString) (wfd.cFileName)).GetLength() - strExt.GetLength());
			strPBInfo = strPBInfo.Right(strPBInfo.GetLength()-strFilePre.GetLength());
			if(IsLoadFromPac(wfd.cFileName))  //.flag file
			{
				strPBInfo = GetStrFileTitle(strPBInfo.operator LPCTSTR());
			}
			
			CString strFile;
			strFile.Format(_T("%s\\%s"),agFilePathInfo[0].operator LPCTSTR(),wfd.cFileName);
			
			if(strFile.CompareNoCase(strFilePath) != 0)
			{
				agFiles.Add(strFile);
			}

			strPBInfo.MakeLower();

			bFind = ::FindNextFile(hFind, &wfd);
			
			DWORD dwPBCount = 0;
			
			if(mapPBInfo.Lookup(strPBInfo,dwPBCount))
			{
				dwPBCount += 1;				
			}
			else
			{
				dwPBCount = 1;				
			}
			mapPBInfo.SetAt(strPBInfo,dwPBCount);
		}

		if(INVALID_HANDLE_VALUE != hFind)
		{
			::FindClose(hFind);	
		}			
	}

	POSITION pos = mapPBInfo.GetStartPosition();
	while(NULL != pos)
	{
		CString strKey;
		DWORD dwPBCount = 0;
      
        mapPBInfo.GetNextAssoc( pos, strKey, dwPBCount );

		if(dwPBCount != (DWORD)nPBFileCount)
		{
			strErr.Format(_T("Block-Page [%s] only have %d image files.\nIt must be equal to %d."),
				          strKey,dwPBCount,nPBFileCount);
			g_theApp.MessageBox(strErr.operator LPCTSTR());
			agFiles.RemoveAll();
			return -1;
		}
	}
	return agFiles.GetSize();
}

int CMainPage::GetMultiNVFiles(CStringArray &agFiles,CString& strWarning)
{
	agFiles.RemoveAll();

	PFILE_INFO_T pFileInfo = m_pTmpProductInfo->pFileInfoArr;
	int nCount = (int)m_pTmpProductInfo->dwFileCount;
	std::map<std::pair<CString,CString>,CString> mapCheckNVInfo;
	CMap<CString, LPCTSTR,CString, LPCTSTR> mapMultiNVInfo;
	int i=0;
	CString strErr = _T("");
	CStringArray agChipName;
	CUIntArray   agChipID;
	for(UINT i = 0 ; i< m_pTmpProductInfo->tRfChips.dwCount; i++)
	{
		agChipName.Add(m_pTmpProductInfo->tRfChips.pChips[i].szName);
		agChipID.Add(m_pTmpProductInfo->tRfChips.pChips[i].dwID);
	}		
	for(i = 0; i< nCount; i++)
	{	
		CString strFileType = pFileInfo[i].szType;
		CString strFileID = pFileInfo[i].szID;
		CString strFilePath = m_lstProductInfo.GetItemText(i,1);
		if( 
			strFilePath.IsEmpty() || 
			!m_lstProductInfo.GetCheck(i) ||
			pFileInfo[i].isSelByRf != 1
		  )
		{
			continue;
		}
		if (_tcsnicmp(strFileID.operator LPCTSTR(),_T("NV"),2))
		{
			strErr.Format(_T("This version just support multi nv,But this is %s file."),strFileID);
			g_theApp.MessageBox(strErr);
			return -1;
		}

		CString strRFChips;
		CStringArray agFilePathInfo;
		GetFilePathInfo(strFilePath,agFilePathInfo);
		CString strFileName = agFilePathInfo[1];

		CString strFilePre;		//prefix_RFNAME_*nvitem.bin ->prefix
		CString strCurrentChip;	//prefix_RFNAME_*nvitem.bin ->RFNAME
		
		for(int i = 0; i< m_pTmpProductInfo->tRfChips.dwCount; i++)
		{
			//sharkl2_pubcp_3595A_cmcc_nvitem.bin
			//sharkl2_pubcp_3595AL_la_nvitem.bin
			CString strChipName;
			strChipName.Format(_T("_%s_"),m_pTmpProductInfo->tRfChips.pChips[i].szName);
			
			int nIndex = strFileName.Find(strChipName);
			if (nIndex != -1 && -1 != strFileName.Find(g_sz_NVITEM))
			{
				strFilePre = strFileName.Left(nIndex);
				strCurrentChip = m_pTmpProductInfo->tRfChips.pChips[i].szName;
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
			strErr.Format(_T("The nv file [%s] is invalid! \nFile name must be match with \"prefix_rfname_*nvitem.bin\"\n%s"),strFilePath,strTmp);
			g_theApp.MessageBox(strErr);
			agFiles.RemoveAll();
			return -1;	
		}
		
		strRFChips = strCurrentChip;

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

			CString strFile;
			strFile.Format(_T("%s\\%s"),agFilePathInfo[0].operator LPCTSTR(),wfd.cFileName);

			if(strFile.CompareNoCase(strFilePath) != 0)
			{
				for(int n = 0; n < m_pTmpProductInfo->tRfChips.dwCount; ++n)
				{
					if (0 == agChipName.GetAt(n).CompareNoCase(strRFName.operator LPCTSTR()))
					{
						agFiles.Add(strFile);
						strRFChips += _T(",");
						strRFChips += strRFName;

						std::map<std::pair<CString,CString>,CString> ::iterator it = mapCheckNVInfo.find(std::make_pair(strRFName,strFileID));
						if (mapCheckNVInfo.end() != it)
						{
							strErr.Format(_T("%s can't have multiple nv file."),strRFName.operator LPCTSTR());
							g_theApp.MessageBox(strErr);
							agFiles.RemoveAll();
							return -1;
						}
						mapCheckNVInfo[std::make_pair(strRFName,strFileID)] = strFile;
						break;
					}
				}
				
			}
			bFind = ::FindNextFile(hFind, &wfd);

		}

		if(INVALID_HANDLE_VALUE != hFind)
		{
			::FindClose(hFind);	
		}	

		mapMultiNVInfo.SetAt(strFileID,strRFChips);
		
	}

	POSITION pos = mapMultiNVInfo.GetStartPosition();
	while(NULL != pos)
	{
		CString strKey;
		CString strRFChips;
		CStringArray agRFChips;
		mapMultiNVInfo.GetNextAssoc( pos, strKey, strRFChips );

		int nCount = SplitStr(strRFChips,agRFChips,_T(','));
		if((DWORD)nCount != m_pTmpProductInfo->tRfChips.dwCount)
		{
			strWarning.Format(_T("%s just select %d image files,It should be equal to %d.\n"),
				strKey,nCount,m_pTmpProductInfo->tRfChips.dwCount);
			//g_theApp.MessageBox(strErr.operator LPCTSTR());
			//agFiles.RemoveAll();
			//return -1;
		}
	}
	return agFiles.GetSize();
}

int CMainPage::GetBackupFiles(CStringArray &agID,BOOL bSharkNand)
{
	int nCount = m_pCaliPage->GetBackupFiles(agID,bSharkNand);
	
	for(int i = 0; i<nCount; i++)
	{
		BOOL bCheck = TRUE;
		
		CString strID = agID[i];	
		strID.MakeUpper();
		if( !m_mapDLState.Lookup(strID,bCheck) || !bCheck)
		{
			agID.RemoveAt(i);
			nCount--;
			i--;
		}
	}
	
	return agID.GetSize();
}

int CMainPage::GetDLNVID(CStringArray &agID)
{
	agID.RemoveAll();
	if( NULL == m_pCurProductInfo)
	{		
		return 0;
	}
	CStringArray agNVID;
	int nNVBackupCount = m_pCaliPage->GetNVBackupFiles(agNVID);

	UINT nCount = m_pCurProductInfo->dwFileCount;
	for(UINT i = 0;i<nCount;i++)
	{
		CString strID = m_pCurProductInfo->pFileInfoArr[i].szID;
		CString strType = m_pCurProductInfo->pFileInfoArr[i].szType;
		strID.MakeUpper();
		BOOL bCheck = FALSE;	
		CString strFile;

		m_mapDLState.Lookup(strID,bCheck);
		m_mapDLFiles.Lookup(strID,strFile);		
		
		if(bCheck)
		{
			if(strID.Find(_T("NV"))==0)
			{
				if(!strFile.IsEmpty() && strFile.CompareNoCase(FILE_OMIT) != 0)
				{
					for(int j = 0; j< nNVBackupCount; j++)
					{
						if(strID.CompareNoCase(agNVID[j]) == 0)
						{
							agID.Add(strID);
							break;
						}
					}						
				}
			}
		}			
	}

	return agID.GetSize();
}  

BOOL   CMainPage::IsModifiedDLFile(LPCTSTR lpszDLFile)
{   
    UNUSED_ALWAYS( lpszDLFile );
    BOOL bOK = FALSE;
    FILETIME ftOrg;
    FILETIME ftDst;
    GetLastModifyTime(lpszDLFile,ftDst);
    CString strFileName = GetStrFileTitle(lpszDLFile,TRUE,TRUE);
    if ( m_mapLastTime.Lookup(strFileName,ftOrg) && ftOrg.dwLowDateTime != ftDst.dwLowDateTime && ftOrg.dwHighDateTime != ftDst.dwHighDateTime )
    {
        bOK = TRUE;
    }
    return bOK;
}

BOOL CMainPage::IsLoadFromPac(LPCTSTR lpDLFile)
{
	BOOL bRet = FALSE;
	CString strFileName(lpDLFile);
	if( strFileName.GetLength() > MAX_FLAG_LEN && 0 == strFileName.Right(MAX_FLAG_LEN).Compare(FLAG_FILE_SUFFIX) )
	{
		bRet = TRUE;
	}
	return bRet;

}


BOOL CMainPage::IsMapPBFileBuf()
{
	int nCount = 0;
	if(m_pCurProductInfo == NULL)
		return FALSE;
	
	DWORD nFileNum = m_pCurProductInfo->dwFileCount;
	
	for(UINT i = 0 ; i< nFileNum; i++)
	{
		CString strID = m_pCurProductInfo->pFileInfoArr[i].szID;
		strID.MakeUpper();
		BOOL bCheck = FALSE;	
		CString strFile;
		m_mapDLState.Lookup(strID,bCheck);
		m_mapDLFiles.Lookup(strID,strFile);

		if( bCheck &&m_pCurProductInfo->pFileInfoArr[i].isSelByFlashInfo==1	)
		{
			nCount++;
		}
	}
	if(nCount>0)
		return TRUE;
	return FALSE;
}

DATA_INFO_T CMainPage::GetDataInfo(LPCTSTR lpDLFile)
{
	CString strFile = GetStrFileTitle(lpDLFile,TRUE,TRUE);
	DATA_INFO_T dataInfo;
	if (!m_mapDataInfo.Lookup(strFile,dataInfo))
	{
		dataInfo.llOffset = dataInfo.llSize = 0;
	}
	return dataInfo;

}


BOOL CMainPage::ParseCuRef(LPCTSTR lpFile)
{
    USES_CONVERSION;
    std::ifstream fin(lpFile, std::ios::in);
    char line[1024]={0};
    while(fin.getline(line, sizeof(line)))
    {
        m_listCu.push_back(A2T(line));
    }
    fin.clear();
    fin.close();    
    return TRUE;
}

BOOL CMainPage::CheckCU(LPCTSTR lpCU)
{
    
	if (NULL == lpCU || _tcslen(lpCU) <12)
	{
		return FALSE;
	}
	BOOL bRet=FALSE;
    TCHAR szCu[TCT_CU_REF_LEN]={0};
    memcpy(szCu, lpCU, _tcslen(lpCU)*sizeof(TCHAR));
    for(int i=0; i<m_listCu.size(); ++i)
    {
		LPCTSTR lpSrc = m_listCu[i].operator LPCTSTR();
        if( _tcslen(lpSrc)>=12  &&
			szCu[0] == lpSrc[0] && 
			szCu[1] == lpSrc[1] && 
			szCu[2] == lpSrc[2] && 
			szCu[3] == lpSrc[3] && 
			szCu[4] == lpSrc[4] && 
			szCu[8] == lpSrc[8] && 
			szCu[9] == lpSrc[9] && 
			szCu[10] == lpSrc[10] && 
			szCu[11] == lpSrc[11])
        {
            bRet = TRUE;
            break;
        }
    }

    return bRet;
}