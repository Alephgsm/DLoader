#pragma once

#include "NumericEdit.h"
#include "CoolListCtrl.h"
#include "PageVolFreq.h"

#define SPRD_MAGIC_HDR            0x5555AAAA
#define SPRD_MAGIC_END            0xAAAA5555
#define SPRD_MAX_SYS_NAME_LEN     20
#define SPRD_MAX_UPORT_SEL_NUM    11
#define SPRD_MAX_UART_PORT_NUM    15
#define SPRD_VER1                 0


typedef struct _pin_uart_port_sel_t
{
    DWORD index;
    DWORD reg;
    DWORD select;
    char name[SPRD_MAX_UART_PORT_NUM][SPRD_MAX_SYS_NAME_LEN];

    _pin_uart_port_sel_t()
    {
        memset(this, 0, sizeof(_pin_uart_port_sel_t));
    }
}pin_uart_port_sel_t;


typedef struct _UART_PORT_HEADER
{
    BYTE  ver[4];
    DWORD dwUartPortNum;
    _UART_PORT_HEADER()
    {
        memset(this,0,sizeof(_UART_PORT_HEADER));
    }
}UART_PORT_HEADER,*PUART_PORT_HEADER_PTR;

typedef struct _UART_PORT_TURNING_ITEM
{
    pin_uart_port_sel_t vti;
    DWORD   dwIndex;        // item index
    DWORD   dwCtrlType;     // 0: EditBox,1:ComBox
    DWORD   dwUnit;
    TCHAR   szName[64];     //show name
    _UART_PORT_TURNING_ITEM()
    {
        memset(this,0,sizeof(_UART_PORT_TURNING_ITEM));
        dwUnit = 0;
    }
}UART_PORT_TURNING_ITEM,*PUART_PORT_TURNING_ITEM;


/////////////////////////////////////////////////////////////////////////////
// CPageUartPortSwitch dialog

class CPageUartPortSwitch : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageUartPortSwitch)

// Construction
public:
	CPageUartPortSwitch();
	~CPageUartPortSwitch();
	BOOL	LoadSettings(LPCTSTR pFileName,CString &strErrorMsg);
	BOOL	SaveSettings(LPCTSTR pFileName);
	void    SetConfig(BOOL bShowWnd,LPCTSTR lpszFile);

// Dialog Data
	//{{AFX_DATA(CPageVolFreq)
	enum { IDD = IDD_PROPPAGE_UART_PORT_SWITCH };
	CCoolListCtrl   m_lstUartPortTurning;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageVolFreq)
	public:
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageVolFreq)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDebug();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    BOOL FindParam(LPCTSTR lpszFile);
	void ShowParam(BOOL bShow);
	BOOL SaveUbootFile();


private:
	BOOL _FindParam(LPCTSTR lpszFile, LPBYTE &pBuf, DWORD &dwSize, DWORD &dwOffset,DWORD& dwCodeLen );
    void AddUartPortTurningItem(pin_uart_port_sel_t* pTI,int nIndex,int nCtrlType = E_EDITBOX);
    void InitList();
    void FillList(BOOL bEnable);
    DWORD GetRegData(char pszName[][20], CString& strValue);

private:

    std::vector<UART_PORT_TURNING_ITEM> m_vecTurningItem;
    std::vector<UART_PORT_TURNING_ITEM> m_vecTmpTurningItem;

	CString m_strUbootFile;
	CString m_strTmpUbootFile;
	BOOL    m_bRightUboot;
	BOOL    m_bTmpRightUboot;
	int     m_nUartPortVer;
	int     m_nTmpUartPortVer;

public:
    afx_msg void OnLvnEndlabeleditListUartPortTurning(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickListUartPortTurning(NMHDR *pNMHDR, LRESULT *pResult);
};

