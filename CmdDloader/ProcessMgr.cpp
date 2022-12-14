// ProcessMgr.cpp: implementation of the ProcessMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <TlHelp32.h>
#include "ProcessMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ProcessMgr::ProcessMgr()
{
	m_hMoudle = NULL;
	m_pGetProcessImageFileName = NULL;
	LoadPsapiDll();
}

ProcessMgr::~ProcessMgr()
{
	if(m_hMoudle)
	{
		FreeLibrary(m_hMoudle);
		m_hMoudle = NULL;
	}
}
BOOL ProcessMgr::IsSingleIntance(LPCTSTR lpProcess)
{		
	if(NULL == lpProcess)
	{
		return FALSE;
	}
	DWORD dwCount = GetProcCountByFullPath(lpProcess);
	return dwCount==1 ? TRUE :FALSE;
}
DWORD	ProcessMgr::GetProcCountByFullPath(LPCTSTR lpProcessFile)
{
	if (NULL == lpProcessFile || 2 > _tcslen(lpProcessFile))
	{
		return 0;
	}

	CString strDosProcFile = ConvertLogicPath2DosDrive(lpProcessFile);
	DWORD dwCount = 0;
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;  
	PROCESSENTRY32 pe32;  
	hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);   
	pe32.dwSize=sizeof(PROCESSENTRY32);  
	if(INVALID_HANDLE_VALUE !=hProcessSnap && Process32First(hProcessSnap,&pe32))  
	{  
		do 
		{  
			HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS,0,pe32.th32ProcessID);
			if (hProc && IsDiscoveredProc(hProc,strDosProcFile.operator LPCTSTR()))
			{
				++dwCount;
			}
			HANDLE_FREE(hProc);
		}  
		while(Process32Next(hProcessSnap,&pe32));  	
	}  
	if (INVALID_HANDLE_VALUE !=hProcessSnap)
	{
		CloseHandle(hProcessSnap);  
		hProcessSnap = NULL;
	}	 
	return dwCount;
}
void ProcessMgr::KillXProcess_ByFullPath(LPCTSTR lpProcessFile)
{

	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;  
	PROCESSENTRY32 pe32;  
	CString strDosProcFile = ConvertLogicPath2DosDrive(lpProcessFile);
	hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);   
	pe32.dwSize=sizeof(PROCESSENTRY32);  
	if(INVALID_HANDLE_VALUE !=hProcessSnap && Process32First(hProcessSnap,&pe32))  
	{  
		do 
		{  
			HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS,0,pe32.th32ProcessID);
			if (hProc && IsDiscoveredProc(hProc,strDosProcFile.operator LPCTSTR()))
			{
				TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS,0,pe32.th32ProcessID),0); 
			}
			HANDLE_FREE(hProc);
		}  
		while(Process32Next(hProcessSnap,&pe32));  	
	}  
	if (INVALID_HANDLE_VALUE !=hProcessSnap)
	{
		CloseHandle(hProcessSnap);  
		hProcessSnap = NULL;
	}	 
}
void ProcessMgr::KillXProcess_ByPid(DWORD dwPid)
{
	if (0 == dwPid)		return;
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;  
	PROCESSENTRY32 pe32;  
	hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);   
	pe32.dwSize=sizeof(PROCESSENTRY32);  
	if(INVALID_HANDLE_VALUE !=hProcessSnap && Process32First(hProcessSnap,&pe32))  
	{  
		do 
		{  
			if(pe32.th32ProcessID == dwPid)
			{  
				TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS,0,pe32.th32ProcessID),0);  
				break;
			}		
		}  
		while(Process32Next(hProcessSnap,&pe32));  	
	}  
	if (INVALID_HANDLE_VALUE !=hProcessSnap)
	{
		CloseHandle(hProcessSnap);  
		hProcessSnap = NULL;
		
	}	 
}

BOOL	ProcessMgr::LoadPsapiDll()
{
	m_hMoudle= LoadLibrary(_T("psapi.dll"));
	if (m_hMoudle)
	{
#ifdef UNICODE
		m_pGetProcessImageFileName = (GetProcessImageFileNamePtr)GetProcAddress(m_hMoudle,"GetProcessImageFileNameW");	
#else
		m_pGetProcessImageFileName = (GetProcessImageFileNamePtr)GetProcAddress(m_hMoudle,"GetProcessImageFileNameA");
#endif // !UNICODE
		
		
	}

	return m_pGetProcessImageFileName ? TRUE: FALSE;
}
CString ProcessMgr::ConvertLogicPath2DosDrive(LPCTSTR lpProcFile)
{
	// "\Device\HarddiskVolume3\Bin\DspLogger.exe"		//Dos path
	// "E:\Bin\DspLogger.exe"							//Logic path
	CString strDosPath(lpProcFile);
	if (NULL == lpProcFile || 2 > _tcslen(lpProcFile))
	{
		return strDosPath;
	}
	TCHAR szDosDrive[MAX_PATH] = {0};
	CString strLogicPath(lpProcFile);
	strLogicPath = strLogicPath.Mid(0,2); 
	if (0 != QueryDosDevice(strLogicPath.operator LPCTSTR(), szDosDrive, MAX_PATH))
	{
		strDosPath = strDosPath.Mid(2); 
		strDosPath.Insert(0,szDosDrive);
	}
	return strDosPath;
}
BOOL    ProcessMgr::IsDiscoveredProc(HANDLE hProc,LPCTSTR lpProcFile)
{

	BOOL bRet						= FALSE;
	TCHAR szProcImageFile[MAX_PATH] = {0};

	if ( 
			lpProcFile && hProc && m_pGetProcessImageFileName &&
			m_pGetProcessImageFileName(hProc,szProcImageFile,MAX_PATH)
	   )
	{
			
		if (0 == _tcsicmp(szProcImageFile,lpProcFile))
		{
			bRet = TRUE;
		}
		
	}
	return bRet;
}
BOOL	ProcessMgr::FindProcess(LPCTSTR lpProcessName)
{
	ASSERT(NULL != lpProcessName);
	BOOL bFind = FALSE;
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;  
	PROCESSENTRY32 pe32;  
	hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);   
	pe32.dwSize=sizeof(PROCESSENTRY32);  
	if(INVALID_HANDLE_VALUE !=hProcessSnap && Process32First(hProcessSnap,&pe32))  
	{  
		do 
		{  
			if(0 == _tcsicmp(pe32.szExeFile,lpProcessName))
			{  
				bFind = TRUE;
				break;
			}		
		}  
		while(Process32Next(hProcessSnap,&pe32));  	
	}  
	if (INVALID_HANDLE_VALUE !=hProcessSnap)
	{
		CloseHandle(hProcessSnap);  
		hProcessSnap = NULL;
		
	}	 

	return bFind;
}