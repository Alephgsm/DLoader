// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__1DDDF3B9_ED10_47BF_8EF9_3AD8B814C217__INCLUDED_)
#define AFX_MAINFRM_H__1DDDF3B9_ED10_47BF_8EF9_3AD8B814C217__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786 4284)
#pragma warning(push,3)
#include <map>
#include <list>
#pragma warning(pop)

#include <afxtempl.h>
#include "BMAGlobal.h"
#include "IBMAFramework.h"
#include "SettingsSheet.h"
#include "IDevHound.h"
#include "BootModeitf.h"
#include "XRandom.h"
#include "CoolToolBar.h"
#include "./phasecheck/PhaseCheckBuild.h"
#include "CoolStatusBar.h"


#ifndef _SPUPGRADE
#include "BarcodeDlg.h"
#define WM_REQUIRE_SN			(WM_USER + 0x1213)
#endif

#define PRODUCTION_INFO_SIZE    (MAX_PRODUCTIONINFO_SIZE)         //8K
#define X_SN_LEN                (SP15_MAX_SN_LEN)
#define FLASH_UID_LEN           (16)
#define SOFTSIM_EID_LEN         (20)
#define CHIP_UID_LEN            (64)


struct PORT_DATA
{
    DWORD  dwPort;
    LPBYTE lpPhaseCheck;
	HANDLE hSNEvent;
	char   szSN[X_SN_LEN+1];
    PORT_DATA()
    {
        memset(this,0, sizeof(PORT_DATA));
    }
};


#define CUST_MISCDATA_OFFSET	 (0xC0000)	 //768K
#define SOFT_VERSION_LEN (100)
#pragma pack(push, 1)
struct MISCDATA_TCL
{
    CHAR CU[TCT_CU_REF_LEN];
    CHAR reserved[4];
    INT  root_flag; // 0x52: root   0x4E: un-root
    CHAR SOFTVERSION[SOFT_VERSION_LEN];
    INT  inproduction;
    MISCDATA_TCL()
    {
        memset(this,0,sizeof(MISCDATA_TCL));
    }
};
#pragma pack(pop)


#define MAX_RECEIVE_SIZE       0x0800

#define WM_STOP_ONE_PORT		(WM_USER + 0x1214)
#define WM_POWER_MANAGE         (WM_USER + 1002)
#define WM_STOP_AUTODLOADER     (WM_USER + 1003)
#define WM_DWONLOAD_START       (WM_USER + 1012)
#define WM_DEV_HOUND			(WM_USER + 1022)
#define WM_USER_HELP            (WM_USER + 1032)


enum {
	REPORT_NORMAL = 0,
	REPORT_PRESS  = 1,
	REPORT_SOFTSIM
};

enum {
    E_IMEI      = 0,
    E_SN        = 1,
    E_CHIPUID   = 2
};

typedef enum {
	E_SAVE_NV_TO_LOCAL			= 0,
	E_SAVE_PRODNV_TO_LOCAL		= 1,
	E_SAVE_PHASECHECK_TO_LOCAL	= 2,
	E_SAVE_TO_LOCAL_UNDEFINE	
}BACKUP_FILE_TYPE;

static const TCHAR* g_szBackupFileType[] = 
{
	_T("NV"),
	_T("ProdNV"),
	_T("PhaseCheck"),
	_T("Undefine")
};

typedef struct _SOFTSIM_INFO
{
	TCHAR szFile[MAX_PATH];
	BYTE* pBuf;
	DWORD dwSize;
	_SOFTSIM_INFO()
	{
		memset(this,0, sizeof(_SOFTSIM_INFO));
	}
	void Clear()
	{
		SAFE_DELETE_ARRAY(pBuf);
		memset(this,0, sizeof(_SOFTSIM_INFO));
	}
	
}SOFTSIM_INFO,*SOFTSIM_INFO_PTR;

typedef struct _BACKUP_INFO
{
	TCHAR szNVFile[MAX_PATH];
	BYTE* pBuf;
	DWORD dwSize;
	_BACKUP_INFO()
	{
		memset(this,0, sizeof(_BACKUP_INFO));
	}
	void Clear()
	{
		SAFE_DELETE_ARRAY(pBuf);
		memset(this,0, sizeof(_BACKUP_INFO));
	}

}BACKUP_INFO,*BACKUP_INFO_PTR;


