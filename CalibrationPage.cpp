// CalibrationgPage.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "CalibrationPage.h"
#include "supergridctrl.h"
#include "MainPage.h"
#include "SettingsSheet.h"
#include "FileDlg.h"

// NV flag section
static _TCHAR g_sz_NV_PART[] = _T("NV");
static _TCHAR g_sz_NV_FLAG[] = _T("Backup calibration");
static _TCHAR g_sz_NV_REPL[] = _T("Replace flag");
static _TCHAR g_sz_NV_CONTINUE[]=_T("Continue flag");
static _TCHAR g_sz_NV_IMEI_FLAG[] = _T("Backup IMEI");
static _TCHAR g_sz_NV_BACKUP_PATH[]=_T("BackupPath");
static _TCHAR g_sz_NV_SAVE_LOCAL_FLAG[]=_T("SaveToLocalFlag");



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalibrationPage dialog
IMPLEMENT_DYNCREATE(CCalibrationPage, CPropertyPage)


CCalibrationPage::CCalibrationPage()
	: CPropertyPage(CCalibrationPage::IDD)
{
	//{{AFX_DATA_INIT(CCalibrationPage)
	m_strPath = _T("");
	m_bTmpSaveToLocal = FALSE;
	//}}AFX_DATA_INIT

	m_strIniFile = _T("");
	m_strCurProduct = _T("");
	m_strTempCurProduct = _T("");
	m_strLocalPath = _T("Backup");
	m_strPath = m_strLocalPath;

	m_bSaveToLocal = m_bTmpSaveToLocal;
	m_nBackupProdnvMiscdata = 0;

}

CCalibrationPage::~CCalibrationPage()
{
}
void CCalibrationPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalibrationPage)
	DDX_Control(pDX, IDC_LST_FILE_BACKUP, m_lstFileBackup);
	DDX_Control(pDX, IDC_LIST_FRAME, m_stcFrame);
	DDX_Text(pDX, IDC_DC_EDT_PATH, m_strPath);
	DDV_MaxChars(pDX, m_strPath, 256);
	DDX_Check(pDX, IDC_CHK_SAVE_TO_LOCAL, m_bTmpSaveToLocal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalibrationPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCalibrationPage)
	ON_BN_CLICKED(IDC_CHK_SAVE_TO_LOCAL, OnChkSaveToLocal)
	ON_BN_CLICKED(IDC_DC_BTN_BROWSE, OnDcBtnBrowse)
	//}}AFX_MSG_MAP
    ON_NOTIFY(NM_CLICK, IDC_LST_FILE_BACKUP, OnNMClickLstFileBackup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalibrationPage message handlers

BOOL CCalibrationPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_lstBackup.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0,0,0,0),this,0x1001);	
	m_lstBackup.ModifyStyleEx(0,WS_EX_CLIENTEDGE);

    m_lstBackup.FillList(m_vBackFiles.size()?m_vBackFiles[0].szID : NULL);

	InitFileBackupList();

	CRect rc;
	m_stcFrame.GetWindowRect(rc);
	ScreenToClient(rc);
	m_stcFrame.ShowWindow(SW_HIDE);
	if(m_lstBackup.GetSafeHwnd())
		m_lstBackup.MoveWindow(rc);
	
	GetDlgItem(IDC_DC_EDT_PATH)->EnableWindow(m_bSaveToLocal);
	GetDlgItem(IDC_STC_VER_SAP)->EnableWindow(m_bSaveToLocal);
	GetDlgItem(IDC_DC_BTN_BROWSE)->EnableWindow(m_bSaveToLocal);
	GetDlgItem(IDC_STC_LOCAL_PATH)->EnableWindow(m_bSaveToLocal);

	((CEdit *)GetDlgItem(IDC_DC_EDT_PATH))->UpdateData(FALSE);

	m_bTmpSaveToLocal = m_bSaveToLocal;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
