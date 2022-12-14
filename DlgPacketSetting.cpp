// DlgPacketSetting.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "DlgPacketSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPacketSetting dialog


CDlgPacketSetting::CDlgPacketSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPacketSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPacketSetting)
	m_strPath = _T("");
	m_strVersion = _T("");
	m_strPrdName = _T("");
	m_strPrdAlias = _T("");
	//}}AFX_DATA_INIT
	UNUSED_ALWAYS(IDC_STC_PRD_VER);
	UNUSED_ALWAYS(IDC_STC_SAVE_PATH);
}


void CDlgPacketSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPacketSetting)
	DDX_Control(pDX, IDC_EDT_PRD_ALIAS, m_edtPrdAlias);
	DDX_Control(pDX, IDC_EDT_PRD_VER, m_edtVersion);
	DDX_Control(pDX, IDC_EDT_PATH, m_edtPath);
	DDX_Text(pDX, IDC_EDT_PATH, m_strPath);
	DDX_Text(pDX, IDC_EDT_PRD_VER, m_strVersion);
	DDX_Text(pDX, IDC_PS_STC_PRD_NANE, m_strPrdName);
	DDV_MaxChars(pDX, m_strPrdName, 256);
	DDX_Text(pDX, IDC_EDT_PRD_ALIAS, m_strPrdAlias);
	DDV_MaxChars(pDX, m_strPrdAlias, 99);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPacketSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgPacketSetting)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPacketSetting message handlers

void CDlgPacketSetting::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	if(m_strPath.IsEmpty() || m_strVersion.IsEmpty())
	{
		AfxMessageBox(_T("Save Path and Product version can not be empty!"));
		return;
	}
	int nFind = m_strPath.ReverseFind(_T('\\'));
	if(nFind == -1)
	{
		AfxMessageBox(_T("Save Path is invalidate!"));
		return;
	}
	CString strDir = m_strPath.Left(nFind);
	CFileFind find;
	if(strDir.GetLength() == 2)
	{
		if(strDir.Find(_T(':'))== -1)
		{
			AfxMessageBox(_T("Save Path is invalidate!"));
			return;
		}
	}
	else if(!find.FindFile(strDir))
	{
		AfxMessageBox(_T("Save Path is invalidate!"));
		return;
	}
	
	if(m_strPath.Right(4).CompareNoCase(_T(".pac"))!=0)
	{
		CString str;
		str.Format(_T("%s.pac"),m_strPath.operator LPCTSTR());
		m_strPath = str;
		UpdateData(FALSE);
	}
	
	CDialog::OnOK();
}

void CDlgPacketSetting::OnBtnBrowse() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	static _TCHAR BASED_CODE szFilter[] = _T("Packet Files (*.pac)|*.pac||");
    CString strFile=_T("");	
	strFile = m_strVersion;
    CFileDialog dlg(FALSE, _T(".pac"), strFile, OFN_NOCHANGEDIR |OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);
	
	if(dlg.DoModal() == IDOK)
    {
        // Copy the file path name
        m_strPath = dlg.GetPathName();
		UpdateData(FALSE);		
    }
	m_edtPath.SetFocus();
}
