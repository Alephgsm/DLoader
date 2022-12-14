// DlgPassword.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "DlgPassword.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  MAX_PASSWORD_LEN  (20)
#define  DEFAULT_PASSWORD  _T("admin")

/////////////////////////////////////////////////////////////////////////////
// CDlgPassword dialog


CDlgPassword::CDlgPassword(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPassword::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPassword)
	m_strPassword = _T("");
	m_strNewPassword = _T("");
	m_strNewPassword2 = _T("");
	//}}AFX_DATA_INIT
	m_bReset = FALSE;
}


void CDlgPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPassword)
	DDX_Text(pDX, IDC_EDT_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_EDT_NEW_PASSWORD, m_strNewPassword);
	DDX_Text(pDX, IDC_EDT_NEW_PASSWORD2, m_strNewPassword2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPassword, CDialog)
	//{{AFX_MSG_MAP(CDlgPassword)
	ON_EN_CHANGE(IDC_EDT_PASSWORD, OnChangePassword)
	ON_EN_CHANGE(IDC_EDT_NEW_PASSWORD, OnChangeNewPassword)
	ON_EN_CHANGE(IDC_EDT_NEW_PASSWORD2, OnChangeNewPassword2)
	ON_BN_CLICKED(IDC_BTN_RESET, OnReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPassword message handlers

void CDlgPassword::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	if(m_strPassword.IsEmpty())
	{
		AfxMessageBox(_T("Password cannot be empty!"));
		return;
	}
	if(!CheckPassword())
	{
		AfxMessageBox(_T("Password is wrong!"));			
		return;
	}
	
	CDialog::OnOK();
}

BOOL CDlgPassword::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ModifyBorder();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgPassword::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	CWnd * pCurWnd = GetFocus();
	CWnd * pPasswordWnd = GetDlgItem(IDC_EDT_PASSWORD);
	CWnd * pNewPasswordWnd = GetDlgItem(IDC_EDT_NEW_PASSWORD);
	CWnd * pNewPasswordWnd2 = GetDlgItem(IDC_EDT_NEW_PASSWORD2);
	if(pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_ESCAPE)	
		{		
			return TRUE;
		}
	}
	if( pMsg->message == WM_CHAR && pCurWnd != NULL && 
		(pCurWnd->GetSafeHwnd() == pPasswordWnd->GetSafeHwnd() || 
		 pCurWnd->GetSafeHwnd() == pNewPasswordWnd->GetSafeHwnd() ||
		 pCurWnd->GetSafeHwnd() == pNewPasswordWnd2->GetSafeHwnd()) )	
	{		
		if(pMsg->wParam == VK_RETURN)	
		{
			OnOK();			
			return TRUE;
		}
		
		_TCHAR cValue = (_TCHAR)(pMsg->wParam);
		if( !( (cValue >= _T('0') && cValue <= _T('9')) || 
			 (cValue >= _T('A') && cValue <= _T('Z')) ||
			 (cValue >= _T('a') && cValue <= _T('z')) ||
			 //cValue == 3 || cValue == 22 || cValue == 24 || //Ctrl+c Ctrl+v Ctrl+x
			 cValue == VK_BACK))
		{
			return TRUE;
		}
		
	}	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgPassword::OnChangePassword() 
{	
	UpdateData();
	if(m_strPassword.GetLength() > MAX_PASSWORD_LEN)
	{
		m_strPassword.Delete(MAX_PASSWORD_LEN,1);
		//UpdateData(FALSE);
		int nSelStart, nSelEnd;
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDT_PASSWORD);
		pEdit->GetSel(nSelStart, nSelEnd);
		pEdit->SetWindowText(m_strPassword);
		pEdit->SetSel(nSelStart-1, nSelEnd-1);

	}

}

void CDlgPassword::OnChangeNewPassword() 
{
	UpdateData();
	if(m_strNewPassword.GetLength() > MAX_PASSWORD_LEN)
	{
		m_strNewPassword.Delete(MAX_PASSWORD_LEN,1);
		//UpdateData(FALSE);
		int nSelStart, nSelEnd;
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDT_NEW_PASSWORD);
		pEdit->GetSel(nSelStart, nSelEnd);
		pEdit->SetWindowText(m_strNewPassword);
		pEdit->SetSel(nSelStart-1, nSelEnd-1);
	}
}

void CDlgPassword::OnChangeNewPassword2() 
{
	UpdateData();
	if(m_strNewPassword2.GetLength() > MAX_PASSWORD_LEN)
	{
		m_strPassword.Delete(MAX_PASSWORD_LEN,1);
		//UpdateData(FALSE);
		int nSelStart, nSelEnd;
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDT_NEW_PASSWORD2);
		pEdit->GetSel(nSelStart, nSelEnd);
		pEdit->SetWindowText(m_strNewPassword2);
		pEdit->SetSel(nSelStart-1, nSelEnd-1);
	}
}


