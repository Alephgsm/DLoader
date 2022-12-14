#if !defined(AFX_MAINPAGE_H__1E1CBF30_F40E_423E_913F_1D0D8057C816__INCLUDED_)
#define AFX_MAINPAGE_H__1E1CBF30_F40E_423E_913F_1D0D8057C816__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainPage.h : header file
//

#include "IXmlConfigParse.h"
#include "BMAGlobal.h"
#include "XListCtrl.h"
#include "CoolStatic.h"
#include "CalibrationPage.h"
#include "PageMultiLang.h"
#include "PageLcdConfig.h"
#include "PageOptions.h"
#include "PageVolFreq.h"
#include "PageUartPortSwitch.h"
#include "PageCustomization.h"
#include "FlashOptPage.h"
#include "SecBinPackApi.h"

#pragma  warning(disable:4786)
#pragma  warning(push,3)
#include <vector>
#pragma  warning(pop)


// Product select section
static _TCHAR g_sz_SELECT[]				 = _T("Selection");
static _TCHAR g_sz_SELECT_PRODUCT[]      = _T("SelectProduct");
static _TCHAR g_sz_NVITEM[]				 = _T("nvitem");
/////////////////////////////////////////////////////////////////////////////
// CMainPage dialog
enum _FDL2_FALG{
	FDL2F_OMADM,
	FDL2F_PRELOAD,
	FDL2F_KERNELIMG2,
	FDL2F_ROMDISK
};

typedef struct _DATA_INFO_T
{
	unsigned __int64 llSize;
	unsigned __int64 llOffset;
	_DATA_INFO_T()
	{
		memset(this,0, sizeof(_DATA_INFO_T));
	}
} DATA_INFO_T;

class CMainPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMainPage)

// Construction
public:
	CMainPage();
	~CMainPage();

    BOOL	LoadSettings(LPCTSTR pFileName,CString &strErrorMsg);
	BOOL	SaveSettings(LPCTSTR pFileName);

	BOOL	FillList(LPCTSTR lpcstrProduct);   
	BOOL	LoadAllProduct(LPCTSTR lpszXMLConfig);
	int		GetProductIndex(LPCTSTR lpcstrProuct);
	void	SetCaliPage(CCalibrationPage * pCaliPage){m_pCaliPage = pCaliPage;}
//	void	SetNandPage(CNandFlashSettings * pNandPage){m_pNandPage = pNandPage;}
	void    SetOptionsPage(CPageOptions *pOptionPage) {m_pOptionsPage =pOptionPage; }
	void	SetMultiLangPage(CPageMultiLang * pMultiLangPage){m_pMultiLangPage = pMultiLangPage;}
	void	SetLCDCfigPage(CPageLcdConfig * pLcdCfigPage){m_pLCDCfigPage = pLcdCfigPage;}
	void    SetVolFreqPage(CPageVolFreq * pPage){m_pVolFreqPage = pPage;}
    void    SetUartPortSwitchPage(CPageUartPortSwitch * pPage){m_pUartPortSwitchPage = pPage;}
	void    SetFlashOptPage(CFlashOptPage* pPage){m_pFlashOptPage = pPage;}
    void    SetCustomizationPage(CPageCustomization* pPage){m_pCustomizationPage = pPage;}
	void	InitFlashOptPage(PPRODUCT_INFO_T pCurProduct,LPCTSTR pFileName);
    void	InitCaliPage(PPRODUCT_INFO_T pCurProduct);
	void	InitNandPage(PPRODUCT_INFO_T pCurProduct);
	void    InitMultiLangPage(BOOL bUpdateWnd = FALSE);
	void    InitLCDCfigPage(BOOL bUpdateWnd = FALSE);
	void    InitVolFreqPage(BOOL bUpdateWnd = FALSE);
    void    InitUartPortSwitchPage(BOOL bUpdateWnd = FALSE);

	int		GetFileInfo(LPCTSTR lpszFileID, LPDWORD ppFileInfo, BOOL bWnd = FALSE);
	int		GetAllFileInfo(LPDWORD ppFileInfo, BOOL bWnd = FALSE);

	BOOL	LoadPacket(LPCTSTR lpszPacketName);
	BOOL	LoadConfig(LPCTSTR pFileName,CString &strErrorMsg); // only for SPUpgrade

	BOOL	DoPacket();
	BOOL    FindLangNVItem(LPCTSTR lpszFilePath,CUIntArray &agLangFlag, FILETIME * pLastWriteTime);
	BOOL    FindLCDItem(LPCTSTR lpszFilePath,VEC_LCD_CFIG &vLcdCfig, FILETIME * pLastWriteTime);

    BOOL    GetFdl2Flag(LPCTSTR lpszFilePath,UINT nType);
	int     GetDLFiles(CStringArray &agFiles);
    BOOL    CheckCrcDLFiles(CString& strCrcFailFileId);

	int     GetBackupFiles(CStringArray &agID,BOOL bSharkNand);

	int     GetDLNVID(CStringArray &agID);
	BOOL    IsMapPBFileBuf();
	BOOL    IsModifiedDLFile(LPCTSTR lpszDLFile);
	BOOL	IsLoadFromPac(LPCTSTR lpDLFile);
	DATA_INFO_T GetDataInfo(LPCTSTR lpDLFile);

    BOOL    ParseCuRef(LPCTSTR lpFile);
    BOOL    CheckCU(LPCTSTR lpCU);


