#if !defined(AFX_PAGEVOLFREQ_H__E771CFB9_EBEF_43B9_ABB2_D88C048332FF__INCLUDED_)
#define AFX_PAGEVOLFREQ_H__E771CFB9_EBEF_43B9_ABB2_D88C048332FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageVolFreq.h : header file
//
#include "NumericEdit.h"
#include "CoolListCtrl.h"

extern "C"
{
#include "./hash/sha256.h"
}
#define M_HZ      (1*1000*1000)
#define SHA256_DIGEST_SIZE  32
#define SEC_SPL_MAGIC       (0x42544844)

typedef struct _SPL_HEADER_T
{
    DWORD           dwMagic;                          //  0x42544844
    DWORD           dwVersion;
    unsigned char   sha256Hash[SHA256_DIGEST_SIZE];   //  sha256 hash val
    __int64         llImgAddr;                        //  image loader address
    DWORD           dwImgSize;                        //  image size
    unsigned char   reserved[460];                    //  460 + 13*4 = 512

    _SPL_HEADER_T()
    {
        memset(this,0,sizeof(_SPL_HEADER_T));
        dwMagic     = SEC_SPL_MAGIC;
        dwVersion   = 0x01;
    }
}SPL_HEADER_T;


#define VOL_FREQ_MAGIC_HDR 0x5555AAAA
#define VOL_FREQ_MAGIC_END 0xAAAA5555
#define MAX_DEBUG_NUM      20

namespace VOL_FREQ_V1       //sizeof(VOL_FREQ_V1) = 11*4
{
	typedef struct _VOL_FREQ_PARAM
	{
		BYTE ver[4];
		DWORD CLK_CA7_CORE;
		DWORD DDR_FREQ;
		DWORD CLK_CA7_AXI;
		DWORD CLK_CA7_DGB;
		DWORD CLK_CA7_AHB;
		DWORD CLK_CA7_APB;
		DWORD CLK_PUB_AHB;
		DWORD CLK_AON_APB;
		DWORD DCDC_ARM;
		DWORD DCDC_CORE;
		
		_VOL_FREQ_PARAM()
		{
			memset(this,0,sizeof(_VOL_FREQ_PARAM));
			CLK_CA7_CORE = 100000000;
			DDR_FREQ     = 100000000;
			CLK_CA7_AXI  = 100000000;
			CLK_CA7_DGB  = 10000000;
			DCDC_ARM = 600;
			DCDC_CORE = 1500;
		}
		
		void Init()
		{
			memset(this,0,sizeof(_VOL_FREQ_PARAM));
			CLK_CA7_CORE = 100000000;
			DDR_FREQ     = 100000000;
			CLK_CA7_AXI  = 100000000;
			CLK_CA7_DGB  = 10000000;
			DCDC_ARM = 600;
			DCDC_CORE = 1500;
		}
		
	}VOL_FREQ_PARAM;
}

namespace VOL_FREQ_V2   //sizeof(VOL_FREQ_V2) = 13*4 + 20*4
{
	typedef struct _VOL_FREQ_PARAM
	{
		BYTE  ver[4];
		DWORD CLK_CA7_CORE;
		DWORD DDR_FREQ;
		DWORD CLK_CA7_AXI;
		DWORD CLK_CA7_DGB;
		DWORD CLK_CA7_AHB;
		DWORD CLK_CA7_APB;
		DWORD CLK_PUB_AHB;
		DWORD CLK_AON_APB;
		DWORD DCDC_ARM;
		DWORD DCDC_CORE;
		DWORD DCDC_MEM;
		DWORD DCDC_GEN;
		DWORD debug[MAX_DEBUG_NUM];
		
		_VOL_FREQ_PARAM()
		{
			memset(this,0,sizeof(_VOL_FREQ_PARAM));
			CLK_CA7_CORE = 100000000;
			DDR_FREQ     = 100000000;
			CLK_CA7_AXI  = 100000000;
			CLK_CA7_DGB  = 10000000;
			DCDC_ARM = 600;
			DCDC_CORE = 1500;
			DCDC_MEM = 600;
			DCDC_GEN = 600;
		}
		
		void Init()
		{
			memset(this,0,sizeof(_VOL_FREQ_PARAM));
			CLK_CA7_CORE = 100000000;
			DDR_FREQ     = 100000000;
			CLK_CA7_AXI  = 100000000;
			CLK_CA7_DGB  = 10000000;
			DCDC_ARM = 600;
			DCDC_CORE = 1500;
			DCDC_MEM = 600;
			DCDC_GEN = 600;
		}
		
	}VOL_FREQ_PARAM;
}

#define   VOL_FREQ_VER1 0
#define   VOL_FREQ_VER2 1
const int VOL_FREQ_VER3 = 0x02;

#define VOL_FREQ_VER2_BASE_SIZE (sizeof(VOL_FREQ_V2::VOL_FREQ_PARAM)-MAX_DEBUG_NUM*sizeof(DWORD))

enum {
    E_EDITBOX = 0,
    E_COMBOX  = 1
};


enum {
    E_UNIT_NONE = 0,
    E_UNIT_MHZ  = 1,
    E_UNIT_MV   = 2,
    E_UNIT_COUNT
};


