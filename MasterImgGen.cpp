#include "stdafx.h"
#include "MasterImgGen.h"
#include "BootParamDef.h"
#include <io.h>

#define SECTOR_SIZE			512
static const unsigned char MAGIC[] = { 'S', 'T', 'B', 'P' };

#define NEW_ARRAY( p, type, size ) \
{                                  \
	(p) = new type [ size ];       \
	if ( NULL == (p) )             \
		__leave;                   \
}

#define DELETE_ARRAY( p ) \
	if ( NULL != (p) ) {  \
		delete[] (p);     \
		(p) = NULL;       \
	} 

#define CLOSE_FILE( f )  \
	if ( NULL != (f) ) { \
		::fclose( f );   \
		(f) = NULL;      \
	}



void *CMasterImgGen::MakeMasterImage(DWORD *pImageSize,
									 int NumOfImages,
									 IMAGE_PARAM * lpImageParam,
									 int PageType)
{
	ASSERT( NULL != pImageSize );
	ASSERT( NumOfImages > 0 );
	ASSERT( NULL != lpImageParam );
	ASSERT((SMALL_PAGE == PageType) || (LARGE_PAGE == PageType));
	if(pImageSize == NULL || NumOfImages == 0 || lpImageParam == NULL)
	{
		return NULL;
	}

	void * pImage = NULL;
	FILE * pMaster = NULL;
	m_SctsPerBlock = (SMALL_PAGE == PageType) ? 32 : 256;

	TCHAR szPath[MAX_PATH] = {0};

	/* @hongliang.xin 2009-9-2 modify the temp file location */
	LPTSTR lpszPath = _tgetenv(_T("tmp"));
	_stprintf(szPath,_T("%s\\tmp%d"),lpszPath,GetTickCount());
	
	__try {

		/* @hongliang.xin 2009-9-2 modify the temp file location */
		pMaster = ::_tfopen(szPath,_T("w+b"));

		//noticed by hongliang.xin 2009-9-2
		//pMaster = ::tmpfile();

		if ( NULL == pMaster )
			__leave;

		if ( !CalcImageOffset( NumOfImages, lpImageParam ) )
			__leave;

		if (!AddBootParam( pMaster, NumOfImages, lpImageParam ) )
			__leave;

		for ( int i=0; i<NumOfImages; ++i )
			if ( !MakeImage( pMaster, lpImageParam + i ) ) //lint !e613
				__leave;

		VERIFY( 0 == ::fflush( pMaster ) );
		*pImageSize = ::_filelength( _fileno( pMaster ) );
		pImage = new unsigned char[*pImageSize];
		if (NULL == pImage)
			__leave;

		VERIFY(0 == ::fseek(pMaster, 0, SEEK_SET));
		VERIFY(*pImageSize == (int)::fread(pImage, 1, *pImageSize, pMaster));
	} __finally {
		CLOSE_FILE( pMaster );
		/*@hongliang.xin 2009-9-2 */
		::DeleteFile(szPath);
	}

	return pImage;
}

BOOL CMasterImgGen::CalcImageOffset( int NumOfImages,
		                             IMAGE_PARAM * lpImageParam ) 
{
	ASSERT( NumOfImages > 0 );
	ASSERT( NULL != lpImageParam );
	if(lpImageParam == NULL || NumOfImages <= 0)
	{
		return FALSE;
	}

	// Boot parameters occupy the first block, skip it.
	int StartSector = m_SctsPerBlock;
	while ( NumOfImages-- > 0 ) {
		FILE * pImage = ::_tfopen( lpImageParam->szPath, _T("rb") );
		if ( NULL == pImage )
			return FALSE;

		// The image data just occupy the main area of a sector.
		int nSectors = ( ::_filelength( _fileno( pImage ) ) + SECTOR_SIZE - 1 ) / SECTOR_SIZE;
		::fclose( pImage );
		pImage = NULL;

		// Every image must start from the boundary of a sector and must
		// occupy space of integral multiple of sectors.
		lpImageParam->Offset = StartSector;   // in sector
		lpImageParam->PaddingSize = nSectors; // in sector

		StartSector += nSectors;
		++lpImageParam;
	}

	return TRUE;
}


