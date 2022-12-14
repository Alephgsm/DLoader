#pragma once
#include "afxwin.h"


// CCombCtrlDlg dialog

class CCombCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CCombCtrlDlg)

public:
	CCombCtrlDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCombCtrlDlg();

// Dialog Data
	enum { IDD = IDD_COMBOX_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CComboBox m_cbCtrl;
    int m_nItem;
    int m_nSubItem;
    int m_nCurSel;
    CString	m_strText;
    CStringArray m_agString;
    void SetCellComboContent(CONST CStringArray &clOptions);
    afx_msg void OnDestroy();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnCbnKillfocusComboList();
    virtual BOOL OnInitDialog();
};
