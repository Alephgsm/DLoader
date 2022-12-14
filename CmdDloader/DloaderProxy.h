#pragma once
#include "ProcessMgr.h"
class CDloaderProxy
{
public:
    CDloaderProxy(void);
    virtual ~CDloaderProxy(void);
public:
    void SetFlashArgv(CString& strPacFile,DWORD dwPort,DWORD dwCount = 1,BOOL bEzMode = FALSE){m_strPacFile = strPacFile;m_dwPort=dwPort;m_bEzMode=bEzMode;m_dwMultiCount=dwCount;}
	void EnableReadFixnv(BOOL bEnable,CString strFixNV = _T(""));
    void SetFlashApp(CString& strFlashAppFile);
    BOOL StartDloader();
    void ExitDloader();
    void KillDloader();
    BOOL IsEnd();
private:
    BOOL    m_bStop;
	BOOL    m_bEzMode;
	BOOL	m_bEnableReadNV;
    CString m_strFlashApp;
    CString m_strPacFile;
	CString m_strRFixNV;
    DWORD   m_dwPort;
	DWORD   m_dwMultiCount;
    PROCESS_INFORMATION m_stPi;
    ProcessMgr m_cProcMgr;
};
