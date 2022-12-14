#pragma once
#include <vector>
#include "afxwin.h"


/////////////////////////////////////////////////////////////////////////////
// CPageUartPortSwitch dialog

class CPageCustomization : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageCustomization)

// Construction
public:
	CPageCustomization();
	~CPageCustomization();

    BOOL	LoadSettings(LPCTSTR pFileName,CString &strErrorMsg);
    BOOL	SaveSettings(LPCTSTR pFileName);
    CString GetCU();
	BOOL    IsChangedCU();
    BOOL    IsCheckRoot();
    BOOL    IsCheckInproduction();
    int     GetBadFlashRate();


// Dialog Data
	//{{AFX_DATA(CPageCustomization)
	enum { IDD = IDD_PROPPAGE_CUST };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageCustomization)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageCustomization)
    afx_msg void OnBnClickedCheckCu();
    afx_msg void OnBnClickedCheckBadFlashRate();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()   

    virtual void OnOK();
	virtual BOOL OnKillActive();

    CButton m_btnCheckCU;
    CEdit m_editCU;  
    CString m_strCU;
    CString m_strEditCU;
    BOOL m_bChangedCU;

    CButton m_btnCheckRoot;
    BOOL m_bCheckRoot;

    CString m_strIniFile;

    BOOL m_bCheckBadFlashRate;
    int m_nBadFlashRate;
    CButton m_btnBadFlashRate;
    CEdit m_editBadFlashRate;
    int m_nEditBadFlashRate;
    CStatic m_GroupCu;
    CStatic m_GroupRoot;
    BOOL m_bCheckInproduction;
};

