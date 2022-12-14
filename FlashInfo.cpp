// FlashInfo.cpp: implementation of the CFlashInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dloader.h"
#include "FlashInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlashInfo::CFlashInfo():m_dwErrCode(0xFFFFFFFF),m_pXMLDoc(NULL)
{
	/*lint -e1401*/
}
/*lint -restore*/

CFlashInfo::~CFlashInfo()
{

}

BOOL CFlashInfo::LoadFlashInfo(LPCTSTR lpszCfgFileName,DWORD dwReserved /*=0*/ )
{
	UNUSED_ALWAYS(dwReserved);

	if(lpszCfgFileName == NULL)
		return FALSE;

	CoInitialize(NULL);

	HRESULT	hr;
	hr=m_pXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if(FAILED(hr))
	{  
		TRACE(_T("Can't create DOMDocument object, please check if fixed the MS XML Parser runtime lib!\n"));
		return FALSE;
	}		
	//Load File 
	m_pXMLDoc->put_validateOnParse(FALSE);
	if(!m_pXMLDoc->load(lpszCfgFileName))
	{
		TRACE(_T("load file fail\n"));
		return FALSE;
	}
	
	return TRUE;
}

PFLASH_SPEC_T CFlashInfo::GetFlashInfo(DWORD dwFlashID, DWORD dwDeviceID,
									   DWORD dwExtendID, BOOL bBigEndian /*= TRUE*/,DWORD dwReserved/* = 0*/)
{
	UNUSED_ALWAYS(dwReserved);

	if(m_pXMLDoc == NULL)
		return NULL;

	MSXML2::IXMLDOMNodePtr		pFlashNode;

	CString strFlashSel;
	strFlashSel.Format(_T("//Flash[@flash_id=\"0x%X\"][@device_id=\"0x%X\"][@extend_id=\"0x%X\"]"),
		dwFlashID,dwDeviceID,dwExtendID);
	pFlashNode = m_pXMLDoc->selectSingleNode((_bstr_t)strFlashSel);
	if(pFlashNode == NULL)
	{
		return NULL;
	}

	PFLASH_SPEC_T pFlashSpec = new FLASH_SPEC_T;
	memset(pFlashSpec,0,sizeof(FLASH_SPEC_T));

	pFlashSpec->dwFlashID = dwFlashID;
	pFlashSpec->dwDeviceID = dwDeviceID;
	pFlashSpec->dwExtendID = dwExtendID;

	MSXML2::IXMLDOMNodeListPtr pFlashItemNodes = NULL;
	MSXML2::IXMLDOMNodePtr pFlashItemNode = NULL;
	MSXML2::IXMLDOMNodeListPtr pFlashSubItemNodes = NULL;
	MSXML2::IXMLDOMNodePtr pFlashSubItemNode = NULL;
	MSXML2::IXMLDOMElementPtr pFlashElement = NULL;
	MSXML2::IXMLDOMElementPtr pFlashSubElement = NULL;	

    _variant_t	vtValue;
	CString strNodeValue;
	_bstr_t bstrNodeName;
	CString strNodeName;

	_variant_t	vtSubValue ;	
	CString strSubNodeValue;
	_bstr_t bstrSubNodeName;
	CString strSubNodeName;

	long cSubItem = 0;
				
	int nValue =0;
    pFlashElement = pFlashNode;


	vtValue = pFlashElement->getAttribute((_bstr_t)_T("unique_id"));
	strNodeValue = vtValue.bstrVal;
	_stscanf(strNodeValue,_T("0x%x"),&nValue);
	pFlashSpec->dwReserved[9] = nValue;


	pFlashItemNodes = pFlashNode->GetchildNodes();
	long cItem = pFlashItemNodes->Getlength();
	for(long i = 0; i<cItem; i++)
	{	
		pFlashItemNode = pFlashItemNodes->nextNode();
		bstrNodeName = pFlashItemNode->GetnodeName();
		strNodeName = (TCHAR *)bstrNodeName;
		
		pFlashElement = pFlashItemNode;
		if(strNodeName.Compare(_T("DriverSort"))==0)
		{
			nValue = 0;
			vtValue = pFlashElement->getAttribute((_bstr_t)_T("value"));
			strNodeValue = vtValue.bstrVal;
			_stscanf(strNodeValue,_T("%d"),&nValue);
			pFlashSpec->dwDriverSort = nValue;
		}
		else if(strNodeName.Compare(_T("WriteBufLen"))==0)
		{
			nValue = 0;
			vtValue = pFlashElement->getAttribute((_bstr_t)_T("value"));
			strNodeValue = vtValue.bstrVal;
			_stscanf(strNodeValue,_T("%d"),&nValue);
			pFlashSpec->dwWriteBufLen = nValue;
		}
		else if(strNodeName.Compare(_T("FlashSize"))==0)
		{
			nValue = 0;
			vtValue = pFlashElement->getAttribute((_bstr_t)_T("value"));
			strNodeValue = vtValue.bstrVal;
			_stscanf(strNodeValue,_T("0x%X"),&nValue);
			pFlashSpec->dwFlashSize = nValue;
		}
		else if(strNodeName.Compare(_T("EmcTiming"))==0)
		{
			pFlashSubItemNodes = pFlashItemNode->GetchildNodes();
			cSubItem = pFlashSubItemNodes->Getlength();
			for(int j = 0; j<cSubItem;j++)
			{
				pFlashSubItemNode = pFlashSubItemNodes->nextNode();
				bstrSubNodeName = pFlashSubItemNode->GetnodeName();
				strSubNodeName = (TCHAR *)bstrSubNodeName;
				
				pFlashSubElement = pFlashSubItemNode;
				if(strSubNodeName.Compare(_T("RdTime"))==0)
				{
					nValue = 0;
					vtValue = pFlashSubElement->getAttribute((_bstr_t)_T("value"));
					strNodeValue = vtValue.bstrVal;
					_stscanf(strNodeValue,_T("%d"),&nValue);
					pFlashSpec->etMemTiming.dwRdTime = nValue;
				}
				else if(strSubNodeName.Compare(_T("RdHoldTime"))==0)
				{
					nValue = 0;
					vtValue = pFlashSubElement->getAttribute((_bstr_t)_T("value"));
					strNodeValue = vtValue.bstrVal;
					_stscanf(strNodeValue,_T("%d"),&nValue);
					pFlashSpec->etMemTiming.dwRdHoldTime = nValue;
				}
				else if(strSubNodeName.Compare(_T("WrTime"))==0)
				{
					nValue = 0;
					vtValue = pFlashSubElement->getAttribute((_bstr_t)_T("value"));
					strNodeValue = vtValue.bstrVal;
					_stscanf(strNodeValue,_T("%d"),&nValue);
					pFlashSpec->etMemTiming.dwWrTime = nValue;
				}
				else if(strSubNodeName.Compare(_T("WrHoldTime"))==0)
				{
					nValue = 0;
					vtValue = pFlashSubElement->getAttribute((_bstr_t)_T("value"));
					strNodeValue = vtValue.bstrVal;
					_tscanf(strNodeValue,_T("%d"),&nValue);
					pFlashSpec->etMemTiming.dwWrHoldTime = nValue;
				}
				else if(strSubNodeName.Compare(_T("W2wTrTime"))==0)
				{
					nValue = 0;
					vtValue = pFlashSubElement->getAttribute((_bstr_t)_T("value"));
					strNodeValue = vtValue.bstrVal;
					_stscanf(strNodeValue,_T("%d"),&nValue);
					pFlashSpec->etMemTiming.dwW2wTrTime = nValue;
				}
				else if(strSubNodeName.Compare(_T("W2rTrTime"))==0)
				{
					nValue = 0;
					vtValue = pFlashSubElement->getAttribute((_bstr_t)_T("value"));
					strNodeValue = vtValue.bstrVal;
					_stscanf(strNodeValue,_T("%d"),&nValue);
					pFlashSpec->etMemTiming.dwW2rTrTime = nValue;
				}
				else if(strSubNodeName.Compare(_T("R2wTrTime"))==0)
				{
					nValue = 0;
					vtValue = pFlashSubElement->getAttribute((_bstr_t)_T("value"));
					strNodeValue = vtValue.bstrVal;
					_tscanf(strNodeValue,_T("%d"),&nValue);
					pFlashSpec->etMemTiming.dwR2wTrTime = nValue;
				}
				else if(strSubNodeName.Compare(_T("R2rTrTime"))==0)
				{
					nValue = 0;
					vtValue = pFlashSubElement->getAttribute((_bstr_t)_T("value"));
					strNodeValue = vtValue.bstrVal;
					_stscanf(strNodeValue,_T("%d"),&nValue);
					pFlashSpec->etMemTiming.dwR2rTrTime = nValue;
				}
			}
		}
//		else if(strNodeName.Compare(_T("Reserved"))==0)
//		{
//			_tcscpy(tFileInfo.szType,strNodeValue);
// 		}
		else if(strNodeName.Compare(_T("BlockSpec"))==0)
		{			
			pFlashSubItemNodes = pFlashItemNode->GetchildNodes();
			cSubItem = pFlashSubItemNodes->Getlength();
			if(cSubItem > 10) // Max Item is 10
				cSubItem = 10;
			pFlashSpec->dwRealBlockNum = cSubItem;
			for(int j = 0; j<cSubItem;j++)
			{
				pFlashSubItemNode = pFlashSubItemNodes->nextNode();
				
				pFlashSubElement = pFlashSubItemNode;
			
				nValue = 0;
				vtSubValue = pFlashSubElement->getAttribute((_bstr_t)_T("sector_start_address"));
				strSubNodeValue = vtSubValue.bstrVal;
				_stscanf(strSubNodeValue,_T("0x%X"),&nValue);
				pFlashSpec->bsBlockStruct[j].dwSectorStartAddr = nValue;
				
				nValue = 0;
				vtSubValue = pFlashSubElement->getAttribute((_bstr_t)_T("sector_size"));
				strSubNodeValue = vtSubValue.bstrVal;
				_stscanf(strSubNodeValue,_T("0x%X"),&nValue);
				pFlashSpec->bsBlockStruct[j].dwSectorSize = nValue;

				nValue = 0;
				vtSubValue = pFlashSubElement->getAttribute((_bstr_t)_T("sector_end_address"));
				strSubNodeValue = vtSubValue.bstrVal;
				_stscanf(strSubNodeValue,_T("0x%X"),&nValue);
				pFlashSpec->bsBlockStruct[j].dwSectorEndAddr = nValue;

				nValue = 0;
				vtSubValue = pFlashSubElement->getAttribute((_bstr_t)_T("reserved"));
				strSubNodeValue = vtSubValue.bstrVal;
				_stscanf(strSubNodeValue,_T("0x%X"),&nValue);
				pFlashSpec->bsBlockStruct[j].dwReserved = nValue;
			}
		}
	}
	if(bBigEndian)
	{
		ConvertEndian(pFlashSpec);
	}
	return pFlashSpec;
}

DWORD CFlashInfo::GetLastError()
{
	return m_dwErrCode;
}

void CFlashInfo::RelaseMem(PFLASH_SPEC_T &pFlashSpec)
{
	if( NULL != pFlashSpec)
	{
		delete pFlashSpec;
		pFlashSpec = NULL;
	}
}

void CFlashInfo::ConvertEndian(PFLASH_SPEC_T pSpec)
{
	int nDW = sizeof(FLASH_SPEC_T)/4;
	DWORD *pDW =(DWORD *)pSpec;
	for(int i = 0; i<nDW; i++ )
	{
		*(pDW+i) = ConvEdnDWORD(*(pDW+i));
	}
}

DWORD CFlashInfo::ConvEdnDWORD ( DWORD dwData )
{
     return MAKELONG( MAKEWORD(HIBYTE(HIWORD(dwData)), LOBYTE(HIWORD(dwData))),
                         MAKEWORD(HIBYTE(LOWORD(dwData)), LOBYTE(LOWORD(dwData)))); 
}