// stdafx.cpp : source file that includes just the standard includes
//	DLoader.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

int EnumKeys(LPCTSTR pSection,CStringArray* pKeys)
{
    ASSERT(pSection != NULL);
    ASSERT(pKeys != NULL);
	if(pSection == NULL || pKeys == NULL)
	{
		return 0;
	}
	
    int number = 0;
    for(;;)
    {
        CString strLine = pSection;
        int nLen = strLine.GetLength();
        if(0 == nLen)
        {
            // All keys are read
            break;
        }
        
        int nIndex = strLine.Find(_T('='));
        if(-1 == nIndex)
        {
            // Not a valid key
            continue;
        }
        CString strKey = strLine.Left(nIndex);
        strKey.TrimLeft();
        strKey.TrimRight();
        pKeys->Add(strKey);
		
        CString strData = strLine.Right(nLen - nIndex - 1);
        strData.TrimLeft();
        strData.TrimRight();
        pKeys->Add(strData);
		
        number++;
		
        pSection += nLen + 1;
		
    }
	
    return number;
}

long GetDigit(LPCTSTR lpszText)
{
	if(lpszText == NULL)
		return 0;

    CString strText = lpszText;

    long lRet = 0;
    if(!strText.Left(2).CompareNoCase(_T("0x")))
    {
        // Hex mode
        _stscanf(strText,_T("%x"),&lRet);
    }
    else
    {
        // Decimal mode
        _stscanf(strText,_T("%d"),&lRet);
    }

    return lRet;
}
__int64 GetDigitEx(LPCTSTR lpszText)
{
    if(lpszText == NULL)
        return 0;

    CString strText = lpszText;

    __int64 llRet = 0;
    if(!strText.Left(2).CompareNoCase(_T("0x")))
    {
        // Hex mode
        _stscanf(strText,_T("%llx"),&llRet);
    }
    else
    {
        // Decimal mode
        _stscanf(strText,_T("%lld"),&llRet);
    }

    return llRet;
}
float GetFloatDigit(LPCTSTR lpszText)
{
    if(lpszText == NULL)
        return 0;

    CString strText = lpszText;

    float fRet = 0.0;
    if(!strText.Left(2).CompareNoCase(_T("0x")))
    {
        // Hex mode
        _stscanf(strText,_T("%x"),&fRet);
    }
    else
    {
        // Decimal mode ,float 
        _stscanf(strText,_T("%f"),&fRet);
    }

    return fRet;
}
void GetModuleFilePath( HMODULE hModule, LPTSTR lpszAppPath )
{
    //Get the current running path
    _TCHAR Path[MAX_PATH];
    _TCHAR *pFoundChar=NULL;
    
    GetModuleFileName( hModule, Path, MAX_PATH );
    
    pFoundChar = _tcsrchr(Path,_T('\\'));
	if(pFoundChar != NULL)
	{
		*pFoundChar = _T('\0');
	}
    
    _tcscpy( lpszAppPath,Path);
}

BOOL CreateDeepDirectory(LPCTSTR lpszDir)
{
	if(lpszDir == NULL)
		return FALSE;

	CString			strDir = lpszDir;
	HANDLE			hFile;  
    WIN32_FIND_DATA fileinfo;  
    CStringArray    agDir;  
    BOOL			bOK;  
    int				nCount = 0;  
    CString			strTemp= _T("");  
	
	
    hFile   =   FindFirstFile(strDir,&fileinfo);  
	
    // if the file exists and it is a directory  
    if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)  
    {  
		//     Directory   Exists   close   file   and   return  
		FindClose(hFile);  
		return   TRUE;  
    }  
    agDir.RemoveAll();  
    for( nCount = 0; nCount < strDir.GetLength(); nCount++ )  
    {  
		if( strDir.GetAt(nCount) != _T('\\') )  
			strTemp += strDir.GetAt(nCount);  
		else  
		{  
			agDir.Add(strTemp);  
			strTemp += _T("\\");  
		}  
		if(nCount ==   strDir.GetLength()-1) 
		{
			agDir.Add(strTemp);  
		}
    }  
	
	
    //   Close   the   file  
    FindClose(hFile);  
	
	
    // Now lets cycle through the String Array and create each directory in turn  
    for(nCount = 1; nCount < agDir.GetSize(); nCount++)  
    {  
		strTemp = agDir.GetAt(nCount);  
		bOK = CreateDirectory(strTemp,NULL);  
		
		// If the Directory exists it will return a false  
		if(bOK)
		{
			// If we were successful we set the attributes to normal  
			SetFileAttributes(strTemp,FILE_ATTRIBUTE_NORMAL);  
		}
		else
		{
			if( GetLastError() != ERROR_ALREADY_EXISTS )
 			{ 	
				agDir.RemoveAll(); 
				return FALSE;
 			}  
		}
		
    }  
    // Now lets see if the directory was successfully created  
    hFile   =   FindFirstFile(strDir,&fileinfo);  
	
    agDir.RemoveAll();  
    if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)  
    {  
		// Directory Exists close file and return  
		FindClose(hFile);  
		return   TRUE;  
    }  
    else  
    {  
		// For Some reason the Function Failed Return FALSE  
		FindClose(hFile);  
		return   FALSE;  
    }  
	
}

