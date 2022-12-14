// FlashInfo.h: interface for the CFlashInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLASHINFO_H__6076E802_5C76_4D0E_8BCD_37B9170E15EE__INCLUDED_)
#define AFX_FLASHINFO_H__6076E802_5C76_4D0E_8BCD_37B9170E15EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BMAGlobal.h"

#import <msxml2.dll>

class CFlashInfo  
{
public:
	CFlashInfo();
	virtual ~CFlashInfo();
public:
	BOOL LoadFlashInfo(LPCTSTR lpszCfgFileName,DWORD dwReserved=0);
	PFLASH_SPEC_T GetFlashInfo(DWORD dwFlashID, DWORD dwDeviceID,DWORD dwExtendID, 
								BOOL bBigEndian = TRUE,DWORD dwReserved = 0);
	DWORD GetLastError();
	void RelaseMem(PFLASH_SPEC_T &pFlashSpec);
	DWORD ConvEdnDWORD ( DWORD dwData );
private:
	void  ConvertEndian(PFLASH_SPEC_T pSpec);
	
//	FLASH_SPEC_T  m_FlashInfo;
	DWORD         m_dwErrCode;
	MSXML2::IXMLDOMDocumentPtr  m_pXMLDoc;       // xml document
};

#endif // !defined(AFX_FLASHINFO_H__6076E802_5C76_4D0E_8BCD_37B9170E15EE__INCLUDED_)
