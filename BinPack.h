// BinPack.h: interface for the CBinPack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINPACK_H__8A419B31_D187_415E_A4B6_9F8E1E15AE69__INCLUDED_)
#define AFX_BINPACK_H__8A419B31_D187_415E_A4B6_9F8E1E15AE69__INCLUDED_

//lint ++flb

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import <msxml2.dll>

#define _MAX_BLOCK_NUM   5
#define _MAX_NV_BACKUP_FALG_NUM 5
#define PAC_MAGIC       (0xFFFAFFFA)
#define ZIP_MAGIC		(0x04034B50)
#define MAX_UNZIP_BUF   (0X10000)       //64K


#define PUMP_MESSAGES()  \
    do \
{\
    MSG msg;\
    while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))\
    {\
        TranslateMessage(&msg);\
        DispatchMessage(&msg);\
    }\
} while(0) 

/************************************************************************/
/* File address infomation                                              */
/************************************************************************/
struct _X_BLOCK_T
{
	_X_BLOCK_T()
    {
        memset(this,0,sizeof(_X_BLOCK_T));
    }
	
	DWORD dwBase;
	DWORD dwSize;
};

/************************************************************************/
/* File infomation                                                      */
/************************************************************************/
struct _X_FILE_INFO_T
{
	_X_FILE_INFO_T()
    {
        memset(this,0,sizeof(_X_FILE_INFO_T));
    }
	
	_TCHAR		szID[MAX_PATH];
	_TCHAR		szType[MAX_PATH];
	DWORD		dwFlag;
	_X_BLOCK_T	arrBlock[_MAX_BLOCK_NUM];
	DWORD		dwBlockCount;
	DWORD       dwCheckFlag;  	
};

struct _X_NV_BACKUP_FLAG_T
{
	_TCHAR szFlagName[MAX_PATH];
	DWORD  dwCheck;
};

struct _X_NV_BACKUP_ITEM_T
{
	_X_NV_BACKUP_ITEM_T()
    {
        memset(this,0,sizeof(_X_NV_BACKUP_ITEM_T));
    }
	_TCHAR szItemName[MAX_PATH];
	WORD   wIsBackup;
	WORD   wIsUseFlag;	
	DWORD  dwID;	
	DWORD  dwFlagCount;
	_X_NV_BACKUP_FLAG_T nbftArray[_MAX_NV_BACKUP_FALG_NUM];
};
/************************************************************************/
/* FILE_T struct storing file information                               */
/************************************************************************/

typedef struct _FILE_T
{
	_FILE_T()
	{
		memset(this,0,sizeof(_FILE_T));
		dwSize = sizeof(_FILE_T);
	}
	
    DWORD  dwSize;				// size of this struct itself
	_TCHAR szFileID[256];		// file ID,such as FDL,Fdl2,NV and etc.
	_TCHAR szFileName[256];     // file name,in the packet bin file,it only stores file name
	                            // but after unpacketing, it stores the full path of bin file
	_TCHAR szFileVersion[256];  // Reserved now
	DWORD  nFileSize;           // file size
	int    nFileFlag;           // if "0", means that it need not a file, and 
	                            // it is only an operation or a list of operations, such as file ID is "FLASH"
	                            // if "1", means that it need a file
	DWORD  nCheckFlag;          // if "1", this file must be downloaded; 
	                            // if "0", this file can not be downloaded;										
	DWORD  dwDataOffset;        // the offset from the packet file header to this file data
	DWORD  dwCanOmitFlag;		// if "1", this file can not be downloaded and not check it as "All files" 
								//   in download and spupgrade tool.
	DWORD  dwAddrNum;
	DWORD  dwAddr[5];
	DWORD  dwReserved[249];     // Reserved for future,not used now
}FILE_T/*, *PFILE_T*/;

/************************************************************************/
/* BIN_PACKET_HEADER_T struct storing packet header information         */
/************************************************************************/
typedef struct _BIN_PACKET_HEADER_T
{
	_BIN_PACKET_HEADER_T()
	{
		memset(this,0,sizeof(_BIN_PACKET_HEADER_T));
		dwMagic = PAC_MAGIC;
	}
	_TCHAR szVersion[24];		// packet struct version
	DWORD  dwSize;              // the whole packet size;
	_TCHAR szPrdName[256];		// product name
	_TCHAR szPrdVersion[256];   // product version
	int    nFileCount;          // the number of files that will be downloaded, the file may be an operation
	DWORD  dwFileOffset;		// the offset from the packet file header to the array of FILE_T struct buffer
	DWORD  dwMode;
	DWORD  dwFlashType;
	DWORD  dwNandStrategy;
	DWORD  dwIsNvBackup;
	DWORD  dwNandPageType;
	_TCHAR szPrdAlias[100];    // product alias
	DWORD  dwOmaDmProductFlag;
	DWORD  dwIsOmaDM;
	DWORD  dwIsPreload;
    DWORD  dwReserved[200];
	DWORD  dwMagic;
	WORD   wCRC1;
	WORD   wCRC2;
}BIN_PACKET_HEADER_T,*PBIN_PACKET_HEADER_T;

