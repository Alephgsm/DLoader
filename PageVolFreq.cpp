// PageVolFreq.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "PageVolFreq.h"
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

IMPLEMENT_DYNCREATE(CPageVolFreq, CPropertyPage)

CPageVolFreq::CPageVolFreq() : CPropertyPage(CPageVolFreq::IDD)
{
	//{{AFX_DATA_INIT(CPageVolFreq)
	m_nClkCa7Core = 100000000;
	m_nDdrFreq = 100000000;
	m_nClkCa7Axi = 100000000;
	m_nClkCa7Dgb = 10000000;
	m_nDcdcArm = 600;
	m_nDcdcCore = 600;
	m_nClkAonApb = 0;
	m_nClkCa7Ahb = 0;
	m_nClkCa7Apb = 0;
	m_nClkPubAhb = 0;
	m_nDcdcGen = 0;
	m_nDcdcMem = 0;
	//}}AFX_DATA_INIT
	
	m_bTmpRightSPL = FALSE;
	m_bRightSPL = FALSE;
	m_strTmpSPLFile = _T("");
	m_strSPLFile = _T("");

	m_tTmpVolFreqParam.Init();
	m_tVolFreqParam.Init();

	m_nVolFreqVer = VOL_FREQ_VER2;
	m_nTmpVolFreqVer = VOL_FREQ_VER2;

	m_nDebugCount = 0;
	m_nTmpDebugCount = 0;

	m_nDebugCurSel  = -1;
    m_vecTmpTurningItem.clear();
    m_vecTurningItem.clear();
    m_mapVolLevel.RemoveAll();

}

CPageVolFreq::~CPageVolFreq()
{
}

void CPageVolFreq::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPageVolFreq)
    DDX_Control(pDX, IDC_VFT_EDT_DEBUG, m_edtDebug);
    DDX_Control(pDX, IDC_VFT_CMB_DEBUG, m_cmbDebug);
    DDX_Text(pDX, IDC_VFT_EDT_CLK_CA7_CORE, m_nClkCa7Core);
    DDX_Text(pDX, IDC_VFT_EDT_DDR_FREQ, m_nDdrFreq);
    DDX_Text(pDX, IDC_VFT_EDT_CLK_CA7_AXI, m_nClkCa7Axi);
    DDX_Text(pDX, IDC_VFT_EDT_CLK_CA7_GDB, m_nClkCa7Dgb);
    DDX_Text(pDX, IDC_VFT_EDT_DCDC_ARM, m_nDcdcArm);
    DDX_Text(pDX, IDC_VFT_EDT_DCDC_CORE, m_nDcdcCore);
    DDX_CBIndex(pDX, IDC_VFT_CMB_CLK_AON_APB, m_nClkAonApb);
    DDX_CBIndex(pDX, IDC_VFT_CMB_CLK_CA7_AHB, m_nClkCa7Ahb);
    DDX_CBIndex(pDX, IDC_VFT_CMB_CLK_CA7_APB, m_nClkCa7Apb);
    DDX_CBIndex(pDX, IDC_VFT_CMB_CLK_PUB_AHB, m_nClkPubAhb);
    DDX_Text(pDX, IDC_VFT_EDT_DCDC_GEN, m_nDcdcGen);
    DDX_Text(pDX, IDC_VFT_EDT_DCDC_MEM, m_nDcdcMem);
	DDX_Control(pDX, IDC_LIST_SPL_TURNING, m_lstSplTurning);
    //}}AFX_DATA_MAP
    
}


BEGIN_MESSAGE_MAP(CPageVolFreq, CPropertyPage)
	//{{AFX_MSG_MAP(CPageVolFreq)
	ON_CBN_SELCHANGE(IDC_VFT_CMB_DEBUG, OnSelchangeDebug)
	//}}AFX_MSG_MAP
    ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_SPL_TURNING, OnLvnEndlabeleditListSplTurning)
    ON_NOTIFY(NM_CLICK, IDC_LIST_SPL_TURNING, OnNMClickListSplTurning)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageVolFreq message handlers
BOOL CPageVolFreq::LoadSettings(LPCTSTR pFileName,CString &strErrorMsg)
{
	UNUSED_ALWAYS(pFileName);
	UNUSED_ALWAYS(strErrorMsg);

	return TRUE;
}
BOOL CPageVolFreq::SaveSettings(LPCTSTR pFileName)
{
	UNUSED_ALWAYS(pFileName);
	return TRUE;
}

