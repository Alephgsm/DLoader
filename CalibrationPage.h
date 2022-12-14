#if !defined(AFX_CALIBRATIONGPAGE_H__5EBF3FE0_ED59_4346_8F73_C3E8B91AE400__INCLUDED_)
#define AFX_CALIBRATIONGPAGE_H__5EBF3FE0_ED59_4346_8F73_C3E8B91AE400__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalibrationgPage.h : header file
//
#include "NvBackupOptList.h"

extern BOOL g_bInitSheet;
/////////////////////////////////////////////////////////////////////////////
// CCalibrationPage dialog

class CCalibrationPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CCalibrationPage)
        
// Construction
public:
	CCalibrationPage();   // standard constructor
    ~CCalibrationPage(); 

// Dialog Data
	//{{AFX_DATA(CCalibrationPage)
	enum { IDD = IDD_DIALOG_CALIBRATION_SETTINGS };
	CListCtrl	m_lstFileBackup;
	CStatic	m_stcFrame;	
	CString	m_strPath;
	BOOL	m_bTmpSaveToLocal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalibrationPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();
	virtual BOOL OnKillActive();
	//}}AFX_VIRTUAL

// Implementation
public:
    BOOL SaveSettings(LPCTSTR pFileName);
    BOOL LoadSettings(LPCTSTR pFileName,CString &strErrorMsg);

	void InitFileBackupList();

	void AddBackupFiles(FILE_INFO_T *pFileInfoArr, int nCount, BOOL bInit);
	int  GetBackupFiles(CStringArray &agID,BOOL bSharkNand = FALSE);
	int  GetNVBackupFiles(CStringArray &agID);
    BOOL IsEraseIMEI();

public:
	CNvBackupOptList	m_lstBackup;
	CString             m_strCurProduct;
	CString             m_strTempCurProduct;
	CString				m_strLocalPath;
	BOOL                m_bSaveToLocal;
	std::vector<FILE_INFO_T> m_vBackFiles;
	int                 m_nBackupProdnvMiscdata;
    // 0 - no backup prodnv & miscdata; 1 - backup prodnv & miscdata; 2 - only backup miscdata
    
protected:
	// Generated message map functions
	//{{AFX_MSG(CCalibrationPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkSaveToLocal();
	afx_msg void OnDcBtnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_strIniFile;
public:
    afx_msg void OnNMClickLstFileBackup(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALIBRATIONGPAGE_H__5EBF3FE0_ED59_4346_8F73_C3E8B91AE400__INCLUDED_)
