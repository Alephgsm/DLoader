// PageVolFreq.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "PageUartPortSwitch.h"
#include "BMAGlobal.h"
#include <atlconv.h>
#include <algorithm>
#include "EdtBtnCtrl.h"
#include "CombCtrlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bInitSheet;


/////////////////////////////////////////////////////////////////////////////
// CPageVolFreq property page

IMPLEMENT_DYNCREATE(CPageUartPortSwitch, CPropertyPage)

CPageUartPortSwitch::CPageUartPortSwitch() : CPropertyPage(CPageUartPortSwitch::IDD)
{

}

CPageUartPortSwitch::~CPageUartPortSwitch()
{
}

void CPageUartPortSwitch::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_UART_PORT_TURNING, m_lstUartPortTurning);  
}


BEGIN_MESSAGE_MAP(CPageUartPortSwitch, CPropertyPage)
    ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_UART_PORT_TURNING, OnLvnEndlabeleditListUartPortTurning)
    ON_NOTIFY(NM_CLICK, IDC_LIST_UART_PORT_TURNING, OnNMClickListUartPortTurning)
END_MESSAGE_MAP()


BOOL CPageUartPortSwitch::LoadSettings(LPCTSTR pFileName,CString &strErrorMsg)
{
	UNUSED_ALWAYS(pFileName);
	UNUSED_ALWAYS(strErrorMsg);

	return TRUE;
}
BOOL CPageUartPortSwitch::SaveSettings(LPCTSTR pFileName)
{
	UNUSED_ALWAYS(pFileName);
	return TRUE;
}

void CPageUartPortSwitch::SetConfig(BOOL bShowWnd,LPCTSTR lpszFile)
{
    m_vecTmpTurningItem.clear();
	m_bTmpRightUboot = FindParam(lpszFile);
	m_strTmpUbootFile = lpszFile;

	if(!bShowWnd)
	{
        m_vecTurningItem.clear();
        m_vecTurningItem.assign(m_vecTmpTurningItem.begin(), m_vecTmpTurningItem.end());
        m_bRightUboot = m_bTmpRightUboot;
        m_strUbootFile = m_strTmpUbootFile;
	}
	else
	{
        ShowParam(FALSE);
        CRect rtRange;
        int nMargin = 5;
        GetDlgItem(IDC_STATIC_RANGE)->GetWindowRect(rtRange);
        ScreenToClient (&rtRange);
        rtRange.left    += nMargin;
        rtRange.right   -= nMargin;
        rtRange.top     += nMargin;
        rtRange.bottom  -= nMargin;
        m_lstUartPortTurning.MoveWindow(&rtRange);
        FillList(m_bTmpRightUboot);		
	}
}

void CPageUartPortSwitch::ShowParam(BOOL bShow)
{
    int nCmdShow = bShow? SW_SHOW :SW_HIDE;          
    GetDlgItem(IDC_STATIC_RANGE)->ShowWindow(nCmdShow);          

    GetDlgItem(IDC_LIST_UART_PORT_TURNING)->ShowWindow( bShow? SW_HIDE :SW_SHOW );
	UpdateData(FALSE);
}

void CPageUartPortSwitch::InitList()
{

    DWORD dwExStyle = m_lstUartPortTurning.GetExtendedStyle();
    dwExStyle |= LVS_EX_FULLROWSELECT;
    dwExStyle |= LVS_EX_GRIDLINES;
    dwExStyle |= LVS_EX_INFOTIP;

    m_lstUartPortTurning.SetExtendedStyle(dwExStyle);
    m_lstUartPortTurning.SetBkColor(RGB(232, 232, 232));
    m_lstUartPortTurning.SetRowHeigt(23);
    m_lstUartPortTurning.InsertColumn(0, _T("Port"), LVCFMT_LEFT, 170 );
    m_lstUartPortTurning.InsertColumn(1, _T("Value"), LVCFMT_LEFT,340 );
}