typedef struct _DATA_INFO
{
	BYTE* pBuf;
	DWORD dwSize;
	_DATA_INFO()
	{
		memset(this,0, sizeof(_DATA_INFO));
	}
	void Clear()
	{
		SAFE_DELETE_ARRAY(pBuf);
		memset(this,0, sizeof(_DATA_INFO));
	}

}DATA_INFO,*PDATA_INFO_PTR;

#define DLUID_SECTION    _T("kpanic")
#define DLUID_SIZE       (256*1024)
#define DLUID_LEN        8
typedef struct _DLUID_INFO
{
	BYTE*  pBuf;
	DWORD  dwSize;
	TCHAR  szUID[20];
}DLUID_INFO,*DLUID_INFO_PTR;


#pragma pack(4)

#define REFT_MAGIC_NUMBER		(0x54464552)    //"REFT"

typedef struct _REF_INFO_HEADER_T_
{
	_REF_INFO_HEADER_T_()
	{
		memset(this,0,sizeof(_REF_INFO_HEADER_T_));
		nMagic = REFT_MAGIC_NUMBER;
	}
	unsigned int nMagic;		//"REFT"    
	WORD wYear;
	WORD wMonth;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	unsigned int nReserved[4];                       
}REF_INFO_HEADER_T, *REF_INFO_HEADER_PTR;

#pragma pack()

#define MAX_BACKUP_FILE_NUM 10
#define CUST_REF_INFO_LEN	(MAX_REF_INFO_LEN + 64 )

typedef struct _BMOBJ
{
	_BMOBJ()
	{
		memset(this,0, sizeof(_BMOBJ));
	}

	void Clear()
	{
		for(int i =0; i< MAX_BACKUP_FILE_NUM;i++)
		{
			SAFE_DELETE_ARRAY(tFileBackup[i].pBuf);
			SAFE_DELETE_ARRAY(tNVBackup[i].pBuf);
		}
		SAFE_DELETE_ARRAY(lpRefData);
		SAFE_DELETE_ARRAY(dluid.pBuf);
		tSoftSim.Clear();
		SAFE_CLOSE_HANDLE(hStopEvent);
		memset(this,0, sizeof(_BMOBJ));
	}
	void InitRefInfo()
	{
		REF_INFO_HEADER_T stRefHeader;
		SYSTEMTIME  currentTime ;
		GetLocalTime( &currentTime);
		stRefHeader.wYear = currentTime.wYear;
		stRefHeader.wMonth = currentTime.wMonth;
		stRefHeader.wDay = currentTime.wDay;
		stRefHeader.wHour = currentTime.wHour;
		stRefHeader.wMinute = currentTime.wMinute;
		stRefHeader.wSecond = currentTime.wSecond;

		if (NULL == lpRefData)
		{
			lpRefData = new BYTE[CUST_REF_INFO_LEN];
		}
		if (lpRefData)
		{
			ZeroMemory(lpRefData,CUST_REF_INFO_LEN);
			memcpy(lpRefData,&stRefHeader,sizeof(REF_INFO_HEADER_T));
		}
		
	}

    DWORD dwCookie;
	DWORD dwIsUart;
	DWORD dwChipID;
	DWORD aFlashType[4];
	char  szSN[X_SN_LEN+1];
	char  szIMEI[X_SN_LEN+1];
	TCHAR szErrorMsg[_MAX_PATH*2];
	TCHAR szMcpInfo[_MAX_PATH];
	TCHAR szBlockPageSize[64];
	BACKUP_INFO tFileBackup[MAX_BACKUP_FILE_NUM];
	BACKUP_INFO tNVBackup[MAX_BACKUP_FILE_NUM];
	DLUID_INFO dluid;
	BOOL	bStoping;
	BOOL	bRMDev;
	HANDLE  hStopEvent;
	char	szFlashUID[FLASH_UID_LEN+1];
	SOFTSIM_INFO tSoftSim;
	int     nStage;//1: need second enum, 0/2: done!
    char    szChipUID[CHIP_UID_LEN+1];
	//int nTestResultID; //wei.song 20151020
	char szTestResultGUID[50];
	LPBYTE lpRefData;	// CUST_REF_INFO_LEN
	__int64 llCodeSize;
	DWORD dwRFChipType;
    DWORD dwOldMemoryType;

	//char szStartTestTime[30];
}BMOBJ,*BMOBJ_PTR;



