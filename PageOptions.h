#if !defined(AFX_PAGEOPTIONS_H__CB7C2CB5_9F95_45ED_A67D_09A098439565__INCLUDED_)
#define AFX_PAGEOPTIONS_H__CB7C2CB5_9F95_45ED_A67D_09A098439565__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageOptions.h : header file
//

// Do repartition always
#define REPAR_STRATEGY_ALWAYS           0   
// Stop actions and report error when incompatible partition error occured
#define REPAR_STRATEGY_STOP             1
// Ignore incompatible partition error
#define REPAR_STRATEGY_IGNORE           2
// Do repartion action when imcompatible partition error occured
#define REPAR_STRATEGY_DO               3

/////////////////////////////////////////////////////////////////////////////
// CPageOptions dialog

class CPageOptions : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageOptions)

// Construction
public:
	CPageOptions();
	~CPageOptions();

	BOOL LoadSettings(LPCTSTR pFileName,CString &strErrorMsg);
    BOOL SaveSettings(LPCTSTR pFileName);

	int  GetGUINandRepartStrategy();
	int  GetNandRepartStrategy();

	int  GetGUIFlashPageType() {return m_nFlashPageType;}
	int  GetFlashPageType(){return m_nFlashPageType;}

	DWORD GetUnPacPolicy();

// Dialog Data
	//{{AFX_DATA(CPageOptions)
	enum { IDD = IDD_PROPPAGE_OPTIONS };
	BOOL	m_bTmpRepart;
	BOOL	m_bTmpReset;
	BOOL	m_bTmpReadMcpType;
	BOOL	m_bTmpKeepCharge;
    BOOL	m_bTmpDdrCheck;
    BOOL    m_bTmpSelfRefresh;
    BOOL    m_bTmpOldMemoryCheck;
    BOOL	m_bTmpEmmcCheck;
	BOOL	m_bTmpPowerOff;
	BOOL    m_bXmlPowerOff;
	BOOL    m_bIniPowerOff;
	BOOL    m_bUIPowerOff;

    BOOL    m_bTmpReadChipUID;
    BOOL    m_bTmpCheckMatch;
    BOOL    m_bTmpEnableLog;
    BOOL    m_bTmpSecureBoot;
	BOOL	m_bTmpDebugMode;
	BOOL 	m_bTmpUartDownload;
	BOOL	m_bTmpAutoCompare;
	CString m_strTmpCompareApp;
	//}}AFX_DATA

	BOOL    m_bRepart;
	BOOL    m_bReset;
	BOOL    m_bPowerOff;
	BOOL    m_bKeepCharge;
	BOOL	m_bUartDownload;
	BOOL	m_bAutoCompare;
	DWORD	m_dwComparePolicy;
    BOOL    m_bDdrCheck;
    BOOL    m_bSelfRefresh;
    BOOL    m_bOldMemoryCheck;
    BOOL    m_bEmmcCheck;
    BOOL    m_bReadChipUID;
    BOOL    m_bCheckMatch;
    BOOL    m_bEnableLog;
    BOOL    m_bEnableSecureBoot;
    BOOL    m_bEnableEndProcess;
	BOOL	m_bEnableDebugMode;
	CString m_strCompareApp;
	CString m_strCompareParameter;

	int     m_nFlashPageType;
	BOOL    m_bReadMcpType;

	BOOL    m_bSharkNandOption; // around download-uid 

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageOptions)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageOptions)
	virtual BOOL OnInitDialog();
	afx_msg void OnReset();
	afx_msg void OnPowerOff();
	afx_msg void OnSelectCompareApp();
	afx_msg void OnAutoCompare();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void EnableAutoCompareCtrl(BOOL bEnable);
	
private:
	CString m_strIniFile;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEOPTIONS_H__CB7C2CB5_9F95_45ED_A67D_09A098439565__INCLUDED_)