void CPageUartPortSwitch::FillList(BOOL bEnable)
{
    GetDlgItem(IDC_LIST_UART_PORT_TURNING)->EnableWindow(bEnable);

    m_lstUartPortTurning.DeleteAllItems();
    int nCount = m_vecTmpTurningItem.size();
    USES_CONVERSION;
    for (int i=0; i<nCount; ++i)
    {
        CString strValue;
        CString strPortName;
        if ( E_COMBOX == m_vecTmpTurningItem[i].dwCtrlType )
        {
            int index = m_vecTmpTurningItem[i].vti.reg;
            strValue.Format(_T("%s"),A2W(m_vecTmpTurningItem[i].vti.name[index]));
            strPortName.Format(_T("Port%d"), m_vecTmpTurningItem[i].vti.index);
        } 
        m_lstUartPortTurning.InsertItem(i,strPortName.operator LPCTSTR());
        m_lstUartPortTurning.SetItemText(i,1,strValue.operator LPCTSTR()); 
        m_lstUartPortTurning.SetItemData(i,m_vecTmpTurningItem[i].dwCtrlType);
    }

}

void CPageUartPortSwitch::AddUartPortTurningItem(pin_uart_port_sel_t* pTI,int nIndex,int nCtrlType/* = E_EDITBOX*/)
{
    if (pTI)
    {
        USES_CONVERSION;
        UART_PORT_TURNING_ITEM spi;
        memcpy(&(spi.vti),pTI,sizeof(pin_uart_port_sel_t));
        spi.dwIndex = nIndex;
        spi.dwCtrlType = nCtrlType; 
        spi.dwUnit = E_UNIT_NONE;
        
        m_vecTmpTurningItem.push_back(spi);
    }

}

BOOL CPageUartPortSwitch::FindParam(LPCTSTR lpszFile)
{
    LPBYTE pBuf     = NULL;
    DWORD dwSize    = 0;
    DWORD dwOffset  = 0;
    DWORD dwCodeLen  = 0;
    BOOL  bRet      = FALSE;
    if(_FindParam(lpszFile,pBuf,dwSize,dwOffset,dwCodeLen))
    {
        pin_uart_port_sel_t* pTI   = NULL;
        UART_PORT_HEADER* uartPortHeader = (UART_PORT_HEADER *)(pBuf+dwOffset);
        pin_uart_port_sel_t* pagTI = (pin_uart_port_sel_t*)(pBuf+dwOffset+sizeof(UART_PORT_HEADER));
        for (int i=0; i<uartPortHeader->dwUartPortNum; ++i)
        {
            pTI = pagTI + i;
            AddUartPortTurningItem(pTI,i,E_COMBOX);
        }
        
        bRet = TRUE;
    }

    SAFE_DELETE_ARRAY(pBuf);
    return bRet;
}

BOOL CPageUartPortSwitch::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
    InitList();
	
	return TRUE;  
}

void CPageUartPortSwitch::OnOK() 
{
	UpdateData();
	
	m_strUbootFile  = m_strTmpUbootFile;
	m_bRightUboot   = m_bTmpRightUboot;
    m_nUartPortVer = m_nTmpUartPortVer;

    if( SPRD_VER1 == m_nUartPortVer )
    {
        int nCount = m_lstUartPortTurning.GetItemCount();

        for(int i = 0; i<nCount;i++)
        {
            DWORD dwType = m_lstUartPortTurning.GetItemData(i);
            CString strValue = m_lstUartPortTurning.GetItemText(i,1);
           
            if (E_COMBOX == dwType)
            {
                m_vecTmpTurningItem[i].vti.reg = GetRegData(m_vecTmpTurningItem[i].vti.name, strValue);
            }     
        }
        m_vecTurningItem.clear();
        m_vecTurningItem.assign(m_vecTmpTurningItem.begin(), m_vecTmpTurningItem.end());
    }
	SaveUbootFile();
	CPropertyPage::OnOK();
}

BOOL CPageUartPortSwitch::OnKillActive() 
{
	this->SetFocus();
	if( !g_bInitSheet)
	{
		UpdateData();
        if( SPRD_VER1 == m_nTmpUartPortVer )
        {
            return CPropertyPage::OnKillActive();
        }
	}
	return CPropertyPage::OnKillActive();
}