void CPageVolFreq::SetConfig(BOOL bShowWnd,LPCTSTR lpszFile)
{
	m_tTmpVolFreqParam.Init();
    m_vecTmpTurningItem.clear();
    m_mapVolLevel.RemoveAll();
	m_bTmpRightSPL = FindParam(lpszFile);//FindParam(lpszFile,m_tTmpVolFreqParam);
	m_strTmpSPLFile = lpszFile;

	if(!bShowWnd)
	{
        if( VOL_FREQ_VER3 == m_nTmpVolFreqVer )
        {
            m_vecTurningItem.clear();
            m_vecTurningItem.assign(m_vecTmpTurningItem.begin(), m_vecTmpTurningItem.end());
        }
        else
        {
		    memcpy(&m_tVolFreqParam,&m_tTmpVolFreqParam,sizeof(m_tTmpVolFreqParam));
		    m_nDebugCount = m_nTmpDebugCount;
        }
        m_bRightSPL = m_bTmpRightSPL;
        m_strSPLFile = m_strTmpSPLFile;
	}
	else
	{
        if( VOL_FREQ_VER3 == m_nTmpVolFreqVer )
        {
            ShowParam(FALSE);
            CRect rtRange;
            int nMargin = 5;
            GetDlgItem(IDC_STATIC_VOL_RANGE)->GetWindowRect(rtRange);
            ScreenToClient (&rtRange);
            rtRange.left    += nMargin;
            rtRange.right   -= nMargin;
            rtRange.top     += nMargin;
            rtRange.bottom  -= nMargin;
            m_lstSplTurning.MoveWindow(&rtRange);
            FillList(m_bTmpRightSPL);
        }
        else
        {
            ShowParam(TRUE);
            EnableParam(m_bTmpRightSPL);
        }
			
	}
}

void CPageVolFreq::EnableParam(BOOL bEnable)
{
    GetDlgItem(IDC_LIST_SPL_TURNING)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_VFT_EDT_CLK_CA7_AXI)->EnableWindow(bEnable);
    GetDlgItem(IDC_VFT_EDT_CLK_CA7_CORE)->EnableWindow(bEnable);
    GetDlgItem(IDC_VFT_EDT_CLK_CA7_GDB)->EnableWindow(bEnable);
    GetDlgItem(IDC_VFT_EDT_DDR_FREQ)->EnableWindow(bEnable);
    GetDlgItem(IDC_VFT_EDT_DCDC_ARM)->EnableWindow(bEnable);
    GetDlgItem(IDC_VFT_EDT_DCDC_CORE)->EnableWindow(bEnable);
    GetDlgItem(IDC_VFT_CMB_CLK_CA7_AHB)->EnableWindow(bEnable);
    GetDlgItem(IDC_VFT_CMB_CLK_CA7_APB)->EnableWindow(bEnable);
    GetDlgItem(IDC_VFT_CMB_CLK_PUB_AHB)->EnableWindow(bEnable);
    GetDlgItem(IDC_VFT_CMB_CLK_AON_APB)->EnableWindow(bEnable);

    GetDlgItem(IDC_VFT_EDT_DCDC_MEM)->EnableWindow(bEnable && m_nTmpVolFreqVer == VOL_FREQ_VER2);
    GetDlgItem(IDC_VFT_EDT_DCDC_GEN)->EnableWindow(bEnable && m_nTmpVolFreqVer == VOL_FREQ_VER2);

    m_nClkCa7Core = m_tTmpVolFreqParam.CLK_CA7_CORE;
    m_nDdrFreq	  = m_tTmpVolFreqParam.DDR_FREQ;
    m_nClkCa7Axi  = m_tTmpVolFreqParam.CLK_CA7_AXI;
    m_nClkCa7Dgb  = m_tTmpVolFreqParam.CLK_CA7_DGB;
    m_nDcdcArm    = m_tTmpVolFreqParam.DCDC_ARM;
    m_nDcdcCore   = m_tTmpVolFreqParam.DCDC_CORE;
    m_nClkAonApb  = m_tTmpVolFreqParam.CLK_AON_APB;
    m_nClkCa7Ahb  = m_tTmpVolFreqParam.CLK_CA7_AHB;
    m_nClkCa7Apb  = m_tTmpVolFreqParam.CLK_CA7_APB;
    m_nClkPubAhb  = m_tTmpVolFreqParam.CLK_PUB_AHB;
    m_nDcdcMem    = m_tTmpVolFreqParam.DCDC_MEM;
    m_nDcdcGen    = m_tTmpVolFreqParam.DCDC_GEN;

    int nCount = m_cmbDebug.GetCount();
    int i = 0;
    for(i = 0;i<nCount;i++)
    {
        m_cmbDebug.DeleteString(0);
    }

    for(i = 0; i< m_nTmpDebugCount; i++)
    {
        CString str;
        str.Format(_T("DEBUG %d"),i);
        m_cmbDebug.AddString(str);
    }
    if(m_cmbDebug.GetCount())
    {
        m_cmbDebug.SetCurSel(0);
        CString str;
        str.Format(_T("%X"),m_tTmpVolFreqParam.debug[0]);
        m_edtDebug.SetWindowText(str);
        GetDlgItem(IDC_VFT_CMB_DEBUG)->EnableWindow(bEnable);
        GetDlgItem(IDC_VFT_EDT_DEBUG)->EnableWindow(bEnable);
    }
    else
    {
        GetDlgItem(IDC_VFT_CMB_DEBUG)->EnableWindow(FALSE);
        GetDlgItem(IDC_VFT_EDT_DEBUG)->EnableWindow(FALSE);
    }
    UpdateData(FALSE);
}
void CPageVolFreq::ShowParam(BOOL bShow)
{
    int nCmdShow = bShow? SW_SHOW :SW_HIDE;          
    GetDlgItem(IDC_STATIC_VOL_RANGE)->ShowWindow(nCmdShow);          
    GetDlgItem(IDC_STATIC_CLK_CA7_CORE)->ShowWindow(nCmdShow);       
    GetDlgItem(IDC_STATIC_DDR_FREQ)->ShowWindow(nCmdShow);           
    GetDlgItem(IDC_STATIC_CLK_CA7_AXI)->ShowWindow(nCmdShow);        
    GetDlgItem(IDC_STATIC_CLK_CA7_GDB)->ShowWindow(nCmdShow);        
    GetDlgItem(IDC_STATIC_CLK_CA7_AHB)->ShowWindow(nCmdShow);        
    GetDlgItem(IDC_STATIC_CLK_CA7_APB)->ShowWindow(nCmdShow);        
    GetDlgItem(IDC_STATIC_CLK_PUB_AHB)->ShowWindow(nCmdShow);        
    GetDlgItem(IDC_STATIC_CLK_AON_APB)->ShowWindow(nCmdShow);        
    GetDlgItem(IDC_STATIC_DCDC_ARM)->ShowWindow(nCmdShow);           
    GetDlgItem(IDC_STATIC_DCDC_CORE)->ShowWindow(nCmdShow);          
    GetDlgItem(IDC_STATIC_DCDC_MEM)->ShowWindow(nCmdShow);           
    GetDlgItem(IDC_STATIC_DCDC_GEN)->ShowWindow(nCmdShow);           
    GetDlgItem(IDC_STATIC_DEBUG)->ShowWindow(nCmdShow);      
	GetDlgItem(IDC_VFT_EDT_CLK_CA7_AXI)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_VFT_EDT_CLK_CA7_CORE)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_VFT_EDT_CLK_CA7_GDB)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_VFT_EDT_DDR_FREQ)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_VFT_EDT_DCDC_ARM)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_VFT_EDT_DCDC_CORE)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_VFT_CMB_CLK_CA7_AHB)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_VFT_CMB_CLK_CA7_APB)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_VFT_CMB_CLK_PUB_AHB)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_VFT_CMB_CLK_AON_APB)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_VFT_EDT_DCDC_MEM)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_VFT_EDT_DCDC_GEN)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_VFT_CMB_DEBUG)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_VFT_EDT_DEBUG)->ShowWindow(nCmdShow);

    //List ctrl
    GetDlgItem(IDC_LIST_SPL_TURNING)->ShowWindow( bShow? SW_HIDE :SW_SHOW );
	UpdateData(FALSE);
}

