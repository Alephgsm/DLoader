// BarcodeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BarcodeDlg.h"
#include "DLoader.h"
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDLE_TIMER      11
#define IDLE_TIMEOUT    100


/////////////////////////////////////////////////////////////////////////////
// CBarcodeDlg dialog

CBarcodeDlg::CBarcodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBarcodeDlg::IDD, pParent)
{
	m_strBarcode = _T("");
    m_dwTick = 0;
    m_nPort = -1;
 	m_nBarcodeLength = 14;
	m_bAutoStart = FALSE;
	m_bAutoInputSN = TRUE;
	m_bSNWithDateTime = FALSE;

}

CBarcodeDlg::CBarcodeDlg(int nPort,CWnd* pParent /*= NULL*/)
    : CDialog(CBarcodeDlg::IDD, pParent)
{
    CBarcodeDlg::CBarcodeDlg(pParent);

    m_dwTick = 0;
    m_nPort = nPort;
	m_nBarcodeLength = 14;
	m_bAutoStart = FALSE;
	m_bAutoInputSN = TRUE;
	m_bSNWithDateTime = FALSE;
}

CBarcodeDlg::~CBarcodeDlg()
{

}

void CBarcodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBarcodeDlg)
	DDX_Control(pDX, IDC_BUTTON_SUSPEND, m_btnPause);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_EDIT_BARCODE, m_edtBarcode);
	DDX_Text(pDX, IDC_EDIT_BARCODE, m_strBarcode);
//	DDV_MaxChars(pDX, m_strBarcode, 14);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBarcodeDlg, CDialog)
	//{{AFX_MSG_MAP(CBarcodeDlg)
	ON_BN_CLICKED(IDC_BUTTON_SUSPEND, OnButtonSuspend)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_BARCODE, OnChangeEditBarcode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarcodeDlg message handlers

BOOL CBarcodeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UNUSED_ALWAYS(IDI_ICON_OK);
	UNUSED_ALWAYS(IDI_ICON_PAUSE);

	//Set ICON
	//m_btnOK.SetIcon(IDI_ICON_OK);
    //m_btnOK.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 10);
	m_btnOK.SetIcon(::LoadIcon(NULL,MAKEINTRESOURCE(IDI_ICON_OK)));

	m_btnOK.EnableWindow(FALSE);	

	//m_btnPause.SetIcon(IDI_ICON_PAUSE);
    //m_btnPause.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 10);
	
	//Load Config
	TCHAR szFilePath[_MAX_PATH] = {0};
	if(!g_theApp.GetIniFilePath(szFilePath))
	{
		return FALSE;
	}
    _TCHAR szBuf[_MAX_PATH] = {0};
    GetPrivateProfileString(_T("SN"),_T("FixedSN"),_T(""),szBuf,_MAX_PATH,szFilePath);
	m_strFixedSN.Empty();
    m_strFixedSN = szBuf;
	   
	m_nBarcodeLength = GetPrivateProfileInt(_T("SN"),  _T("SN_LENGTH"), 14, szFilePath);
    m_bAutoStart      = GetPrivateProfileInt(_T("SN"),  _T("AutoStartAfterInputSN"), 1, szFilePath);
	m_bSNWithDateTime = GetPrivateProfileInt(_T("SN"),  _T("WithDataTime"), 0, szFilePath);    

	//wei.song 20140901
	if (g_theApp.m_bEnableFlow)
	{
		m_nBarcodeLength = g_theApp.m_nSnLength;
		m_bAutoStart = FALSE;
		m_bAutoInputSN = FALSE;
	}

	//wei.song 20151020
	if (g_theApp.m_bSprdMESEnable)
	{
		m_nBarcodeLength = g_theApp.m_BatchInfo.nSN1_Length;
		m_bAutoStart = FALSE;
		m_bAutoInputSN = FALSE;
	}

	//Set Title
	m_str_title.Format(_T("PORT:%d, Please input SN(Length: %d):"), m_nPort, m_nBarcodeLength);
	SetWindowText(m_str_title);	   
	m_edtBarcode.SetLimitText(m_nBarcodeLength);	
 
	UpdateData(FALSE);  	
   
    m_edtBarcode.SetFocus();
	if (m_bAutoInputSN)
	{	
		CString strSN = _T("");
        if (!m_strFixedSN.IsEmpty())
        {
            if (m_strFixedSN.GetLength()>=m_nBarcodeLength)
            {
                strSN = m_strFixedSN.Left(m_nBarcodeLength);
            }
            else
            {
                strSN = m_strFixedSN;
                int nLen = m_nBarcodeLength - strSN.GetLength();
                CString strTemp;
                for(int i=0; i<nLen; i++)
                {
                    strTemp.Format(_T("%d"), m_rand.GetRandomNumber());
                    strSN += strTemp;
                }			
            }
            
        }
        else
        {
            if(!m_bSNWithDateTime)
            {			
                for(int i = 0; i< m_nBarcodeLength; i++)
                {
                    CString strTmp;
                    strTmp.Format(_T("%d"),m_rand.GetRandomNumber());
                    strSN += strTmp;
                }
            }
            else
            {
                CTime Time = CTime::GetCurrentTime();
                strSN = Time.Format(_T("%Y%m%d%H%M%S"));
                if (strSN.GetLength() >= m_nBarcodeLength)
                {
                    strSN=strSN.Right(m_nBarcodeLength);
                }
                else
                {
                    int nLen = m_nBarcodeLength - strSN.GetLength();
                    CString strTemp;
                    for(int i=0; i<nLen; i++)
                    {
                        strTemp.Format(_T("%d"), m_rand.GetRandomNumber());
                        strSN += strTemp;
                    }			
                }		   

            }

        }
		m_edtBarcode.SetWindowText(strSN);
	}
   
    return FALSE;  
}

