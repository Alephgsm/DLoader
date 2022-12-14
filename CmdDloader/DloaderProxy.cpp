#include "StdAfx.h"
#include "DloaderProxy.h"


CDloaderProxy::CDloaderProxy(void)
:m_dwPort(0)
,m_bStop(TRUE)
{
    ZeroMemory( &m_stPi ,sizeof(m_stPi));
	m_dwMultiCount = 1;
	m_bEnableReadNV = FALSE;
	m_strRFixNV.Empty();
    m_strFlashApp.Format(_T("%s\\ResearchDownload.exe"),GetModuleFilePath(FALSE));
}

CDloaderProxy::~CDloaderProxy(void)
{
    if (m_stPi.hProcess)
    {
        CloseHandle(m_stPi.hProcess);
    }
    if (m_stPi.hThread)
    {
        CloseHandle(m_stPi.hThread);
    }
    ZeroMemory( &m_stPi ,sizeof(m_stPi));
}

void CDloaderProxy::SetFlashApp(CString& strFlashAppFile)
{
    if (!strFlashAppFile.IsEmpty())
    {
        m_strFlashApp = strFlashAppFile;
    }
}

void CDloaderProxy::EnableReadFixnv(BOOL bEnable,CString strFixNV /*= _T("")*/)
{
	m_bEnableReadNV = FALSE;
	m_strRFixNV.Empty();
	if (bEnable && !strFixNV.IsEmpty())
	{
		m_bEnableReadNV = TRUE;
		m_strRFixNV.Format(_T("\"%s\""),strFixNV);
	}
}
BOOL CDloaderProxy::StartDloader()
{
    BOOL bRet = FALSE;
    STARTUPINFO si;

    ZeroMemory( &si, sizeof(si));
    si.cb = sizeof(si);
    si.wShowWindow = SW_HIDE;
    si.dwFlags=STARTF_USESHOWWINDOW| STARTF_USESTDHANDLES;
    ZeroMemory( &m_stPi ,sizeof(m_stPi));
    do 
    {
        if (0 == m_dwPort || 0 == m_dwMultiCount)
        {
            _tprintf(_T("[ERROR] Invalid COM Port.\n"));
            break;
        }

        CFileFind finder;

        if (!finder.FindFile(m_strPacFile.operator LPCTSTR()))
        {
            _tprintf(_T("[ERROR] Pac file [%s] don't exist.\n"),m_strPacFile);
            break;
        }
        finder.Close();

        if (!finder.FindFile(m_strFlashApp.operator LPCTSTR()))
        {
            _tprintf(_T("[ERROR] Dloader application [%s] don't exist.\n"),m_strFlashApp);
            break;
        }
        finder.Close();

        CString strCmdline;	
        strCmdline.Format(_T("\"%s\" -pac \"%s\" -port %d -count %d"),m_strFlashApp,m_strPacFile,m_dwPort,m_dwMultiCount);
		if (m_bEzMode)
		{
			strCmdline += _T(" -EZMode");
		}

		if (m_bEnableReadNV)
		{
			strCmdline += _T(" -ReadNV ");
			strCmdline += m_strRFixNV;
		}

        bRet = CreateProcess(NULL,(TCHAR*)(strCmdline.operator LPCTSTR()),NULL,NULL,FALSE,0,NULL,NULL,&si,&m_stPi);
        if ( !bRet )
        {
            _tprintf(_T("[ERROR] CreateProcess [\"%s\"] failed! [Errorcode=0x%x].\n"),strCmdline,GetLastError());
        }
        else
        {
            m_bStop = FALSE;
        }
    } while (0);

    return bRet;
}

void CDloaderProxy::ExitDloader()
{
    if (!m_bStop && m_stPi.hProcess && m_stPi.dwProcessId)
    { 
        m_cProcMgr.KillXProcess_ByPid(m_stPi.dwProcessId);
        m_bStop = TRUE;
    }
}
void CDloaderProxy::KillDloader()
{
    m_cProcMgr.KillXProcess_ByFullPath(m_strFlashApp);  
}
BOOL CDloaderProxy::IsEnd()
{
    if( !m_bStop )
    {
        if(m_stPi.hProcess && WAIT_OBJECT_0 == WaitForSingleObject(m_stPi.hProcess,1000) )
        {
            CloseHandle(m_stPi.hProcess);
            CloseHandle(m_stPi.hThread);
            ZeroMemory( &m_stPi ,sizeof(m_stPi));
            m_bStop = TRUE;
        }
    }
    return m_bStop;
}