void CPageVolFreq::InitList()
{

    DWORD dwExStyle = m_lstSplTurning.GetExtendedStyle();
    dwExStyle |= LVS_EX_FULLROWSELECT;
    dwExStyle |= LVS_EX_GRIDLINES;
    dwExStyle |= LVS_EX_INFOTIP;

    m_lstSplTurning.SetExtendedStyle(dwExStyle);
    m_lstSplTurning.SetBkColor(RGB(232, 232, 232));
    m_lstSplTurning.SetRowHeigt(23);
    m_lstSplTurning.InsertColumn(0, _T("Item"), LVCFMT_LEFT, 170 );
    m_lstSplTurning.InsertColumn(1, _T("Unit"), LVCFMT_LEFT, 50 );
    m_lstSplTurning.InsertColumn(2, _T("Value"), LVCFMT_LEFT,280 );
}
CString CPageVolFreq::GetLevelName(int nLevel,LPCTSTR lpszItemName)
{
    CString str;
    POSITION pos = m_mapVolLevel.GetStartPosition();
    while( pos )
    {
        CString strName;
        VECSTR vecLevel;

        m_mapVolLevel.GetNextAssoc( pos, strName, vecLevel );	 
        if ( 0 == strName.CompareNoCase(lpszItemName) && vecLevel.size()>nLevel)
        {
            str = vecLevel[nLevel];
            break;
        }
    }
    return str;
}