void CDlgPassword::OnReset() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	m_bReset = !m_bReset;
	if(m_bReset) // 想要设置新密码
	{
		if(!CheckPassword())//先检查旧密码
		{
			m_bReset = FALSE;
			AfxMessageBox(_T("Old password is wrong!"));			
			return;
		}
		else
		{
			GetDlgItem(IDC_EDT_PASSWORD)->EnableWindow(FALSE);
			GetDlgItem(IDOK)->EnableWindow(FALSE);	
		}
	}
	
	if(!m_bReset) //设置新密码
	{
		if(m_strNewPassword.IsEmpty() || m_strNewPassword2.IsEmpty())
		{
			AfxMessageBox(_T("Please input new password!"));			
			return;
		}
		if(m_strNewPassword.Compare(m_strNewPassword2)!=0)
		{
			AfxMessageBox(_T("Passwords are not same!"));			
			return;
		}

		GetDlgItem(IDC_EDT_PASSWORD)->EnableWindow(TRUE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);	

		CString strEncode;

		EncodeString(m_strNewPassword,strEncode);	
		_TCHAR szIniFile[_MAX_PATH] = {0};
	
		if(g_theApp.GetIniFilePath(szIniFile))
		{
			WritePrivateProfileString(_T("GUI"),_T("password"),strEncode,szIniFile);
		}			
					   
	}
	m_strPassword.Empty();
	UpdateData(FALSE);

	ModifyBorder();
}

BOOL CDlgPassword::CheckPassword()
{
	_TCHAR szBuf[_MAX_PATH] = {0};
	_TCHAR szIniFile[_MAX_PATH] = {0};

	if(!g_theApp.GetIniFilePath(szIniFile))
	{
		return FALSE;
	}

	GetPrivateProfileString(_T("GUI"),_T("password"),_T(""),szBuf,_MAX_PATH,szIniFile);

	int nLen = _tcslen(szBuf);
	if(nLen==0)
	{
		if(m_strPassword.Compare(DEFAULT_PASSWORD)==0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		CString strPasswordTmp = szBuf;
		CString strPassword;
		DecodeString(strPasswordTmp,strPassword);
		if(m_strPassword.Compare(strPassword)==0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}		
	}
}

BOOL CDlgPassword::EncodeString(const CString &strSrc,CString &strDst)
{
	strDst = _T("");
	int nLen = strSrc.GetLength();
	for(int i = 0; i< nLen; i++)
	{
		_TCHAR c = strSrc.GetAt(i);
		CString strTmp;
		strTmp.Format(_T("%02X"),c);
		strDst+=strTmp;
	}
	return TRUE;
}
BOOL CDlgPassword::DecodeString(const CString &strSrc,CString &strDst)
{
	strDst = _T("");
	int nLen = strSrc.GetLength()/2;
	for(int i = 0; i< nLen; i++)
	{
		_TCHAR szBuf[3]={0};
		szBuf[0] = strSrc.GetAt(2*i);
		szBuf[1] = strSrc.GetAt(2*i+1);
		_TCHAR szBuf2[2]={0};
		int nChar=0;
		_stscanf(szBuf,_T("%02X"), &nChar);
		szBuf2[0]= (_TCHAR)nChar;
		strDst+=szBuf2;
	}
	return TRUE;
}

void CDlgPassword::ModifyBorder()
{
	UINT uIDMove[] = {
		IDOK,
		IDC_BTN_RESET,
		IDCANCEL
	};
	UINT uIDHideShow[] = {
 		IDC_STC_GP_NEW,
 		IDC_STC_NEW_PW,
 		IDC_STC_NEW_PW2,
		IDC_EDT_NEW_PASSWORD,
		IDC_EDT_NEW_PASSWORD2
	};

	CRect rcDlg;
	CRect rcGPNew;
	GetWindowRect(&rcDlg);
	GetDlgItem(IDC_STC_GP_NEW)->GetWindowRect(&rcGPNew);

	int nDeltH = 0;
	CRect rc;
	CWnd * pWnd =NULL;


	nDeltH = rcGPNew.Height();
	if(m_bReset)
	{
		nDeltH = -nDeltH;
	}

	UINT i = 0;
	for(i =0 ; i< sizeof(uIDMove)/sizeof(uIDMove[0]); i++)
	{	
		pWnd = GetDlgItem(uIDMove[i]);
		pWnd->GetWindowRect(&rc);
		rc.top -= nDeltH;
		rc.bottom -= nDeltH;
		ScreenToClient(&rc);
		pWnd->MoveWindow(&rc);
	}

	for(i =0 ; i< sizeof(uIDHideShow)/sizeof(uIDHideShow[0]); i++)
	{	
		GetDlgItem(uIDHideShow[i])->ShowWindow(m_bReset?SW_SHOW:SW_HIDE);
	}
	rcDlg.bottom -= nDeltH;
	MoveWindow(rcDlg);
	Invalidate();

}