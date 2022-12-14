// FileDlg.cpp: implementation of the CFileDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DLoader.h"
#include "FileDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

_TCHAR CFileDlg::SZ_SEL_DIR[ _MAX_PATH ] = _T("");
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileDlg::CFileDlg(BOOL bOpen /* = TRUE */) : m_bOpen(bOpen)
{
    m_hwndOwner = NULL;
}

CFileDlg::~CFileDlg()
{
    m_hwndOwner = NULL;
}

int CALLBACK CFileDlg::BrowseProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	UNUSED_ALWAYS(lParam);
	UNUSED_ALWAYS(lpData);
	if( uMsg == BFFM_INITIALIZED )
	{
		BOOL bSel = FALSE;
		if( _tcslen(SZ_SEL_DIR) == 2 ) // �̷�
		{
			_tcscat(SZ_SEL_DIR, _T("\\"));
			bSel = TRUE;
		}
		else
		{
			WIN32_FIND_DATA wfd;
			HANDLE hFind = ::FindFirstFile(SZ_SEL_DIR, &wfd);
			if( INVALID_HANDLE_VALUE != hFind )
			{
				::FindClose(hFind);
				bSel = TRUE;
			}
		}

		if( bSel )
			::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)SZ_SEL_DIR);
	}

	return 0;
}

// ��ȡĿ¼��Ŀ¼�����ڷ���ֵΪ��("")��Ŀ¼������󲻺�\"
CString CFileDlg::GetSelectDir()
{
	::CoInitialize(NULL);

    LPMALLOC lpMalloc = NULL;
    SZ_SEL_DIR[0] = _T('\0');
    if(::SHGetMalloc(&lpMalloc) == E_FAIL)
    {
        return SZ_SEL_DIR;
    }

    BROWSEINFO browseInfo;

    browseInfo.hwndOwner      = m_hwndOwner;
    browseInfo.pidlRoot       = NULL; 
    browseInfo.pszDisplayName = NULL;
    browseInfo.lpszTitle      = NULL;   
    browseInfo.ulFlags        = BIF_RETURNONLYFSDIRS;      
    browseInfo.lpfn           = BrowseProc;       
    browseInfo.lParam         = 0;    

    LPITEMIDLIST lpItemIDList = NULL;
    if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
    {
        ::SHGetPathFromIDList(lpItemIDList, SZ_SEL_DIR);
      
        lpMalloc->Free(lpItemIDList);
        lpMalloc->Release();      
    }
        
    ::CoUninitialize();

    // ���Ϊ�̷�����Ҫȥ��ĩβ��'\'
    if( _tcslen(SZ_SEL_DIR) == 3 )
        SZ_SEL_DIR[2] = 0;

    return SZ_SEL_DIR;
}

//  ��ȡ�ļ���·����������ļ������ڣ�����Ϊ��("")
CString CFileDlg::GetPathName( void )
{
    CFileDialog filedlg( m_bOpen,     m_strExt, 
                         NULL,        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                         m_strFilter, NULL );

    // CFileDialog Ĭ�ϵ�·������󳤶�Ϊ_MAX_PATH��ʵ��ʹ���п��ܻ���ֳ����� 
    // ��Χ���ļ�
    const int nPathLen = _MAX_PATH * 2;
    static _TCHAR szPath[nPathLen] = { 0 };

    memset(szPath, 0, nPathLen);

    filedlg.m_ofn.lpstrFile = szPath;
    filedlg.m_ofn.nMaxFile  = nPathLen;

    if(filedlg.DoModal() == IDCANCEL)
    {
        return _T("");
    }

    CString strPathName = filedlg.GetPathName();
    if( m_bOpen )
    {
         WIN32_FIND_DATA wfd;
        HANDLE hFind = ::FindFirstFile(strPathName, &wfd);
        if( hFind != INVALID_HANDLE_VALUE )
        {
            ::FindClose(hFind);
            return strPathName;
        }
        else
        {
            AfxMessageBox(_T("The file not exist!"));
            return _T("");
        }
    }
    else
    {
        return strPathName;
    }
}