/************************************************************************/
/* Class CBinPack, it packets the bin files and unpackets the packet    */
/************************************************************************/
class CBinPack  
{
public:
	CBinPack(BOOL bZipPac =FALSE);
	virtual ~CBinPack();

public:
    void Initialize(BOOL bRcvThread, LPCVOID pReceiver);
/** Open bin packet file for read or write
  *
  * @param lpszFileName: packet file name
  * @param bOpenFlag: if true, open for read, otherwise open for write
  * @return: true,if open successfully;false,otherwise
  */
	BOOL Open(LPCTSTR lpszFileName, BOOL bOpenFlag = FALSE);

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
	BOOL Packet(const PBIN_PACKET_HEADER_T pbph,LPCTSTR lpszCfgFile, FILE_T * pFileArray, 
			   BOOL bNVBackup,_X_NV_BACKUP_ITEM_T *pnbi,int nNBICount,int nFlag);
    BOOL Packet2(const PBIN_PACKET_HEADER_T pbph,LPCTSTR lpszCfgFile, FILE_T * pFileArray);

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
	BOOL Unpacket(LPCTSTR lpszReleaseDirPath,BIN_PACKET_HEADER_T &bph,FILE_T ** ppFileArray);

/** Close the opened file
  * You can use it or not, for it will be invoked in the destruct function
  *
  */
	void Close();

/** Release the memory newed by Unpacket function
  *
  * @param paFile: point to FILE_T buffer
  */
	void ReleaseMem(FILE_T * paFile);

/** Get released directory
  *
  * @return: the path of released path
  */
	CString GetReleaseDir();

/** Remove released directory
  *
  * @param lpszDir: directory path
  * @return: true,if remove successful; false,otherwise
  */
	BOOL RemoveReleaseDir(LPCTSTR lpszDir);

/** Remove released directory
  *
  * @return: true,if remove successful; false,otherwise
  */
	BOOL RemoveReleaseDir();

/** Get file item count
  *
  * @return: the number of files in the product
  */
	int   GetFileCount();

/** Get file item information
  *
  * @param tFileInfo: to store the file information
  * @param index: the index of file in files 
  * @return: the number of files in the product
  */
	BOOL  GetFileInfo(_X_FILE_INFO_T &tFileInfo,int index);

/** Get xml configure file path
  *
  * @return: the path of xml configure
  */
   LPCTSTR GetConfigFilePath();

/*  Check If there is empty file in pac files
 *
 *  @Return: TRUE: Exist more the one empty file;
 *           FALSE: Doesn't exist empty file;
   */
#ifdef _FACTORY
   BOOL IsExistEmptyFile(){ 
	   return m_bIsExistEmptyFile; 
   };
#endif

/*
protected:
	static DWORD WINAPI  GetThreadFunc(LPVOID lpParam);	
	DWORD LoadPacFunc();
	CString m_strPacDir;
	BIN_PACKET_HEADER_T *m_pBph;
	FILE_T ** m_ppFileArray;
	BOOL      m_bOK;
*/
private:

/** Delete the directory,all its sub directories and files
  *
  * @param paFile: the directory name
  */
	BOOL DeleteDirectory(LPCTSTR lpszDirName);

/** Get special product configure from configure files
  *
  * @param lpszSrcCfgFile: configure file name(*.xml)
  * @param lpszPrdName: name of product
  * @param ppbText: to store the xml text 
  * @param nCount: number of bytes in ppbText
  * @return: true,if get successfully; false,otherwise
  */	
    BOOL GetSpcPrdConfig(LPCTSTR lpszCfgFile,LPCTSTR lpszPrdName,BOOL bNVBackup,_X_NV_BACKUP_ITEM_T *pnbi,
							   int nNBICount,LPBYTE *ppbText, DWORD &nCount);

    BOOL GetSpcPrdConfig2(LPCTSTR lpszCfgFile,LPBYTE *ppbText, DWORD &nCount);

/** Load configure file (*.xml)
  *
  * @param lspszCfgFile: name of configure file 
  * @param lpszPrdName: name of product
  * @return: true,if load successfully; false,otherwise
  */
	BOOL  LoadConfig(LPCTSTR lspszCfgFile,LPCTSTR lpszPrdName);

    void PostMessageToUplevel(DWORD dwMsgID, WPARAM wParam, LPARAM lParam );
private:
	HANDLE m_hFile;  // file handle opened for read or write

	/*lint -save -e1001 -e601 */
	MSXML2::IXMLDOMDocumentPtr  m_pXMLDoc;       // xml document
	MSXML2::IXMLDOMNodeListPtr  m_pFileNodeList; // file list
	/*lint -restore */

	static const _TCHAR m_szVersion[24]; // the version of packet struct itself
	
	CString m_strReleaseDir;
	CString m_strCfgPath;
	
    CString 		m_strPacFile;
    BOOL    		m_bZipPac;
    BOOL            m_bRcvThread;
    DWORD		    m_dwRcvThreadID;  // ID of the up-level receiving thread    
    HWND		    m_hRcvWindow;     // Handle of the up-level receiving window    
	
#ifdef _FACTORY
	BOOL m_bIsExistEmptyFile;
#endif

};
//lint --flb
#endif // !defined(AFX_BINPACK_H__8A419B31_D187_415E_A4B6_9F8E1E15AE69__INCLUDED_)
