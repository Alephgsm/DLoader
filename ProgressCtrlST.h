//
//	Class:		CProgressCtrlST
//
//	Compiler:	Visual C++
//	Tested on:	Visual C++ 6.0
//
//	Version:	See GetVersionC() or GetVersionI()
//
//	Created:	01/January/1999
//	Updated:	27/August/2002
//
//	Author:		Davide Calabro'		davide_calabro@yahoo.com
//									http://www.softechsoftware.it
//
//	Disclaimer
//	----------
//	THIS SOFTWARE AND THE ACCOMPANYING FILES ARE DISTRIBUTED "AS IS" AND WITHOUT
//	ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED. NO REPONSIBILITIES FOR POSSIBLE
//	DAMAGES OR EVEN FUNCTIONALITY CAN BE TAKEN. THE USER MUST ASSUME THE ENTIRE
//	RISK OF USING THIS SOFTWARE.
//
//	Terms of use
//	------------
//	THIS SOFTWARE IS FREE FOR PERSONAL USE OR FREEWARE APPLICATIONS.
//	IF YOU USE THIS SOFTWARE IN COMMERCIAL OR SHAREWARE APPLICATIONS YOU
//	ARE GENTLY ASKED TO DONATE 5$ (FIVE U.S. DOLLARS) TO THE AUTHOR:
//
//		Davide Calabro'
//		P.O. Box 65
//		21019 Somma Lombardo (VA)
//		Italy
//
#ifndef _PROGRESSCTRLST_H
#define _PROGRESSCTRLST_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Return values
#ifndef	PROGRESSCTRLST_OK
#define	PROGRESSCTRLST_OK					0
#endif
#ifndef	PROGRESSCTRLST_INVALIDRESOURCE
#define	PROGRESSCTRLST_INVALIDRESOURCE		1
#endif
#define MAX_PROGRESS_RATE   0x10

class CProgressCtrlST : public CProgressCtrl
{
public:
	CProgressCtrlST();
	virtual ~CProgressCtrlST();

	DWORD SetBitmap(int nBitmap, BOOL bRepaint = TRUE);
	DWORD SetBitmap(HBITMAP hBitmap, BOOL bRepaint = TRUE);

	void SetRange(int nLower, int nUpper);
	int SetStep(int nStep);
	int SetPos(int nPos);
	int StepIt();

	static short GetVersionI()		{return 11;}
	static LPCTSTR GetVersionC()	{return (LPCTSTR)_T("1.1");}

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressCtrlST)
	//}}AFX_VIRTUAL

protected:
    //virtual void OnDrawText(CDC* pDC, int nPercentage, CRect rcCtrl, CRect rcProgress, BOOL bVertical);

	//{{AFX_MSG(CProgressCtrlST)
	afx_msg void OnPaint();
	//}}AFX_MSG

	int m_nLower;
	int m_nUpper;
	int m_nRange;

	int m_nStep;
	int m_nPos;
    DWORD m_dwRate;

	HBITMAP		m_hBitmap;			// Handle to bitmap
	DWORD		m_dwWidth;			// Width of bitmap
	DWORD		m_dwHeight;			// Height of bitmap

private:
	void FreeResources(BOOL bCheckForNULL = TRUE);
	void CalcRange();
	void TileBitmap(HDC hdcDest, HDC hdcSrc, const CRect &rect);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