DWORD CPageVolFreq::GetLevelData( LPCTSTR lpszItemName,LPCTSTR lpszLevelName)
{
    DWORD dwLevel = 0;
    VECSTR vecLevel;
    if( m_mapVolLevel.Lookup(lpszItemName,vecLevel))
    {
        for ( int i=0; i<vecLevel.size(); ++i )
        {
            if (0 == vecLevel[i].CompareNoCase(lpszLevelName))
            {
                dwLevel = i;
                break;
            }
        }
    }
    return dwLevel;
}
void CPageVolFreq::FillList(BOOL bEnable)
{
    
    if( VOL_FREQ_VER3 != m_nTmpVolFreqVer )
    {
		GetDlgItem(IDC_LIST_SPL_TURNING)->EnableWindow(FALSE);
        return;
    }
    GetDlgItem(IDC_LIST_SPL_TURNING)->EnableWindow(bEnable);

    m_lstSplTurning.DeleteAllItems();
    int nCount = m_vecTmpTurningItem.size();
    for (int i=0; i<nCount; ++i)
    {
        CString strValue;
        if ( E_COMBOX == m_vecTmpTurningItem[i].dwCtrlType )
        {
            int nLevel = m_vecTmpTurningItem[i].vti.LvlItem.cLevel;
            strValue = GetLevelName( nLevel,m_vecTmpTurningItem[i].szName );
        }
        else
        {
            strValue.Format(_T("%d"),m_vecTmpTurningItem[i].vti.TunItem.dwValue);
            if (E_UNIT_MHZ == m_vecTmpTurningItem[i].dwUnit)
            {
                strValue = ConvetValue2Level(m_vecTmpTurningItem[i].vti.TunItem.dwValue,FALSE);
            }
        } 
        m_lstSplTurning.InsertItem(i,m_vecTmpTurningItem[i].szName);
        m_lstSplTurning.SetItemText(i,1,g_szUnitDesc[m_vecTmpTurningItem[i].dwUnit]); 
        m_lstSplTurning.SetItemText(i,2,strValue.operator LPCTSTR()); 
        m_lstSplTurning.SetItemData(i,m_vecTmpTurningItem[i].dwCtrlType);
    }

}
void CPageVolFreq::AddSplTurningItem(VOL_TURNING_ITEM* pTI,int nIndex,int nCtrlType/* = E_EDITBOX*/)
{
    if (pTI)
    {
        USES_CONVERSION;
        SPL_TURNING_ITEM spi;
        memcpy(&(spi.vti),pTI,sizeof(VOL_TURNING_ITEM));
        spi.dwIndex = nIndex;
        spi.dwCtrlType = nCtrlType; 
        spi.dwUnit = E_UNIT_NONE;
        if ( ( '*' == pTI->byteTag[1] ) && ( 'C' == pTI->byteTag[0] || 'c' == pTI->byteTag[0] ))    //C*
        {
            _tcscpy( spi.szName,_T("CLK_"));
            _tcsncat( spi.szName,A2T((LPCSTR)(pTI->byteTag+2)),6);
            //_tcscat( spi.szName, _T(" (M hz)"));
            spi.dwUnit = E_UNIT_MHZ;
        }
        else if ( ( '*' == pTI->byteTag[1] ) && ( 'D' == pTI->byteTag[0] || 'd' == pTI->byteTag[0]) )//D*
        {
            _tcscpy( spi.szName,_T("DCDC_"));
            _tcsncat( spi.szName,A2T((LPCSTR)(pTI->byteTag+2)),6 );
            //_tcscat( spi.szName, _T(" (mv)"));
            spi.dwUnit = E_UNIT_MV;
        }
        else if ( ( '#' == pTI->byteTag[1] ) && ( 'C' == pTI->byteTag[0] || 'c' == pTI->byteTag[0]) )//C#
        {
            _tcscpy( spi.szName,_T("CLK_"));
            _tcsncat( spi.szName,A2T((LPCSTR)(pTI->byteTag+2)),6 );
            _tcscat( spi.szName, _T("_Level"));           
        }
        else
        {
            _tcsncpy( spi.szName,A2T((LPCSTR)(pTI->byteTag)),8 );
        }
        m_vecTmpTurningItem.push_back(spi);
    }

}
CString CPageVolFreq::ConvetValue2Level(DWORD dwValue,BOOL bHasUnit/* = TRUE*/)
{
    CString str;
    float fValue = (float) dwValue;
    if( dwValue%M_HZ == 0 )
    {
        str.Format(_T("%d"), dwValue/M_HZ);
    }
    else
    {
        str.Format(_T("%.2f"), fValue/M_HZ);
    }
    if (bHasUnit)
    {
        str += _T("M");
    }
    return str;
}
void CPageVolFreq::ParseLevelItem(LPBYTE pBuf,DWORD dwOffset,DWORD dwCodeLen)
{
    if (NULL == pBuf)
    {
        return;
    }
    PVOL_TURNING_HEADER_PTR pHeader = (PVOL_TURNING_HEADER_PTR)(pBuf+dwOffset);
    int nIndex = pHeader->dwTunItemCount;
    DWORD dwLeft = dwOffset+sizeof(VOL_TURNING_HEADER)+ (pHeader->dwTunItemCount) * sizeof(VOL_TURNING_ITEM);
    while(dwLeft < dwOffset+dwCodeLen)
    {
        PVOL_TURNING_ITEM_PTR pLI = (PVOL_TURNING_ITEM_PTR)(pBuf+dwLeft);  
  
        AddSplTurningItem(pLI,nIndex,E_COMBOX);
        VECSTR vecLevel; 
        dwLeft += sizeof(VOL_TURNING_ITEM);
        for (int i=0; i<pLI->LvlItem.cUsed; ++i)
        {            
            CString strLevel;
            strLevel = ConvetValue2Level(*(DWORD*)(pBuf+dwLeft));
            vecLevel.push_back(strLevel);
            dwLeft += sizeof(DWORD);
        }
        dwLeft += (pLI->LvlItem.cTotal - pLI->LvlItem.cUsed)*sizeof(DWORD);
        m_mapVolLevel.SetAt(m_vecTmpTurningItem[m_vecTmpTurningItem.size()-1].szName,vecLevel);
        vecLevel.clear();
        ++nIndex;
    }
}
BOOL CPageVolFreq::FindParam(LPCTSTR lpszFile)
{
    LPBYTE pBuf     = NULL;
    DWORD dwSize    = 0;
    DWORD dwOffset  = 0;
    DWORD dwCodeLen  = 0;
    BOOL  bRet      = FALSE;
    if(_FindParam(lpszFile,pBuf,dwSize,dwOffset,dwCodeLen))
    {
        if( VOL_FREQ_VER3 == m_nTmpVolFreqVer )
        {

            PVOL_TURNING_HEADER_PTR pHeader = (PVOL_TURNING_HEADER_PTR)(pBuf+dwOffset);
            VOL_TURNING_ITEM* pTI   = NULL;
            VOL_TURNING_ITEM* pagTI = (VOL_TURNING_ITEM*)(pBuf+dwOffset+sizeof(VOL_TURNING_HEADER));
            for (int i=0; i<pHeader->dwTunItemCount; ++i)
            {
                pTI = pagTI + i;
                if ( 0 == strlen((char*)(pTI->byteTag)))
                {
                    continue;
                }
                AddSplTurningItem(pTI,i,E_EDITBOX);
            }
            
            ParseLevelItem(pBuf,dwOffset,dwCodeLen);
        }
        else
        {
            DWORD dwSize2 = sizeof(VOL_FREQ_V1::VOL_FREQ_PARAM);
            if(m_nTmpVolFreqVer==VOL_FREQ_VER2)
            {
                dwSize2 = sizeof(VOL_FREQ_V2::VOL_FREQ_PARAM) - MAX_DEBUG_NUM*sizeof(DWORD) + m_nTmpDebugCount*sizeof(DWORD);
            }
            memcpy(&m_tTmpVolFreqParam,pBuf+dwOffset,dwSize2);
        }
        
        bRet = TRUE;
    }

    SAFE_DELETE_ARRAY(pBuf);
    return bRet;
}