BOOL CCalibrationPage::LoadSettings(LPCTSTR pFileName,CString &strErrorMsg)
{
//	UNUSED_ALWAYS(strErrorMsg);
	ASSERT(NULL != pFileName);
    if(NULL == pFileName)
    {
		strErrorMsg += _T("Configure file is empty!\n");
        return FALSE;
    }	
	m_strIniFile = pFileName;
	
//  _TCHAR szBuf[MAX_PATH]={0};  
//	DWORD dwLen = ::GetPrivateProfileString(g_sz_NV_PART,g_sz_NV_BACKUP_PATH,NULL,szBuf,MAX_PATH,pFileName);
//	if(dwLen !=0 )
//	{
//		m_strPath = szBuf;
//		m_strLocalPath = m_strPath;
//	}
	
	int nFind = m_strIniFile.ReverseFind(_T('\\'));
	if(nFind != -1)
	{
		m_strLocalPath = m_strIniFile.Left(nFind);
		m_strLocalPath += _T("\\Backup");
	}

// Download need not save nv to local
// Dloader can set by GUI
// So SPUpgrade must be initialized by configure
#ifdef _SPUPGRADE
	m_bSaveToLocal = ::GetPrivateProfileInt( g_sz_NV_PART, g_sz_NV_SAVE_LOCAL_FLAG, 0, pFileName );
	m_nBackupProdnvMiscdata = ::GetPrivateProfileInt( _T("Options"), _T("BackupProdnvMiscData"), 0, pFileName );
	if ((GetPrivateProfileInt(_T("Options"),_T("SupportCU"),FALSE,pFileName) || GetPrivateProfileInt(_T("Options"),_T("SupportInproduction"),FALSE,pFileName))&& 0 == m_nBackupProdnvMiscdata)
	{
		m_nBackupProdnvMiscdata = 2;
	}
#endif

	if(m_bSaveToLocal && !CreateDeepDirectory(m_strLocalPath))
	{
		CString strErr;
		strErr.Format(_T("Create directory [%s] fail."),m_strLocalPath.operator LPCTSTR());
		AfxMessageBox(strErr);
		return FALSE;
	}
	
	return TRUE;
}
BOOL CCalibrationPage::SaveSettings(LPCTSTR pFileName)
{
    if(NULL == pFileName)
    {
        return FALSE;
    }

	CString strTemp;
    strTemp.Format( _T("%d"), m_bSaveToLocal );
    ::WritePrivateProfileString( g_sz_NV_PART,g_sz_NV_SAVE_LOCAL_FLAG,strTemp,pFileName);

//	::WritePrivateProfileString(g_sz_NV_PART,g_sz_NV_BACKUP_PATH,m_strLocalPath,pFileName);

	return m_lstBackup.SaveSettings(pFileName);
}

void CCalibrationPage::OnOK()
{
    CPropertyPage::OnOK();

	m_bSaveToLocal = m_bTmpSaveToLocal;
    m_strCurProduct = m_strTempCurProduct;
	m_lstBackup.Update();

	((CEdit *)GetDlgItem(IDC_DC_EDT_PATH))->UpdateData();

	UINT i=0;
	for(i=0; i< m_vBackFiles.size(); i++)
	{
		m_vBackFiles[i].isBackup = (BYTE)m_lstFileBackup.GetCheck(i);
	}

// 	if(!m_strPath.IsEmpty())
// 	{
// 		m_strLocalPath=m_strPath;
// 	}

	if(m_bSaveToLocal && m_strLocalPath.GetLength()>2 && !CreateDeepDirectory(m_strLocalPath))
	{
		_ASSERTE( 0 );	//lint !e506 !e774
	}

	//±£´æÉèÖÃ
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
}


BOOL CCalibrationPage::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	this->SetFocus();
	if( !g_bInitSheet)
	{
		m_lstBackup.UpdateTemp();
	}
	/*if(m_lstBackup.m_pNvBkpItmArray == NULL && m_lstBackup.m_pTempNvBkpItmArray != NULL)
	{
		m_lstBackup.Update();
	}*/

	((CEdit *)GetDlgItem(IDC_DC_EDT_PATH))->UpdateData();

	if(m_bTmpSaveToLocal && m_strLocalPath.GetLength()>2 && !CreateDeepDirectory(m_strLocalPath))
	{
		CString strErr;
		strErr.Format(_T("Create directory [%s] fail."),m_strLocalPath.operator LPCTSTR());
		AfxMessageBox(strErr);
		return FALSE;
	}

	return CPropertyPage::OnKillActive();
}

void CCalibrationPage::OnChkSaveToLocal() 
{
	// TODO: Add your control notification handler code here
	m_bTmpSaveToLocal = !m_bTmpSaveToLocal;
	GetDlgItem(IDC_DC_EDT_PATH)->EnableWindow(m_bTmpSaveToLocal);
	GetDlgItem(IDC_STC_VER_SAP)->EnableWindow(m_bTmpSaveToLocal);
	GetDlgItem(IDC_DC_BTN_BROWSE)->EnableWindow(m_bTmpSaveToLocal);
	GetDlgItem(IDC_STC_LOCAL_PATH)->EnableWindow(m_bTmpSaveToLocal);
}