typedef struct _EXT_IMG_INFO
{
	_EXT_IMG_INFO()
	{
		memset(this,0, sizeof(_EXT_IMG_INFO));
	}
	DWORD dwSize;
	BYTE *pBuf;
	TCHAR szFilePath[MAX_PATH];
	BOOL   bIsFileMap;
	HANDLE hFile;
	HANDLE hFileMap;
	DWORD  dwFirstMapSize;

	void clear()
	{
		if(!bIsFileMap)
		{
			if(pBuf != NULL)
			{
				delete [] pBuf;
			}
		}
		else
		{
			if( pBuf != NULL )
			{
				::UnmapViewOfFile(pBuf  );	
			}
			if( hFile != NULL )
			{
				::CloseHandle( hFile );	
			}
			if( hFileMap != NULL )
			{
				::CloseHandle( hFileMap );	
			}
		}

		memset(this,0, sizeof(_EXT_IMG_INFO));
	}
}EXT_IMG_INFO,*EXT_IMG_INFO_PTR;


typedef struct _DUT_KEY_T
{
    _DUT_KEY_T()
    {
        memset(this,0,sizeof(_DUT_KEY_T));
        ver[0] = '1';
    }
    BYTE  ver[4];
    char  szDUTKey[MAX_PATH];	// key
    BYTE  Reserved[760];        // unused
}DUT_KEY_T,*PDUT_KEY_T;

typedef struct _COMPARE_FILE_INFO_
{
	_COMPARE_FILE_INFO_()
	{
		memset(this,0, sizeof(_COMPARE_FILE_INFO_));
	}
	TCHAR  szFile1[MAX_PATH];	//org file
	TCHAR  szFile2[MAX_PATH];	//read back file
}COMPARE_FILE_INFO,*PCOMPARE_FILE_INFO_PTR;

typedef CMap<CString, LPCTSTR,COMPARE_FILE_INFO,COMPARE_FILE_INFO> MAP_CMP_INFO;


typedef CMap<DWORD,DWORD,EXT_IMG_INFO_PTR,EXT_IMG_INFO_PTR> MAP_EXTIMG;
typedef std::map<std::pair<DWORD,CString>,EXT_IMG_INFO_PTR> MAP_FILEBUF;
typedef CMap<CString, LPCTSTR,BACKUP_INFO_PTR,BACKUP_INFO_PTR> MAP_NVFILE;
typedef std::map<std::pair<DWORD,CString>,BACKUP_INFO_PTR> MAP_MULTI_NVFILE;

typedef CMap<DWORD, DWORD, _BMOBJ*, _BMOBJ* > MAP_BMOBJ;
typedef CMap<DWORD,DWORD,PORT_DATA*,PORT_DATA*> MAP_PORT_DATA;
typedef CMap<CString, LPCTSTR,DWORD,DWORD> MAP_STRINT;

typedef std::map<std::pair<CString,CString>,EXT_IMG_INFO_PTR> MAP_FILEBUF_PB; //page block
typedef std::vector<WORD> WORDArray;
typedef CMap<int, int, WORDArray, WORDArray > MAP_CALIFLAG;
struct BMFileInfo_TAG;
class CMainFrame;

static BOOL g_bValidVComDriverVer = FALSE;

