// FileDlg.h: interface for the CFileDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEDLG_H__32FC0DB1_80B1_484A_B696_F14CEA1C38D8__INCLUDED_)
#define AFX_FILEDLG_H__32FC0DB1_80B1_484A_B696_F14CEA1C38D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// �����Ի���ʵ���ļ���Ŀ¼ѡ���ܣ��ɲο�MFC��CFileDialog��ʵ�ֺͰ���
class CFileDlg  
{
public:
    
    void SetFileFilter( LPCTSTR pszFilter ) { m_strFilter = pszFilter; }
    void SetFileExt( LPCTSTR pszExt ) { m_strExt = pszExt; }
	void SetHwnOwner(HWND hwnd) {m_hwndOwner = hwnd;}
    //  ��ȡ�ļ���·����������ļ������ڣ�����Ϊ��("")
    CString GetPathName(void);

    // ��ȡĿ¼��Ŀ¼�����ڷ���ֵΪ��("")��Ŀ¼������󲻺�\"
    CString GetSelectDir();

public:
    
	CFileDlg(BOOL bOpen = TRUE);
	~CFileDlg();

private:
    static int CALLBACK BrowseProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData );

    CString m_strFilter;
    CString m_strExt;
    HWND m_hwndOwner;
    BOOL   m_bOpen;
    static _TCHAR SZ_SEL_DIR[ _MAX_PATH ];
};

#endif // !defined(AFX_FILEDLG_H__32FC0DB1_80B1_484A_B696_F14CEA1C38D8__INCLUDED_)
