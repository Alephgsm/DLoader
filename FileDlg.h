// FileDlg.h: interface for the CFileDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEDLG_H__32FC0DB1_80B1_484A_B696_F14CEA1C38D8__INCLUDED_)
#define AFX_FILEDLG_H__32FC0DB1_80B1_484A_B696_F14CEA1C38D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 弹出对话框，实现文件和目录选择功能，可参考MFC的CFileDialog的实现和帮助
class CFileDlg  
{
public:
    
    void SetFileFilter( LPCTSTR pszFilter ) { m_strFilter = pszFilter; }
    void SetFileExt( LPCTSTR pszExt ) { m_strExt = pszExt; }
	void SetHwnOwner(HWND hwnd) {m_hwndOwner = hwnd;}
    //  获取文件的路径名，如果文件不存在，返回为空("")
    CString GetPathName(void);

    // 获取目录，目录不存在返回值为空("")，目录名的最后不含\"
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