class CBMOprObserver: public IBMOprObserver
{
public:
	CBMOprObserver(){ pThis = NULL;bExistFlashOpr = FALSE;m_llCurCodeSize = 0;}
	virtual HRESULT OnStart( DWORD dwOprCookie, 
							DWORD dwResult ); 
	virtual HRESULT OnEnd ( DWORD dwOprCookie, 
						   DWORD dwResult );         
	virtual HRESULT OnOperationStart(   DWORD dwOprCookie, 
									 LPCWSTR cbstrFileID,
									 LPCWSTR cbstrFileType,		
									 LPCWSTR cbstrOperationType,
									 LPVOID pBMFileInterface );      
	virtual HRESULT OnOperationEnd(  DWORD dwOprCookie, 
								   LPCWSTR cbstrFileID,
								   LPCWSTR cbstrFileType, 
								   LPCWSTR cbstrOperationType, 
								   DWORD dwResult,
								   LPVOID pBMFileInterface );      
	virtual HRESULT OnFileOprStart( DWORD dwOprCookie,
								   LPCWSTR cbstrFileID, 
								   LPCWSTR cbstrFileType , 
								   LPVOID pBMFileInterface );    
	virtual HRESULT OnFileOprEnd(  DWORD dwOprCookie, 
								 LPCTSTR cbstrFileID,
								 LPCTSTR cbstrFileType, 
								 DWORD dwResult );	
	virtual HRESULT OnFilePrepare(   DWORD dwOprCookie,
								  LPCWSTR bstrProduct,
								  LPCWSTR bstrFileName,
								  LPVOID  lpFileInfo,
								  LPVOID   pBMFileInfoArr,
								  LPDWORD  lpBMFileInfoCount,
								  LPDWORD  lpdwFlag);    
public:
	CMainFrame* pThis;
	BOOL bExistFlashOpr;
protected:
    __int64 m_llCurCodeSize;
};

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
    BOOL           m_bPacLoading;
    BOOL		   m_bStarted;
	long		   m_lStartNVRef; //a flag used to prevent user to stop downloading when downloading NV
    int			   m_nFileCount;
    CStringArray   m_arrFile;
	IBMAFramework *m_pBMAF; 		
	CTime          m_tmPacModifyTime;	

public:
	MAP_BMOBJ      m_mapBMObj;
	CStringArray   m_aPacReleaseDir;
	CString        m_strPacketPath;
	CString        m_strSpecConfig;
	CString        m_strPacDirBase;
	MAP_NVFILE     m_mapNVFileInfo;
	MAP_MULTI_NVFILE m_mapMultiNVInfo;
	CSettingsSheet m_sheetSettings;
	BOOL           m_bPacketOpen;	
	BOOL           m_bCheckCali;	
	CString        m_strCodeChipID;	
	BOOL           m_bNeedPhaseCheck;
	CXRandom       m_rand;	
	CUIntArray     m_aIMEIID;	
	LPBYTE         m_pPartitionData;
	DWORD          m_dwPartitionSize;
	CString        m_strLocalBKFolder;
	int            m_nRmImgStoreInterval;
	BOOL           m_bDoReport;	
	int            m_nReportType;
    int            m_nDUTID;
	BOOL           m_bNameWithSN;
    BOOL           m_bShowRate;
    BOOL           m_bSetFirstMode;
	BOOL		   m_bSptFPFirstMode;
    int            m_nFirstMode;

	LPBYTE         m_pExtTblData;
	DWORD          m_dwExtTblSize;

	std::list < TCHAR* > m_lstSoftSim;

	BOOL           m_bPortSecondEnum;
    BOOL           m_bPacHasKey;
    char           m_szPacKey[MAX_PATH];
	FILETIME	   m_ftPacInfo;
// Operations
public:
	CCoolToolBar     m_wndToolBar;
	CString		GetSpecConfigFile();
    void		SetPrdVersion(LPCTSTR lpszSpdVer);	

	PORT_DATA * GetPortDataByPort(int nPort);
	_BMOBJ    * GetBMObj(int nPort);

	static DWORD WINAPI GetThreadFunc(LPVOID lpParam);
	DWORD		ClosePortFunc(DWORD dwPort,BOOL bSuccess);
	BOOL		StartOnePortWork(DWORD dwPort, BOOL bUart);
	void		SetStatusBarText(LPCTSTR pStr);
	void		DeletePacTmpDir();
	void        RemoveOldPacTmpDir();
	void        RemoveOldBKFiles();
	void		Write2Sql(DWORD dwOprCookie, BOOL bSuccess /*= TRUE*/); //wei.song 20140901
    BOOL        CheckKeyMatch(const DUT_KEY_T& stDUTKey,LPTSTR szErrorMsg);
	void		MESWriteResult(DWORD dwOprCookie, BOOL bSuccess); //wei.song 20151020
    BOOL        IsValidVComDriverVer(CString& strError);
    BOOL        IsSupportCU();
    BOOL        IsSupportCheckRoot();
    BOOL        IsSupportCheckInproduction();
