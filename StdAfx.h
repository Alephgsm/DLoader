// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__FAD27263_5E57_4863_9DAB_A8D25896C813__INCLUDED_)
#define AFX_STDAFX_H__FAD27263_5E57_4863_9DAB_A8D25896C813__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4996 4121)
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxtempl.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "Global.h"
#define EZ_MODE_STATUS_FLAG   _T("[Status]")
#define EZ_MODE_REPORT_FLAG   _T("[Report]")


// Format of pSection
// "Key1 = data1\0Key2 = data2\0....Keyn = datan\0\0"
int EnumKeys(LPCTSTR pSection,CStringArray* pKeys);

long GetDigit(LPCTSTR lpszText);
__int64 GetDigitEx(LPCTSTR lpszText);
float GetFloatDigit(LPCTSTR lpszText);

void GetModuleFilePath( HMODULE hModule, LPTSTR lpszAppPath );

BOOL CreateDeepDirectory(LPCTSTR lpszDir);

BOOL GetFilePathInfo(LPCTSTR lpszFile, CStringArray &agInfo);
 
int SplitStr(LPCTSTR lpszStr,CStringArray &agStrs,TCHAR chSeparate = _T(','));
CString GetStrFilePath(LPCTSTR lpFile,	BOOL bMakeUpper = FALSE,	BOOL bPostfix = FALSE);
CString GetStrFileTitle(LPCTSTR lpFile, BOOL bMakeUpper = FALSE,	BOOL bPostfix = FALSE);
CString GetFilePostfix(LPCTSTR lpFile,  BOOL bMakeUpper = FALSE);
BOOL DeleteDirectory(LPCTSTR lpszDirName,BOOL bRmFolder = TRUE);
unsigned __int64 GetFileSizeEx( LPCTSTR lpFileName);
BOOL LoadFileFromLocal(LPCTSTR pszFileName, LPBYTE &pBuf,DWORD &dwSize,unsigned __int64 llSize = 0, unsigned __int64 llOffset = 0);
BOOL SaveFileToLocal(LPCTSTR pszFileName, LPBYTE pBuf,DWORD dwSize);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__FAD27263_5E57_4863_9DAB_A8D25896C813__INCLUDED_)
