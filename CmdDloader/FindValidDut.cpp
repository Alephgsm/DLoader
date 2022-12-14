#include "StdAfx.h"
#include "FindValidDut.h"

CFindValidDut::CFindValidDut(void)
{
	m_dwWaitDUTTimeout = 0;
	m_agDLPort.RemoveAll();
	CreateDevHound(&m_pUsbMoniter);
	CreateChannel(&m_pChannel,CHANNEL_TYPE_COM);
	LoadSettings();
}

CFindValidDut::~CFindValidDut(void)
{
	if(m_pUsbMoniter)
	{
		ReleaseDevHound(m_pUsbMoniter);
	}

	if (m_pChannel)
	{
		ReleaseChannel(m_pChannel);
	}
}

void CFindValidDut::LoadSettings()
{
	CFileFind finder;
	CString strBMFileType;
	strBMFileType.Format(_T("%s\\BMFileType.ini"),GetModuleFilePath(FALSE));	
	if(finder.FindFile(strBMFileType))
	{
		_TCHAR szBuf[_MAX_PATH]={0};
		GetPrivateProfileString(_T("AT_REBOOT_SETTING"),_T("DLPort"),_T(""),szBuf,_MAX_PATH,strBMFileType.operator LPCTSTR());
		SplitStr(szBuf,m_agDLPort,',');
	}


	TCHAR szCfgFile[MAX_PATH]   = {0};
	_stprintf(szCfgFile,_T("%s\\CmdDloader.ini"),GetModuleFilePath(FALSE));
	m_dwWaitDUTTimeout = GetPrivateProfileInt(_T("Setting"),  _T("WaitDUTTimeout"), 0, szCfgFile);
}

BOOL CFindValidDut::IsDLPort(LPCTSTR lpszPort,DWORD dwPort)
{
	BOOL bNameIsDLPort	= FALSE;
	BOOL bIsValidPort	= FALSE;
	int	 nCount			= 0;

	//Check Port  Name
	nCount = m_agDLPort.GetSize();
	if ( 0 == nCount )
	{
		bNameIsDLPort = TRUE;
	}
	else
	{
		CString strPortName = lpszPort;
		for(int i = 0; i< nCount; i++)
		{
			CString strDLPort = m_agDLPort[i];
			if( strPortName.Find(strDLPort) != -1)
			{
				bNameIsDLPort = TRUE;
				break;
			}
		}
	}
	if (bNameIsDLPort&&m_pChannel)
	{
		CHANNEL_ATTRIBUTE ca;
		ca.ChannelType = CHANNEL_TYPE_COM;
		ca.Com.dwPortNum = dwPort;
		ca.Com.dwBaudRate = 115200;

		if(m_pChannel->Open(&ca))
		{
			bIsValidPort = TRUE;
		}
		m_pChannel->Close();
		
	}
	return bNameIsDLPort && bIsValidPort;
}


DWORD CFindValidDut::FindOneValidDut()
{
	DWORD dwPort = 0;
	DWORD dwStart = GetTickCount();
	while(1)
	{
		dwPort = FindPort();
		if (dwPort)
		{
			break;
		}
		if (m_dwWaitDUTTimeout)
		{
			DWORD dwElapsed = GetTickCount() - dwStart;
			if (dwElapsed >= m_dwWaitDUTTimeout)
			{
				printf("Detect download device timeout.\n");
				break;
			}
		}
		Sleep(200);
	}
	return dwPort;
}

DWORD CFindValidDut::FindPort()
{
	DWORD dwPort = 0;
	DEV_INFO* pDevInfo = NULL;
	int nCount = m_pUsbMoniter->ScanPort(&pDevInfo);

	for(int i = 0; i<nCount; ++i)
	{
		if (IsDLPort(pDevInfo[i].szFriendlyName,pDevInfo[i].nPortNum))
		{
			dwPort = pDevInfo[i].nPortNum;
			break;
		}
	}
	m_pUsbMoniter->FreeMemory(pDevInfo);
	return dwPort;
}