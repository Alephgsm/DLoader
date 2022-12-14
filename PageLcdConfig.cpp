// PageLcdConfig.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "PageLcdConfig.h"
#include "SettingsSheet.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageLcdConfig property page

IMPLEMENT_DYNCREATE(CPageLcdConfig, CPropertyPage)

CPageLcdConfig::CPageLcdConfig() : CPropertyPage(CPageLcdConfig::IDD)
{
	//{{AFX_DATA_INIT(CPageLcdConfig)
	m_strTips = _T("");
	//}}AFX_DATA_INIT

	m_bListInit = FALSE;
	m_bRightPS = FALSE;
	m_bTmpRightPS = FALSE;
	memset(&m_ftLastWrite,0,sizeof(m_ftLastWrite));
	memset(&m_ftTmpLastWrite,0,sizeof(m_ftTmpLastWrite));

	m_strPSFile = _T("");
	m_strTmpPSFile = m_strPSFile;	
}

CPageLcdConfig::~CPageLcdConfig()
{
}

void CPageLcdConfig::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageLcdConfig)
	DDX_Control(pDX, IDC_LCP_LST_LCD_CONFIG, m_lstLCD);
	DDX_Text(pDX, IDC_LCD_STC_TIPS, m_strTips);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageLcdConfig, CPropertyPage)
	//{{AFX_MSG_MAP(CPageLcdConfig)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LCP_LST_LCD_CONFIG, OnItemchangedLst)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageLcdConfig message handlers

BOOL CPageLcdConfig::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_lstLCD.SetBkColor(RGB(232, 232, 232));	
	m_lstLCD.ModifyStyle(0, LVS_SHOWSELALWAYS);
	
	DWORD dwExStyle = m_lstLCD.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT;
//	dwExStyle |= LVS_EX_GRIDLINES;
	dwExStyle |= LVS_EX_CHECKBOXES;
	
	m_lstLCD.SetExtendedStyle(dwExStyle);

	m_lstLCD.SetColumnWidth(0, 200 );

	m_strTips.LoadString(IDS_LCD_TIPS);

	ShowLCDCfig(m_bRightPS);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
BOOL CPageLcdConfig::LoadSettings(LPCTSTR pFileName,CString &/*strErrorMsg*/)
{
	UNUSED_ALWAYS(pFileName);
	//UNUSED_ALWAYS(strErrorMsg);
	return TRUE;

}
BOOL CPageLcdConfig::SaveSettings(LPCTSTR pFileName)
{
	UNUSED_ALWAYS(pFileName);
	return TRUE;
}

BOOL CPageLcdConfig::FillList()
{
	m_bListInit = FALSE;

	int nLcdCfigNum = m_vTmpLCDCfig.size();
	m_lstLCD.DeleteAllItems();

	for(int i=0; i<nLcdCfigNum;i++)
	{	
		int nItem = m_lstLCD.GetItemCount();
		m_lstLCD.InsertItem(nItem,m_vTmpLCDCfig[i].szName);

		if(m_vTmpLCDCfig[i].dwFlag > 0)
		{
			m_lstLCD.SetCheck(nItem);
		}		
	}

	m_bListInit = TRUE;

	return TRUE;
}

void CPageLcdConfig::ShowLCDCfig(BOOL bShow)
{		
	m_lstLCD.ShowWindow(bShow?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_LCD_STC_TIPS)->ShowWindow(bShow?SW_HIDE:SW_SHOW);
	FillList();
}

void  CPageLcdConfig::SetLCDCfig(BOOL bRightPS,VEC_LCD_CFIG &vLcdCfig,FILETIME *pLastWrite,BOOL bShowWnd,LPCTSTR lpszPSFile)
{
	if(IsSamePS(pLastWrite,bShowWnd))
	{
		return;
	}
	int i = 0;
	m_vTmpLCDCfig.clear();
	int nSize = vLcdCfig.size();
	for(i = 0; i< nSize; i++)
	{
		m_vTmpLCDCfig.push_back(vLcdCfig[i]);
	}

	m_bTmpRightPS = bRightPS;
	memcpy(&m_ftTmpLastWrite,pLastWrite,sizeof(FILETIME));
	m_strTmpPSFile = lpszPSFile;
	if(!bShowWnd)
	{
		m_vLCDCfig.clear();
		for(i = 0; i< nSize; i++)
		{
			m_vLCDCfig.push_back(vLcdCfig[i]);
		}
		m_bRightPS = bRightPS;
		memcpy(&m_ftLastWrite,pLastWrite,sizeof(FILETIME));
		m_strPSFile = lpszPSFile;
	}

	if(bShowWnd)
	{
		ShowLCDCfig(bRightPS);
	}
}

