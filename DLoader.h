// DLoader.h : main header file for the DLOADER application
//

#if !defined(AFX_DLOADER_H__09DF2514_AE78_4B20_8103_993EB56996BA__INCLUDED_)
#define AFX_DLOADER_H__09DF2514_AE78_4B20_8103_993EB56996BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define UDSIK_IMG_NAME_LEN		(13)

#include "resource.h"       // main symbols
#include "IBMAFramework.h"
#include "IXmlConfigParse.h"
#include "ICommChannel.h"
#include "PipeServer.h"
#pragma warning(disable : 4786)
#pragma warning(disable : 4018)
#pragma warning(push,3)
#include <afxtempl.h>
#pragma warning(pop)

//wei.song 20150920
#include "SprdMesExport.h"
#define TCT_CU_REF_LEN    (20)

/////////////////////////////////////////////////////////////////////////////
// CDLoaderApp:
// See DLoader.cpp for the implementation of this class
//

typedef BOOL (*pfCreateXmlConfigParse)( IXmlConfigParse ** );
typedef BOOL (*pfCreateBMAFramework)( IBMAFramework ** );

typedef BOOL (*pfCreateChannel)( ICommChannel ** , CHANNEL_TYPE);
typedef void (*pfReleaseChannel)( ICommChannel *);


class CDLoaderApp : public CWinApp
{
public:
	CDLoaderApp();
	void GetVersion(void); //wei.song 20140901
	BOOL ProcessFlowInit(void); //wei.song 20140901
	BOOL SprdMESInit(void);  //wei.song 20151020
	BOOL GetIniFilePath(LPTSTR pName);
	BOOL InitBMAFrameLib();
	BOOL InitChannelLib();
	BOOL IsPartialFlash() { return m_cPipeSrv.IsPartialFlash();}
	BOOL IsSkipFile(LPCTSTR lpFileID);
	void FreeLib();

	pfCreateXmlConfigParse m_pfCreateXmlConfigParse;
	pfCreateBMAFramework   m_pfCreateBMAFramework;
	HMODULE m_hBMAFrameLib;

	pfCreateChannel m_pfCreateChannel;
	pfReleaseChannel m_pfReleaseChannel;
	HMODULE  m_hChannelLib;

	IBMAFramework * m_pBMAFramework;
    //@ Liu Kai 2004-08-23
    CString m_strVersion;
    CString m_strBuild;
	
	BOOL    m_bShowOtherPage;
	BOOL	m_bNeedUpRefInfo;
	//@ Hongliang Xin 2009-6-3
	BOOL    m_bScriptCtrl;
	CString m_strResultPath;		// result.txt
	BOOL    m_bResultPathWithPort; // result_com5.txt
	
	//@ Hongliang Xin 2009-7-7
	BOOL    m_bColorFlag;   // use color to mark the last failed.
	BOOL    m_bClosePortFlag;   // close port for USB,when finish the download
	BOOL    m_bResultHolding;   // when start, the waiting state holding the previous result

	BOOL    m_bManual;
	
	BOOL    m_bCMDPackage;
    BOOL    m_bCMDFlash;
	BOOL    m_bEZMode;
	BOOL	m_bReadFixNV;
	DWORD	m_dwMultiCount;
	DWORD   m_dwSumCMDFlash;
	CString m_strRFixNV;
	CString m_strDoPacPath;
	CString m_strInPacFile;
	CString m_strPrdVersion;
    DWORD   m_dwPort;
	
	BOOL    m_bKeepPacNVState;
	
	BOOL    m_bShowMcpTypePage;
	BOOL    m_bClearCaliFlag;
	
	BOOL    m_bNeedPassword;

	DWORD   m_dwWaitTimeForNextChip;

	BOOL    m_bDeleteTmpDir;
	BOOL    m_bCheckDriverVer;
	
	BOOL    m_bFilterPort;
	CMap<DWORD,DWORD,DWORD,DWORD> m_mapFilterPort;
	CMap<DWORD,DWORD,DWORD,DWORD> m_mapFixPort;
	
	CString m_strFileFilter;
    int     m_nEnumPortTimeOut;
    CString m_strEnumPortErrorMsg;
    BOOL    m_bStopDownloadIfOldMemory;

	//Add for ProcessFlow, wei.song 20140901
	BOOL	m_bEnableFlow;
	int		m_nSnLength;
	char	m_szProject[16];
	char	m_szOrderNum[20];
	//For SprdMES, wei.song 20151020
	BOOL m_bSprdMESEnable;
	char m_szBatchName[30];
	BATCH_INFO_T m_BatchInfo;
	//
	void ReportToPipe(CString sLogContent,BOOL isError = TRUE);

	void MessageBox(CString strInfo,UINT nType = MB_OK,BOOL isError = TRUE);
	/************************************************************************/
	/*if 0, not backup below data for GSM calibration Ver FF0A

		    RF_ramp_delta_timing for GSM calibration

	  if 1, not backup below data for GSM calibration Ver FF0A

	        temperature_and_voltage_composate_structure (reserved)
	        RF_ramp_table
	        RF_ramp_PA_power_on_duration
	        RF_ramp_delta_timing

	  if 2, backup below data for GSM calibration Ver FF0A	

			rf_common_param_dsp_use		
			
			rf_gsm_param_dsp_use->agc_ctrl_word
			rf_gsm_param_dsp_use->rx_compensate_value
			rf_gsm_param_dsp_use->max_rf_gain_index	
			
			rf_gsm_param_dsp_use->rf_ramppwr_step_factor
			rf_gsm_param_dsp_use->rf_edge_tx_gain_table
			rf_gsm_param_dsp_use->rf_8psk_tx_compensation
            rf_gsm_param_dsp_use->rf_ramp_param_constant_value  
	
	        adc                                                             */
	/************************************************************************/
	int     m_nGSMCaliVaPolicy;
protected:
    CPipeServer m_cPipeSrv;
protected:
    BOOL ParseCMDParam(int argc, TCHAR** argv);
	void LoadFilterPortSetting(LPCTSTR pszConfigFile);
	void FetchPort(LPTSTR lpszStr,CMap<DWORD,DWORD,DWORD,DWORD>& mapPort,TCHAR chSeparate = _T(','));

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLoaderApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDLoaderApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// declare the global App instance
extern CDLoaderApp g_theApp;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLOADER_H__09DF2514_AE78_4B20_8103_993EB56996BA__INCLUDED_)