protected:
    //@ Liu Kai 2004-3-25 CR8123
	PORT_DATA* CreatePortData( DWORD nPort );
    void  RmLatestTempFiles();
	BOOL  _LoadSettings();
    BOOL  LoadSettings();    
    void  SetDLTitle();    
	BOOL  StopOnePortWork(DWORD dwPort,BOOL bRemoved = FALSE);
    void  StartWork();	
	DWORD Buf2TString(LPBYTE pBuf,DWORD dwSize,LPTSTR szStr,DWORD dwStrLen);
	DWORD BCDToWString(LPBYTE pBcd,DWORD dwSize,LPTSTR szStr,DWORD dwStrLen);
    void  ParseChipUID(DWORD dwBlk0,DWORD dwBlk1,char* szStr,DWORD dwStrLen);
	void  DoReport(DWORD dwOprCookie,BOOL bSuccess = TRUE);	
	BOOL  CheckDLFiles();
	BOOL  InitReportInfo();
	BOOL  InitNVBuffer();
	BOOL  InitMultiNVBuffer();
	BOOL  InitUDiskBufMap();	
	BOOL  InitChipDspBufmap();
	void  InitCodeChipID();
	BOOL  InitPSFile();
	BOOL  InitIMEIID();
	BOOL  InitMultiFileBuf();
	void  InitDebugData();
	void  ClearMultiFileBuf();
	BOOL  InitPartitionData();
	BOOL  InitExtTblData();
	BOOL  InitSoftSIM();
	void  ClearExtImgMap(MAP_EXTIMG &mapExtImg);	
	void  GetOprErrorCodeDescription(DWORD dwErrorCode,
									LPTSTR lpszErrorDescription, 
									int nSize );	

	BOOL SaveBackupFileToLocal(BACKUP_FILE_TYPE eBackupType,_BMOBJ *pbj, BYTE *pBuf, DWORD dwSize);
	BOOL SendAT2Reboot(UINT nPort);
	BOOL IsATPort(LPCTSTR lpszPort);
	BOOL IsDLPort(LPCTSTR lpszPort,DWORD dwPort);
	void ClearNVMap();
	void ClearMultiNVMap();
	BOOL AddReadSN(PBMFileInfo pBMFileInfo);
	BOOL AddEraseAll(PBMFileInfo pBMFileInfo);
	BOOL AddBackupFiles(PBMFileInfo pBMFileInfo, int &nCount,_BMOBJ * pbj, PFILE_INFO_T pFileInfo);

	BMOBJ_PTR InitBMObj(DWORD dwPort, BOOL bUart);
	BOOL             InitMapPBFileBuf();
	void             ClearMapPBFileBuf();
	EXT_IMG_INFO_PTR LoadPageBlockFile(LPCTSTR lpszFile);

	BOOL CreateDluidInfo(DLUID_INFO &dluid);
	BOOL GetDluidInfo(DLUID_INFO &dluid);
	void RemoveLocalFile(LPCTSTR pszDluid);
	BOOL CreateBKFileFolder();
	void WaitForStopedObj();
	BOOL FindSoftSim(LPCTSTR lpSrcDir,std::list<TCHAR*>& lstFile,LPCTSTR lpPostfix);
	void RemoveLstSoftSim();
	BOOL CheckCalibration(LPCTSTR lpszNVID,LPBYTE lpPhoBuf,DWORD dwPhoSize,CString &strErr);

	void CheckPort2ndEnum();
    void InitDownloadLog();
    BOOL CheckPacKey();
    BOOL GetDUTKeyInfo(LPBYTE pBuf, DWORD dwSize,DUT_KEY_T& cDutKey);
    void GetNeedCheckCaliID(LPCTSTR lpszNVID,BYTE byteCali,MAP_CALIFLAG& mapCaliInfo);
    BOOL ClearCalibrationFlag(LPCTSTR lpNVID,LPCTSTR lpNVFile);
	BOOL VerifyIntegrity(LPCTSTR lpFileID,LPCTSTR lpFileName);
	BOOL CompareFile(LPCTSTR lpFile1,LPCTSTR lpFile2,__int64 llOffset = 0);
	CString GetMD5(LPCTSTR lpszFile,__int64 llOffset = 0,__int64 llSize = 0);
	BOOL AddCompareFileInfo(const PBMFileInfo pBMFileInfoCur);
	void AutoCompare(DWORD dwOprCookie,_BMOBJ * pbj);
	void BuiltinComparison(CString& strError);
	void ThirdPartyComparison(CString& strError);
	CString GetReadFlashPath();
	BOOL	NeedCompare();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CCoolStatusBar   m_wndStatusBar;
    void*			 m_pMasterImg;	
	FILE *			 m_pReportFile;
	DWORD			 m_dwSinglePort;
	DWORD			 m_dwMaxNVLength;	
    DWORD			 m_dwMaxUbootLen;
    DWORD            m_dwCheckMatchPolicy;
    BOOL             m_bEnableWriteFlash;
    BOOL             m_bStopDownloadIfOldMemory;
	MAP_PORT_DATA    m_mapPortData;	
	MAP_EXTIMG		 m_mapUDiskIMg;
	MAP_EXTIMG		 m_mapChipDsp;
	MAP_FILEBUF		 m_mapMultiFileBuf;
	MAP_FILEBUF_PB   m_mapPBFileBuf;
	MAP_STRINT       m_mapPBInfo;
	CString			 m_strOprErrorConfigFile;	
	BOOL			 m_bShowFailedMsgbox;
	BOOL			 m_bFailedMsgboxShowed;
	BOOL			 m_bShowSafetyTips;
    BOOL			 m_bSupportCU;
    BOOL			 m_bSupportCheckRoot;
    BOOL             m_bSupportCheckInproduction;
	BOOL			 m_bNeedCompare;
	CRITICAL_SECTION m_csPortData;
	CRITICAL_SECTION m_csReportFile;
	CRITICAL_SECTION m_csPowerManage;
	CRITICAL_SECTION m_csProcessFlow; //Add for ProcessFlow, wei.song 20140901
	CRITICAL_SECTION m_csSoftSim;
	CRITICAL_SECTION m_csMultCmdFlash;
	
    CString          m_strKeyPartID;
	CString          m_strPacVerison;
	CString			 m_strSafetyTipsMsg;
    CString			 m_strEraseIMEIMsg;
    CString			 m_strEraseAllFlashMsg;
	DATA_INFO		 m_tDebugData;
	MAP_CMP_INFO	 m_mapCmpInfo;
	
	
