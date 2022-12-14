#include "StdAfx.h"
#include "CfgInfoOpr.h"



CCfgInfoOpr::CCfgInfoOpr(void)
{
    LoadCfg();
}

CCfgInfoOpr::~CCfgInfoOpr(void)
{
}


void CCfgInfoOpr::LoadCfg()
{
    TCHAR szCfgFile[MAX_PATH]   = {0};
    TCHAR szBuf[MAX_PATH]       = {0};
    _stprintf(szCfgFile,_T("%s\\CmdDloader.ini"),GetModuleFilePath(FALSE));

    GetPrivateProfileString(_T("Setting"),_T("FlashApp"),_T("ResearchDownload.exe"),szBuf,MAX_PATH,szCfgFile);
    m_strFlashAppName = szBuf;

    GetPrivateProfileString(_T("Setting"),_T("PipeServer"),_T("."),szBuf,MAX_PATH,szCfgFile);
    m_strPipeServer = szBuf;

    GetPrivateProfileString(_T("Setting"),_T("PipeName"),_T("ResearchDownload"),szBuf,MAX_PATH,szCfgFile);
    m_strPipeName = szBuf;

    m_bShowProcess = GetPrivateProfileInt(_T("Setting"),  _T("ShowProcess"), TRUE, szCfgFile);

	m_dwWaitDUTTimeout = GetPrivateProfileInt(_T("Setting"),  _T("WaitDUTTimeout"), 0, szCfgFile);

	m_bEZMode = GetPrivateProfileInt(_T("Setting"),  _T("EZMode"), FALSE, szCfgFile);

}