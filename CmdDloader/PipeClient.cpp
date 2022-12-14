// PipeClient.cpp: implementation of the CPipeClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PipeClient.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPipeClient::CPipeClient()
:m_hPipe(NULL)
,m_strPipeName(_T("ResearchDownload"))
,m_strPipeServerName(_T("."))
,m_pCBRevData(NULL)
,m_hThread(NULL)
,m_dwThreadID(0)
,m_bExit(FALSE)
,m_dwCookie(0)
{
}

CPipeClient::~CPipeClient()
{
    UnInit();
}

void CPipeClient::SetPipeName(CString& strName)
{
	if (!strName.IsEmpty())
	{
        m_strPipeName = strName;
	}	
}

void CPipeClient::SetPipeServerName(CString& strName)
{
	if (strName.IsEmpty())
	{
		m_strPipeServerName = strName;
	}
}

BOOL CPipeClient::OpenPipe()
{
    CString strFullPipeName;
	strFullPipeName.Format(_T("\\\\%s\\pipe\\%s"), m_strPipeServerName, m_strPipeName);
    if (m_dwCookie)
    {
        strFullPipeName.Format(_T("\\\\%s\\pipe\\%s_%d"), m_strPipeServerName, m_strPipeName,m_dwCookie);
    }
	if (!::WaitNamedPipe(strFullPipeName, NMPWAIT_WAIT_FOREVER))
	{
        //_tprintf(_T("[ERROR] Failed to WaitNamedPipe [%s],ErrorCode=0x%x\n"), strFullPipeName,GetLastError());
		return FALSE;
	}
    if (NULL == m_hPipe)
    {
        m_hPipe  = ::CreateFile(strFullPipeName,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    }
    if (INVALID_HANDLE_VALUE == m_hPipe)
	{
		_tprintf(_T("[ERROR] Failed to open Pipe,ErrorCode=0x%x\n"), GetLastError());
		m_hPipe = NULL;
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}

void CPipeClient::ClosePipe()
{
    if (m_hPipe)
    {
        CloseHandle(m_hPipe);
        m_hPipe = NULL;
    }
}

BOOL CPipeClient::ReadData()
{
	if (NULL == m_hPipe )
	{
		return FALSE;
	}

    BYTE byteReadBuf[MAX_PIPE_BUF] = {0};
	DWORD dwReadLen = 0;

	if(!ReadData(byteReadBuf,MAX_PIPE_BUF-1,dwReadLen))
	{
        //_tprintf(_T("[ERROR] Failed to ReadData,ErrorCode=0x%x\n"), GetLastError());
		return FALSE;
	}
	else
	{
//         CString strInfo(byteReadBuf);
//         if (-1 != strInfo.Find(_T("%")) && dwReadLen == 4)
//         {
//             printf("\b\b\b\b\b\b(%s)",byteReadBuf);
//         }
//         else
//         {
//             printf("\n%-56s ",byteReadBuf);            
//         }
        
		if ( dwReadLen>0 && m_pCBRevData)
		{
			m_pCBRevData(byteReadBuf,dwReadLen);	
		}	
	}
    return TRUE;
}

BOOL CPipeClient::ReadData(OUT LPBYTE pBuf, IN DWORD dwBufLen, OUT DWORD &dwReadLen)
{
	dwReadLen = 0;
	BOOL bRet = FALSE;
    if ( m_hPipe && pBuf && dwBufLen>0 )
    {
        bRet = ReadFile(m_hPipe, pBuf, dwBufLen, &dwReadLen, NULL);
    }
    return bRet;
}

void CPipeClient::SetRecvCallback(CBRECVDATA pRecvCallback)
{
	m_pCBRevData = pRecvCallback;
}

DWORD WINAPI CPipeClient::RecvProc(LPVOID pParam)
{
    CPipeClient* pPipeClient = (CPipeClient *)pParam;
    if (NULL == pPipeClient)
    {
        return 1;
    }
    while(!pPipeClient->m_bExit)
    {
        if(!pPipeClient->OpenPipe())
        {
            Sleep(100);
            continue;
        }
        //_tprintf(_T("Successfully connected to Dloader.\n"));
        break;		
    }

    while (!pPipeClient->m_bExit)
    {
        if( !pPipeClient->ReadData())
        {
            break;
        }
    }
    return 0;
}

void CPipeClient::Init(DWORD dwCookie)
{ 
    UnInit();
    m_dwCookie = dwCookie;
    m_hThread = CreateThread(NULL,0,RecvProc,this,0,&m_dwThreadID); 		
}

void CPipeClient::UnInit()
{
	m_bExit = TRUE;
	if (m_hThread)
	{
        if( WAIT_OBJECT_0 != WaitForSingleObject(m_hThread,2000) )
        {
            TerminateThread(m_hThread,1);
        }
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
    ClosePipe();
    m_bExit = FALSE;
}