BOOL CPageVolFreq::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
    InitList();
	m_edtDebug.SetDispMode(HEX);
	EnableParam(m_bTmpRightSPL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPageVolFreq::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData();
	
	m_strSPLFile  = m_strTmpSPLFile;
	m_bRightSPL   = m_bTmpRightSPL;
    m_nVolFreqVer = m_nTmpVolFreqVer;

    if( VOL_FREQ_VER3 == m_nVolFreqVer )
    {
        int nCount = m_lstSplTurning.GetItemCount();

        for(int i = 0; i<nCount;i++)
        {
            DWORD dwType = m_lstSplTurning.GetItemData(i);
            CString strValue = m_lstSplTurning.GetItemText(i,2);
            DWORD dwRate = 1;
           
            if (E_COMBOX == dwType)
            {
                m_vecTmpTurningItem[i].vti.LvlItem.cLevel = (UINT8)GetLevelData(m_vecTmpTurningItem[i].szName, strValue);
            }
            else
            {
                if (E_UNIT_MHZ == m_vecTmpTurningItem[i].dwUnit)
                {
                    dwRate = M_HZ;
                }
                m_vecTmpTurningItem[i].vti.TunItem.dwValue = (DWORD)(GetFloatDigit(strValue) * dwRate);
            }
            
        }
        m_vecTurningItem.clear();
        m_vecTurningItem.assign(m_vecTmpTurningItem.begin(), m_vecTmpTurningItem.end());
    }
    else
    {
        m_nDebugCount = m_nTmpDebugCount;
        m_tTmpVolFreqParam.CLK_CA7_CORE= m_nClkCa7Core ;
        m_tTmpVolFreqParam.DDR_FREQ    = m_nDdrFreq	   ;
        m_tTmpVolFreqParam.CLK_CA7_AXI = m_nClkCa7Axi  ;
        m_tTmpVolFreqParam.CLK_CA7_DGB = m_nClkCa7Dgb  ;
        m_tTmpVolFreqParam.DCDC_ARM    = m_nDcdcArm    ;
        m_tTmpVolFreqParam.DCDC_CORE   = m_nDcdcCore   ;
        m_tTmpVolFreqParam.CLK_AON_APB = m_nClkAonApb  ;
        m_tTmpVolFreqParam.CLK_CA7_AHB = m_nClkCa7Ahb  ;
        m_tTmpVolFreqParam.CLK_CA7_APB = m_nClkCa7Apb  ;
        m_tTmpVolFreqParam.CLK_PUB_AHB = m_nClkPubAhb  ;

        m_tTmpVolFreqParam.DCDC_MEM    = m_nDcdcMem    ;
        m_tTmpVolFreqParam.DCDC_GEN    = m_nDcdcGen    ;

        if(m_cmbDebug.GetCount())
        {
            int nItem = m_cmbDebug.GetCurSel();
            if(nItem != -1)
            {
                m_tTmpVolFreqParam.debug[nItem]  = m_edtDebug.GetDigit();
            }
        }

        memcpy(&m_tVolFreqParam,&m_tTmpVolFreqParam,sizeof(m_tVolFreqParam));
    }

	SaveSPLFile();
	
	CPropertyPage::OnOK();
}