void CBarcodeDlg::OnCancel() 
{  
	EndDialog(IDIGNORE);
    return;
}

void CBarcodeDlg::OnButtonSuspend() 
{
    EndDialog(IDIGNORE);
}

void CBarcodeDlg::OnOK() 
{
    UpdateData(TRUE);
	
	BOOL bIsSNValid = TRUE;
	int i = 0;
	for (i=0; i<m_strBarcode.GetLength(); i++)
	{
		if (!( _istdigit( m_strBarcode.GetAt(i)) ||
			((m_strBarcode.GetAt(i) >= 0X61 && m_strBarcode.GetAt(i) <= 0X7A) //  >'a'  <'z'
			||(m_strBarcode.GetAt(i) >= 0x41 && m_strBarcode.GetAt(i) <= 0x5A ) )
			))
		{
			bIsSNValid = FALSE;
			break;
		}
	}
	if (!bIsSNValid)
	{
		AfxMessageBox(_T("There are invalid chars in the SN inputted, please input again"));
		return;
	}

    if(m_strBarcode.IsEmpty())
    {
		GetDlgItem(IDC_EDIT_BARCODE)->SetFocus();
		return;
    }
	else if (m_strBarcode.GetLength() !=  m_nBarcodeLength)
	{
		GetDlgItem(IDC_EDIT_BARCODE)->SetFocus();    
		return;
	}

	for (i=0; i<m_strBarcode.GetLength(); i++)
	{
		TCHAR ch = m_strBarcode.GetAt(i);
		if (!_istalnum(ch))
		{
			AfxMessageBox(_T("There are invalid chars in the SN inputted, please input again"));
			return;
		}
	}

	//wei.song 20140901
	if (g_theApp.m_bEnableFlow)
	{
		char szTemp[24] = {0};
		
		for (i=0; i<m_strBarcode.GetLength(); i++)
		{
			szTemp[i] = (char)m_strBarcode.GetAt(i);
		}

		szTemp[strlen(g_theApp.m_szOrderNum)] = '\0';
		if (0 != strcmp(szTemp, g_theApp.m_szOrderNum))
		{
			CString strTemp;
			USES_CONVERSION;
			strTemp.Format(_T("SN and OrderNO(%s) mismatch, please input again"), A2W(g_theApp.m_szOrderNum));
			AfxMessageBox(strTemp);
			return;
		}
	}

	//wei.song 20151020
	if (g_theApp.m_bSprdMESEnable)
	{
		USES_CONVERSION;
		char szPreSnChk[64]={NULL};
		int nPreSnLen = 0;
		nPreSnLen = strlen(g_theApp.m_BatchInfo.szPreSN1);
		sprintf(szPreSnChk, "%s", W2A(m_strBarcode.Left(nPreSnLen)));
		//szPreSnChk[nPreSnLen] = '\0';
		if (0 != strcmp(szPreSnChk, g_theApp.m_BatchInfo.szPreSN1))
		{
			CString strTemp;
			
			strTemp.Format(_T("SN and PreSN1(%s) mismatch, please input again"), A2W(g_theApp.m_BatchInfo.szPreSN1));
			AfxMessageBox(strTemp);
			return;
		}
	}

    EndDialog(IDOK);
}

BOOL CBarcodeDlg::PreTranslateMessage(MSG* pMsg) 
{
    /*if(pMsg->message == WM_KEYUP || pMsg->message == WM_KEYDOWN)
    {
        DWORD dwTick = m_dwTick;
        m_dwTick = ::GetTickCount();
        
        char c = (char)pMsg->wParam;
        if(isdigit(c))
        {
            return FALSE;
        }
        
        if(isalpha(c))
        {
            return FALSE;
        }
        
        if(m_dwTick - dwTick < 100)
        {
            return TRUE;
        }
    }*/

	return CDialog::PreTranslateMessage(pMsg);
}

void CBarcodeDlg::OnChangeEditBarcode() 
{		
    CString strTitle;
	GetWindowText(strTitle);
    CString strText;
    m_edtBarcode.GetWindowText(strText);
  /*
	for (int i=0; i<strText.GetLength(); i++)
	{
		if (!( _istdigit( strText.GetAt(i)) ||
			((strText.GetAt(i) >= 0X61 && strText.GetAt(i) <= 0X7A) //  >'a'  <'z'
			||(strText.GetAt(i) >= 0x41 && strText.GetAt(i) <= 0x5A ) )
			))
		{
			strText.Delete(i);
		}
	}*/

 	CString str;
	str.Format(_T("%d"), strText.GetLength());
	str += _T("  :  ");
	str += m_str_title;
	SetWindowText(str); 

    if(strText.GetLength() == m_nBarcodeLength)
	{
      m_btnOK.EnableWindow(TRUE);
	  if (m_bAutoStart)
	  {
		  OnOK();
	  }	  
	}
	else
	{
       m_btnOK.EnableWindow(FALSE);	
	   m_edtBarcode.SetFocus();
	   Invalidate();
	}
}