BOOL CMasterImgGen::AddBootParam( FILE * pMaster,
		                          int NumOfImages,
					              const IMAGE_PARAM * lpImageParam )
{
	ASSERT( NULL != pMaster );
	ASSERT( NumOfImages > 0 );
	ASSERT( NULL != lpImageParam );
	if(pMaster == NULL || lpImageParam == NULL || NumOfImages ==0 )
	{
		return FALSE;
	}

	ASSERT( 0 == ::fflush( pMaster ) );
	ASSERT( 0 == ::_filelength( _fileno( pMaster ) ) );

	BOOL ret = FALSE;
	unsigned char * pBuf = NULL;
	__try {
		int BufSize = sizeof( BOOT_PARAM ) + NumOfImages * sizeof(OS_INFO);
		NEW_ARRAY( pBuf, unsigned char, BufSize );

		// Initialize fields of boot parameters.
		BOOT_PARAM * pBoot = ( BOOT_PARAM* )pBuf;
		::memmove( pBoot->Magic, MAGIC, sizeof(pBoot->Magic) );
		pBoot->Size = SwapWord( sizeof( BOOT_PARAM ));
		pBoot->TotalSize = SwapWord( ( unsigned short )BufSize );
		::time( (time_t*)&pBoot->TimeStamp );
		pBoot->TimeStamp = SwapDword( pBoot->TimeStamp );
		pBoot->TraceOn = 1;
		pBoot->Reserved = 0;
		pBoot->CurrentOS = ( unsigned char )0;
		pBoot->NumOfOS = ( unsigned char )NumOfImages;
		pBoot->SizeOfOSInfo = SwapWord( sizeof( OS_INFO ));
		pBoot->OSOffset = SwapWord( sizeof( BOOT_PARAM ));

		// Initialize OS information.
		OS_INFO * pOS = ( OS_INFO* )( pBuf + sizeof( BOOT_PARAM ));
		while ( NumOfImages-- > 0 ) {
			::memset( pOS, 0, sizeof(*pOS) );

			ASSERT( ::strlen( lpImageParam->szName ) < sizeof( pOS->Name) );
			ASSERT( ::strlen( lpImageParam->szDescription ) < sizeof( pOS->Description) );
			ASSERT( ::strlen( lpImageParam->szVersion ) < sizeof( pOS->Version) );
			::strcpy( pOS->Name, lpImageParam->szName );
			::strcpy( pOS->Description, lpImageParam->szDescription );
			::strcpy( pOS->Version, lpImageParam->szVersion );
			// All in sectors.
			pOS->Offset = 
				SwapWord( (unsigned short)lpImageParam->Offset ); 
			pOS->Size = 
				SwapWord( (unsigned short)lpImageParam->PaddingSize );

			++pOS;
			++lpImageParam;
		}
		
		ASSERT(0 == m_SctsPerBlock % 2);
		if ( !WriteBootParam( pMaster, pBuf, BufSize, m_SctsPerBlock / 2 ) )
			__leave;

		// Write backup boot parameter block
		if ( !WriteBootParam( pMaster, pBuf, BufSize, m_SctsPerBlock / 2 ))
			__leave;

		ret = TRUE;
	} __finally {
		DELETE_ARRAY( pBuf );
	}

	return ret;
}
	