BOOL CPageUartPortSwitch::SaveUbootFile()
{
	CString strUboot = m_strUbootFile;	
	BOOL bOK = TRUE;
	
	if(m_bRightUboot && !strUboot.IsEmpty() && strUboot.CompareNoCase(FILE_OMIT) != 0)
	{
		LPBYTE pBuf = NULL;
		DWORD dwSize = 0;
		DWORD dwOffset = 0;
        DWORD dwCodeLen  = 0;
		
		if(_FindParam(strUboot,pBuf,dwSize,dwOffset,dwCodeLen))
		{
            if( SPRD_VER1 == m_nTmpUartPortVer )
            {
                int nCount = m_vecTurningItem.size();
                DWORD dwLeft = dwOffset + sizeof(UART_PORT_HEADER);
                DWORD dwItemOffset=0;
                for (int i=0; i<nCount; ++i)
                {
                    dwItemOffset = dwLeft + (m_vecTurningItem[i].dwIndex)*sizeof(pin_uart_port_sel_t);
                    memcpy(pBuf+dwItemOffset,&(m_vecTurningItem[i].vti),sizeof(pin_uart_port_sel_t));
                }

            }

			HANDLE hFile = ::CreateFile(strUboot,
				GENERIC_WRITE,
				FILE_SHARE_READ,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			
			if(hFile == INVALID_HANDLE_VALUE)
			{
				CString strFormatted;
				strFormatted.Format(_T("Can not create file [%s]."),strUboot.operator LPCTSTR());
				AfxMessageBox(strFormatted);
				SAFE_DELETE_ARRAY(pBuf);
				return FALSE;
			}
			else
			{	 
				DWORD dwWrite= 0;
				bOK = WriteFile(hFile,pBuf,dwSize,&dwWrite,NULL);
				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;
				SAFE_DELETE_ARRAY(pBuf);
			}
		}		
	}

	return bOK;
}
BOOL CPageUartPortSwitch::_FindParam(LPCTSTR lpszFile, LPBYTE &pBuf, DWORD &dwSize, DWORD &dwOffset,DWORD& dwCodeLen)
{
	pBuf     = NULL;
	dwSize   = 0;
	dwOffset = 0;
    dwCodeLen = 0;

	if(lpszFile == NULL)
		return FALSE;
	
	CFileFind finder;
	if(!finder.FindFile(lpszFile))
	{
		return FALSE;
	}
	
	BOOL bRlt = FALSE;
	
	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = ::CreateFile(lpszFile,
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
	
	dwSize = GetFileSize(hFile,NULL);
	
	if(dwSize == 0 || dwSize == 0xFFFFFFFF)
	{
        CloseHandle(hFile);
		return FALSE;
	}
	
	pBuf = new BYTE[dwSize];
	
	DWORD dwRealRead =  0;
	ReadFile(hFile,pBuf,dwSize,&dwRealRead,NULL);
	
	CloseHandle(hFile);
	
	const DWORD dwMagicHdr = SPRD_MAGIC_HDR;
	const DWORD dwMagicEnd = SPRD_MAGIC_END;
	
	BYTE* lpPos = NULL;
	BYTE* lpBeginPos = NULL;
	BYTE* lpEndPos = NULL;
	BYTE* lpTmpPos = pBuf;
	DWORD dwLeft = dwSize;
	
	do 
	{
		lpPos = std::search( lpTmpPos,lpTmpPos + dwLeft,(BYTE*)&dwMagicHdr,(BYTE*)&dwMagicHdr + sizeof( dwMagicHdr ) - 1 );
		
		if(lpPos < (lpTmpPos + dwLeft) )
		{
			lpBeginPos = lpPos;
			DWORD dwTmpLength = dwSize-(lpBeginPos-pBuf);
			
			lpPos = std::search( lpBeginPos,lpBeginPos + dwTmpLength,(BYTE*)&dwMagicEnd,(BYTE*)&dwMagicEnd + sizeof( dwMagicEnd ) - 1 );
			if(lpPos < lpBeginPos + dwTmpLength)
			{
				lpEndPos = lpPos;
                DWORD dwVer = 0;
                if (lpEndPos - lpBeginPos >= sizeof(DWORD)+ sizeof(dwMagicHdr))
                {
                    dwVer = *(DWORD*)(lpBeginPos+sizeof(dwMagicHdr));
                }
                if( SPRD_VER1 == dwVer ) 
                {
                    m_nTmpUartPortVer = SPRD_VER1;
                    dwOffset = lpBeginPos - pBuf +sizeof(dwMagicHdr);
                    dwCodeLen = lpEndPos - lpBeginPos - sizeof(dwMagicHdr);
                    bRlt = TRUE;
                    break;
                }   
			}
		}
	} while(lpPos < (lpTmpPos + dwLeft));

	if(!bRlt)
	{
		SAFE_DELETE_ARRAY(pBuf);
	}
	
	return bRlt;
}


void CPageUartPortSwitch::OnLvnEndlabeleditListUartPortTurning(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    // TODO: Add your control notification handler code here
    m_lstUartPortTurning.SetItemText(pDispInfo->item.iItem,pDispInfo->item.iSubItem,pDispInfo->item.pszText);
    *pResult = 0;
}

void CPageUartPortSwitch::OnNMClickListUartPortTurning(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here
    int nItem = pNMItemActivate->iItem;
    int nSubItem = pNMItemActivate->iSubItem;
    if(nItem == -1 || nSubItem != 1)
    {
        return;
    }
    CWnd* pWndCtrl          = NULL;
    CCombCtrlDlg* pCombCtrl = NULL;
    if (E_COMBOX == m_vecTmpTurningItem[nItem].dwCtrlType)
    {  
        CStringArray agString;
        for (int i=0; i<SPRD_MAX_UART_PORT_NUM; ++i )
        {
            if(strlen(m_vecTmpTurningItem[nItem].vti.name[i]) > 0)
            {
                agString.Add(m_vecTmpTurningItem[nItem].vti.name[i]);
            }  
        }

        pCombCtrl = new CCombCtrlDlg();
        
        if(NULL != pCombCtrl)
        {		
            pCombCtrl->m_nItem = nItem;
            pCombCtrl->m_nSubItem = nSubItem;
            pCombCtrl->m_strText = m_lstUartPortTurning.GetItemText(nItem,nSubItem);
            pCombCtrl->m_nCurSel = m_vecTmpTurningItem[nItem].vti.reg;
            pCombCtrl->SetCellComboContent(agString);

            if(!pCombCtrl->Create(IDD_COMBOX_DLG,FromHandle(m_lstUartPortTurning.m_hWnd)))
            {
                AfxMessageBox(_T("Error to create EdtBtnCtrl"));
                return;
            }
        }
        else
        {
            return;
        }

        pWndCtrl = pCombCtrl;  
    }
   
    CRect rect;
    m_lstUartPortTurning.GetSubItemRect(nItem,nSubItem,LVIR_BOUNDS,rect);
    CRect rtList;
    m_lstUartPortTurning.GetClientRect(&rtList);
    if(rect.right>rtList.right)
        rect.right = rtList.right;
    if(rect.left < rtList.left)
        rect.left = rtList.left;
    if(rect.bottom>rtList.bottom)
        rect.bottom = rtList.bottom;
    if(rect.top < rtList.top)
        rect.top = rtList.top;

    pWndCtrl->SetWindowPos(&wndTop,rect.left,rect.top-1,rect.right-rect.left,rect.bottom-rect.top,NULL);

    pWndCtrl->ShowWindow(SW_SHOW);
    *pResult = 0;
}

DWORD CPageUartPortSwitch::GetRegData(char pszName[][20], CString& strValue)
{
    USES_CONVERSION;
    for(int i=0; i<SPRD_MAX_UART_PORT_NUM; i++)
    {
        if(strnicmp(pszName[i], W2A(strValue.GetBuffer(0)), strValue.GetLength()) == 0)
        {
            return i;
        }
    }
    return 0;
}