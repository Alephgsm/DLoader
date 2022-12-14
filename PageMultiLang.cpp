// PageMultiLang.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "PageMultiLang.h"
#include "SettingsSheet.h"
#include "Calibration_Struct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_LANG_IMG_IDX  (1)
#define NOT_DEF_LANG_IMG_IDX  (0)

/////////////////////////////////////////////////////////////////////////////
// CPageMultiLang property page

IMPLEMENT_DYNCREATE(CPageMultiLang, CPropertyPage)

CPageMultiLang::CPageMultiLang() : CPropertyPage(CPageMultiLang::IDD)
{
	//{{AFX_DATA_INIT(CPageMultiLang)
	m_bBackup = FALSE;
	m_strTips = _T("");
	//}}AFX_DATA_INIT
	m_bListInit = FALSE;
	m_bRightNV = FALSE;
	m_bBackupLang = FALSE;

	m_bTmpRightNV = FALSE;
	memset(&m_ftLastWrite,0,sizeof(m_ftLastWrite));
	memset(&m_ftTmpLastWrite,0,sizeof(m_ftTmpLastWrite));

	m_pImglist = NULL;
}

CPageMultiLang::~CPageMultiLang()
{
/*lint -save -e1551 */
	m_agLangFlag.RemoveAll();
	m_agLangNames.RemoveAll();

	m_agTmpLangFlag.RemoveAll();
	
	if(m_pImglist != NULL)
	{
		m_pImglist->DeleteImageList();
		delete m_pImglist;
		m_pImglist = NULL;
	}
/*lint -restore */
}

void CPageMultiLang::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageMultiLang)
	DDX_Control(pDX, IDC_DML_LST_LANG, m_lstLang);
	DDX_Check(pDX, IDC_DML_CHK_BACKUP_LANG, m_bBackup);
	DDX_Text(pDX, IDC_DML_STC_TIPS, m_strTips);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageMultiLang, CPropertyPage)
	//{{AFX_MSG_MAP(CPageMultiLang)
	ON_BN_CLICKED(IDC_DML_CHK_BACKUP_LANG, OnChkBackupLang)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DML_LST_LANG, OnItemchangedLst)
	ON_NOTIFY(NM_DBLCLK, IDC_DML_LST_LANG, OnDblclkDmlLstLang)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageMultiLang message handlers

BOOL CPageMultiLang::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pImglist = new CImageList();
	m_pImglist->Create(IDB_BMP_LANG_ICON,16,2,RGB(255,0,255));

	m_lstLang.SetBkColor(RGB(232, 232, 232));	
	m_lstLang.ModifyStyle(0, LVS_SHOWSELALWAYS);
	
	m_lstLang.SetImageList(m_pImglist,LVSIL_SMALL);
	
	DWORD dwExStyle = m_lstLang.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT;
//	dwExStyle |= LVS_EX_GRIDLINES;
	dwExStyle |= LVS_EX_CHECKBOXES;
	
	m_lstLang.SetExtendedStyle(dwExStyle);
	m_lstLang.SetColumnWidth(0,200);

	m_bBackup = m_bBackupLang;
	if(m_bBackup)
	{
		m_lstLang.EnableWindow(FALSE);
	}
	else
	{
		m_lstLang.EnableWindow(TRUE);
	}

	ShowLang(m_bRightNV);

	m_strTips.LoadString(IDS_MULTI_LANG_TIPS);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
BOOL CPageMultiLang::LoadSettings(LPCTSTR pFileName,CString &strErrorMsg)
{
	if(NULL == pFileName)
    {
		strErrorMsg += _T("Configure file is empty!\n");
        return FALSE;
    }
	
	_TCHAR szAppPath[_MAX_PATH]={0};
	GetModuleFilePath(g_theApp.m_hInstance,szAppPath);
	CString strBMFileType;
	strBMFileType.Format(_T("%s\\BMFileType.ini"),szAppPath);

    _TCHAR szKeyValue[ MAX_BUF_SIZE ]={0}; //hongliang.xin 2007-12-11 for ocupying CPU 50%
    GetPrivateProfileSection( _T("MultiLangs"), szKeyValue, MAX_BUF_SIZE, strBMFileType.operator LPCTSTR() );
	m_agLangNames.RemoveAll();

	if(_tcslen(szKeyValue) == 0)
	{
		return TRUE;
	}

	CStringArray arrKeyData;    
	UINT nLangNum = (UINT)EnumKeys(szKeyValue,&arrKeyData);
	UINT i=0;

	CString strCheckFile;
	CString strCheck;
//	int nCheck;
//	int nFind = -1;
	for(i=0;i<nLangNum;i++)
	{
		m_agLangNames.Add(arrKeyData[i*2+1]);
	}

	return TRUE;

}
BOOL CPageMultiLang::SaveSettings(LPCTSTR pFileName)
{
	pFileName;
	return TRUE;
}