void CPageLcdConfig::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class		
	UpdateData();

	m_strPSFile = m_strTmpPSFile;
	m_bRightPS    = m_bTmpRightPS;
	memcpy(&m_ftLastWrite,&m_ftTmpLastWrite,sizeof(FILETIME));

	int nCount = m_lstLCD.GetItemCount();
	int nSize = m_vTmpLCDCfig.size();
	ASSERT(nSize == nCount);

	m_vLCDCfig.clear();
	int i = 0;
	for(i = 0; i< nSize; i++)
	{
		m_vLCDCfig.push_back(m_vTmpLCDCfig[i]);
	}	

	for(i= 0; i<nCount; i++)
	{
		m_vTmpLCDCfig[i].dwFlag = m_lstLCD.GetCheck(i);
		m_vLCDCfig[i].dwFlag = m_lstLCD.GetCheck(i);
	}

	SavePSFile();

	CPropertyPage::OnOK();

}

BOOL CPageLcdConfig::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	
	return CPropertyPage::OnKillActive();
}

BOOL CPageLcdConfig::IsFileTimeEqual(FILETIME *pft1,FILETIME *pft2)
{
	if(pft1->dwHighDateTime==pft2->dwHighDateTime &&
	   pft1->dwLowDateTime ==pft2->dwLowDateTime )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CPageLcdConfig::IsSamePS(FILETIME *pLastWrite, BOOL bShowWnd)
{
	if(IsFileTimeEqual(pLastWrite,bShowWnd ? (&m_ftTmpLastWrite) : (&m_ftLastWrite) ))
	{
		return TRUE;
	}

	return FALSE;	
}

void CPageLcdConfig::OnItemchangedLst(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(!m_bListInit)
	{
		return;
	}

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int nItem = pNMListView->iItem;
	int nIndex = m_lstLCD.GetItemData(nItem);

	if(nIndex >= (int)m_vTmpLCDCfig.size())
	{
		return;
	}

	BOOL bExistCheckFlag = FALSE;
	int i=0;
	for(i = 0; i< m_lstLCD.GetItemCount();i++)
	{
		if(i != nItem && m_lstLCD.GetCheck(i))
		{					
			bExistCheckFlag = TRUE;
			break;				
		}
	}

	if(bExistCheckFlag)
	{
		if(pNMListView->uNewState == 0x00002000) // check
		{
			for(i = 0; i< m_lstLCD.GetItemCount();i++)
			{
				if(i != nItem && m_lstLCD.GetCheck(i))
				{					
					m_lstLCD.SetCheck(i,FALSE);				
				}
			}
		}		
	}
	else
	{
		if(pNMListView->uNewState == 0x00001000 ) // uncheck
		{	
			AfxMessageBox(_T("Must select one LCD configure!!!"));
			m_lstLCD.SetCheck(nItem);
		}
	}	
	
    UpdateData(FALSE);
	*pResult = 0;
}

BOOL CPageLcdConfig::CheckLCDConfig(VEC_LCD_CFIG &vLcdCfig1,VEC_LCD_CFIG &vLcdCfig2)
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

BOOL CPageLcdConfig::SavePSFile()
{
	CString strPSFileName = m_strPSFile;	
	
	if(m_bRightPS && !strPSFileName.IsEmpty() && strPSFileName.CompareNoCase(FILE_OMIT) != 0)
	{	
		int nVSize = m_vLCDCfig.size();
		if(nVSize == 0)
		{
			return TRUE;
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
			AfxMessageBox(strFormatted);
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

		CSettingsSheet * pParent = (CSettingsSheet*)GetParent();

		if(dwSize != 0)
		{
			VEC_LCD_CFIG vLcdCfig;
			if(pParent->FindLCDItem(strPSFileName,vLcdCfig))
			{
				if(CheckLCDConfig(vLcdCfig,m_vLCDCfig))
				{
					int nSize = vLcdCfig.size();
					for(int i = 0; i<nSize; i++)
					{
						*(pBuf+vLcdCfig[i].dwFlagOffset) = (BYTE)m_vLCDCfig[i].dwFlag; //lint !e613
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
							AfxMessageBox(strFormatted);
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
					strFormatted.Format(_T("PS/USER_IMG file [%s] is changed,please select it again."),strPSFileName.operator LPCTSTR());
					AfxMessageBox(strFormatted);
					SAFE_DELETE_ARRAY(pBuf);
					return FALSE;
				}
			}					
		}	
		SAFE_DELETE_ARRAY(pBuf);
	}

	return TRUE;
}