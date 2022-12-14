// BinPack.cpp: implementation of the CBinPack class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "BinPack.h"
#include "DLoader.h"
extern "C"
{
#include "crc16.h"
}



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_RW_SIZE   (0xA00000)   //10M

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// version 1.0.0
const _TCHAR CBinPack::m_szVersion[24] = _T("BP_R1.0.0");

CBinPack::CBinPack(BOOL bZipPac/* =FALSE*/)
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_strReleaseDir = _T("");
	m_strCfgPath = _T("");
	m_pXMLDoc = NULL;   //lint !e63
	m_pFileNodeList = NULL; //lint !e63
	#ifdef _FACTORY
	m_bIsExistEmptyFile = FALSE;
	#endif
    m_bZipPac       = bZipPac;
    m_bRcvThread    = TRUE;
    m_dwRcvThreadID = 0;        
    m_hRcvWindow    = NULL;     
	/*lint -e1401*/
}
/*lint -restore*/

CBinPack::~CBinPack()
{
	Close();
	/*lint -save -e1740 */
}
/*lint -restore */

void CBinPack::Initialize(BOOL bRcvThread, LPCVOID pReceiver)
{
    m_bRcvThread = bRcvThread;

    if ( bRcvThread )
    {
        m_dwRcvThreadID = * (DWORD *) pReceiver;
    }
    else
    {
        m_hRcvWindow = *(HWND*)pReceiver;
    }
}
/** Open bin packet file for read or write
  *
  * @param lpszFileName: packet file name
  * @param bOpenFlag: if true, open for read, otherwise open for write
  * @return: true,if open successfully;false,otherwise
  */
