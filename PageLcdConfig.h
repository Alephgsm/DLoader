#if !defined(AFX_PAGELCDCONFIG_H__ED0FAFA3_A254_4925_AE20_D9B44A92500B__INCLUDED_)
#define AFX_PAGELCDCONFIG_H__ED0FAFA3_A254_4925_AE20_D9B44A92500B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageLcdConfig.h : header file
//
#pragma warning(push,3)
#include <vector>
#pragma warning(pop)

typedef struct _LCD_ITEM
{
	DWORD dwFlag;
	DWORD dwFlagOffset;	
	DWORD dwNameLen;
	TCHAR szName[128];	
}LCD_ITEM;

typedef std::vector<LCD_ITEM> VEC_LCD_CFIG;
/////////////////////////////////////////////////////////////////////////////
// CPageLcdConfig dialog

class CPageLcdConfig : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageLcdConfig)

// Construction
public:
	CPageLcdConfig();
	~CPageLcdConfig();

	BOOL	LoadSettings(LPCTSTR pFileName,CString &strErrorMsg);
	BOOL	SaveSettings(LPCTSTR pFileName);
	void    SetLCDCfig(BOOL bRightPS,VEC_LCD_CFIG &vLcdCfig, FILETIME *pLastWrite, BOOL bShowWnd,LPCTSTR lpszPSFile);
	BOOL    IsSamePS(FILETIME *pLastWrite, BOOL bShowWnd);

// Dialog Data
	//{{AFX_DATA(CPageLcdConfig)
	enum { IDD = IDD_PROPPAGE_LCD };  //lint !e30
	CListCtrl	m_lstLCD;
	CString	m_strTips;
	//}}AFX_DATA
	VEC_LCD_CFIG m_vLCDCfig;
	BOOL		 m_bRightPS; 
	FILETIME     m_ftLastWrite;

	VEC_LCD_CFIG m_vTmpLCDCfig;
	BOOL         m_bTmpRightPS;
	FILETIME     m_ftTmpLastWrite;
	
	CString      m_strPSFile;
	CString      m_strTmpPSFile;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageLcdConfig)
	public:
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageLcdConfig)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedLst(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bListInit;

private:
	BOOL FillList();
	void ShowLCDCfig(BOOL bShow);
	BOOL IsFileTimeEqual(FILETIME *pft1,FILETIME *pft2);
	BOOL SavePSFile();
	BOOL CheckLCDConfig(VEC_LCD_CFIG &vLcdCfig1,VEC_LCD_CFIG &vLcdCfig2);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGELCDCONFIG_H__ED0FAFA3_A254_4925_AE20_D9B44A92500B__INCLUDED_)
