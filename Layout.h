#ifndef _LAYOUT_H
#define _LAYOUT_H

#define lPARENT 0x10000
#define IDC_LASTCHILD 0x10001     // Special child id for last child in list.
                                  // Guaranteed not to be used because you
                                  // cannot have control ids above 0xffff.


/////////////////////////////////// Limits ////////////////////////////////////


#define NUMPARTS     7            // There are 7 parts: left, top, right,
                                  // bottom, width, height and group
#define NUMMETRICS   6            // There are 6 metrics: left, top, right,
                                  // bottom, width and height.
#define NUMSIDES     4            // There are 4 sides: left, top, right and
                                  // bottom.

typedef enum{
	lSTRETCH,
	lMOVE,
	lVCENTER,
	lHCENTER,
	lEND
}lACTION;

typedef enum{
	lpLEFT,
	lpTOP,
	lpRIGHT,
	lpBOTTOM,
	lpWIDTH,
	lpHEIGHT,
	lpGROUP
}lPART;

#define lLEFT(idc)					lpLEFT,		idc,	0
#define lTOP(idc)					lpTOP,		idc,	0
#define lRIGHT(idc)					lpRIGHT,	idc,	0
#define lBOTTOM(idc)				lpBOTTOM,	idc,	0
#define lWIDTH(idc)					lpWIDTH,	idc,	100
#define lHEIGHT(idc)				lpHEIGHT,	idc,	100
#define lWIDTHOF(idc,percent)		lpWIDTH,	idc,	percent
#define lHEIGHTOF(idc,percent)		lpHEIGHT,	idc,	percent
#define lCHILD(idc)					lpGROUP,	idc,	idc
#define lGROUP(idcFirst,idcLast)	lpGROUP,	idcFirst,	idcLast

typedef struct{
	union{
		int nPart;
		int nMetric;
		int nSide;
	};
	union{
		int idc;
		int idcFirst;
	};
	union{
		int nPercent;
		int idcLast;
	};
}LACTIONINFO;

typedef struct{
	lACTION Action;
	LACTIONINFO ActOn;
	LACTIONINFO RelTo;
	int nOffset;
	UINT fState;
	int nPixelOffset;
}RULE;
//////////////////////// ID for Child List Terminator /////////////////////////



typedef enum{
	UNKNOWN,
	KNOWN
}MFLAG;

enum {
   UNAPPLIED,                     // Rule has not been applied
   APPLIED                        // Rule has been applied
};
#pragma warning(disable:4201)
typedef struct{
	int idc;
	union{
		int anMetric[NUMMETRICS];
		struct{
			RECT rc;
			int nWidth;
			int nHeight;
		};
	};
	BOOL fFixed;
	MFLAG afMetric[NUMMETRICS];
}CHILD;

#define adgARRAY_SIZE(Array) (sizeof(Array) / sizeof((Array)[0]))
#define adgINRANGE(lo, val, hi) (((lo) <= (val)) && ((val) <= (hi)))
#define ISPART(n)   adgINRANGE(lpLEFT, (n), lpGROUP)
#define ISMETRIC(n) adgINRANGE(lpLEFT, (n), lpHEIGHT)
#define ISSIDE(n)   adgINRANGE(lpLEFT, (n), lpBOTTOM)
#define adgMB(s) {                                                   \
      TCHAR szTMP[128];                                              \
      GetModuleFileName(NULL, szTMP, adgARRAY_SIZE(szTMP));          \
      MessageBox(GetActiveWindow(), s, szTMP, MB_OK);                \
   }

////////////////////////// Local Function Prototypes //////////////////////////
	
	BOOL adgMapDialogRect (HWND hwndParent, PRECT prc);
	
	int Layout_GetOppositeSide (int nSide);
	int Layout_GetOtherUnknownMetric (int nUnknownMetric);
	BOOL Layout_MetricIsVertical (int nMetric);
	void Layout_SolveChild (CHILD* pChild);
	CHILD* Layout_FindChild (CHILD* pChildList, int idcChild);
	CHILD* Layout_CreateChildList (HWND hwndParent, int* pnChildren);
	void Layout_ConvertDlgUnits (HWND hwndParent, RULE* pRules, CHILD* pChildList);
	void Layout_MarkUnknowns (HWND hwndParent, RULE* pRules, CHILD* pChildList);
	BOOL Layout_CheckChild (CHILD* pChild);
	BOOL Layout_ApplyRule (HWND hwndParent, RULE* pRules, CHILD* pChildList, RULE* pRule);
	BOOL Layout_ApplyRules (HWND hwndParent, RULE* pRules, CHILD* pChildList);
	
	BOOL WINAPI Layout_ComputeLayout (HWND hwndDlg, RULE* pRules);
	
#endif