// Dialog Data
	//{{AFX_DATA(CMainPage)
	enum { IDD = IDD_PROPPAGE_MAIN }; //lint !e30
	CEdit	m_edtPrdVersion;
	CCoolStatic	m_stcProdComment;
	CXListCtrl	m_lstProductInfo;
	CComboBox	m_cmbProduct;
	CComboBox	m_cmbPort;
	CComboBox	m_cmbBaudrate;
	BOOL	m_bAllFiles;
	BOOL	m_bChkWriteSN;
    vector<CString> m_listCu;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMainPage)
	public:
	virtual void OnOK();
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
//	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
    int m_nBaudRate;
    int m_nComPort;

	WORD m_wLangNVItemID;

	CString			m_strPrdVersion;
	CString			m_strCurProduct;
	PRODUCT_INFO_T *m_pCurProductInfo;
	int				m_nCurProduct;

	CString			m_strTempProduct;
	PRODUCT_INFO_T *m_pTmpProductInfo;
	int				m_nTmpProduct;
	
	std::vector<PRODUCT_INFO_T*> m_vctProductInfo;
		
	CStringArray m_arrProductName;

//	CStringArray m_arrFileName;
//	CStringArray m_arrAllFileName;
    CStringArray m_arrBaudRateDefault;
    CStringArray m_arrBaudRateConfig;
	

	typedef CMap<CString, LPCTSTR, CString, LPCTSTR>   MAP_DLFILE;  //id->file
	typedef CMap<CString, LPCTSTR, BOOL, BOOL>         MAP_DLSTATE; //id->check
	typedef CMap<CString, LPCTSTR, __int64, __int64>       MAP_DLSIZE;  //id->Size
	typedef CMap<CString, LPCTSTR, DATA_INFO_T, DATA_INFO_T>   MAP_DATAINFO;  //FileName->DATAINFO
	typedef CMap<CString, LPCTSTR, FILETIME, FILETIME> MAP_FILELASTTIME;	  //FileName->LastWriteTime
	MAP_DLFILE  m_mapDLFiles;	
	MAP_DLSTATE m_mapDLState;
	MAP_DLSIZE  m_mapDLSize;
	MAP_DATAINFO m_mapDataInfo;
	MAP_FILELASTTIME m_mapLastTime;

	int m_nProductCount;
	BOOL m_bInit;


	BIN_PACKET_HEADER_T m_bph;

	BOOL m_bOmaDM;
	BOOL m_bTmpOmaDM;

	BOOL m_bPreload;
	BOOL m_bTmpPreload;

	BOOL m_bKernelImg2;
	BOOL m_bTmpKernelImg2;
	
	BOOL m_bRomDisk;
	BOOL m_bTmpRomDisk;

	BOOL m_bEnableChipDspMap;
	
	BOOL m_bWriteSN;
	FILETIME	   m_ftPacOrg;

private:
	IXmlConfigParse*	m_pXmlConfigParse;
	
    CCalibrationPage*	m_pCaliPage;
	CPageMultiLang*		m_pMultiLangPage;
	CPageLcdConfig*		m_pLCDCfigPage;
	CPageOptions*		m_pOptionsPage;
	CPageVolFreq*		m_pVolFreqPage;
    CPageUartPortSwitch* m_pUartPortSwitchPage;
	CFlashOptPage*		m_pFlashOptPage;
    CPageCustomization* m_pCustomizationPage;

	CString				m_strIniFile;	
	CString				m_strPacketFile;

	typedef CMap<CString, LPCTSTR, CString, LPCTSTR>   PRODUCT_CONFIG_FILE_MAP;
	PRODUCT_CONFIG_FILE_MAP m_mapPrdCfg;

	BOOL m_bCanSel;
	BOOL m_bCanEdit;
	
	BOOL m_bListLock;

	BOOL m_bShowChinese;	

  
// Implementation
protected:
    void GetLastModifyTime(LPCTSTR lpszFile,FILETIME& ft);
	BOOL DeepCopyProductInfo( PPRODUCT_INFO_T pDst, const PPRODUCT_INFO_T pSrc);
	void ClearProdInfoVector();
	BOOL CheckFileValidate(LPCTSTR lpszFile);
	BOOL CheckFileChipPrefix( LPCTSTR lpszFile, CString &strChipPre );
	int  GetUDiskFiles(LPCTSTR lpszBaseUDiskFile, CStringArray &agFiles);
	int  GetDspFiles(LPCTSTR lpszBaseDspFile, CStringArray &agFiles);
	int  GetMultiFiles(CStringArray &agFiles, CString &strErr);
	int  GetPageBlockFiles(CStringArray &agFiles);
	int  GetMultiNVFiles(CStringArray &agFiles,CString& strWarning);
	
	void ModifyItemGray();
	void InitBaudrate();
	void InitPort();
	void InitListCtrl();
	void LoadDLFiles(LPCTSTR lpszConfigFile);
	BOOL InitProdInfo();
	BOOL CheckInitDLFiles();
	// Generated message map functions
	//{{AFX_MSG(CMainPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCmbProduct();
	afx_msg void OnChkAll();
	afx_msg void OnDblclkLstProductInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditLstProductInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedLstProductInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRclickLstProductInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickLstProductInfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
private:
	BOOL _LoadConfig(CString &strErrorMsg);
    CString GetXmlFile();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINPAGE_H__1E1CBF30_F40E_423E_913F_1D0D8057C816__INCLUDED_)
