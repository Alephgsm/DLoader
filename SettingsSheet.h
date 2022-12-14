#if !defined(AFX_SETTINGSSHEET_H__3B16967D_5736_432E_A398_2FE0117D093C__INCLUDED_)
#define AFX_SETTINGSSHEET_H__3B16967D_5736_432E_A398_2FE0117D093C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsSheet.h : header file
//
//#define CONCTRL_ELAPSE_DL         23 
//#define CONCTRL_ELAPSE_FLASH      25
#define MAX_FILE_COUNT	        40
#define MAX_BUF_SIZE            4096

enum {
	E_NOT_COMPARE						= 0,
	E_BUILT_IN_COMPARE					= 1,
	E_3RD_PROGRAM_COMPARE				= 2,
	E_BUILT_IN_PLUS_3RD_PROGRAM_COMPARE	= 3

};

/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet
#include "MainPage.h"
#include "CalibrationPage.h"
#include "PageMultiLang.h"
#include "BMAGlobal.h"
#include "FlashOptPage.h"
#include "PageLcdConfig.h"
#include "PageMcp.h"
#include "PageOptions.h"
#include "PageVolFreq.h"
#include "PageUartPortSwitch.h"
#include "PageCustomization.h"

/*
	magic(4Byte) | Version(1Byte) | Unit(1Byte) | table count(1Byte)|Reserved(1Byte) | table tag(4) | table offset(2)| table size(2)|
	Magic:			0x3A726170,  字符串"par:"
	Version:		0x01, 当前版本0x01
	Unit:			0x0,  1M Byte; 0x1, 512K Byte; 0x2, 1K Byte; 0x3, 1 Byte; 0x4, 1 Sector
	table count:	分区表的个数
	Table tag:		0x3A6C6274，字符串"tbl:", 分区表的标签
	table offset:	从magic开始的偏移，当前应该是0x10
	table size:		总的分区表占用的字节数
*/

#define SPPT_HEADER_MAGIC	0x3A726170  //"par:"
#define SPPT_TABLE_TAG      0x3A6C6274  //"tbl:"

typedef struct _SPPT_HEADER_T
{
	_SPPT_HEADER_T()
	{
		memset(this,0,sizeof(_SPPT_HEADER_T));
		dwMagic = SPPT_HEADER_MAGIC;
		byteVersion = 1;
		byteUnit	= 0x03;	//1 Byte
	}
	DWORD	dwMagic;
	BYTE	byteVersion;
	BYTE	byteUnit;
	BYTE	byteTblCount;
	BYTE	Reserved;
}SPPT_HEADER_T,*PSPPT_HEADER_T;


typedef struct _SPPT_TABLE_T
{
	_SPPT_TABLE_T()
	{
		memset(this,0,sizeof(_SPPT_TABLE_T));
		dwTag = SPPT_TABLE_TAG;
	}
	DWORD	dwTag;
	WORD	wOffset;
	WORD	wSize;
}SPPT_TABLE_T,*PSPPT_TABLE_T;

class CSettingsSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSettingsSheet)

// Construction
public:
	CSettingsSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSettingsSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

    BOOL LoadSettings(LPCTSTR pFileName,CString &strErrorMsg);
	BOOL LoadPacket(LPCTSTR pFileName);
    
    CString GetCurProduct()
    {
        return m_pageMain.m_strCurProduct;
    }

	CString GetCurPacVersion()
	{
		return m_pageMain.m_strPrdVersion;
	}

