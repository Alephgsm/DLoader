#if !defined(AFX_BARCODEDLG_H__C7D74CD5_21A7_40F8_B810_6E42D31C4A8F__INCLUDED_)
#define AFX_BARCODEDLG_H__C7D74CD5_21A7_40F8_B810_6E42D31C4A8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BarcodeDlg.h : header file
//
#include "FilterEdit.h"
#include "Resource.h"
#include "XRandom.h"

/////////////////////////////////////////////////////////////////////////////
// CBarcodeDlg dialog
class CBarcodeDlg : public CDialog
{
// Construction
public:
	CBarcodeDlg(CWnd* pParent = NULL);   // standard constructor
    CBarcodeDlg(int nPort, CWnd* pParent = NULL);
    virtual ~CBarcodeDlg(void);

// Dialog Data
	//{{AFX_DATA(CBarcodeDlg)
	enum { IDD = IDD_DIALOG_BARCODE };  //lint !e30
	CButton	m_btnPause;
	CButton	m_btnOK;
	CFilterEdit	m_edtBarcode;
	CString	m_strBarcode;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBarcodeDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
    void SetSNInputMethod(BOOL bAutoInput = FALSE) //设置是否自动产生SN号.
	{
		m_bAutoInputSN = bAutoInput;
	}

	BOOL m_bAutoInputSN;
	int  GetPortNum() const{
		return m_nPort;
	};
	void Close()
	{
		CDialog::OnCancel();
	}

public:	
	CString   m_str_title;
	int m_nBarcodeLength;
	
protected:
//    void GetErrorDetail(int nErrorCode);
    

    int  m_nPort;
	BOOL m_bAutoStart;
	BOOL m_bSNWithDateTime;
	
    CXRandom m_rand;
    CString m_strFixedSN;
    
   
// Implementation
protected:
    DWORD m_dwTick;
	// Generated message map functions
	//{{AFX_MSG(CBarcodeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnButtonSuspend();
	virtual void OnOK();
	afx_msg void OnChangeEditBarcode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BARCODEDLG_H__C7D74CD5_21A7_40F8_B810_6E42D31C4A8F__INCLUDED_)