BOOL CPageVolFreq::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	this->SetFocus();
	if( !g_bInitSheet)
	{
		UpdateData();
        if( VOL_FREQ_VER3 == m_nTmpVolFreqVer )
        {
            return CPropertyPage::OnKillActive();
        }

		m_tTmpVolFreqParam.CLK_CA7_CORE= m_nClkCa7Core ;
		m_tTmpVolFreqParam.DDR_FREQ    = m_nDdrFreq	   ;
		m_tTmpVolFreqParam.CLK_CA7_AXI = m_nClkCa7Axi  ;
		m_tTmpVolFreqParam.CLK_CA7_DGB = m_nClkCa7Dgb  ;
		m_tTmpVolFreqParam.DCDC_ARM    = m_nDcdcArm    ;
		m_tTmpVolFreqParam.DCDC_CORE   = m_nDcdcCore   ;
		m_tTmpVolFreqParam.DCDC_MEM    = m_nDcdcMem    ;
		m_tTmpVolFreqParam.DCDC_GEN    = m_nDcdcGen    ;

		CString strErr;

//CLK_CA7_CORE: 100000000 ~ 2000000000(100M~1G)
//DDR_FREQ:     100000000 ~  800000000(100M~800M)    
//CLK_CA7_AXI,  100000000 ~  800000000(100M~800M)
//CLK_CA7_DGB,   10000000 ~  400000000(10M~400M)			  
//DCDC_ARM,     600 ~ 1500 (0.6V ~ 1.5V)
//DCDC_CORE     600 ~ 1500 (0.6V ~ 1.5V)
	
		if(m_nClkCa7Core<100000000 || m_nClkCa7Core> 2000000000)
		{
			strErr += _T("CLK_CA7_CORE\n");
		}
		if(m_nDdrFreq<100000000 || m_nDdrFreq>800000000)
		{
			strErr += _T("DDR_FREQ\n");
		}
		if(m_nClkCa7Axi<100000000 || m_nClkCa7Axi>800000000)
		{
			strErr += _T("CLK_CA7_AXI\n");
		}
		if(m_nClkCa7Dgb<10000000 || m_nClkCa7Dgb> 400000000)
		{
			strErr += _T("CLK_CA7_DGB\n");
		}
		if(m_nDcdcArm<600 || m_nDcdcArm>1500)
		{
			strErr += _T("DCDC_ARM\n");
		}
		if(m_nDcdcCore<600 || m_nDcdcCore>1500)
		{
			strErr += _T("DCDC_CORE\n");
		}
/*
		if(m_nTmpVolFreqVer == VOL_FREQ_VER2)
		{
			if(m_nDcdcMem<600 || m_nDcdcMem>3000)
			{
				strErr += _T("DCDC_MEM\n");
			}
			if(m_nDcdcGen<600 || m_nDcdcGen>4000)
			{
				strErr += _T("DCDC_GEN\n");
			}
		}
*/
		if(!strErr.IsEmpty())
		{
			CString strMsg;
			strMsg = _T("Follow value is over its range:\n");
			strMsg += strErr;
			strMsg += _T("Please input right values!");
			AfxMessageBox(strMsg);
			return FALSE;
		}		

	}
	return CPropertyPage::OnKillActive();
}

