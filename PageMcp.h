#if !defined(AFX_PAGEMCP_H__AD07A222_D542_44CE_B334_9919886AE47F__INCLUDED_)
#define AFX_PAGEMCP_H__AD07A222_D542_44CE_B334_9919886AE47F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMcp.h : header file
//

typedef CMap<CString, LPCTSTR, CString, LPCTSTR> MAP_MCP;

/////////////////////////////////////////////////////////////////////////////
// CPageMcp dialog

class CPageMcp : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMcp)

// Construction
public:
	CPageMcp();
	~CPageMcp();

	BOOL	LoadSettings(LPCTSTR pFileName,CString &strErrorMsg);
	BOOL	SaveSettings(LPCTSTR pFileName);

// Dialog Data
	//{{AFX_DATA(CPageMcp)
	enum { IDD = IDD_PROPPAGE_MCP };
	CListCtrl	m_lstMcpType;
	BOOL	    m_bTmpCheckMcpType;
	//}}AFX_DATA

	MAP_MCP			m_mapAllMcpType;
	MAP_MCP			m_mapCheckMcpType;
	CStringArray	m_agMcpIDs;
	BOOL            m_bCheckMcpType;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageMcp)
	public:
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageMcp)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkMcptype();
	afx_msg void OnAddMcp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bListInit;
	CString m_strCfgFile;

private:
	BOOL FillList();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEMCP_H__AD07A222_D542_44CE_B334_9919886AE47F__INCLUDED_)
