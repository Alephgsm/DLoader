#pragma once

enum {
	EX_SRV  = 0,
	IN_SRV  = 1,
	AUTO_DETECT
};
class CUsageStatistics
{
public:
	CUsageStatistics(void);
	virtual ~CUsageStatistics(void);
	BOOL PingSrv();
	BOOL UploadData2Srv(const char* pSN,LPBYTE lpBuf,DWORD dwSize);
protected:
	void LoadSetting();
	BOOL IsExistPath(LPCTSTR lpPath);

private:
	static unsigned int m_nSrvType;
	TCHAR m_szExsrv[MAX_PATH];
	TCHAR m_szInsrv[MAX_PATH];
};
