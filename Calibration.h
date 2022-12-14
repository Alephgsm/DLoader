
#ifndef _CALIBRATION_H
#define	_CALIBRATION_H

//#define AGC_SIZE     2 * 142 * 2   
//#define APC_SIZE     90 * 2

#define NV_MULTI_LANG_ID   (405)
#define GSM_CALI_ITEM_ID   (0x2)
#define GSM_IMEI_ITEM_ID   (0x5)
#define XTD_CALI_ITEM_ID   (0x516)
#define LTE_CALI_ITEM_ID   (0x9C4)
#define BT_ITEM_ID         (0x191)
#define WCDMA_CALI_ITEM_ID   (0x12D)

#define BT_ADDR_LEN  6
typedef struct BT_CONFIG_T
{
	BYTE  bt_addr[BT_ADDR_LEN];
	WORD  xtal_dac;
}BT_CONFIG;

enum {
	E_GSM_CALI		= 0,    //bit 0
	E_TD_CALI		= 1,    //bit 1
	E_WCDMA_CALI	= 2,    //bit 2
	E_LTE_CALI		= 3,    //bit 3
    E_CALI_COUNT
};

struct TCALIINFO
{
    WORD    wDefNvID;
    WORD    wDefCheckID;
    TCHAR   szCaliKeyWord[MAX_PATH];
};
const TCALIINFO g_CaliFlagTable[] =		
{
    { GSM_CALI_ITEM_ID,     GSM_CALI_ITEM_ID,     _T("Calibration")       },    // GSM 
    { XTD_CALI_ITEM_ID,     GSM_CALI_ITEM_ID,     _T("TD_Calibration")    },    // TD  ,check flag same as GSM
    { WCDMA_CALI_ITEM_ID,   WCDMA_CALI_ITEM_ID,   _T("W_Calibration")     },    // WCDMA 
    { LTE_CALI_ITEM_ID,     LTE_CALI_ITEM_ID,     _T("LTE_Calibration")   },    // LTE
};

CString GetErrorDesc( UINT dwID );

///////////////////////////////////////////////////////////////////////
BOOL  XFindNVOffset(WORD wId,LPBYTE lpCode,DWORD dwCodeSize,
                     DWORD& dwOffset,DWORD& dwLength,BOOL bBigEndian = TRUE);

BOOL  XFindNVOffsetEx(WORD wId,LPBYTE lpCode,DWORD dwCodeSize,
                      DWORD& dwOffset,DWORD& dwLength,BOOL &bBigEndian, BOOL bModule);

DWORD GSMCaliPreserve( WORD wID,LPBYTE lpCode, DWORD dwCodeSize, 
                       LPBYTE lpReadBuffer, DWORD dwReadSize,
                       BOOL bOldReplaceNew , BOOL bContinue );

DWORD XTDCaliPreserve(  WORD wID,LPBYTE lpCode, DWORD dwCodeSize, 
                        LPBYTE lpReadBuffer, DWORD dwReadSize,
                        BOOL bOldReplaceNew , BOOL bContinue );

DWORD XPreserveNVItem( WORD wID, LPBYTE lpCode, DWORD dwCodeSize, 
					   LPBYTE lpReadBuffer, DWORD dwReadSize,
					   BOOL bOldReplaceNew=FALSE, BOOL bContinue=FALSE);

BOOL  XCheckGSMCali(WORD wId,LPBYTE lpPhoBuf,DWORD dwPhoSize,CString &strErr,BOOL bModule);

BOOL  XCheckNVStructEx(LPBYTE lpPhoBuf, DWORD dwPhoSize,BOOL &bBigEndian,BOOL bModule);

//BOOL  XSetRandomBT( WORD wId, LPBYTE lpCode, DWORD dwCodeSize, const BT_CONFIG &bt);

DWORD  XPreserveIMEIs( CUIntArray *pIMEIID,LPBYTE lpCode, DWORD dwCodeSize, 
					  LPBYTE lpReadBuffer, DWORD dwReadSize, int &nFailedIMEIIndex,
					  BOOL bOldReplaceNew=FALSE, BOOL bContinue=FALSE);
/*
DWORD  LTECaliPreserve(  LPBYTE lpCode, DWORD dwCodeSize, 
					  LPBYTE lpReadBuffer, DWORD dwReadSize,
                        BOOL bOldReplaceNew , BOOL bContinue );
*/
BOOL  XCheckLTECali(WORD wId,LPBYTE lpPhoBuf,DWORD dwPhoSize,CString &strErr,BOOL bModule);
BOOL  XCheckWCDMACali(WORD wId,LPBYTE lpPhoBuf,DWORD dwPhoSize,CString &strErr,BOOL bModule);
BOOL  CalibrationCheck(WORD wId,LPBYTE lpPhoBuf,DWORD dwPhoSize,CString &strErr,int nCaliType);//nCaliType=0:GSM,1:TD,2:WCDMA,3:LTE

BOOL ClearGSMCaliFlag(WORD wId,LPBYTE lpNVBuf,DWORD dwNVSize);
BOOL ClearWCDMACaliFlag(WORD wId,LPBYTE lpNVBuf,DWORD dwNVSize);
BOOL ClearLTECaliFlag(WORD wId,LPBYTE lpNVBuf,DWORD dwNVSize);
void GSM_VerBToB( LPBYTE pFileBuf,DWORD dwOffsetFile,BOOL bFileBigEndian,LPBYTE pModuBuf,DWORD dwOffsetModu,BOOL bModuBigEndian );

//////////////////////////////////////////////////////////////////////////
               
#endif // _CALIBRATION_H