void CCalibrationPage::OnDcBtnBrowse() 
{
	CFileDlg fdlg;
	fdlg.SetHwnOwner(this->m_hWnd);
    CString strDir = fdlg.GetSelectDir();
    if( strDir.IsEmpty() )
        return;
	
	m_strPath = strDir;
	GetDlgItem(IDC_DC_EDT_PATH)->SetWindowText(m_strPath);
}

void CCalibrationPage::InitFileBackupList()
{
	m_lstFileBackup.SetBkColor(RGB(232, 232, 232));	
	m_lstFileBackup.ModifyStyle(0, LVS_SHOWSELALWAYS);
	
	DWORD dwExStyle = m_lstFileBackup.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT;
	dwExStyle |= LVS_EX_GRIDLINES;	
	dwExStyle |= LVS_EX_CHECKBOXES;	
	m_lstFileBackup.SetExtendedStyle(dwExStyle);
	
	//Load Column
	CString strClmn;
	VERIFY( strClmn.LoadString(IDS_FILE_BACK_COLUMN) );	
	int nLen   = strClmn.GetLength();
	LPTSTR lpBuf  = strClmn.GetBuffer(nLen);
	LPTSTR lpFind = _tcschr(lpBuf, _T(','));
	int     nIndex = 0;
	while(lpFind != NULL)
	{
		*lpFind = _T('\0');
		if(nIndex == 1)
			m_lstFileBackup.InsertColumn(nIndex++,lpBuf,LVCFMT_LEFT,100);
		else
			m_lstFileBackup.InsertColumn(nIndex++,lpBuf,LVCFMT_LEFT,100);
		lpBuf = lpFind + 1;
		lpFind = _tcschr(lpBuf, _T(','));
	}
	m_lstFileBackup.InsertColumn(nIndex, lpBuf, LVCFMT_LEFT, 100 );
	strClmn.ReleaseBuffer();	
	
	m_lstFileBackup.EnableToolTips();

	for(UINT i = 0; i< m_vBackFiles.size(); i++)
	{
		m_lstFileBackup.InsertItem(i,m_vBackFiles[i].szID);
		CString strText;
		if(_tcslen(m_vBackFiles[i].arrBlock[0].szRepID) != 0)
		{
			m_lstFileBackup.SetItemText(i,1,m_vBackFiles[i].arrBlock[0].szRepID);
		}
		else
		{				
			strText.Format(_T("0x%08X"),m_vBackFiles[i].arrBlock[0].llBase);
			m_lstFileBackup.SetItemText(i,1,strText);
		}

		strText.Format(_T("0x%X"),m_vBackFiles[i].arrBlock[0].llSize);
		m_lstFileBackup.SetItemText(i,2,strText);

		m_lstFileBackup.SetCheck(i,m_vBackFiles[i].isBackup);
	}

    if (m_vBackFiles.size())
    {
        m_lstFileBackup.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED|LVIS_FOCUSED);
        m_lstFileBackup.SetFocus();
    }
    
}

void CCalibrationPage::AddBackupFiles(FILE_INFO_T *pFileInfoArr, int nCount,BOOL bInit)
{
	m_vBackFiles.clear();

	int nItemCount = 0;
	for(int i = 0; i< nCount; i++)
	{
		FILE_INFO_T * pCur = pFileInfoArr + i;
		if(pCur->isBackup==1 && _tcsnicmp(pCur->szID,_T("NV"),2) != 0)
		{
			m_vBackFiles.push_back(*pCur);
			nItemCount++;
		}
		else if(_tcsnicmp(pCur->szID,_T("NV"),2) == 0 && _tcsnicmp(pCur->szType,_T("NV"),2) == 0 )
		{
			if(pCur->isBackup == 255 || pCur->isBackup == 1)
			{	
				m_vBackFiles.push_back(*pCur);
				m_vBackFiles[nItemCount].isBackup = 1;				
				nItemCount++;
			}			
		}
	}

	if(!bInit)
	{
		m_lstFileBackup.DeleteAllItems();
		UINT i = 0;
		for(i = 0; i< m_vBackFiles.size(); i++)
		{
			CString strText;
			m_lstFileBackup.InsertItem(i,m_vBackFiles[i].szID);
			if(_tcslen(m_vBackFiles[i].arrBlock[0].szRepID) != 0)
			{
				m_lstFileBackup.SetItemText(i,1,m_vBackFiles[i].arrBlock[0].szRepID);
			}
			else
			{				
				strText.Format(_T("0x%08X"),m_vBackFiles[i].arrBlock[0].llBase);
				m_lstFileBackup.SetItemText(i,1,strText);
			}
			
			strText.Format(_T("0x%X"),m_vBackFiles[i].arrBlock[0].llSize);
			m_lstFileBackup.SetItemText(i,2,strText);

			m_lstFileBackup.SetCheck(i);
		}
	}
}

