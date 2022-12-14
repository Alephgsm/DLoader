#include "stdafx.h"
#include "UsageStatistics.h"
#include <atlconv.h>
#include <WinInet.h>

unsigned int CUsageStatistics::m_nSrvType = AUTO_DETECT;

CUsageStatistics::CUsageStatistics(void)
{
	ZeroMemory(m_szExsrv,sizeof(m_szExsrv));
	ZeroMemory(m_szInsrv,sizeof(m_szInsrv));
	LoadSetting();
}

CUsageStatistics::~CUsageStatistics(void)
{
	
}
BOOL CUsageStatistics::IsExistPath(LPCTSTR lpPath)
{
	if (NULL == lpPath || 0 == _tcslen(lpPath))
	{
		return FALSE;
	}

	TCHAR szFind[MAX_PATH] = {0};
	WIN32_FIND_DATA findFileData;

	_tcscpy_s(szFind, MAX_PATH, lpPath);
	_tcscat_s(szFind, _T("\\*.*")); 

	HANDLE hFind = ::FindFirstFile(szFind, &findFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return FALSE;
	}
	::FindClose(hFind);
	return TRUE;
}
BOOL CUsageStatistics::PingSrv()
{
	BOOL bRet = FALSE;
	DWORD dwFlag = 0;
	if(!InternetGetConnectedState(&dwFlag,0)) //not connected to network
	{
		return FALSE;
	}
	TCHAR szDoMain[MAX_PATH] = {0};
	DWORD dwSize = sizeof(szDoMain);
	if (!GetComputerNameEx(ComputerNameDnsDomain, szDoMain, &dwSize))
	{
		return FALSE;
	}
	if(_tcsicmp(szDoMain,_T("spreadtrum.com")) && _tcsicmp(szDoMain,_T("unisoc.com")))
	{
		return FALSE;
	}

	m_nSrvType = AUTO_DETECT;
	LoadSetting();

	if (0 == _tcslen(m_szExsrv) && 0 == _tcslen(m_szInsrv))
	{
		return FALSE;
	}

	if(_tcslen(m_szExsrv) && IsExistPath(m_szExsrv))
	{	
		bRet = TRUE;
		m_nSrvType = EX_SRV;
	}
	else
	{
		if(_tcslen(m_szInsrv) &&IsExistPath(m_szInsrv))
		{	
			bRet = TRUE;
			m_nSrvType = IN_SRV;
		}
	}
	
	return bRet;
}

void  CUsageStatistics::LoadSetting()
{
	TCHAR szCfgFilePath[MAX_PATH] = {0};
	ZeroMemory(m_szExsrv,sizeof(m_szExsrv));
	ZeroMemory(m_szInsrv,sizeof(m_szInsrv));
	GetModuleFilePath(NULL,szCfgFilePath);
	_tcscat(szCfgFilePath,_T("\\ResearchDownload.ini"));

	GetPrivateProfileString(_T("UsageStatistics"),_T("Exsrv"),_T(""),m_szExsrv,_MAX_PATH,szCfgFilePath);
	GetPrivateProfileString(_T("UsageStatistics"),_T("Insrv"),_T(""),m_szInsrv,_MAX_PATH,szCfgFilePath);
}

BOOL CUsageStatistics::UploadData2Srv(const char* pSN,LPBYTE lpBuf,DWORD dwSize)
{
	if (NULL == pSN	|| NULL == lpBuf || 0 == dwSize)
	{
		return FALSE;
	}
	USES_CONVERSION;
	
	BOOL bRet = FALSE;
	_TCHAR szRefFileName[MAX_PATH] = {0};

	SYSTEMTIME  currentTime ;
	GetLocalTime( &currentTime);
	do 
	{
		if (AUTO_DETECT == m_nSrvType)
		{
			PingSrv();
		}
		if (AUTO_DETECT == m_nSrvType)
		{
			break;
		}
		_stprintf( szRefFileName, _T("%s\\%04d_%02d_%02d"), 
			(EX_SRV == m_nSrvType) ? m_szExsrv:m_szInsrv,
			currentTime.wYear,
			currentTime.wMonth,
			currentTime.wDay );
		CreateDirectory(szRefFileName,NULL);
		_stprintf( szRefFileName, _T("%s\\%02d_%02d_%02d_%03d_%s.bin"), 
			szRefFileName,
			currentTime.wHour,
			currentTime.wMinute,
			currentTime.wSecond,
			currentTime.wMilliseconds,
			A2T(pSN));

		bRet = SaveFileToLocal(szRefFileName,lpBuf,dwSize);

	} while (0);

	return bRet;
}