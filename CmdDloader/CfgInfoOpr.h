#pragma once

class CCfgInfoOpr
{
public:
    CCfgInfoOpr(void);
    ~CCfgInfoOpr(void);
public:
    CString m_strFlashAppName;
    CString m_strPipeServer;
    CString m_strPipeName;
    BOOL    m_bShowProcess;
	DWORD   m_dwWaitDUTTimeout;
	BOOL    m_bEZMode;
private:
    void    LoadCfg();
};
