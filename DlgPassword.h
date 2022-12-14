#if !defined(AFX_DLGPASSWORD_H__2F0B81E4_A0F5_449C_8F73_8417B737F157__INCLUDED_)
#define AFX_DLGPASSWORD_H__2F0B81E4_A0F5_449C_8F73_8417B737F157__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPassword.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPassword dialog

class CDlgPassword : public CDialog
{
// Construction
public:
	CDlgPassword(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPassword)
	enum { IDD = IDD_DLG_PASSWORD };
	CString	m_strPassword;
	CString	m_strNewPassword;
	CString	m_strNewPassword2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPassword)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL CheckPassword();

	BOOL EncodeString(const CString &strSrc,CString &strDst);
	BOOL DecodeString(const CString &strSrc,CString &strDst);

	void ModifyBorder();

	// Generated message map functions
	//{{AFX_MSG(CDlgPassword)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePassword();
	afx_msg void OnChangeNewPassword();
	afx_msg void OnChangeNewPassword2();
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bReset;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPASSWORD_H__2F0B81E4_A0F5_449C_8F73_8417B737F157__INCLUDED_)