BOOL CPageMultiLang::FillList()
{
	m_bListInit = FALSE;

	int nLangNum = m_agLangNames.GetSize();

	m_lstLang.DeleteAllItems();

	// m_agTmpLangFlag is temp var
	int nLangNumInNV = m_agTmpLangFlag.GetSize();

	int nItem = 0;
	
	int nFirstCheckLang = -1;
	BOOL bFlag = FALSE;

	CString strLangName;

	for(int i=0; i<nLangNumInNV;i++)
	{
		// m_agTmpLangFlag is temp var
		BYTE flag = (BYTE)m_agTmpLangFlag.GetAt(i);
		if(IS_LANG_SUPPORT(flag))
		{
			if(i>=nLangNum)
			{
				strLangName = _T("Unknown");
			}
			else
			{
			    strLangName = m_agLangNames.GetAt(i);
			}
			int nImgIdx = IS_LANG_DEFAULT(flag)?DEFAULT_LANG_IMG_IDX:NOT_DEF_LANG_IMG_IDX; 
			if(nImgIdx == DEFAULT_LANG_IMG_IDX)
			{
				bFlag = TRUE;
			}
			m_lstLang.InsertItem(nItem,strLangName,nImgIdx);
			m_lstLang.SetItemData(nItem,(DWORD)i);
			if(!IS_LANG_CAN_DISABLE(flag) || IS_LANG_ENABLE(flag))
			{
				m_lstLang.SetCheck(nItem);

				if(nFirstCheckLang == -1)
				{
					nFirstCheckLang = nItem;
				}
			}

			nItem++;
		}		
	}

	if(!bFlag && nFirstCheckLang != -1)
	{
		LVITEM lv = {0};
		lv.iItem = nFirstCheckLang;
		lv.iSubItem = 0;
		lv.mask = LVIF_IMAGE;
		lv.iImage = DEFAULT_LANG_IMG_IDX;
		m_lstLang.SetItem(&lv);

	}

	m_bListInit = TRUE;

	return TRUE;
}

void CPageMultiLang::OnChkBackupLang() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_bBackup)
	{
		m_lstLang.EnableWindow(FALSE);
	}
	else
	{
		m_lstLang.EnableWindow(TRUE);
	}
}
void CPageMultiLang::OnItemchangedLst(NMHDR* pNMHDR, LRESULT* pResult)
{
	if(!m_bListInit)
	{
		return;
	}

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int nItem = pNMListView->iItem;
	int nIndex = m_lstLang.GetItemData(nItem);

	if(nIndex >= m_agTmpLangFlag.GetSize())
	{
		return;
	}
	
	BYTE flag = (BYTE)m_agTmpLangFlag.GetAt(nIndex);

	if(IS_LANG_CAN_DISABLE(flag))
	{
		if(pNMListView->uNewState == 0x00001000 ) // uncheck
		{
			LVITEM lv={0};
			lv.iItem = pNMListView->iItem;
			lv.iSubItem = pNMListView->iSubItem;
			lv.mask = LVIF_IMAGE;
			lv.iImage = NOT_DEF_LANG_IMG_IDX;
			m_lstLang.SetItem(&lv);

			int nFirstCheckLang = -1;
			BOOL bFlag = FALSE;

			for(int i = 0; i< m_lstLang.GetItemCount();i++)
			{
				if(i != nItem && m_lstLang.GetCheck(i))
				{
					if(nFirstCheckLang == -1)
					{
						nFirstCheckLang = i;
					}
					lv.iItem = i;
					m_lstLang.GetItem(&lv);
					if(lv.iImage == DEFAULT_LANG_IMG_IDX)
					{
						bFlag = TRUE;
						break;
					}
				}
			}

			if(!bFlag && nFirstCheckLang != -1)
			{
				lv.iItem = nFirstCheckLang;
				lv.iImage = DEFAULT_LANG_IMG_IDX;
				m_lstLang.SetItem(&lv);
			}
		}
		else if(pNMListView->uNewState == 0x00002000 ) // check
		{			
			BOOL bExistCheckFlag = FALSE;
			for(int i = 0; i< m_lstLang.GetItemCount();i++)
			{
				if(i != nItem && m_lstLang.GetCheck(i))
				{					
					bExistCheckFlag = TRUE;
					break;				
				}
			}
			if(!bExistCheckFlag)
			{
				LVITEM lv={0};
				lv.iItem = pNMListView->iItem;
				lv.iSubItem = pNMListView->iSubItem;
				lv.mask = LVIF_IMAGE;
				lv.iImage = DEFAULT_LANG_IMG_IDX;
				m_lstLang.SetItem(&lv);
			}
		}
		return;
	}

	if(pNMListView->uNewState == 0x00001000 )
	{
		CString strWarning;
		strWarning.Format(_T("Must select %s!!!"),m_lstLang.GetItemText(nItem,0).operator LPCTSTR());
		AfxMessageBox(strWarning);
		m_lstLang.SetCheck(nItem);
	}
	
    UpdateData(FALSE);
	*pResult = 0;
}