/*agInfo: file-path,file-name,file-ext */
BOOL GetFilePathInfo(LPCTSTR lpszFile, CStringArray &agInfo)
{
	agInfo.RemoveAll();
	if(lpszFile == NULL)
	{
		return FALSE;
	}
	
	CString strFilePath = lpszFile;
	int nIndx = strFilePath.ReverseFind('\\');
	CString strFileName = strFilePath;
	CString strExt = _T("");
	if(nIndx != -1)
	{
		strFileName = strFilePath.Right(strFilePath.GetLength()-nIndx -1);
		strFilePath = strFilePath.Left(nIndx);
	}
	else
	{
		strFilePath.Empty();
	}
	
	nIndx = strFileName.ReverseFind('.');
	if(nIndx != -1)
	{
		strExt = strFileName.Right(strFileName.GetLength()-nIndx);
		strFileName = strFileName.Left(nIndx);
	}
	
	agInfo.Add(strFilePath);
	agInfo.Add(strFileName);
	agInfo.Add(strExt);
	
	return TRUE;
}

int SplitStr(LPCTSTR lpszStr,CStringArray &agStrs,TCHAR chSeparate /*= _T(',')*/)
{
	agStrs.RemoveAll();

	CString strSrc(lpszStr);
	int nLen = _tcslen(lpszStr);

	if(nLen == 0)
		return 0;

	LPTSTR  lpBuf  = (LPTSTR)strSrc.operator LPCTSTR();
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

CString GetStrFilePath(LPCTSTR lpFile,BOOL bMakeUpper, BOOL bPostfix)//{"C:\Test"}
{
	CString str(lpFile);
	if (str.Find(_T('\\')) != -1)
		str = bPostfix ? str.Left(str.ReverseFind(_T('\\')) + 1) : str.Left(str.ReverseFind(_T('\\')));
	else if (str.Find(_T('/')) != -1)
		str = bPostfix ? str.Left(str.ReverseFind(_T('/')) + 1) : str.Left(str.ReverseFind(_T('/')));
	if (bMakeUpper)
		str.MakeUpper();
	return str;
}


CString GetStrFileTitle(LPCTSTR lpFile, BOOL bMakeUpper, BOOL bPostfix)
{
	CString str(lpFile);
	
	if (str.Find(_T('\\')) != -1)
		str = str.Right(str.GetLength()-str.ReverseFind(_T('\\'))-1);
	else if (str.Find(_T('/')) != -1)
		str = str.Right(str.GetLength()-str.ReverseFind(_T('/'))-1);
	
	if (!bPostfix)
	{
		if (str.ReverseFind(_T('.')) != -1)
			str = str.Left(str.ReverseFind(_T('.')));
	}
	if (bMakeUpper)
		str.MakeUpper();
	return str;
}

CString GetFilePostfix(LPCTSTR lpFile, BOOL bMakeUpper)
{
	CString str(lpFile);
	
	if (str.ReverseFind(_T('.')) != -1)
	{
		str = str.Right(str.GetLength()-str.ReverseFind(_T('.'))-1);
	}
	
	if (bMakeUpper)
	{
		str.MakeUpper();
	}
	return str;
}

BOOL DeleteDirectory(LPCTSTR lpszDirName,BOOL bRmFolder/* = TRUE*/)// DeleteDirectory(_T("c:\\aaa"))
{
    if(lpszDirName== NULL)
        return TRUE;

    CFileFind tempFind;
    BOOL IsFinded = TRUE;
    IsFinded = (BOOL)tempFind.FindFile(lpszDirName);
    if(!IsFinded)
    {
        tempFind.Close();
        return TRUE;
    }
    tempFind.Close();

    DWORD dwAtt=0;
    _TCHAR tempFileFind[MAX_PATH] = {0};
    _stprintf(tempFileFind,_T("%s\\*.*"),lpszDirName);
    IsFinded=(BOOL)tempFind.FindFile(tempFileFind);
    while(IsFinded)
    {
        IsFinded=(BOOL)tempFind.FindNextFile();
        if(!tempFind.IsDots())
        {
            _TCHAR foundFileName[MAX_PATH] = {0};
            _tcscpy(foundFileName,tempFind.GetFileName().GetBuffer(MAX_PATH));
            if(tempFind.IsDirectory())
            {
                _TCHAR tempDir[MAX_PATH] = {0};
                _stprintf(tempDir,_T("%s\\%s"),lpszDirName,foundFileName);
                DeleteDirectory(tempDir);
            }
            else
            {
                _TCHAR tempFileName[MAX_PATH] = {0};
                _stprintf(tempFileName,_T("%s\\%s"),lpszDirName,foundFileName);
                dwAtt = 0;						
                dwAtt	= ::GetFileAttributes(tempFileName);
                dwAtt &= ~FILE_ATTRIBUTE_READONLY;
                ::SetFileAttributes(tempFileName,dwAtt);		
                DeleteFile(tempFileName);
            }
        }
    }
    tempFind.Close();
    dwAtt = 0;						
    dwAtt	= ::GetFileAttributes(lpszDirName);
    dwAtt &= ~FILE_ATTRIBUTE_READONLY;
    ::SetFileAttributes(lpszDirName,dwAtt);	
    if(bRmFolder)
    {
       RemoveDirectory(lpszDirName);
    }
    return TRUE;
}


unsigned __int64 GetFileSizeEx( LPCTSTR lpFileName)
{
    if (NULL == lpFileName)
    {
        return 0;
    }
    HANDLE			 hResult =  NULL;    
    WIN32_FIND_DATA	 FindFileData;  
    unsigned __int64 llSize = 0;
    FindFileData.nFileSizeHigh = FindFileData.nFileSizeLow = 0;

    hResult  =  FindFirstFile(lpFileName, &FindFileData);   
    if(hResult  !=  INVALID_HANDLE_VALUE)   
    {   
        llSize = FindFileData.nFileSizeHigh ;
        llSize = llSize<<32;
        llSize |= FindFileData.nFileSizeLow;
        FindClose(hResult);
        hResult = NULL;
    }   
    return llSize;

} 

BOOL LoadFileFromLocal(LPCTSTR pszFileName, LPBYTE &pBuf,DWORD &dwSize,unsigned __int64 llSize /*= 0*/, unsigned __int64 llOffset /*= 0*/)
{	
    FILE *pFile = _tfopen(pszFileName,_T("rb"));
    if(pFile == NULL)
        return FALSE;
	if (llSize)
	{
		dwSize = (DWORD)llSize;
	}
	else
	{
		fseek(pFile,0,SEEK_END);
		dwSize = ftell(pFile);
		fseek(pFile,0,SEEK_SET);
	}

	if (llOffset)
	{
		fseek(pFile,(long)llOffset,SEEK_SET);
	}

    pBuf = new BYTE[dwSize];
    if(pBuf == NULL)
        return FALSE;

    DWORD dwRead = fread(pBuf,1,dwSize,pFile);
    fclose(pFile);

    if(dwRead != dwSize)
    {
        SAFE_DELETE_ARRAY(pBuf);
        return FALSE;
    }

    return TRUE;
}

BOOL SaveFileToLocal(LPCTSTR pszFileName, LPBYTE pBuf,DWORD dwSize)
{
    CString strFilePath(pszFileName);
    FILE *pFile = _tfopen(strFilePath,_T("wb"));
    if(pFile == NULL)
        return FALSE;

    DWORD dwWrite = fwrite(pBuf,1,dwSize,pFile);
    fclose(pFile);

    if(dwWrite != dwSize)
    {
        return FALSE;
    }

    return TRUE;
}
