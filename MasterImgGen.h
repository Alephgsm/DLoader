#ifndef MASTERIMGGEN_H
#define MASTERIMGGEN_H

typedef struct tagIMAGE_PARAM {
	_TCHAR szPath[MAX_PATH];
	char szName[24];
	char szDescription[48];
	char szVersion[24];
	int Offset;      // in sectors
	int PaddingSize; // in sectors
} IMAGE_PARAM/*, * PIMAGE_PARAM*/;


// Page Type
#define SMALL_PAGE		0
#define LARGE_PAGE		1

class CMasterImgGen {
public:
	CMasterImgGen(void):m_SctsPerBlock(0) {}
	virtual ~CMasterImgGen(void) {}

public:
	void *MakeMasterImage(DWORD *pImageSize,
		                  int NumOfImages,
						  IMAGE_PARAM * lpImageParam,
						  int PageType );
	void *MakeMasterImageSingle(DWORD *pImageSize,
		                  int NumOfImages,
						  IMAGE_PARAM * lpImageParam,
						  int PageType,
						  unsigned __int64 llSize = 0,
						  unsigned __int64 llOffset = 0);

protected:
	BOOL CalcImageOffset( int NumOfImages,
		                  IMAGE_PARAM * lpImageParam );
	BOOL AddBootParam( FILE * pMaster,
		               int NumOfImages,
					   const IMAGE_PARAM * lpImageParam );
	BOOL WriteBootParam( FILE * pMaster,
		                 const void * pBootParam,
						 int ParamSize,
						 int TotalPages );
	BOOL MakeImage( FILE * pMaster,
		            const IMAGE_PARAM * lpImageParam );
	virtual BOOL WriteMainArea( FILE * pMaster,
		                        const unsigned char * pMArea );

	LPBYTE MakeMasterImageHeader(DWORD ps_size,DWORD *pimg_hdr_size, int page_type );

protected:
	int m_SctsPerBlock;
};


#endif // MASTERIMGGEN_H