public:	
	BOOL       m_bPowerManage;
	BOOL       m_bPMInDLProcess;
	DWORD      m_dwPowerMgrInter;
	CString    m_strLoadSettingMsg;
	//CDevHound  m_usbMoniter;
    IDevHound*   m_pUsbMoniter;
private:
	BOOL       LoadPac();
	
	CStringArray m_agATPort;
	CStringArray m_agDLPort;
	
// Generated message map functions
protected:
	BOOL m_bAutoStart;
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSettings();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnUpdateStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStop(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnUpdateSettings(CCmdUI* pCmdUI);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnLoadPacket();
	afx_msg void OnUpdateLoadPacket(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LRESULT OnWarnMessageBox(WPARAM wParam,LPARAM lpParam);
	afx_msg LRESULT OnInitalPacket(WPARAM wParam,LPARAM lpParam);
    afx_msg LRESULT OnStopAutoDloader(WPARAM wParam,LPARAM lpParam);
	afx_msg LRESULT OnStopOnePort(WPARAM wParam,LPARAM lpParam);
	afx_msg LRESULT OnPowerManage(WPARAM wParam,LPARAM lpParam);
    afx_msg LRESULT OnStartDownload(WPARAM wParam,LPARAM lpParam);
	afx_msg LRESULT OnDevHound(WPARAM wParam,LPARAM lpParam);
	afx_msg LRESULT OnHelp(WPARAM wParam,LPARAM lpParam);
	afx_msg LRESULT OnProgressMsg(WPARAM wParam,LPARAM lpParam);
	DECLARE_MESSAGE_MAP()    
		
	CBMOprObserver m_xBMOprObserver;
	friend class CBMOprObserver;
};

class CAutoCS
{
public:
    CAutoCS( CRITICAL_SECTION &cs ) 
    { 
        m_pCS = &cs; 
        EnterCriticalSection( m_pCS ); 
    }
	
    ~CAutoCS( ) { LeaveCriticalSection( m_pCS ); }
private:
    CRITICAL_SECTION * m_pCS;
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__1DDDF3B9_ED10_47BF_8EF9_3AD8B814C217__INCLUDED_)