int CCalibrationPage::GetBackupFiles(CStringArray &agID,BOOL bSharkNand)
{
	UNUSED_ALWAYS(bSharkNand);

	agID.RemoveAll();
#ifndef _FACTORY
	for(UINT i= 0; i< m_vBackFiles.size(); i++)
	{
		if(m_vBackFiles[i].isBackup==1 && _tcsnicmp(m_vBackFiles[i].szID,_T("NV"),2) != 0)
		{			
#ifdef _SPUPGRADE
			if(m_nBackupProdnvMiscdata==0)
			{
				if(!bSharkNand)
				{
					// UpgradeDownload omit the prodnv and miscdata
					if(_tcsicmp(m_vBackFiles[i].arrBlock[0].szRepID,_T("miscdata")) == 0 ||
						_tcsicmp(m_vBackFiles[i].arrBlock[0].szRepID,_T("prodnv")) == 0	)
					{
						continue;
					}
				}
			}	
            else if(m_nBackupProdnvMiscdata==2)
            {
                if(!bSharkNand)
                {
                    // UpgradeDownload omit the miscdata
                    if(_tcsicmp(m_vBackFiles[i].arrBlock[0].szRepID,_T("prodnv")) == 0	)
                    {
                        continue;
                    }
                }
            }
#endif

			agID.Add(m_vBackFiles[i].szID);
		}
	}
#endif
	return agID.GetSize();
}

int CCalibrationPage::GetNVBackupFiles(CStringArray &agID)
{
	agID.RemoveAll();
#ifndef _FACTORY
	for(UINT i= 0; i< m_vBackFiles.size(); i++)
	{
		if(m_vBackFiles[i].isBackup==1 && _tcsnicmp(m_vBackFiles[i].szID,_T("NV"),2) == 0)
		{			
			agID.Add(m_vBackFiles[i].szID);
		}
	}
#endif
	return agID.GetSize();
}

void CCalibrationPage::OnNMClickLstFileBackup(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    LPNMITEMACTIVATE pnia = (LPNMITEMACTIVATE)pNMHDR;
    int nItem = pnia->iItem;
    int nSubItem = pnia->iSubItem;
    if(nItem == -1 || nSubItem == -1)
    {
        return;
    }

    CString strID = m_lstFileBackup.GetItemText(nItem,0);
    if (_tcsnicmp(strID.operator LPCTSTR(),_T("NV"),2) == 0)  //is NV file
    {
        m_lstBackup.FillList(strID.operator LPCTSTR(),TRUE);
    }
    else
    {
        m_lstBackup.DeleteAll();
    }
    *pResult = 0;
}

BOOL CCalibrationPage::IsEraseIMEI()
{
    BOOL bErased = FALSE;

    for(int i = 0; i< m_vBackFiles.size(); ++i)
    {
        if ( _tcsnicmp(m_vBackFiles[i].szID,_T("NV"),2) == 0    && 
            _tcsnicmp(m_vBackFiles[i].szType,_T("NV"),2) == 0  )
        {     
            PNV_BACKUP_ITEM_T pNvBkpItem = NULL;
            int nCount = m_lstBackup.GetNvBkpItemCount(m_vBackFiles[i].szID);
            for(int k=0;k<nCount;++k)
            {           
                pNvBkpItem = m_lstBackup.GetNvBkpItemInfo(k,m_vBackFiles[i].szID);
                if ( pNvBkpItem && _tcscmp(pNvBkpItem->szItemName,_T("IMEI"))==0 )
                {
                    if (1 != m_vBackFiles[i].isBackup || 1 != pNvBkpItem->wIsBackup)
                    {
                        bErased = TRUE;
                        break;
                    }
                }
            }
            if (bErased)
            {
                break;
            }

        }
    }
    return bErased;
}