BOOL CMasterImgGen::WriteBootParam( FILE * pMaster,
		                            const void * pBootParam,
									int ParamSize,
						            int TotalPages )
{
	// Write boot parameter block to Master Image.
	int nPages = 0;
	const unsigned char * pRemain = (const unsigned char*)pBootParam;
	unsigned char * pMBuf = NULL;
	BOOL ret;

	ret = FALSE;
	__try {
		NEW_ARRAY( pMBuf, unsigned char, SECTOR_SIZE );  //lint !e774

	    while ( ParamSize > 0 ) {
			::memset( pMBuf, 0xFF, SECTOR_SIZE );
			int nSize = min( ParamSize, SECTOR_SIZE );			
			::memmove( pMBuf, pRemain, nSize ); //lint !e670

			if ( !WriteMainArea( pMaster, pMBuf ) )
				__leave;

			ParamSize -= nSize;
			pRemain += nSize; //lint !e662
			++nPages;
		}
		ASSERT(nPages <= TotalPages);

		// Padding.
		::memset( pMBuf, 0xFF, SECTOR_SIZE );
		while ( nPages++ < TotalPages ) {
			if ( !WriteMainArea( pMaster, pMBuf ) )
				__leave;
		}
		
		ret = TRUE;
	} __finally {
		DELETE_ARRAY( pMBuf );
	}

	return ret;
}

BOOL CMasterImgGen::MakeImage( FILE * pMaster, 
		                       const IMAGE_PARAM * lpImageParam )
{
	ASSERT( NULL != pMaster );
	ASSERT( NULL != lpImageParam );
	if(pMaster == NULL || lpImageParam == NULL)
	{
		return FALSE;
	}

	ASSERT( 0 == ::fflush( pMaster ) );
	ASSERT( ::_filelength( _fileno( pMaster ) ) 
		== lpImageParam->Offset * SECTOR_SIZE);

	BOOL ret = FALSE;
	unsigned char * pMBuf = NULL;
	FILE * pImage = NULL;
	__try {
		pImage = ::_tfopen( lpImageParam->szPath, _T("rb") );
		if ( NULL == pImage )
			__leave;
		NEW_ARRAY( pMBuf, unsigned char, SECTOR_SIZE ); //lint !e774

		int nPages = 0;
		::memset( pMBuf, 0xFF, SECTOR_SIZE );
		while ( ::fread( pMBuf, sizeof( unsigned char ), SECTOR_SIZE, pImage ) ) {
			if ( !WriteMainArea( pMaster, pMBuf ) )
				__leave;

			++nPages;
			::memset( pMBuf, 0xFF, SECTOR_SIZE );
		}
		ASSERT( 0 == ferror( pImage ) );
		ASSERT( nPages == lpImageParam->PaddingSize );
		
		ret = TRUE;
	} __finally {
		CLOSE_FILE( pImage );
		DELETE_ARRAY( pMBuf );
	}

	return ret;
}


BOOL CMasterImgGen::WriteMainArea( FILE * pMaster,
		                           const unsigned char * pMArea )
{
	ASSERT( NULL != pMaster );
	ASSERT( NULL != pMArea );
	if(pMaster == NULL || pMArea == NULL)
	{
		return FALSE;
	}

	return (SECTOR_SIZE == (int)::fwrite(pMArea, sizeof(unsigned char), SECTOR_SIZE, 
		pMaster));
}