BOOL CBinPack::Open(LPCTSTR lpszFileName, BOOL bOpenFlag /*= FALSE*/)
{
	_ASSERTE(lpszFileName != NULL);

	CFileException e;
    m_strPacFile = lpszFileName;

	if(!bOpenFlag)
	{
		m_hFile = CreateFile(lpszFileName,
							GENERIC_WRITE, FILE_SHARE_READ,
							NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(INVALID_HANDLE_VALUE == m_hFile)
		{
			CString strError;
			strError.Format(_T("Can not create file [%s]"),lpszFileName);
			TRACE(strError);
			g_theApp.MessageBox(strError);
			return FALSE;
		}
	}
	else
	{
		m_hFile = CreateFile(lpszFileName,
							GENERIC_READ, FILE_SHARE_READ,
							NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(INVALID_HANDLE_VALUE ==  m_hFile)
		{
			CString strError;
			strError.Format(_T("Can not open file [%s]\n"),lpszFileName);
			TRACE(strError);
			g_theApp.MessageBox(strError);
			return FALSE;
		}
	}
	return TRUE;
}

/** Packet bin files into one file with certain struct
  *
  * @param lpszPrdName: the product name
  * @param lpszPrdVersion: the version of the product
  * @param lpszCfgFile: the name of configure file (*.xml)
  * @param pFileArray: point to an array of FILE_T struct
  * @param nFileCount: the number of FILE_T in the buffer pointer by pFileArray
  * @param nFlag: not used now
  * @return: true,if packet successfully;false,otherwise
  */
BOOL CBinPack::Packet(const PBIN_PACKET_HEADER_T pbph,LPCTSTR lpszCfgFile, FILE_T * pFileArray, 
					  BOOL bNVBackup,_X_NV_BACKUP_ITEM_T *pnbi,int nNBICount,int nFlag)
{
	UNUSED_ALWAYS(nFlag);
	_ASSERTE(pbph != NULL);
	_ASSERTE(lpszCfgFile != NULL);
	_ASSERTE(pFileArray != NULL);
	
	CWaitCursor wait;

	if(m_hFile == INVALID_HANDLE_VALUE || pbph == NULL || lpszCfgFile == NULL || pFileArray == NULL)
		return FALSE;

	BIN_PACKET_HEADER_T bph;
    memcpy(&bph,pbph,sizeof(bph));
	_tcscpy(bph.szVersion,m_szVersion);
	bph.nFileCount = pbph->nFileCount + 1; // add a configure file
	bph.dwFileOffset = sizeof(bph);

#if _MSC_VER < 1300  //VC 6.0
	CFile file((int)m_hFile);
#else
	CFile file(m_hFile);
#endif 

	file.Write(&bph,sizeof(bph));

	int nFileCount = bph.nFileCount;
	CString strPrdName = bph.szPrdName;

	FILE_T * pFT = NULL;
    FILE_T   ft;

	DWORD dwOffset = sizeof(bph);
	dwOffset += sizeof(FILE_T)*((UINT)nFileCount);

	WORD wCRC1 = 0;
	WORD wCRC2 = 0;

	
//[[ record bin file information
	for(int i = 0;i< nFileCount-1;i++)
	{
		pFT = pFileArray+i;

        memset(&ft,0,sizeof(FILE_T));
		ft.dwSize = sizeof(FILE_T);

		if(_tcslen(pFT->szFileName) != 0)
		{
			HANDLE hFile = CreateFile(pFT->szFileName,
							GENERIC_READ, FILE_SHARE_READ,
							NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				CString strError;
				strError.Format(_T("Can not open file [%s]\n"),pFT->szFileName);
				TRACE(strError);
				g_theApp.MessageBox(strError);
				return FALSE;
			}
			pFT->nFileSize = GetFileSize(hFile,NULL);
			CloseHandle(hFile); 	


			pFT->dwDataOffset = dwOffset;

			memcpy(&ft,pFT,sizeof(FILE_T));

			CString strFileName = ft.szFileName;
			int nFind = strFileName.ReverseFind(_T('\\'));
			strFileName = strFileName.Right(strFileName.GetLength()-nFind -1);

			memset(ft.szFileName,0,sizeof(ft.szFileName));
			_tcscpy(ft.szFileName,strFileName);
		}
		else
		{
			pFT->nFileSize = 0;
			pFT->dwDataOffset = 0;

			memcpy(&ft,pFT,sizeof(FILE_T));
		}		

		file.Write(&ft,sizeof(ft));

		wCRC2 = crc16(wCRC2,(BYTE*)&ft,sizeof(ft));

		dwOffset+= (DWORD)(pFT->nFileSize);
	}
//]] record bin file information

//[[ record configure file information
	LPBYTE pbText = NULL;
	DWORD nCfgSize = 0;
	memset(&ft,0,sizeof(FILE_T));
	ft.dwSize = sizeof(FILE_T);
	CString str;
	str.Format(_T("%s.xml"),strPrdName.operator LPCTSTR());
	_tcscpy(ft.szFileName,str);

	BOOL bOK = GetSpcPrdConfig(lpszCfgFile,strPrdName,bNVBackup,pnbi,nNBICount,&pbText,nCfgSize);
	if(!bOK)
	{
		CString strError;
		strError.Format(_T("Set xml-configure file failed\n"));
		TRACE(strError);
		g_theApp.MessageBox(strError);
		return FALSE;
	}
/*
	FILE *pTmpFile = _tfopen(lpszCfgFile,_T("rb"));
	fseek(pTmpFile,0,SEEK_END);
	nCfgSize = ftell(pTmpFile);
	fseek(pTmpFile,0,SEEK_SET);

	pbText = new BYTE[nCfgSize];
	fread(pbText,1,nCfgSize,pTmpFile);
*/

	ft.nFileSize = nCfgSize;
	ft.dwDataOffset = dwOffset;
	ft.nFileFlag = 2; // means configure file

	file.Write(&ft,sizeof(ft));
	wCRC2 = crc16(wCRC2,(BYTE*)&ft,sizeof(ft));
	
//]] record configure file information


//[[ write file data
	for(int j=0;j<nFileCount-1;j++)
	{
		pFT = pFileArray+j;
		if(_tcslen(pFT->szFileName) != 0)
		{
			HANDLE hFile2 = CreateFile(pFT->szFileName,
							GENERIC_READ, FILE_SHARE_READ,
							NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if (hFile2 == INVALID_HANDLE_VALUE)
			{
				CString strError;
				strError.Format(_T("Can not open file [%s]\n"),pFT->szFileName);
				TRACE(strError);
				g_theApp.MessageBox(strError);
				return FALSE;
			}	
			
			LPBYTE pBuf = new BYTE[MAX_RW_SIZE];
			
			DWORD dwLeft = pFT->nFileSize;
			DWORD dwReadSize = MAX_RW_SIZE;
			DWORD dwRetSize = 0;
			while(dwLeft > 0)
			{
				memset(pBuf,0,MAX_RW_SIZE);
				if(dwLeft < MAX_RW_SIZE)
				{
					dwReadSize = dwLeft;
				}

				BOOL bRet = ReadFile(hFile2,pBuf,dwReadSize,&dwRetSize,NULL);
				if(!bRet || dwRetSize != dwReadSize)
				{
					CloseHandle(hFile2);
					delete [] pBuf;
					return FALSE;
				}

				dwLeft -= dwReadSize;

				file.Write(pBuf,dwReadSize);
				
				wCRC2 = crc16(wCRC2,(BYTE*)pBuf,dwReadSize);
			}

			delete [] pBuf;

			CloseHandle(hFile2);
		}
	}
	
	file.Write(pbText,nCfgSize); // write configure file
	wCRC2 = crc16(wCRC2,(BYTE*)pbText,nCfgSize);
	if(pbText != NULL)
	{
		delete [] pbText;
		pbText = NULL;
	}
//]] write file data

//write packet header
	file.Flush();
	bph.dwSize = (DWORD)file.SeekToEnd();
	wCRC1 = crc16(wCRC1,(BYTE*)&bph,sizeof(bph) -sizeof(DWORD));
	//CONVERT_SHORT(wCRC1,bph.wCRC1);
	//CONVERT_SHORT(wCRC2,bph.wCRC2);	
	bph.wCRC1 = wCRC1;
	bph.wCRC2 = wCRC2;

	file.SeekToBegin();
	file.Write(&bph,sizeof(bph));
	file.Flush();

	return TRUE;	
}
BOOL CBinPack::Packet2(const PBIN_PACKET_HEADER_T pbph,LPCTSTR lpszCfgFile, FILE_T * pFileArray)
{
	_ASSERTE(lpszCfgFile != NULL);
	_ASSERTE(pFileArray != NULL);
	
	CWaitCursor wait;

	if(m_hFile == INVALID_HANDLE_VALUE || pbph == NULL || lpszCfgFile == NULL || pFileArray == NULL)
		return FALSE;

	BIN_PACKET_HEADER_T bph;
    memcpy(&bph,pbph,sizeof(bph));
	_tcscpy(bph.szVersion,m_szVersion);
	bph.nFileCount = pbph->nFileCount + 1; // add a configure file
	bph.dwFileOffset = sizeof(bph);

#if _MSC_VER < 1300  //VC 6.0
	CFile file((int)m_hFile);
#else
	CFile file(m_hFile);
#endif 

	file.Write(&bph,sizeof(bph));

	int nFileCount = bph.nFileCount;
	CString strPrdName = bph.szPrdName;

	FILE_T * pFT = NULL;
    FILE_T   ft;

	DWORD dwOffset = sizeof(bph);
	dwOffset += sizeof(FILE_T)*((UINT)nFileCount);

	WORD wCRC1 = 0;
	WORD wCRC2 = 0;

	
//[[ record bin file information
	for(int i = 0;i< nFileCount-1;i++)
	{
		pFT = pFileArray+i;

        memset(&ft,0,sizeof(FILE_T));
		ft.dwSize = sizeof(FILE_T);

		if(_tcslen(pFT->szFileName) != 0)
		{
			HANDLE hFile = CreateFile(pFT->szFileName,
							GENERIC_READ, FILE_SHARE_READ,
							NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				CString strError;
				strError.Format(_T("Can not open file [%s]\n"),pFT->szFileName);
				TRACE(strError);
				g_theApp.MessageBox(strError);
				return FALSE;
			}
			pFT->nFileSize = GetFileSize(hFile,NULL);
			CloseHandle(hFile); 	


			pFT->dwDataOffset = dwOffset;

			memcpy(&ft,pFT,sizeof(FILE_T));

			CString strFileName = ft.szFileName;
			int nFind = strFileName.ReverseFind(_T('\\'));
			strFileName = strFileName.Right(strFileName.GetLength()-nFind -1);

			memset(ft.szFileName,0,sizeof(ft.szFileName));
			_tcscpy(ft.szFileName,strFileName);
		}
		else
		{
			pFT->nFileSize = 0;
			pFT->dwDataOffset = 0;

			memcpy(&ft,pFT,sizeof(FILE_T));
		}		

		file.Write(&ft,sizeof(ft));

		wCRC2 = crc16(wCRC2,(BYTE*)&ft,sizeof(ft));

		dwOffset+= (DWORD)(pFT->nFileSize);
	}
//]] record bin file information

//[[ record configure file information
	LPBYTE pbText = NULL;
	DWORD nCfgSize = 0;
	memset(&ft,0,sizeof(FILE_T));
	ft.dwSize = sizeof(FILE_T);
	CString str;
	str.Format(_T("%s.xml"),strPrdName.operator LPCTSTR());
	_tcscpy(ft.szFileName,str);

	BOOL bOK = GetSpcPrdConfig2(lpszCfgFile,&pbText,nCfgSize);
	if(!bOK)
	{
		CString strError;
		strError.Format(_T("Set xml-configure file failed\n"));
		TRACE(strError);
		g_theApp.MessageBox(strError);
		return FALSE;
	}
/*
	FILE *pTmpFile = _tfopen(lpszCfgFile,_T("rb"));
	fseek(pTmpFile,0,SEEK_END);
	nCfgSize = ftell(pTmpFile);
	fseek(pTmpFile,0,SEEK_SET);

	pbText = new BYTE[nCfgSize];
	fread(pbText,1,nCfgSize,pTmpFile);
*/

	ft.nFileSize = nCfgSize;
	ft.dwDataOffset = dwOffset;
	ft.nFileFlag = 2; // means configure file

	file.Write(&ft,sizeof(ft));
	wCRC2 = crc16(wCRC2,(BYTE*)&ft,sizeof(ft));
	
//]] record configure file information


//[[ write file data
	for(int j=0;j<nFileCount-1;j++)
	{
		pFT = pFileArray+j;
		if(_tcslen(pFT->szFileName) != 0)
		{
			HANDLE hFile2 = CreateFile(pFT->szFileName,
							GENERIC_READ, FILE_SHARE_READ,
							NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if (hFile2 == INVALID_HANDLE_VALUE)
			{
				CString strError;
				strError.Format(_T("Can not open file [%s]\n"),pFT->szFileName);
				TRACE(strError);
				g_theApp.MessageBox(strError);
				return FALSE;
			}	
			
			LPBYTE pBuf = new BYTE[MAX_RW_SIZE];
			
			DWORD dwLeft = pFT->nFileSize;
			DWORD dwReadSize = MAX_RW_SIZE;
			DWORD dwRetSize = 0;
			while(dwLeft > 0)
			{
				memset(pBuf,0,MAX_RW_SIZE);
				if(dwLeft < MAX_RW_SIZE)
				{
					dwReadSize = dwLeft;
				}

				BOOL bRet = ReadFile(hFile2,pBuf,dwReadSize,&dwRetSize,NULL);
				if(!bRet || dwRetSize != dwReadSize)
				{
					CloseHandle(hFile2);
					delete [] pBuf;
					return FALSE;
				}

				dwLeft -= dwReadSize;

				file.Write(pBuf,dwReadSize);
				
				wCRC2 = crc16(wCRC2,(BYTE*)pBuf,dwReadSize);
			}

			delete [] pBuf;

			CloseHandle(hFile2);
		}
	}
	
	file.Write(pbText,nCfgSize); // write configure file
	wCRC2 = crc16(wCRC2,(BYTE*)pbText,nCfgSize);
	if(pbText != NULL)
	{
		delete [] pbText;
		pbText = NULL;
	}
//]] write file data

//write packet header
	file.Flush();
	bph.dwSize = (DWORD)file.SeekToEnd();
	wCRC1 = crc16(wCRC1,(BYTE*)&bph,sizeof(bph) -sizeof(DWORD));
	//CONVERT_SHORT(wCRC1,bph.wCRC1);
	//CONVERT_SHORT(wCRC2,bph.wCRC2);	
	bph.wCRC1 = wCRC1;
	bph.wCRC2 = wCRC2;

	file.SeekToBegin();
	file.Write(&bph,sizeof(bph));
	file.Flush();

	return TRUE;	
}
/** Unpacket the packet to bin files into one specified directory 
  *
  * @param lpszReleaseDirPath: the directory to release bin files
  *                            If it equals NULL, program will create the directory to system temp directory.
  *                            Suggest to set it null.
  * @param bph: store the packet header
  * @param ppFileArray: store the bin files information into FILE_T struct
  *                     it must be release by the "ReleaseMem" function
  * @return: true,if unpacket successfully;false,otherwise
  */
BOOL CBinPack::Unpacket(LPCTSTR lpszReleaseDirPath,BIN_PACKET_HEADER_T &bph,FILE_T ** ppFileArray)
{
	CWaitCursor wait;
	_ASSERTE(ppFileArray != NULL);

	if(ppFileArray == NULL)
	{
		g_theApp.MessageBox(_T("[CBinPacket]Invalidate params"));
		return FALSE;
	}

	if(m_hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	*ppFileArray = NULL;

	DWORD nPacketSize = (DWORD)GetFileSize(m_hFile,NULL);

	//check packet size, it must large than the header struct
    if( nPacketSize < (DWORD)sizeof(BIN_PACKET_HEADER_T) )
	{
		g_theApp.MessageBox(_T("Bin packet's size is too small,maybe it has been destructed!"));
		return FALSE;
	}

	UINT uReadSize = 0;
#if _MSC_VER < 1300  //VC 6.0
	CFile file((int)m_hFile);
#else
	CFile file(m_hFile);
#endif 

	WORD wCRC1 = 0;
	WORD wCRC2 = 0;

	uReadSize = file.Read(&bph,sizeof(BIN_PACKET_HEADER_T));

	BOOL bCRC = FALSE;

	if(bph.dwMagic == PAC_MAGIC)
	{
		bCRC = TRUE;
		wCRC1 = crc16(wCRC1,(BYTE*)&bph,sizeof(bph)-sizeof(WORD));
		if(wCRC1 != 0)
		{
			g_theApp.MessageBox(_T("CRC Error!\nPAC file may be damaged!"));
			return FALSE;
		}
	}

#ifdef _FACTORY
    bph.dwIsNvBackup = FALSE; //The Backup NV Function should be disable in Production Line.
#endif
	
	CString strPrdName = bph.szPrdName;
	if(strPrdName.Find(_T("PAC_")) != 0)
	{
		strPrdName.Insert(0,_T("PAC_"));
		_tcscpy(bph.szPrdName,strPrdName);
	}


	// check packet version
	if( uReadSize !=sizeof(BIN_PACKET_HEADER_T) 
		|| _tcscmp(bph.szVersion, m_szVersion)!=0 )
	{
		g_theApp.MessageBox(_T("Bin packet version is not support!"));
		return FALSE;
	}

	// check packet size recorded by itself
	if(bph.dwSize != (DWORD)nPacketSize)
	{
		g_theApp.MessageBox(_T("Bin packet's size is not correct,maybe it has been destructed!"));
		return FALSE;
	}

	// check the number of files packeted by the packet
	if(bph.nFileCount == 0)
	{
		g_theApp.MessageBox(_T("There is no files in packet!"));
		return FALSE;	
	}

	CString strDisk;
//[[ create temp download file directory
	CString strReleaseDir;
	if(lpszReleaseDirPath != NULL && _tcslen(lpszReleaseDirPath)>=2)
	{
		CString strTemp = lpszReleaseDirPath;
		strTemp.TrimRight(_T('\\'));
		m_strReleaseDir.Format(_T("%s\\_DownloadFiles%d"),strTemp.operator LPCTSTR(),::GetTickCount());	
		strDisk = strTemp.Left(2); 
	}
	else
	{
		_TCHAR szFilePath[_MAX_PATH] = {0};
		if(::GetTempPath(_MAX_PATH,szFilePath)==0)
		{
			g_theApp.MessageBox(_T("Can not get temp path!"));
			return FALSE;
		}
		m_strReleaseDir.Format(_T("%s_DownloadFiles%d"),szFilePath,::GetTickCount());
		
		strDisk = szFilePath;
		strDisk = strDisk.Left(2);
	}	
	
	CString strErr;
	strErr.Format(_T("Extract PAC file failed!\nThe left space of \"%s\" disk maybe not enough!"),strDisk);


	CFileFind find;
	if(find.FindFile(m_strReleaseDir))
	{
		DeleteDirectory(m_strReleaseDir);
	}
	find.Close();

	if(!CreateDirectory(m_strReleaseDir,NULL))
	{
		if( GetLastError() != ERROR_ALREADY_EXISTS )
		{
			_ASSERTE( 0 );  //lint !e506 !e774
			CString strError;
			strError.Format(_T("Can not create directory[%s]"),m_strReleaseDir.operator LPCTSTR());
			g_theApp.MessageBox(strError);
			return FALSE;
		}
	}
	strReleaseDir.Format(_T("%s\\"),m_strReleaseDir.operator LPCTSTR());
//]] create temp download file directory

    uReadSize = 0;
	FILE_T * paFile = new FILE_T[(UINT)bph.nFileCount];
	if(paFile == NULL)
	{		
		delete [] paFile;
		g_theApp.MessageBox(_T("[CBinPaket]Out of memory!"));
		return FALSE;
	}
	memset(paFile,0,sizeof(FILE_T)*((UINT)bph.nFileCount));
	
	uReadSize = file.Read(paFile,(UINT)bph.nFileCount * sizeof(FILE_T));
	if(uReadSize != ((UINT)bph.nFileCount) * sizeof(FILE_T))
	{
		delete [] paFile;
		g_theApp.MessageBox(_T("Read pakcet failed,maybe it has been destructed!"));
		return FALSE;
	}

	if(bCRC)
	{
		wCRC2 = crc16(wCRC2,(BYTE*)paFile,(UINT)bph.nFileCount * sizeof(FILE_T));
	}
    FILE_T * pFT = NULL;
	int nOtherFileNum = 0;

    PostMessageToUplevel(WM_PROG_MSG,PROG_BEGIN,100);
	
	for(int i = 0; i < bph.nFileCount; i++)
	{
        wait.Restore();
		pFT = paFile + i;
		// check file size if validate
		if(pFT->nFileSize == MAXDWORD || (pFT->nFileSize)>=nPacketSize)
		{
			delete [] paFile;
			g_theApp.MessageBox(_T("Read pakcet failed,maybe it has been destructed!"));
			PostMessageToUplevel(WM_PROG_MSG,PROG_END,0);
			return FALSE;
		}

		if(pFT->nFileSize == 0)
		{
#ifdef _FACTORY
			if (pFT->nFileFlag != 0 && pFT->dwCanOmitFlag == 0)  //It needs a file
			{
				m_bIsExistEmptyFile = TRUE;
			}
#endif
			continue;
		}
		CString strFilePath = strReleaseDir;
		strFilePath += pFT->szFileName;

		int n=1;
		while(find.FindFile(strFilePath))
		{
			strFilePath.Format(_T("%s%s(%d)"),strReleaseDir,pFT->szFileName,n);	
			n++;
		}

		memset(pFT->szFileName,0,sizeof(pFT->szFileName));

		_tcscpy(pFT->szFileName,strFilePath);

		if(i == bph.nFileCount-1 )
		{
			m_strCfgPath = strFilePath;
		}
		
		// other files,such as UDISK_IMG 
		if(_tcslen(pFT->szFileID)==0)
		{
			nOtherFileNum++;
		}

		HANDLE fh;
		DWORD dwWritten = 0;
		
		fh = CreateFile(strFilePath, GENERIC_WRITE,0, NULL, CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);        
		
		if (fh == INVALID_HANDLE_VALUE) 
		{	
			delete [] paFile;

			g_theApp.MessageBox(strErr);
			PostMessageToUplevel(WM_PROG_MSG,PROG_END,0);
			return FALSE;
		}

		DWORD  dwLeft = (DWORD)(pFT->nFileSize);
		DWORD  dwRWSize = MAX_RW_SIZE;
		LPBYTE pBuf = new BYTE[MAX_RW_SIZE];
		BOOL   bFlag = FALSE;

		while(dwLeft>0)
		{
            wait.Restore();
			PostMessageToUplevel(WM_PROG_MSG,PROG_PROCEED,(UINT)((100 * file.GetPosition())/nPacketSize));

			memset(pBuf,0,MAX_RW_SIZE);
			if(dwLeft<MAX_RW_SIZE)
			{
				dwRWSize = dwLeft;
			}
			
			file.Read(pBuf,dwRWSize);
			if(bCRC)
			{
				wCRC2 = crc16(wCRC2,(BYTE*)pBuf,dwRWSize);
			}

			if(i == bph.nFileCount-1 )
			{
				if(dwRWSize == dwLeft)
				{
					if(pBuf[dwRWSize-1] == 0)
					{
						dwRWSize--;
						bFlag = TRUE;
					}
				}
			}

			dwWritten = 0;
			BOOL bOk = WriteFile(fh, pBuf, dwRWSize, &dwWritten, NULL); 			
			if(!bOk || dwWritten != dwRWSize)
			{	
				CloseHandle(fh);
				delete [] paFile;
				delete [] pBuf;
				g_theApp.MessageBox(strErr);
				PostMessageToUplevel(WM_PROG_MSG,PROG_END,0);
				return FALSE;
			}

			if(bFlag )
			{				
				dwRWSize++;			
			}

			dwLeft -= dwRWSize;
		}

		delete [] pBuf;
		CloseHandle(fh);
	}

	PostMessageToUplevel(WM_PROG_MSG,PROG_END,100);

	bph.nFileCount = bph.nFileCount-nOtherFileNum;

	*ppFileArray = new FILE_T[(UINT)bph.nFileCount];
	memcpy(*ppFileArray,paFile,sizeof(FILE_T)*(UINT)bph.nFileCount);
	delete [] paFile;

	if(bCRC)
	{
		wCRC2 = crc16(wCRC2,(BYTE*)&bph.wCRC2,sizeof(WORD));
		if(wCRC2 != 0)
		{	
			g_theApp.MessageBox(_T("CRC Error!\nPAC file may be damaged!"));
			return FALSE;
		}
	}
	
	return TRUE;
}

/** Close the opened file
  * You can use it or not, for it will be invoked in the destruct function
  *
  */
void CBinPack::Close()
{
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	return;
}

/** Release the memory newed by Unpacket function
  *
  * @param paFile: point to FILE_T buffer
  */
void CBinPack::ReleaseMem(FILE_T * paFile)
{
	if(paFile != NULL)
	{
		delete [] paFile;
	}
}

/** Delete the directory,all its sub directories and files
  *
  * @param paFile: the directory name
  */
BOOL CBinPack::DeleteDirectory(LPCTSTR lpszDirName)// DeleteDirectory(_T("c:\\aaa"))
{
	_ASSERTE(lpszDirName != NULL);
	if(lpszDirName== NULL)
		return TRUE;

	CFileFind tempFind;
	BOOL IsFinded = TRUE;
	IsFinded = (BOOL)tempFind.FindFile(lpszDirName);
	if(!IsFinded)
	{
		tempFind.Close();
		return TRUE;
	}
	tempFind.Close();

    _TCHAR tempFileFind[MAX_PATH] = {0};
	_stprintf(tempFileFind,_T("%s\\*.*"),lpszDirName);
    IsFinded=(BOOL)tempFind.FindFile(tempFileFind);
    while(IsFinded)
    {
        IsFinded=(BOOL)tempFind.FindNextFile();
        if(!tempFind.IsDots())
        {
            _TCHAR foundFileName[MAX_PATH] = {0};
            _tcscpy(foundFileName,tempFind.GetFileName().GetBuffer(MAX_PATH));
            if(tempFind.IsDirectory())
            {
                _TCHAR tempDir[MAX_PATH] = {0};
                _stprintf(tempDir,_T("%s\\%s"),lpszDirName,foundFileName);
                DeleteDirectory(tempDir);
            }
            else
            {
                _TCHAR tempFileName[MAX_PATH] = {0};
                _stprintf(tempFileName,_T("%s\\%s"),lpszDirName,foundFileName);
                DeleteFile(tempFileName);
            }
        }
    }
    tempFind.Close();
    if(!RemoveDirectory(lpszDirName))
    {
		//CString strError;
		//strError.Format(_T("Delete directory [%s] faild!"),lpszDirName);        
        //return FALSE;
		TRACE(_T("Delete directory [%s] faild!"),lpszDirName);
    }
    return TRUE;
}
/** Get released directory
  *
  * @return: the path of released path
  */
CString CBinPack::GetReleaseDir()
{
	return m_strReleaseDir;
}
/**  Remove released directory
  *
  * @param lpszDir: directory path
  * @return: true,if remove successful; false,otherwise
  */
BOOL CBinPack::RemoveReleaseDir(LPCTSTR lpszDir)
{
	if(lpszDir == NULL)
		return TRUE;
	
	return DeleteDirectory(lpszDir);
}

/** Remove released directory
  *
  * @return: true,if remove successful; false,otherwise
  */
BOOL CBinPack::RemoveReleaseDir()
{
	return DeleteDirectory(m_strReleaseDir);
}

/** Load configure file (*.xml)
  *
  * @param lspszCfgFile: name of configure file 
  * @param lpszPrdName: name of product
  * @return: true,if load successfully; false,otherwise
  */
BOOL CBinPack::LoadConfig(LPCTSTR lspszCfgFile,LPCTSTR lpszPrdName)
{
#ifndef _lint
/*lint -save -e628 */
	::CoInitialize(NULL);

	HRESULT	hr;
	hr=m_pXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if(FAILED(hr))
	{  
		TRACE(_T("Can't create DOMDocument object, please check if fixed the MS XML Parser runtime lib!\n"));
		return FALSE;
	}		
	//Load File 
	m_pXMLDoc->put_validateOnParse(FALSE);
	if(!m_pXMLDoc->load(lspszCfgFile))
	{
		TRACE(_T("load file fail\n"));
		return FALSE;
	}
	MSXML2::IXMLDOMNodePtr		pPrdNode;

	CString strPrdSel;
	strPrdSel.Format(_T("/BMAConfig/ProductList/Product[@name=\"%s\"]"),lpszPrdName);
	pPrdNode = m_pXMLDoc->selectSingleNode((_bstr_t)strPrdSel);
	if(pPrdNode == NULL)
		return FALSE;

	MSXML2::IXMLDOMNodeListPtr pChilNodes = NULL;
	MSXML2::IXMLDOMNodeListPtr pSubChilNodes = NULL;

    _variant_t	vtValue ;
	CString strNodeValue;
	
	CString strSNSel;
	strSNSel.Format(_T("/BMAConfig/ProductList/Product[@name=\"%s\"]/SchemeName"),lpszPrdName);
    MSXML2::IXMLDOMNodePtr		pSNNode;
	pSNNode = m_pXMLDoc->selectSingleNode((_bstr_t)strSNSel);
	if(pSNNode == NULL)
		return FALSE;
	pSNNode->get_nodeTypedValue(&vtValue);
	strNodeValue = (_TCHAR *)(_bstr_t)vtValue;

	CString strScmSel;
	strScmSel.Format(_T("/BMAConfig/SchemeList/Scheme[@name=\"%s\"]/File"),strNodeValue.operator LPCTSTR());
	m_pFileNodeList = m_pXMLDoc->selectNodes((_bstr_t)strScmSel);
	if(m_pFileNodeList == NULL)
		return FALSE;
#endif //_lint	
	return TRUE;
/*lint -restore */
}

/** Get file item count
  *
  * @return: the number of files in the product
  */
int CBinPack::GetFileCount()
{
	int nCount = 0;
#ifndef _lint
/*lint -save -e628 -e1055 -e746*/
	nCount = (int)(m_pFileNodeList->Getlength());
/*lint -restore */
#endif //_lint
	return nCount;
}

/** Get file item count
  *
  * @param tFileInfo: to store the file information
  * @param index: the index of file in files 
  * @return: the number of files in the product
  */
BOOL CBinPack::GetFileInfo(_X_FILE_INFO_T &tFileInfo,int index)
{
#ifndef _lint
/*lint -save -e628 */
	if(m_pFileNodeList == NULL)
		return FALSE;
	
	long lCount = m_pFileNodeList->Getlength();  // the number of files
	if(lCount == 0)    // the number of files equal zero
		return FALSE;

	if((long)index >= lCount) // invalidate index
		return FALSE;
    
	memset(&tFileInfo,0,sizeof(tFileInfo)); //zero the struct

	MSXML2::IXMLDOMNodeListPtr pFileItemNodes = NULL;
	MSXML2::IXMLDOMNodePtr pFileItemNode = NULL;
	MSXML2::IXMLDOMNodeListPtr pFileSubItemNodes = NULL;
	MSXML2::IXMLDOMNodePtr pFileSubItemNode = NULL;
    MSXML2::IXMLDOMNodePtr pFileNode = NULL;
	
	pFileNode = m_pFileNodeList->item[index];
	
	pFileItemNodes = pFileNode->GetchildNodes();
    _variant_t	vtValue;
	CString strNodeValue;
	_bstr_t bstrNodeName;
	CString strNodeName;

	long cItem = pFileItemNodes->Getlength();
	int nBlockCout = 0;
	for(long i = 0; i<cItem; i++)
	{
		pFileItemNode = pFileItemNodes->nextNode();
		bstrNodeName = pFileItemNode->GetnodeName();
		vtValue = pFileItemNode->GetnodeTypedValue();

		strNodeName = (TCHAR *)bstrNodeName;
		strNodeValue = vtValue.bstrVal;
		if(strNodeName.Compare(_T("ID"))==0)
		{
			_tcscpy(tFileInfo.szID,strNodeValue);
		}
		else if(strNodeName.Compare(_T("Type"))==0)
		{
			_tcscpy(tFileInfo.szType,strNodeValue);
		}
		else if(strNodeName.Compare(_T("Block"))==0)
		{
			if(nBlockCout > _MAX_BLOCK_NUM) // max is 5
				continue;

			pFileSubItemNodes = pFileItemNode->GetchildNodes();
			_variant_t	vtSubValue ;	
			CString strSubNodeValue;
			_bstr_t bstrSubNodeName;
			CString strSubNodeName;
			for(long j=0; j<pFileSubItemNodes->Getlength(); j++)
			{
				pFileSubItemNode = pFileSubItemNodes->nextNode();
				bstrSubNodeName = pFileSubItemNode->GetnodeName();
				vtSubValue = pFileSubItemNode->GetnodeTypedValue();
				strSubNodeName = (TCHAR *)bstrSubNodeName;
		        strSubNodeValue = vtSubValue.bstrVal;
				if(strSubNodeName.Compare(_T("Base"))==0)
				{
					_stscanf(strSubNodeValue,_T("0x%x"),&(tFileInfo.arrBlock[nBlockCout].dwBase)); //lint !e662
				}
				else if(strSubNodeName.Compare(_T("Size"))==0)
				{
					_stscanf(strSubNodeValue,_T("0x%x"),&(tFileInfo.arrBlock[nBlockCout].dwSize)); //lint !e662
				}
			}
			nBlockCout++;
		}
		else if(strNodeName.Compare(_T("Flag"))==0)
		{
			_stscanf(strNodeValue,_T("%d"),&(tFileInfo.dwFlag));
		}
		else if(strNodeName.Compare(_T("CheckFlag"))==0)
		{
			_stscanf(strNodeValue,_T("%d"),&(tFileInfo.dwCheckFlag));
		}
	}

	tFileInfo.dwBlockCount = nBlockCout;

//	CString str;
//	str.Format(_T("ID:%s\nType:%s\nBase:0x%X\nSize:0x%x\nFlag:%d\nCheckFlag:%d\n"),
//		tFileInfo.szID,
//		tFileInfo.szType,
//		tFileInfo.arrBlock[0].dwBase,
//		tFileInfo.arrBlock[0].dwSize,
//		tFileInfo.dwFlag,
//		tFileInfo.dwCheckFlag);
//	AfxMessageBox(str);

/*lint -restore */

#endif //_lint
	return TRUE;
/*lint -save -e1764*/
}/*lint -restore */

BOOL CBinPack::GetSpcPrdConfig(LPCTSTR lpszCfgFile,LPCTSTR lpszPrdName,BOOL bNVBackup,
							   _X_NV_BACKUP_ITEM_T *pnbi, int nNBICount,LPBYTE *ppbText, DWORD &nCount)
{
#ifndef _lint
/*lint -save -e628 */
	::CoInitialize(NULL);

	HRESULT	hr;
	hr=m_pXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if(FAILED(hr))
	{  
		TRACE(_T("Can't create DOMDocument object, please check if fixed the MS XML Parser runtime lib!\n"));
		return FALSE;
	}		
	//Load File 
	m_pXMLDoc->put_validateOnParse(FALSE);
	if(!m_pXMLDoc->load(lpszCfgFile))
	{	
		TRACE(_T("load file fail\n"));
		return FALSE;
	}
	MSXML2::IXMLDOMNodePtr		pPrdNode;
	MSXML2::IXMLDOMNodePtr		pScmNode;
	CString strPrdSel;
	strPrdSel.Format(_T("/BMAConfig/ProductList/Product[@name=\"%s\"]"),lpszPrdName);
	pPrdNode = m_pXMLDoc->selectSingleNode((_bstr_t)strPrdSel);
	if(pPrdNode == NULL)
		return FALSE;

	MSXML2::IXMLDOMNodeListPtr pChilNodes = NULL;
	MSXML2::IXMLDOMNodeListPtr pSubChilNodes = NULL;


    _variant_t	vtValue ;
	_bstr_t bstrNodeValue;
	CString strNodeValue;
	
	CString strSNSel;
	strSNSel.Format(_T("/BMAConfig/ProductList/Product[@name=\"%s\"]/SchemeName"),lpszPrdName);
    MSXML2::IXMLDOMNodePtr		pSNNode;
	pSNNode = m_pXMLDoc->selectSingleNode((_bstr_t)strSNSel);
	if(pSNNode == NULL)
		return FALSE;

	pSNNode->get_nodeTypedValue(&vtValue);
	strNodeValue = (_TCHAR *)(_bstr_t)vtValue;

	CString strScmSel;
	strScmSel.Format(_T("/BMAConfig/SchemeList/Scheme[@name=\"%s\"]"),strNodeValue.operator LPCTSTR());
	pScmNode = m_pXMLDoc->selectSingleNode((_bstr_t)strScmSel);
    if(pScmNode == NULL)
		return FALSE;

	MSXML2::IXMLDOMDocumentPtr  pXMLDoc;
    pXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	MSXML2::IXMLDOMElementPtr   pRootElement,pPrdList,pScmList;
	pRootElement = pXMLDoc->createElement((_bstr_t)_T("BMAConfig"));

	pXMLDoc->appendChild(pRootElement);
	
	pPrdList = pXMLDoc->createElement((_bstr_t)_T("ProductList"));

	pRootElement->appendChild(pPrdList);

	pScmList = pXMLDoc->createElement((_bstr_t)_T("SchemeList"));

	pRootElement->appendChild(pScmList);

	pPrdList->appendChild(pPrdNode);
	pScmList->appendChild(pScmNode);

    MSXML2::IXMLDOMElementPtr pElement;
	MSXML2::IXMLDOMElementPtr pSubElement;
	if(!bNVBackup)
	{
		pElement = pXMLDoc->selectSingleNode(_T("//NVBackup"));
		if(pElement != NULL)
		     pElement->setAttribute(_T("backup"),(_variant_t)_T("0"));
	}
	else
	{
        _X_NV_BACKUP_ITEM_T *pnbit = NULL;
		CString strAttValue;
		for(long i=0;i<(long)nNBICount; i++)
		{	
			pnbit = pnbi+i;
			CString strSel;			
			strSel.Format(_T("//NVItem[@name=\"%s\"]"),pnbit->szItemName);
			pElement = pXMLDoc->selectSingleNode((_bstr_t)strSel);
			if(pElement != NULL)
			{
				strAttValue.Format(_T("%d"),pnbit->wIsBackup);
				pElement->setAttribute(_T("backup"),(_variant_t)((LPCTSTR)strAttValue));
                if(pnbit->wIsBackup == 1 && pnbit->wIsUseFlag == 1)
				{
					for( int j = 0; j< (int)(pnbit->dwFlagCount); j++)
					{
						strSel.Format(_T("//NVItem[@name=\"%s\"]/BackupFlag/NVFlag[@name=\"%s\"]"),pnbit->szItemName,pnbit->nbftArray[j].szFlagName);
					    pSubElement = pXMLDoc->selectSingleNode((_bstr_t)strSel);
						if(pSubElement != NULL)
						{
							strAttValue.Format(_T("%d"),pnbit->nbftArray[j].dwCheck);
				            pSubElement->setAttribute(_T("check"),(_variant_t)(strAttValue.operator LPCTSTR()));//lint !e530
						}
					}					
				}
			}
		}
	}

	_bstr_t bstrText = pXMLDoc->xml;

	LPTSTR str = (_TCHAR *)bstrText;
    nCount = (DWORD)_tcslen(str); //omit '\0'
    *ppbText = new BYTE[(UINT)nCount*2];
	memset(*ppbText,0,nCount*2);
	nCount = WideCharToMultiByte(CP_ACP,0,str,nCount,(char *)*ppbText,nCount*2,NULL,NULL);
	if(nCount > 0 && (*ppbText) != NULL && (*ppbText)[nCount-1] == '\0')
	{
		nCount--;
	}

	pXMLDoc.Release();
	
#endif //_lint

	return TRUE;
/*lint -restore */
/*lint -save -e1764*/
}/*lint -restore */


BOOL CBinPack::GetSpcPrdConfig2(LPCTSTR lpszCfgFile,LPBYTE *ppbText, DWORD &nCount)
{
    
    FILE*   pFile   = _tfopen(lpszCfgFile,_T("rb"));
    LPBYTE  pBuf    = NULL;
    DWORD   dwSize  = 0;

    if(pFile == NULL)
        return FALSE;
    fseek(pFile,0,SEEK_END);
    dwSize = ftell(pFile);
    fseek(pFile,0,SEEK_SET);

    pBuf = new BYTE[dwSize];
    if(pBuf == NULL)
        return FALSE;

    DWORD dwRead = fread(pBuf,1,dwSize,pFile);
    fclose(pFile);

    if(dwRead != dwSize)
    {
        SAFE_DELETE_ARRAY(pBuf);
        return FALSE;
    }

    *ppbText = pBuf;
    nCount   = dwSize;
    return TRUE;
}
LPCTSTR CBinPack::GetConfigFilePath()
{
	return m_strCfgPath;
}

void CBinPack::PostMessageToUplevel(DWORD dwMsgID, WPARAM wParam, LPARAM lParam  )
{

    if( (m_hRcvWindow == NULL) && ( m_dwRcvThreadID == 0) )
    {
        return;
    }

    if ( m_bRcvThread )  
    {
        if( m_dwRcvThreadID != 0 )
        {
            PUMP_MESSAGES();
            PostThreadMessage( m_dwRcvThreadID,  dwMsgID, wParam , lParam );   
        }
    }
    else 
    {
        if( m_hRcvWindow != NULL )
        {
            PUMP_MESSAGES();
            PostMessage( m_hRcvWindow, dwMsgID, wParam , lParam );            
        }
    }
}