const CString g_szUnitDesc[] = 
{
    _T(""),
    _T("MHz"),
    _T("mv"),
    _T(""),
};

typedef struct _VOL_TURNING_HEADER
{
    BYTE  ver[4];
    DWORD dwTunItemCount;
    DWORD dwLevItemCount;
    _VOL_TURNING_HEADER()
    {
        memset(this,0,sizeof(_VOL_TURNING_HEADER));
        ver[0]=0x2;
    }
}VOL_TURNING_HEADER,*PVOL_TURNING_HEADER_PTR;
#define  MAX_TAG_LEN 8
#ifndef UINT8
typedef unsigned char UINT8;
#endif
typedef struct _VOL_TURNING_ITEM
{
    BYTE    byteTag[MAX_TAG_LEN];	// item name    
    union {
        // Turning Item
        struct {
            DWORD  dwValue; // item value
        } TunItem;

        // Level Item
        struct {			
            UINT8 cUsed;    // used Count
            UINT8 cLevel;   // level value
            UINT8 cTotal;   // total item
            UINT8 reserved1;
        } LvlItem;	    
    };
    _VOL_TURNING_ITEM()
    {
        memset(this,0,sizeof(_VOL_TURNING_ITEM));
    }
}VOL_TURNING_ITEM,*PVOL_TURNING_ITEM_PTR;

typedef struct _SPL_TURNING_ITEM
{
    VOL_TURNING_ITEM vti;
    DWORD   dwIndex;        // item index
    DWORD   dwCtrlType;     // 0: EditBox,1:ComBox
    DWORD   dwUnit;
    TCHAR   szName[64];     //show name
    _SPL_TURNING_ITEM()
    {
        memset(this,0,sizeof(_SPL_TURNING_ITEM));
        dwUnit = 0;
    }
}SPL_TURNING_ITEM,*PSPL_TURNING_ITEMPTR;

typedef std::vector<CString> VECSTR;
typedef CMap<CString, LPCTSTR, VECSTR, VECSTR > MAP_VOLLEVEL;

/////////////////////////////////////////////////////////////////////////////
// CPageVolFreq dialog

class CPageVolFreq : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageVolFreq)

// Construction
public:
	CPageVolFreq();
	~CPageVolFreq();
	BOOL	LoadSettings(LPCTSTR pFileName,CString &strErrorMsg);
	BOOL	SaveSettings(LPCTSTR pFileName);
	void    SetConfig(BOOL bShowWnd,LPCTSTR lpszFile);

// Dialog Data
	//{{AFX_DATA(CPageVolFreq)
	enum { IDD = IDD_PROPPAGE_VOL_FREQ };
	CNumericEdit	m_edtDebug;
	CComboBox	m_cmbDebug;
	UINT	m_nClkCa7Core;
	UINT	m_nDdrFreq;
	UINT	m_nClkCa7Axi;
	UINT	m_nClkCa7Dgb;
	UINT	m_nDcdcArm;
	UINT	m_nDcdcCore;
	int		m_nClkAonApb;
	int		m_nClkCa7Ahb;
	int		m_nClkCa7Apb;
	int		m_nClkPubAhb;
	UINT	m_nDcdcGen;
	UINT	m_nDcdcMem;
	CCoolListCtrl m_lstSplTurning;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageVolFreq)
	public:
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageVolFreq)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDebug();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    BOOL FindParam(LPCTSTR lpszFile);
	void ShowParam(BOOL bShow);
    void EnableParam(BOOL bEnable);
	BOOL SaveSPLFile();


private:
	BOOL _FindParam(LPCTSTR lpszFile, LPBYTE &pBuf, DWORD &dwSize, DWORD &dwOffset,DWORD& dwCodeLen );
    void ParseLevelItem(LPBYTE pBuf,DWORD dwOffset,DWORD dwCodeLen);
    void AddSplTurningItem(VOL_TURNING_ITEM* pTI,int nIndex,int nCtrlType = E_EDITBOX);
    void InitList();
    void FillList(BOOL bEnable);
    CString ConvetValue2Level(DWORD dwValue,BOOL bHasUnit = TRUE);
    CString GetLevelName(int nLevel,LPCTSTR lpszItemName);
    DWORD  GetLevelData( LPCTSTR lpszItemName,LPCTSTR lpszLevelName);
private:
	VOL_FREQ_V2::VOL_FREQ_PARAM m_tVolFreqParam;
	VOL_FREQ_V2::VOL_FREQ_PARAM m_tTmpVolFreqParam;

    std::vector<SPL_TURNING_ITEM> m_vecTurningItem;
    std::vector<SPL_TURNING_ITEM> m_vecTmpTurningItem;
    MAP_VOLLEVEL m_mapVolLevel;

	CString m_strSPLFile;
	CString m_strTmpSPLFile;

	BOOL    m_bRightSPL;
	BOOL    m_bTmpRightSPL;
	int     m_nVolFreqVer;
	int     m_nTmpVolFreqVer;
	int     m_nDebugCount;
	int     m_nTmpDebugCount;
	int     m_nDebugCurSel;
public:
    afx_msg void OnLvnEndlabeleditListSplTurning(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickListSplTurning(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEVOLFREQ_H__E771CFB9_EBEF_43B9_ABB2_D88C048332FF__INCLUDED_)
