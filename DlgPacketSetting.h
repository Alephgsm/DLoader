#if !defined(AFX_DLGPACKETSETTING_H__C0B7BCE4_4BCD_4F12_9452_20F13E53194B__INCLUDED_)
#define AFX_DLGPACKETSETTING_H__C0B7BCE4_4BCD_4F12_9452_20F13E53194B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPacketSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPacketSetting dialog

class CDlgPacketSetting : public CDialog
{
// Construction
public:
	CDlgPacketSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPacketSetting)
	enum { IDD = IDD_DLG_PAKET_SETTING };
	CEdit	m_edtPrdAlias;
	CEdit	m_edtVersion;
	CEdit	m_edtPath;
	CString	m_strPath;
	CString	m_strVersion;
	CString	m_strPrdName;
	CString	m_strPrdAlias;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPacketSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPacketSetting)
	virtual void OnOK();
	afx_msg void OnBtnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPACKETSETTING_H__C0B7BCE4_4BCD_4F12_9452_20F13E53194B__INCLUDED_)