BOOL CPageVolFreq::SaveSPLFile()
{
	CString strSPL = m_strSPLFile;	

	BOOL bOK = TRUE;
	
	if(m_bRightSPL && !strSPL.IsEmpty() && strSPL.CompareNoCase(FILE_OMIT) != 0)
	{
		LPBYTE pBuf = NULL;
		DWORD dwSize = 0;
		DWORD dwOffset = 0;
        DWORD dwCodeLen  = 0;
		
		if(_FindParam(strSPL,pBuf,dwSize,dwOffset,dwCodeLen))
		{
            if( VOL_FREQ_VER3 == m_nTmpVolFreqVer )
            {
                int nCount = m_vecTurningItem.size();
                DWORD dwLeft = dwOffset + sizeof(VOL_TURNING_HEADER);
                DWORD dwItemOffset=0;
                for (int i=0; i<nCount; ++i)
                {
                    dwItemOffset = dwLeft + (m_vecTurningItem[i].dwIndex)*sizeof(VOL_TURNING_ITEM);
                    memcpy(pBuf+dwItemOffset,&(m_vecTurningItem[i].vti),sizeof(VOL_TURNING_ITEM));
                    if (E_COMBOX == m_vecTurningItem[i].dwCtrlType )
                    {
                        dwLeft += (m_vecTurningItem[i].vti.LvlItem.cTotal)*sizeof(DWORD);
                    }
                }

            }
            else
            {
			    DWORD dwSize2 = sizeof(VOL_FREQ_V1::VOL_FREQ_PARAM);
			    if(m_nTmpVolFreqVer==VOL_FREQ_VER2)
			    {
				    dwSize2 = VOL_FREQ_VER2_BASE_SIZE + m_nTmpDebugCount*sizeof(DWORD);
			    }
			    memcpy(pBuf+dwOffset,&m_tVolFreqParam,dwSize2);
            }

			HANDLE hFile = ::CreateFile(strSPL,
				GENERIC_WRITE,
				FILE_SHARE_READ,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			
			if(hFile == INVALID_HANDLE_VALUE)
			{
				CString strFormatted;
				strFormatted.Format(_T("Can not create file [%s]."),strSPL.operator LPCTSTR());
				AfxMessageBox(strFormatted);
				SAFE_DELETE_ARRAY(pBuf);
				return FALSE;
			}
			else
			{	
                
                if (dwSize > sizeof(SPL_HEADER_T))
                {
                    SPL_HEADER_T splHeader;
                    memcpy(&splHeader,pBuf,sizeof(SPL_HEADER_T));
                    if (splHeader.dwMagic == SEC_SPL_MAGIC && splHeader.dwImgSize+sizeof(SPL_HEADER_T)==dwSize)
                    {
                        sha256_context ctx;
                        sha256_starts( &ctx );
                        sha256_update( &ctx, pBuf+sizeof(SPL_HEADER_T), splHeader.dwImgSize );
                        sha256_finish( &ctx, splHeader.sha256Hash );
                        memcpy(pBuf+8,splHeader.sha256Hash,SHA256_DIGEST_SIZE);
                    }
                }
                
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
BOOL CPageVolFreq::_FindParam(LPCTSTR lpszFile, LPBYTE &pBuf, DWORD &dwSize, DWORD &dwOffset,DWORD& dwCodeLen)
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
	
	const DWORD dwMagicHdr = VOL_FREQ_MAGIC_HDR;
	const DWORD dwMagicEnd = VOL_FREQ_MAGIC_END;
	
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
                if( VOL_FREQ_VER3 == dwVer ) //0x02
                {
                    m_nTmpVolFreqVer = VOL_FREQ_VER3;
                    m_nTmpDebugCount = 0;
                    dwOffset = lpBeginPos - pBuf +sizeof(dwMagicHdr);
                    dwCodeLen = lpEndPos - lpBeginPos - sizeof(dwMagicHdr);
                    bRlt = (dwCodeLen <sizeof(VOL_TURNING_HEADER))? FALSE : TRUE;
                    break;
                }
                else if( (lpEndPos - lpBeginPos - sizeof(dwMagicHdr)) == sizeof(VOL_FREQ_V1::VOL_FREQ_PARAM) )
				{
					m_nTmpVolFreqVer = VOL_FREQ_VER1;
					m_nTmpDebugCount = 0;
					bRlt = TRUE;					
					dwOffset = lpBeginPos - pBuf +sizeof(dwMagicHdr);
					break;
				}
				else if( (lpEndPos - lpBeginPos - sizeof(dwMagicHdr)) <= sizeof(VOL_FREQ_V2::VOL_FREQ_PARAM) && (lpEndPos - lpBeginPos - sizeof(dwMagicHdr)) >= VOL_FREQ_VER2_BASE_SIZE )
				{
					m_nTmpVolFreqVer = VOL_FREQ_VER2;
					m_nTmpDebugCount = ((lpEndPos - lpBeginPos - sizeof(dwMagicHdr)) - VOL_FREQ_VER2_BASE_SIZE) / sizeof(DWORD);
					bRlt = TRUE;					
					dwOffset = lpBeginPos - pBuf +sizeof(dwMagicHdr);
					break;
				}
				else
				{
					dwLeft -= (lpBeginPos - lpTmpPos + sizeof(dwMagicHdr));
					lpTmpPos = lpBeginPos + sizeof(dwMagicHdr);		
				}
                
			}
			else
			{
				dwLeft -= (lpBeginPos - lpTmpPos + sizeof(dwMagicHdr));
				lpTmpPos = lpBeginPos + sizeof(dwMagicHdr);		
			}

		}
		
	} while(lpPos < (lpTmpPos + dwLeft));

	if(!bRlt)
	{
		SAFE_DELETE_ARRAY(pBuf);
	}
	
	return bRlt;
}

void CPageVolFreq::OnSelchangeDebug() 
{
	// TODO: Add your control notification handler code here
	if(m_nDebugCurSel != -1)
	{
		UpdateData();
		m_tTmpVolFreqParam.debug[m_nDebugCurSel]  = m_edtDebug.GetDigit();
	}

	m_nDebugCurSel = m_cmbDebug.GetCurSel();
	if(m_nDebugCurSel!=-1)
	{
		CString str;
		str.Format(_T("%X"),m_tTmpVolFreqParam.debug[m_nDebugCurSel]);
		m_edtDebug.SetWindowText(str);
		UpdateData(FALSE);
	}
}

void CPageVolFreq::OnLvnEndlabeleditListSplTurning(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    // TODO: Add your control notification handler code here
    m_lstSplTurning.SetItemText(pDispInfo->item.iItem,pDispInfo->item.iSubItem,pDispInfo->item.pszText);
    *pResult = 0;
}

void CPageVolFreq::OnNMClickListSplTurning(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here
    int nItem = pNMItemActivate->iItem;
    int nSubItem = pNMItemActivate->iSubItem;
    if(nItem == -1 || nSubItem != 2)
    {
        return;
    }
    CWnd* pWndCtrl          = NULL;
    CCombCtrlDlg* pCombCtrl = NULL;
    CEdtBtnCtrl* pEdtCtrl   = NULL;
    if (E_COMBOX == m_vecTmpTurningItem[nItem].dwCtrlType)
    {  
        VECSTR vecLevel;
        if( !m_mapVolLevel.Lookup(m_vecTmpTurningItem[nItem].szName,vecLevel) || 0== vecLevel.size() )
        {
            return;
        }
        CStringArray agString;
        for (int i=0; i<vecLevel.size();++i )
        {
            agString.Add(vecLevel[i]);
        }

        pCombCtrl = new CCombCtrlDlg();
        
        if(NULL != pCombCtrl)
        {		
            pCombCtrl->m_nItem = nItem;
            pCombCtrl->m_nSubItem = nSubItem;
            pCombCtrl->m_strText = m_lstSplTurning.GetItemText(nItem,nSubItem);
            pCombCtrl->m_nCurSel = m_vecTmpTurningItem[nItem].vti.LvlItem.cLevel;
            pCombCtrl->SetCellComboContent(agString);

            if(!pCombCtrl->Create(IDD_COMBOX_DLG,FromHandle(m_lstSplTurning.m_hWnd)))
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
    else
    {
        
        pEdtCtrl = new CEdtBtnCtrl();

        if(NULL != pEdtCtrl)
        {		
            pEdtCtrl->EnableFileSelButton(FALSE);
            pEdtCtrl->SetBtnOpenFlag(FALSE);
            pEdtCtrl->m_nItem = nItem;
            pEdtCtrl->m_nSubItem = nSubItem;
            pEdtCtrl->m_strText = m_lstSplTurning.GetItemText(nItem,nSubItem);
            if(!pEdtCtrl->Create(IDD_EDT_BTN_DLG,FromHandle(m_lstSplTurning.m_hWnd)))
            {
                AfxMessageBox(_T("Error to create EdtBtnCtrl"));
                return;
            }
        }
        else
        {
            return;
        }
        pWndCtrl = pEdtCtrl;
    }
    CRect rect;
    m_lstSplTurning.GetSubItemRect(nItem,nSubItem,LVIR_BOUNDS,rect);
    CRect rtList;
    m_lstSplTurning.GetClientRect(&rtList);
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