void CPageMultiLang::ShowLang(BOOL bShow)
{
	if(bShow)
	{		
		m_lstLang.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DML_CHK_BACKUP_LANG)->ShowWindow(SW_SHOW);	
		GetDlgItem(IDC_DML_STC_NOTICE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DML_STC_TIPS)->ShowWindow(SW_HIDE);
	}
	else
	{
		m_lstLang.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DML_CHK_BACKUP_LANG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DML_STC_NOTICE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DML_STC_TIPS)->ShowWindow(SW_SHOW);
	}

#ifdef _FACTORY
	GetDlgItem(IDC_DML_CHK_BACKUP_LANG)->ShowWindow(SW_HIDE);
#endif

	FillList();
}

void  CPageMultiLang::SetLangFlag(BOOL bRightNV,const CUIntArray &agLangFlag,FILETIME *pLastWrite,BOOL bShowWnd)
{
	if(IsSameNV(pLastWrite,bShowWnd))
	{
		return;
	}

	m_agTmpLangFlag.RemoveAll();
	m_agTmpLangFlag.Copy(agLangFlag);
	m_bTmpRightNV = bRightNV;
	memcpy(&m_ftTmpLastWrite,pLastWrite,sizeof(FILETIME));

	if(!bShowWnd)
	{
		m_agLangFlag.RemoveAll();
		m_agLangFlag.Copy(agLangFlag);
		m_bRightNV = bRightNV;
		memcpy(&m_ftLastWrite,pLastWrite,sizeof(FILETIME));
	}

	if(bShowWnd)
	{
		ShowLang(bRightNV);
	}
}

void CPageMultiLang::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class		
	UpdateData();
	m_bBackupLang = m_bBackup;
	m_bRightNV    = m_bTmpRightNV;
	memcpy(&m_ftLastWrite,&m_ftTmpLastWrite,sizeof(FILETIME));

	int nCount = m_lstLang.GetItemCount();

	m_agLangFlag.RemoveAll();
	m_agLangFlag.Copy(m_agTmpLangFlag);

	ASSERT(m_agLangFlag.GetSize() >= nCount);

	LVITEM lv = {0};
	lv.mask = LVIF_IMAGE;
	lv.iSubItem = 0;
	

	for(int i= 0; i<nCount; i++)
	{
		lv.iItem = i;
		m_lstLang.GetItem(&lv);
		int nIndex = m_lstLang.GetItemData(i);
		ASSERT(nIndex < m_agLangFlag.GetSize());
		BYTE flag = (BYTE)m_agLangFlag.GetAt(nIndex);
		if(m_lstLang.GetCheck(i))
		{
			flag = (BYTE)MAKE_LANG_ENABLE(flag);
		}
		else
		{
			flag = (BYTE)MAKE_LANG_DISABLE(flag);
		}

		if(lv.iImage == DEFAULT_LANG_IMG_IDX)
		{
			flag = (BYTE)MAKE_LANG_DEFAULT(flag);
		}
		else
		{
			flag = (BYTE)MAKE_LANG_NOTDEFAULT(flag);
		}
		

		m_agLangFlag.SetAt(nIndex,(UINT)flag);
		m_agTmpLangFlag.SetAt(nIndex,(UINT)flag);
	}

	CPropertyPage::OnOK();
}


BOOL CPageMultiLang::IsFileTimeEqual(FILETIME *pft1,FILETIME *pft2)
{
	if(pft1->dwHighDateTime==pft2->dwHighDateTime &&
	   pft1->dwLowDateTime ==pft2->dwLowDateTime )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CPageMultiLang::IsSameNV(FILETIME *pLastWrite, BOOL bShowWnd)
{
	if(IsFileTimeEqual(pLastWrite,bShowWnd ? (&m_ftTmpLastWrite) : (&m_ftLastWrite) ))
	{
		return TRUE;
	}

	return FALSE;	
}


void CPageMultiLang::OnDblclkDmlLstLang(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	LPNMITEMACTIVATE pnia = (LPNMITEMACTIVATE)pNMHDR;

	int nItem = pnia->iItem;
	int nSubItem = pnia->iSubItem;
	if(nItem == -1 || nSubItem != 0)
		return;

	if(!m_lstLang.GetCheck(nItem))
	{
		return;
	}

	LVITEM lv = {0};
	lv.iItem = nItem;
	lv.iSubItem = nSubItem;
	lv.mask = LVIF_IMAGE;
// 	m_lstLang.GetItem(&lv);
// 
// 	if(lv.iImage == DEFAULT_LANG_IMG_IDX)
// 	{
// 		*pResult = 0;
// 		return;
// 	}
// 	else
	{
		lv.iImage = DEFAULT_LANG_IMG_IDX;
		m_lstLang.SetItem(&lv);

		int nCount = m_lstLang.GetItemCount();
		for(int i= 0; i< nCount; i++)
		{
			if(i!= nItem)
			{
				lv.iItem = i;
				lv.iImage = NOT_DEF_LANG_IMG_IDX;
				m_lstLang.SetItem(&lv);
			}
		}
		
	}

	*pResult = 0;
}
