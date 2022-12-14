#pragma once
#include "IDevHound.h"
#include "ICommChannel.h"
class CFindValidDut
{
public:
	CFindValidDut(void);
	virtual ~CFindValidDut(void);
	DWORD FindOneValidDut();
private:
	void LoadSettings();
	BOOL IsDLPort(LPCTSTR lpszPort,DWORD dwPort);
	DWORD FindPort();

private:
	IDevHound*    m_pUsbMoniter;
	ICommChannel* m_pChannel;
	CStringArray  m_agDLPort;
	DWORD		  m_dwWaitDUTTimeout;
};
