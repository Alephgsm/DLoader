// CombCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DLoader.h"
#include "CombCtrlDlg.h"


// CCombCtrlDlg dialog

IMPLEMENT_DYNAMIC(CCombCtrlDlg, CDialog)

CCombCtrlDlg::CCombCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCombCtrlDlg::IDD, pParent)
{
    m_strText = _T("");
    m_nCurSel = 0;
}

CCombCtrlDlg::~CCombCtrlDlg()
{
}

void CCombCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_LIST, m_cbCtrl);
}


BEGIN_MESSAGE_MAP(CCombCtrlDlg, CDialog)
    ON_WM_DESTROY()
    ON_CBN_KILLFOCUS(IDC_COMBO_LIST, OnCbnKillfocusComboList)
END_MESSAGE_MAP()


// CCombCtrlDlg message handlers
void CCombCtrlDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Add your message handler code here
    delete this;
}

BOOL CCombCtrlDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if( pMsg->message == WM_KEYDOWN )	
    {		
        if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)	
        {
            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);			
            return 1;
        }					

    }
    return CDialog::PreTranslateMessage(pMsg);
}


void CCombCtrlDlg::OnCbnKillfocusComboList()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);	
    
    int nIndex = m_cbCtrl.GetCurSel();
    m_cbCtrl.GetLBText( nIndex, m_strText);
    m_strText.TrimLeft();
    m_strText.TrimRight();
    LV_DISPINFO lvDispInfo;
    lvDispInfo.hdr.hwndFrom = GetParent()->m_hWnd;
    lvDispInfo.hdr.idFrom = GetDlgCtrlID();	
    lvDispInfo.hdr.code = LVN_ENDLABELEDIT; //lint !e648
    lvDispInfo.item.mask = LVIF_TEXT;	
    lvDispInfo.item.iItem = m_nItem;
    lvDispInfo.item.iSubItem = m_nSubItem;
    lvDispInfo.item.pszText = const_cast<LPTSTR>(m_strText.operator LPCTSTR());
    lvDispInfo.item.cchTextMax = m_strText.GetLength();

    GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(),(LPARAM)&lvDispInfo);

    DestroyWindow();
}

BOOL CCombCtrlDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here

//     CFont font;
//     LOGFONT lf;
//     memset(&lf,0,sizeof(LOGFONT));
//     lf.lfHeight = 18;
//     _tcscpy(lf.lfFaceName,_T("Arial"));
//     font.CreateFontIndirect(&lf);
//     m_cbCtrl.SetFont(&font);

    CFont* font = GetParent()->GetFont();
    m_cbCtrl.SetFont(font);
    int nCount = m_agString.GetSize();
    for ( int n=0; n < nCount; ++n )
    {
        m_cbCtrl.AddString(m_agString.GetAt(n));
    }
    if (m_nCurSel>= nCount)
    {
        m_nCurSel = 0;
    }
    m_cbCtrl.SetCurSel(m_nCurSel);
    m_cbCtrl.SetFocus();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CCombCtrlDlg::SetCellComboContent(CONST CStringArray &clOptions)
{
    m_agString.RemoveAll();
    m_agString.Copy(clOptions);
}