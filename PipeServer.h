#pragma once
#define DO_PACKET_PORT (-1)
const TCHAR g_szDoPackFlag[] = _T("CMDPAC");

class CPipeServer
{
public:
    CPipeServer(void);
    virtual ~CPipeServer(void);
public:
    BOOL Init(DWORD dwCookie = 0);
    void UnInit();

    void UpdateDataToPipe(CString strInfo);
	BOOL IsPartialFlash(){ return m_bPartialFlash;}
	BOOL IsSkipFile(LPCTSTR lpFileID);

private:
    void WriteStringToClient(CString strInfo);
    BOOL ConnectPipe();
    void LoadCfg();
	
protected:
    HANDLE  m_hPipeHandle;
    BOOL    m_bConnect;
	BOOL    m_bPartialFlash;
    CString m_strPipeServer;
    CString m_strPipeName;

	CStringArray m_agSkipID;
};
