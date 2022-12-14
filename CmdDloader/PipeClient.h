// PipeClient.h: interface for the CPipeClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIPECLIENT_H__D23786DC_3722_4046_9CB0_314BBDAD7E0C__INCLUDED_)
#define AFX_PIPECLIENT_H__D23786DC_3722_4046_9CB0_314BBDAD7E0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>
#define MAX_PIPE_BUF 2048
typedef VOID (CALLBACK* CBRECVDATA)(LPBYTE lpRecvData,DWORD dwDataLen);

class CPipeClient  
{
public:
	CPipeClient();
	virtual ~CPipeClient();

	void SetPipeName(CString& strName);
	void SetPipeServerName(CString& strName);
	void SetRecvCallback(CBRECVDATA pRecvCallback);

    void Init(DWORD dwCookie = 0);
	void UnInit();

    BOOL OpenPipe();
    void ClosePipe();
    BOOL ReadData();

	
protected:
	HANDLE      m_hPipe;
	CString     m_strPipeName;
	CString     m_strPipeServerName;
	CBRECVDATA  m_pCBRevData;	
	HANDLE      m_hThread;
	DWORD       m_dwThreadID;
	BOOL        m_bExit;
    DWORD       m_dwCookie;
private:
    BOOL ReadData(LPBYTE pBuf,DWORD dwBufLen, DWORD &dwReadLen);
    static DWORD WINAPI RecvProc(LPVOID pParam);

};

#endif // !defined(AFX_PIPECLIENT_H__D23786DC_3722_4046_9CB0_314BBDAD7E0C__INCLUDED_)
