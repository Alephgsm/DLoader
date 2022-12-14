#if !defined(AFX_FLASHOPTPAGE_H__F0AE4C5F_B91F_4AF2_A1BD_831B0C22BF22__INCLUDED_)
#define AFX_FLASHOPTPAGE_H__F0AE4C5F_B91F_4AF2_A1BD_831B0C22BF22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlashOptPage.h : header file
//

typedef CMap<CString, LPCTSTR, CString, LPCTSTR>   MAP_DLFILE;  //id->file

/////////////////////////////////////////////////////////////////////////////
// CFlashOptPage dialog

class CFlashOptPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFlashOptPage)

// Construction
public:
	CFlashOptPage();
	~CFlashOptPage();
// Dialog Data
	//{{AFX_DATA(CFlashOptPage)
	enum { IDD = IDD_PROPPAGE_FLASH };
	CListCtrl	m_lstWrite;
	CListCtrl	m_lstErase;
	CListCtrl	m_lstRead;
	BOOL	m_bTmpActiveRead;
	BOOL	m_bTmpEraseAll;
	BOOL	m_bTmpActiveErase;
	BOOL	m_bTmpActiveWrite;
	//}}AFX_DATA

	CUIntArray m_agFlashOpr;
	BOOL	   m_bActiveRead;
	BOOL	   m_bActiveErase;
	BOOL	   m_bActiveWrite;
	BOOL	   m_bEraseAll;

	BOOL	LoadSettings(LPCTSTR pFileName,CString &strErrorMsg);
	BOOL	SaveSettings(LPCTSTR pFileName);
	void	InitReadFlashCfg(PPRODUCT_INFO_T pCurProduct,LPCTSTR pFileName, MAP_DLFILE& mapDLFiles);
	__int64 GetPartitionSize(LPCTSTR lpPartName,PPRODUCT_INFO_T pProdInfo);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFlashOptPage)
	public:
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	Clear();
	void    InitList(CListCtrl &ctrList,int nList);
	void    FillList(CListCtrl &ctrList,CStringArray &agList,int nList);

	BOOL    LoadFlashOpr(LPCTSTR lpszSec, CStringArray &agList);

	void    SaveFlashOpr(CListCtrl &ctrList,LPCTSTR lpszSec,LPCTSTR lpszFile);

	void    ClickList(NMHDR* pNMHDR,CListCtrl &ctrList,int nList);
	
	// Generated message map functions
	//{{AFX_MSG(CFlashOptPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickLstRead(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditLstRead(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnClear();
	afx_msg void OnChkActiveReadFlash();
	afx_msg void OnEraseClear();
	afx_msg void OnWriteClear();
	afx_msg void OnChkActiveEraseFlash();
	afx_msg void OnChkActiveWriteFlash();
	afx_msg void OnClickLstErase(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickLstWrite(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditLstErase(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditLstWrite(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CStringArray m_agReadFlash;
	CStringArray m_agEraseFlash;
	CStringArray m_agWriteFlash;
	CString m_strIniFile;

public:
	afx_msg void OnSelAllReadflash();
	BOOL m_bSelAllReadItems;
	afx_msg void OnReadItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLASHOPTPAGE_H__F0AE4C5F_B91F_4AF2_A1BD_831B0C22BF22__INCLUDED_)
