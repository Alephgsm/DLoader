#include "stdafx.h"
#include <atlconv.h>
#include "PipeServer.h"

CPipeServer::CPipeServer()
:m_hPipeHandle(NULL)
,m_bConnect(FALSE)
{
	m_bPartialFlash = FALSE;
    LoadCfg();
}

CPipeServer::~CPipeServer(void)
{
    SAFE_CLOSE_HANDLE(m_hPipeHandle);
}

void CPipeServer::LoadCfg()
{
    TCHAR szCfgFile[MAX_PATH]   = {0};
    TCHAR szBuf[MAX_PATH*2]       = {0};
    GetModuleFilePath(NULL,szCfgFile);
    _tcscat(szCfgFile,_T("\\CmdDloader.ini"));

    GetPrivateProfileString(_T("Setting"),_T("PipeServer"),_T("."),szBuf,MAX_PATH,szCfgFile);
    m_strPipeServer = szBuf;

    GetPrivateProfileString(_T("Setting"),_T("PipeName"),_T("ResearchDownload"),szBuf,MAX_PATH,szCfgFile);
    m_strPipeName = szBuf;

	m_bPartialFlash = GetPrivateProfileInt( _T("Setting"), _T("PartialFlash"),FALSE, szCfgFile); 

	GetPrivateProfileString(_T("Setting"),_T("SkipFileID"),_T(""),szBuf,_MAX_PATH*2,szCfgFile);
	SplitStr(szBuf,m_agSkipID,',');

}

BOOL  CPipeServer::IsSkipFile(LPCTSTR lpFileID)
{
	int nCount = m_agSkipID.GetSize();
	if (0 == nCount || NULL == lpFileID)
	{
		return FALSE;
	}
	BOOL bSkip = FALSE;
	for(int i = 0; i< nCount; i++)
	{
		CString strModule = m_agSkipID[i];
		if( 0 == strModule.CompareNoCase(lpFileID) )
		{
			bSkip = TRUE;
			break;
		}
	}
	return bSkip;
}

BOOL CPipeServer::Init(DWORD dwCookie)
{
    int nRetryTime = 5;
    BOOL bOK = FALSE;
    CString strFullPipeName;
    strFullPipeName.Format(_T("\\\\%s\\pipe\\%s"), m_strPipeServer, m_strPipeName);
    if (dwCookie)
    {
        strFullPipeName.Format(_T("\\\\%s\\pipe\\%s_%d"), m_strPipeServer, m_strPipeName,dwCookie);
    }
    do 
    {

        SAFE_CLOSE_HANDLE(m_hPipeHandle);
        m_hPipeHandle = CreateNamedPipe(strFullPipeName,PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE,1,1024,1024,NMPWAIT_USE_DEFAULT_WAIT,NULL);
        if( m_hPipeHandle == INVALID_HANDLE_VALUE )
        {
            _tprintf(_T("Failed to CreateNamedPipe [%s],ErrorCode=0x%x"),strFullPipeName,GetLastError());
            m_hPipeHandle = NULL;
            --nRetryTime;
        }
        else
        {
            bOK = TRUE;
            break;
        }
    } while( nRetryTime>0 && !m_hPipeHandle );

    return bOK;
}


void CPipeServer::WriteStringToClient(CString strInfo)
{
    DWORD dwSize = strInfo.GetLength();
    if ( NULL == m_hPipeHandle || 0 == dwSize)
    {
        return;
    }
    DWORD dwBytesWritten = 0;
    USES_CONVERSION;	
    WriteFile(m_hPipeHandle, T2A( (TCHAR*)(strInfo.operator LPCTSTR())),dwSize, &dwBytesWritten, NULL);
	FlushFileBuffers(m_hPipeHandle);

}

void CPipeServer::UpdateDataToPipe(CString strInfo)
{
    WriteStringToClient(strInfo);
}

BOOL CPipeServer::ConnectPipe()
{
    BOOL bRet = FALSE;
    HANDLE hEvent = NULL;
    CString strInfo;
    OVERLAPPED ovlpd;
    do
    {
        hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
        if(NULL == hEvent)
        {  
            strInfo.Format(_T("Failed to CreateEvent,ErrorCode=0x%x"),GetLastError());
            break;
        }
        ZeroMemory(&ovlpd,sizeof(OVERLAPPED));
        ovlpd.hEvent = hEvent;

        ///Connected to PIPE
        if(!ConnectNamedPipe(m_hPipeHandle, &ovlpd))
        {
            if( ERROR_IO_PENDING != GetLastError() )
            {
                break;
            }
        }

        //Waiting for client
        if(WAIT_FAILED == WaitForSingleObject(hEvent,INFINITE))
        {
            strInfo.Format(_T("Failed to WaitForClent,ErrorCode=0x%x."),GetLastError());
            break;
        }
        bRet = TRUE;

    }while(0);

    SAFE_CLOSE_HANDLE(hEvent);
    return bRet;
}
