// ProcessMgr.h: interface for the ProcessMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSMGR_H__FB4F4993_B188_4DFF_95E9_88D812045021__INCLUDED_)
#define AFX_PROCESSMGR_H__FB4F4993_B188_4DFF_95E9_88D812045021__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HANDLE_FREE(p) do{\
	if(p)\
{\
	CloseHandle(p);\
	p = NULL;\
}\
}while(0)


typedef DWORD (WINAPI* GetProcessImageFileNamePtr)(HANDLE hProcess,LPTSTR lpImageFileName,DWORD dwSize);
class ProcessMgr  
{
public:
	ProcessMgr();
	virtual ~ProcessMgr();
	BOOL	IsSingleIntance(LPCTSTR lpProcess);
	void	KillXProcess_ByPid(DWORD dwPid);
	void	KillXProcess_ByFullPath(LPCTSTR lpProcessFile);
	BOOL	FindProcess(LPCTSTR lpProcessName);
	DWORD	GetProcCountByFullPath(LPCTSTR lpProcessFile);
private:
	BOOL	LoadPsapiDll();
	BOOL    IsDiscoveredProc(HANDLE hProc,LPCTSTR lpProcFile);
	CString ConvertLogicPath2DosDrive(LPCTSTR szDrive);

private:
	HMODULE			m_hMoudle;
	GetProcessImageFileNamePtr	m_pGetProcessImageFileName;

};

#endif // !defined(AFX_PROCESSMGR_H__FB4F4993_B188_4DFF_95E9_88D812045021__INCLUDED_)
