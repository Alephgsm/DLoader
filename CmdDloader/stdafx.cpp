// stdafx.cpp : source file that includes just the standard includes
// CmdDloader.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#if _MSC_VER >= 1300    // for VC 7.0 
// from ATL 7.0 sources 
#ifndef _delayimp_h
extern "C" IMAGE_DOS_HEADER __ImageBase; 
#endif 
#endif     

HMODULE GetCurrentModule() 
{ 
#if _MSC_VER < 1300    // earlier than .NET compiler (VC 6.0)           
    // Here's a trick that will get you the handle of the module     
    // you're running in without any a-priori knowledge:    
    MEMORY_BASIC_INFORMATION mbi;     
    static int dummy;     
    VirtualQuery( &dummy, &mbi, sizeof(mbi) );

    return reinterpret_cast<HMODULE>(mbi.AllocationBase); 
#else // VC 7.0     
    // from ATL 7.0 sources     
    return reinterpret_cast<HMODULE>(&__ImageBase); 
#endif 
} 


CString GetModuleFilePath(BOOL bPostfix/* = TRUE*/)//{"D:\View\Debug\"}
{  
    //Get the current running path
    _TCHAR  szPath[MAX_PATH] = {0};
    _TCHAR* pFoundChar       = NULL; 
    _TCHAR* pNextChar        = NULL;
    GetModuleFileName( GetCurrentModule(), szPath, MAX_PATH );
    pFoundChar = pNextChar = szPath;
    while(pFoundChar)
    {
        //Search the first char '\'
        pFoundChar = _tcschr( pNextChar + 1,_T('\\'));
        //find the char '\'
        if(pFoundChar != NULL)
        {
            //move to the next char pointer to the current address 
            pNextChar = pFoundChar;
            continue;
        }
    }

    //escape the program name 
    *pNextChar = _T('\0');
    CString strModuleDir(szPath);
    if (bPostfix)
    {
        strModuleDir+=_T('\\');
    }
    return strModuleDir;

}

int SplitStr(LPCTSTR lpszStr,CStringArray &agStrs,TCHAR chSeparate /*= _T(',')*/)
{
	agStrs.RemoveAll();

	int nLen = _tcslen(lpszStr);

	if(nLen == 0)
		return 0;

	LPTSTR  lpBuf  = (LPTSTR)lpszStr;
	LPTSTR  lpFind = _tcschr(lpBuf, chSeparate);
	CString strTmp;
	while(lpFind != NULL)
	{
		*lpFind = _T('\0');
		strTmp = lpBuf;
		strTmp.TrimLeft();
		strTmp.TrimRight();
		if(!strTmp.IsEmpty())
			agStrs.Add(strTmp);
		lpBuf = lpFind + 1;
		lpFind = _tcschr(lpBuf, chSeparate);
	}

	strTmp = lpBuf;
	strTmp.TrimLeft();
	strTmp.TrimRight();
	if(!strTmp.IsEmpty())
		agStrs.Add(strTmp);

	return agStrs.GetSize();
}

CString GetStrFileTitle( LPCTSTR lpFile,BOOL bPostfix/* = FALSE*/,BOOL bMakeUpper/*=FALSE*/)
{
	CString str(lpFile);
	str.Replace(_T('/'),_T('\\'));
	if (str.Find(_T('\\')) != -1)
		str = str.Right(str.GetLength()-str.ReverseFind(_T('\\'))-1);
	else if (str.Find(_T('/')) != -1)
		str = str.Right(str.GetLength()-str.ReverseFind(_T('/'))-1);

	if (!bPostfix)
	{
		if (str.Find(_T('.')) != -1)
			str = str.Left(str.Find(_T('.')));
	}


	if (bMakeUpper)
		str.MakeUpper();

	return str;
}