LPBYTE CMasterImgGen::MakeMasterImageHeader(DWORD dwPSSize,DWORD *pdwHdrSize, int page_type )
{

	DWORD		dwSectsPerBlock = 0;
	DWORD		dwHeadSize = 0;
	DWORD		dwBootSize= 0;
	BOOT_PARAM * pBoot = NULL;
	OS_INFO    * pOS= NULL;

	BOOT_PARAM * pBootBak= NULL;
	OS_INFO    * pOSBak= NULL;


	LPBYTE pHeadBuf = NULL;

	dwSectsPerBlock = (SMALL_PAGE == page_type) ? 32 : 256;
	dwBootSize = sizeof(BOOT_PARAM) + sizeof( OS_INFO );

	dwHeadSize = dwSectsPerBlock * SECTOR_SIZE;

	pHeadBuf = new BYTE[dwHeadSize];
	memset(pHeadBuf,0xFF,dwHeadSize);

	//init boot param	
	pBoot = (BOOT_PARAM *)pHeadBuf;
	memset(pBoot,0,sizeof(BOOT_PARAM));
	memcpy( pBoot->Magic, MAGIC, sizeof(pBoot->Magic) );
	pBoot->Size = SwapWord((WORD)sizeof( BOOT_PARAM ));
	pBoot->TotalSize = SwapWord((WORD)dwBootSize);
	time((time_t*)&pBoot->TimeStamp);
	pBoot->TimeStamp = SwapDword( pBoot->TimeStamp );
	pBoot->TraceOn   = ( BYTE )1;
	pBoot->Reserved  = ( BYTE )0;
	pBoot->CurrentOS = ( BYTE )0;
	pBoot->NumOfOS   = ( BYTE )1;
	pBoot->SizeOfOSInfo =  SwapWord((WORD)sizeof( OS_INFO ));
	pBoot->OSOffset  = SwapWord((WORD)sizeof( BOOT_PARAM ));

	//init OS info
	pOS = (OS_INFO*)(((BYTE *)pHeadBuf) + sizeof(BOOT_PARAM));
	memset(pOS,0,sizeof(OS_INFO));
	pOS->Offset =  SwapWord((WORD)dwSectsPerBlock);
	pOS->Size =  SwapWord((WORD)(( dwPSSize + SECTOR_SIZE - 1 ) / SECTOR_SIZE ));

	pBootBak = (BOOT_PARAM*)(pHeadBuf + dwHeadSize/2);
	pOSBak = (OS_INFO*)( ((BYTE*)(pBootBak)) + sizeof(BOOT_PARAM));
	memcpy(pBootBak,pBoot,sizeof(BOOT_PARAM));
	memcpy(pOSBak,pOS,sizeof(OS_INFO));

	*pdwHdrSize = dwHeadSize;

	return pHeadBuf;
	
}

void * CMasterImgGen::MakeMasterImageSingle(DWORD *pImageSize,
		                  int NumOfImages,
						  IMAGE_PARAM * lpImageParam,
						  int PageType,
						  unsigned __int64 llSize/* = 0*/,
						  unsigned __int64 llOffset/* = 0*/)
{
	if(pImageSize == NULL || NumOfImages != 1 || lpImageParam == NULL)
	{
		return NULL;
	}

	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = ::CreateFile(lpImageParam->szPath,
		                 GENERIC_READ,
						 FILE_SHARE_READ,
						 NULL,
						 OPEN_EXISTING,
						 FILE_ATTRIBUTE_NORMAL,
						 NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	DWORD dwSize = llSize ? (DWORD)llSize : GetFileSize(hFile,NULL);
	
	DWORD dwHeadSize = 0;

	LPBYTE pHead = MakeMasterImageHeader(dwSize,&dwHeadSize,PageType);
	if(pHead == NULL || dwHeadSize==0)
	{
		CloseHandle(hFile);
		return NULL;
	}

	DWORD dwImgSize= dwHeadSize + (((dwSize + SECTOR_SIZE - 1 ) / SECTOR_SIZE) * SECTOR_SIZE);

	LPBYTE pImg = new BYTE[dwImgSize];
	if(pImg == NULL)
	{
		delete [] pHead;
		CloseHandle(hFile);
		return NULL;

	}

	memset(pImg,0xFF,dwImgSize);

	memcpy(pImg,pHead,dwHeadSize);
	delete [] pHead;
	pHead = NULL;

	if ( 0 != llOffset )
	{
		LARGE_INTEGER liOffset;
		liOffset.QuadPart = llOffset;
		if (::SetFilePointer(hFile,liOffset.LowPart, &liOffset.HighPart,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			delete [] pImg;
			CloseHandle(hFile);
			return NULL;
		}
	}
	
	DWORD dwRead = 0;
	BOOL bOK = ReadFile(hFile,pImg+dwHeadSize,dwSize,&dwRead,NULL);
	if(!bOK || dwRead != dwSize)
	{
		delete [] pImg;
		CloseHandle(hFile);
		return NULL;
	}

	*pImageSize = dwImgSize;

	CloseHandle(hFile);

	return pImg;
}
