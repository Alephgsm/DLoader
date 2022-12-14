#if !defined(AFX_PAGEMULTILANG_H__9689358C_5BBC_41B0_A1EF_B3442B452A4C__INCLUDED_)
#define AFX_PAGEMULTILANG_H__9689358C_5BBC_41B0_A1EF_B3442B452A4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMultiLang.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageMultiLang dialog

class CPageMultiLang : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMultiLang)

// Construction
public:
	CPageMultiLang();
	~CPageMultiLang();

	BOOL	LoadSettings(LPCTSTR pFileName,CString &strErrorMsg);
	BOOL	SaveSettings(LPCTSTR pFileName);
	void    SetLangFlag(BOOL bRightNV,const CUIntArray &agLangFlag, FILETIME *pLastWrite, BOOL bShowWnd);
	BOOL    IsSameNV(FILETIME *pLastWrite, BOOL bShowWnd);

// Dialog Data
	//{{AFX_DATA(CPageMultiLang)
	enum { IDD = IDD_DIALOG_MULTI_LANG }; //lint !e30
	CListCtrl	m_lstLang;
	BOOL	m_bBackup;   //temp var
	CString	m_strTips;
	//}}AFX_DATA
	CStringArray m_agLangNames;

	CUIntArray   m_agLangFlag;
	BOOL		 m_bRightNV; 
	BOOL         m_bBackupLang;
	FILETIME     m_ftLastWrite;

	CUIntArray   m_agTmpLangFlag;
	BOOL         m_bTmpRightNV;
	FILETIME     m_ftTmpLastWrite;
	

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageMultiLang)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageMultiLang)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkBackupLang();
	afx_msg void OnItemchangedLst(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkDmlLstLang(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bListInit;
	CImageList* m_pImglist;


private:
	BOOL	FillList();
	void    ShowLang(BOOL bShow);
	BOOL    IsFileTimeEqual(FILETIME *pft1,FILETIME *pft2);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEMULTILANG_H__9689358C_5BBC_41B0_A1EF_B3442B452A4C__INCLUDED_)