public:
    int		GetBaudRate();
    int		GetComPort();
    int		GetDownloadFile( CStringArray& aryDownloadFile ); 
    BOOL    CheckCrcDLFiles(CString& strCrcFailFileId);
    int		GetRepartitionFlag();
    int		GetFlashPageType();
	int		GetNvNewBasePosition();
	int		GetFileInfo(LPCTSTR lpszFileID, LPDWORD ppFileInfo);
	int     GetAllFileInfo(LPDWORD ppFileInfo);
	CString GetDownloadFilePathByPartitionName(LPCTSTR lpszPartitionName);
	CString GetDownloadFilePath(LPCTSTR lpszFileID,BOOL bCheckSel = TRUE);
	int		GetNvBkpItemCount(LPCTSTR lpNvID);	
	PNV_BACKUP_ITEM_T GetNvBkpItemInfo(int nIndex,LPCTSTR lpNvID);
	BOOL	IsNandFlash();  // if NAND return true, NOR return false
	BOOL	IsMainPageInit();
	BOOL	IsBackupNV(LPCTSTR lpszNVID = NULL);         //for NV page
	BOOL	IsNvBaseChange();
	BOOL	IsReadFlashInFDL2();	
	BOOL    IsNVSaveToLocal();
	CString GetNVSavePath();
	void	Resize( int nChangeWidth, int nChangeHeight );
	BOOL    IsBackupLang();
	BOOL    IsHasLang();
	WORD    GetLangNVItemID();	
	BOOL    IsNVOrgDownload();
	int     GetNVOrgBasePosition();	
	BOOL    IsOmaDM();
	int     GetFlashOprFileInfo(CUIntArray &agFlashOpr);	
	BOOL    IsEnableChipDspMap();	
	BOOL    IsAutoGenSN();
	BOOL    IsHasLCD();	
	BOOL    IsEnableMultiFileBuf();
	int     GetAllChipName(CStringArray &agChipNames,CUIntArray &agChipIDs);
	BOOL    GetChipName(DWORD dwChipID, CString &strName);
	int		GetAllRFChipName(CStringArray &agChipNames,CUIntArray &agChipIDs);
	BOOL	GetRFChipName(DWORD dwChipID, CString &strName);
	BOOL	GetRFChipID(CString strName,DWORD& dwChipID);
	int     GetAllFileID(CStringArray &agFileID);
	BOOL    FindLCDItem(LPCTSTR lpszFileName, VEC_LCD_CFIG &vLcdCfig);
	BOOL    IsEraseAll();
    BOOL    IsEraseIMEI();
	BOOL    IsCheckMCPType();
	CString GetMCPTypeDesc(LPCTSTR lpszMcpType,BOOL &bMatch);	
	BOOL    IsReadMcpType();
    BOOL    IsReadChipUID();
	BOOL    IsEnableRFChipType();
    BOOL    IsCheckMatch();
    BOOL    IsEnableSecureBoot();
	BOOL	IsEnableDebugMode();
	BOOL    IsReset();	
	BOOL    IsPowerOff();
	BOOL    IsNeedCheckNV();
	int     GetBackupFiles(CStringArray &agID);
	int     IsBackupFile(LPCTSTR lpszFileID);	
	BOOL    IsNeedRebootByAT();
	BOOL	IsEnableAPR();
	BOOL    IsKeepCharge();
	DWORD	GetComparePolicy();
	DWORD	GetUnPacPolicy();
	CString GetCompareApp();
	CString GetCompareParameter();
	BOOL    IsUartDownlod();
    BOOL    IsDdrCheck();
    BOOL    IsSelfRefresh();
    BOOL    IsCheckOldMemory();
    BOOL    IsEmmcCheck();
    BOOL    IsEnableLog();
	int     GetDLNVID(CStringArray &agID);   
	int     GetDLNVIDIndex(LPCTSTR lpszFileID);
	BOOL    IsBackupNVFile(LPCTSTR lpszFileID);	
	BOOL    HasPartitionInfo();
	BOOL	IsHostProType();
	BOOL	HasExtTblInfo();
	BOOL	IsSoftSimPrj();
	BOOL	IsSoftSimFile(LPCTSTR lpFile);
	CString GetSoftSimFile();
	BOOL	IsValidSoftSIM(LPCTSTR lpSoftSimDir);
	LPBYTE  GetPartitionData(DWORD &dwSize);
	BOOL    IsMapPBFileBuf();
	BOOL    IsSharkNand();
    int     GetBackupProdnvMiscdataType();
    BOOL    IsBackupMiscdata();
    BOOL    IsEnableEndProcess();
	BOOL    IsModifiedDLFile(LPCTSTR lpszDLFile);
	BOOL	IsLoadFromPac(LPCTSTR lpDLFile);
	DATA_INFO_T GetDataInfo(LPCTSTR lpDLFile);

	LPBYTE  GetExtTblData(DWORD &dwSize);
    BOOL    CheckCU(LPCTSTR lpCU);
  
    CFont				m_fntPage; 
	CCalibrationPage    m_pageCalibration;
	CPageMultiLang		m_pageMultiLang;
	CPageLcdConfig      m_pageLCDCfig;
	CPageVolFreq        m_pageVolFreq;
    CPageUartPortSwitch m_pageUartPortSwitch;
    CPageCustomization  m_pageCustomization;

// Attributes
protected:
	CMainPage           m_pageMain;  
	CFlashOptPage       m_pageFlashOpt;
	CPageMcp            m_pageMcp;
	CPageOptions        m_pageOptions;

// Operations
public:
    RECT m_rctPage;
    
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSettingsSheet)
	public:
    virtual BOOL OnInitDialog();
	protected:
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
    
// Implementation
public:
    virtual ~CSettingsSheet();
    
// Generated message map functions
protected:
    virtual void BuildPropPageArray ();
    //{{AFX_MSG(CSettingsSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
    afx_msg LONG OnResizePage (UINT, LONG);
//	afx_msg void OnPressButton(int nButtion);
	afx_msg void OnApply();
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSSHEET_H__3B16967D_5736_432E_A398_2FE0117D093C__INCLUDED_)
